#include "Blas.h"
#include "../Loader/FbxModelResourceMgr.h"
#include "../Render/RenderData.h"
#include "../DirectXCommon/DirectX12Device.h"
#include "../DirectXCommon/DirectX12CmdList.h"
//#include "../Raytracing/HitGroupMgr.h"
#include "../Buffer/DescriptorHeapMgr.h"
#include "../Buffer/VertexBufferMgr.h"
#include <memory>

Raytracing::Blas::Blas(bool IsOpaque, RESOURCE_HANDLE arg_vertexDataHandle, int arg_meshNumber, RESOURCE_HANDLE arg_textureHandle, int arg_hitGroupIndex)
{

	/*===== コンストラクタ =====*/

	//Blasの構築に必要な形状データを取得。
	m_vertexDataHandle = arg_vertexDataHandle;
	m_meshNumber = arg_meshNumber;
	m_textureHandle = arg_textureHandle;
	m_hitGroupIndex = arg_hitGroupIndex;
	m_geomDesc = GetGeometryDesc(IsOpaque);

	//Blasを構築。
	BuildBlas(m_geomDesc);

}

void Raytracing::Blas::Update()
{

	//更新のための値を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC asDesc{};
	auto& inputs = asDesc.Inputs;
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &m_geomDesc;
	//BLAS の更新処理を行うためのフラグを設定する。
	inputs.Flags =
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

	//インプレース更新を実行する。
	asDesc.SourceAccelerationStructureData = m_blasBuffer->GetGPUVirtualAddress();
	asDesc.DestAccelerationStructureData = m_blasBuffer->GetGPUVirtualAddress();
	//更新用の作業バッファを設定する。
	asDesc.ScratchAccelerationStructureData = m_updateBuffer->GetGPUVirtualAddress();

	//コマンドリストに積む。
	DirectX12CmdList::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&asDesc, 0, nullptr
	);

	//リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = m_blasBuffer.Get();
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);

}

uint8_t* Raytracing::Blas::WriteShaderRecord(uint8_t* arg_dst, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stageObject, LPCWSTR arg_hitGroupName)
{

	/*===== シェーダーレコードを書き込む =====*/

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
	arg_stageObject.As(&rtsoProps_);
	auto entryBegin = arg_dst;

	auto mode_ = rtsoProps_->GetShaderIdentifier(arg_hitGroupName);
	if (mode_ == nullptr) {
		throw std::logic_error("Not found ShaderIdentifier");
	}

	//シェーダー識別子を書き込む。
	memcpy(arg_dst, mode_, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	arg_dst += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	//頂点用データ
	auto vertexData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_vertexDataHandle);

	//今回のプログラムでは以下の順序でディスクリプタを記録。
	//[0] : インデックスバッファ
	//[1] : 頂点バッファ
	//※ ローカルルートシグネチャの順序に合わせる必要がある。
	//テクスチャを書き込む。
	arg_dst += WriteGPUDescriptor(arg_dst, &DescriptorHeapMgr::Instance()->GetGpuDescriptorView(m_textureHandle));
	arg_dst += WriteGPUDescriptor(arg_dst, &DescriptorHeapMgr::Instance()->GetGpuDescriptorView(vertexData.indexBuffer[m_meshNumber]->bufferWrapper->GetViewHandle()));
	arg_dst += WriteGPUDescriptor(arg_dst, &DescriptorHeapMgr::Instance()->GetGpuDescriptorView(vertexData.vertBuffer[m_meshNumber]->bufferWrapper->GetViewHandle()));

	arg_dst = entryBegin + arg_recordSize;
	return arg_dst;

}

D3D12_RAYTRACING_GEOMETRY_DESC Raytracing::Blas::GetGeometryDesc(bool arg_isOpaque)
{

	/*===== Blasの形状設定に必要な構造体を返す =====*/

	//頂点用データ
	auto vertexData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_vertexDataHandle);

	//形状データのフラグを設定。
	auto geometryDesc = D3D12_RAYTRACING_GEOMETRY_DESC{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	//不透明フラグを設定。 GPU側でAnyHitShaderを呼ぶかどうかをここで決定。
	if (arg_isOpaque) {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	}
	else {
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
	}

	//形状データの細かい項目を設定。
	auto& triangles = geometryDesc.Triangles;
	triangles.VertexBuffer.StartAddress = vertexData.vertBuffer[m_meshNumber]->bufferWrapper->GetGpuAddress();
	triangles.VertexBuffer.StrideInBytes = vertexData.vertBuffer[m_meshNumber]->structureSize;
	triangles.VertexCount = vertexData.vertBuffer[m_meshNumber]->elementNum;
	triangles.IndexBuffer = vertexData.indexBuffer[m_meshNumber]->bufferWrapper->GetGpuAddress();
	triangles.IndexCount = vertexData.indexBuffer[m_meshNumber]->elementNum;
	triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

	return geometryDesc;

}

void Raytracing::Blas::BuildBlas(const D3D12_RAYTRACING_GEOMETRY_DESC& arg_geomDesc)
{

	/*===== Blasを構築 =====*/

	//AS(レイとの判定を迅速に行うために必要なポリゴンデータ構造体)の設定に必要な各項目を設定。
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildASDesc{};
	auto& inputs = buildASDesc.Inputs;	//D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS
	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	inputs.NumDescs = 1;
	inputs.pGeometryDescs = &arg_geomDesc;

	//関数を使って必要なメモリ量を求める.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO blasPrebuild{};
	DirectX12Device::Instance()->dev->GetRaytracingAccelerationStructurePrebuildInfo(
		&inputs, &blasPrebuild
	);

	//スクラッチバッファを生成する。
	m_scratchBuffer = CreateBuffer(
		blasPrebuild.ScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(m_scratchBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, barrierToUAV);
	m_scratchBuffer->SetName(L"BlasScratchBuffer");

	//Blasのバッファを生成する。
	m_blasBuffer = CreateBuffer(
		blasPrebuild.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_HEAP_TYPE_DEFAULT
	);
	m_blasBuffer->SetName(L"BlasBuffer");

	//更新用バッファを生成する。
	m_updateBuffer = CreateBuffer(
		blasPrebuild.UpdateScratchDataSizeInBytes,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_HEAP_TYPE_DEFAULT
	);
	barrierToUAV[0] = { CD3DX12_RESOURCE_BARRIER::Transition(m_updateBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS) };
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, barrierToUAV);
	m_updateBuffer->SetName(L"BlasUpdateBuffer");

	//Blasを構築する。
	buildASDesc.ScratchAccelerationStructureData = m_scratchBuffer->GetGPUVirtualAddress();
	buildASDesc.DestAccelerationStructureData = m_blasBuffer->GetGPUVirtualAddress();

	//コマンドリストに積んで実行する。
	DirectX12CmdList::Instance()->cmdList->BuildRaytracingAccelerationStructure(
		&buildASDesc, 0, nullptr /* pPostBuildInfoDescs */
	);

	//リソースバリアの設定。
	D3D12_RESOURCE_BARRIER uavBarrier{};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = m_blasBuffer.Get();
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &uavBarrier);

}

Microsoft::WRL::ComPtr<ID3D12Resource> Raytracing::Blas::CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_bufferName)
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

int Raytracing::Blas::GetHitGroupIndex()
{
	return m_hitGroupIndex;
}
