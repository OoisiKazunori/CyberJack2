#include "Tlas.h"
#include "DirectXCommon/DirectX12Device.h"
#include "DirectXCommon/DirectX12CmdList.h"
#include "../Buffer/UavViewHandleMgr.h"
#include "../Buffer/DescriptorHeapMgr.h"

namespace Raytracing {

	Raytracing::Tlas::Tlas()
	{

		/*===== コンストラクタ =====*/

		m_instanceCapacity = -1;
		m_descHeapIndex = -1;

	}

	void Raytracing::Tlas::Build(BlasVector arg_blasVector)
	{

		/*===== Tlasを構築 =====*/

		//現在のTlasに組み込むInstanceの数。
		int nowInstanceCount = arg_blasVector.GetInstanceCount();

		//何もTlasに登録されていなかったら処理を飛ばす。
		if (nowInstanceCount <= 0) return;

		//Tlasに組み込むInstanceの数が、現在のTlasの許容Instance数を超えていたらバッファを再構築する。
		if (m_instanceCapacity < nowInstanceCount) {

			//許容量を更新。
			m_instanceCapacity = nowInstanceCount;

			//バッファを再構築。
			ReBuildTlas(arg_blasVector);

		}
		else {

			//バッファを更新。
			UpdateTlas(arg_blasVector);

		}

	}

	void Raytracing::Tlas::ReBuildTlas(BlasVector arg_blasVector)
	{

		/*===== Tlasを再構築 =====*/

		//インスタンスの情報を記録したバッファを準備する。
		int instanceDescSize = m_instanceCapacity * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
		m_instanceDescBuffer = CreateBuffer(
			instanceDescSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD);
		m_instanceDescBuffer->SetName(L"InstanceDescBuffer");
		m_instanceDescBuffer->Map(0, nullptr, &m_instanceDescMapAddress_);

		//生成したバッファにデータを書き込む。
		WriteToMemory(m_instanceDescMapAddress_, arg_blasVector.GetInstanceData(), instanceDescSize);

		//メモリ量を求めるための設定を行う。
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
		auto& inputs = buildASDesc.Inputs;
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = m_instanceCapacity;
		inputs.InstanceDescs = m_instanceDescBuffer->GetGPUVirtualAddress();
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

		//メモリ量を求める関数を実行する。
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuild{};
		DirectX12Device::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
			&inputs, &tlasPrebuild
		);

		//スクラッチメモリ(バッファ)を確保。
		m_scratchBuffer = CreateBuffer(
			tlasPrebuild.ScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_HEAP_TYPE_DEFAULT
		);
		D3D12_RESOURCE_BARRIER barrier = { CD3DX12_RESOURCE_BARRIER::Transition(m_scratchBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
		m_scratchBuffer->SetName(L"TlasScratchBuffer");

		//TLAS用メモリ(バッファ)を確保。
		m_tlasBuffer = CreateBuffer(
			tlasPrebuild.ResultDataMaxSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			D3D12_HEAP_TYPE_DEFAULT
		);
		m_tlasBuffer->SetName(L"TlasBuffer");

		//AccelerationStructure構築。
		buildASDesc.ScratchAccelerationStructureData = m_scratchBuffer->GetGPUVirtualAddress();
		buildASDesc.DestAccelerationStructureData = m_tlasBuffer->GetGPUVirtualAddress();

		//コマンドリストに積んで実行する。
		DirectX12CmdList::Instance()->cmdList->BuildRaytracingAccelerationStructure(
			&buildASDesc, 0, nullptr
		);

		//リソースバリアの設定。
		D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_tlasBuffer.Get());
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);

		//Tlasがまだ生成されていなかったら。
		if (m_descHeapIndex == -1) {

			//加速構造体の設定。
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.RaytracingAccelerationStructure.Location = m_tlasBuffer->GetGPUVirtualAddress();

			//Indexを取得する。
			m_descHeapIndex = UavViewHandleMgr::Instance()->GetHandle();

			//ヒープの先頭にバッファを生成。
			DescriptorHeapMgr::Instance()->CreateAccelerationStructure(m_descHeapIndex, srvDesc);

		}
		else {

			//加速構造体の設定。
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.RaytracingAccelerationStructure.Location = m_tlasBuffer->GetGPUVirtualAddress();

			//すでにバッファを生成してある位置に上書きする形で生成。
			DescriptorHeapMgr::Instance()->CreateAccelerationStructure(m_descHeapIndex, srvDesc);

		}

	}

	void Raytracing::Tlas::UpdateTlas(BlasVector arg_blasVector)
	{

		/*===== Tlasを更新 =====*/

		//生成したバッファにデータを書き込む。
		int instanceDescSize = m_instanceCapacity * sizeof(D3D12_RAYTRACING_INSTANCE_DESC);
		WriteToMemory(m_instanceDescMapAddress_, arg_blasVector.GetInstanceData(), instanceDescSize);

		//メモリ量を求めるための設定を行う。
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc = {};
		auto& inputs = buildASDesc.Inputs;
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = m_instanceCapacity;
		inputs.InstanceDescs = m_instanceDescBuffer->GetGPUVirtualAddress();
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

		/*-- BLASのアドレスとスクラッチバッファアドレスとTLASのアドレスを指定して確保処理をコマンドリストに積む --*/

		//AccelerationStructure構築。
		buildASDesc.ScratchAccelerationStructureData = m_scratchBuffer->GetGPUVirtualAddress();
		buildASDesc.DestAccelerationStructureData = m_tlasBuffer->GetGPUVirtualAddress();

		//コマンドリストに積んで実行する。
		DirectX12CmdList::Instance()->cmdList->BuildRaytracingAccelerationStructure(
			&buildASDesc, 0, nullptr
		);

		//リソースバリアの設定。
		D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_tlasBuffer.Get());
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);

	}

	void Raytracing::Tlas::WriteToMemory(void* m_mapAddress, const void* m_pData, size_t m_dataSize)
	{

		/*===== メモリに値を書き込む処理 =====*/

		//nullチェック。
		if (m_mapAddress == nullptr) assert(0);

		//マップ処理を行う。
		memcpy(m_mapAddress, m_pData, m_dataSize);

	}

	Microsoft::WRL::ComPtr<ID3D12Resource> Raytracing::Tlas::CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_bufferName)
	{

		/*===== バッファを生成 =====*/

		D3D12_HEAP_PROPERTIES heapProps{};
		if (arg_heapType == D3D12_HEAP_TYPE_DEFAULT) {
			heapProps = D3D12_HEAP_PROPERTIES{
			D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
			};
		}
		if (arg_heapType == D3D12_HEAP_TYPE_UPLOAD) {
			heapProps = D3D12_HEAP_PROPERTIES{
			D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
			};
		}
		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = arg_size;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc = { 1, 0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = arg_flags;

		hr = DirectX12Device::Instance()->dev->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			arg_initState,
			nullptr,
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
		);
		if (FAILED(hr)) {
			OutputDebugStringA("CreateBuffer failed.\n");
		}
		if (resource != nullptr && arg_bufferName != nullptr) {
			resource->SetName(arg_bufferName);
		}
		return resource;

	}

}