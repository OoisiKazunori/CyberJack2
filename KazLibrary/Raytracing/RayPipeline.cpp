#include "RayPipeline.h"

namespace Raytracing {

#include "../Raytracing/RayPipeline.h"
#include "../Raytracing/RayRootsignature.h"
#include "../Raytracing/HitGroupMgr.h"
#include "../DirectXCommon/DirectX12Device.h"
#include "../DirectXCommon/DirectX12CmdList.h"
#include "../Buffer/DescriptorHeapMgr.h"
#include "../DirectXCommon/DirectX12.h"
#include "../Raytracing/ShaderStorage.h"
#include "../Raytracing/BlasReferenceVector.h"
#include "../Raytracing/Tlas.h"
#include "../Raytracing/RaytracingOutput.h"
#include "../DirectXCommon/DirectX12.h"
#include <DirectXMath.h>

	RayPipeline::RayPipeline(const std::vector<RayPipelineShaderData>& InputData, HitGroupMgr::HITGROUP_ID UseHitGroup, int SRVCount, int CBVCount, int UAVCount, int PayloadSize, int AttribSize, int ReflectionCount)
	{

		/*===== セッティング処理 =====*/

		// ヒットグループ名を保存。
		hitGroupName_ = HitGroupMgr::Instance()->hitGroupNames[UseHitGroup];

		// 入力されたデータを保存する。
		const int INPUT_COUNT = static_cast<int>(InputData.size());
		for (int index = 0; index < INPUT_COUNT; ++index) {

			// 入力されたデータ構造体。
			RayPipelineShaderData buff;

			// 保存する。
			buff.shaderPath_ = InputData[index].shaderPath_;

			// 保存されているエントリポイントを保存。
			const int RG_ENTRY_COUNT = static_cast<int>(InputData[index].rayGenEnteryPoint_.size());
			for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

				// 保存する。
				buff.rayGenEnteryPoint_.push_back(InputData[index].rayGenEnteryPoint_[entryPointIndex]);

			}
			const int MS_ENTRY_COUNT = static_cast<int>(InputData[index].missEntryPoint_.size());
			for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

				// 保存する。
				buff.missEntryPoint_.push_back(InputData[index].missEntryPoint_[entryPointIndex]);

			}

			// 保存する。
			const int HS_ENTRY_COUNT = static_cast<int>(InputData[index].hitgroupEntryPoint_.size());
			for (int entryPointIndex = 0; entryPointIndex < HS_ENTRY_COUNT; ++entryPointIndex) {
				buff.hitgroupEntryPoint_.push_back(InputData[index].hitgroupEntryPoint_[entryPointIndex]);
			}

			// 保存する。
			shaderData_.push_back(buff);

		}

		// グローバルルートシグネチャを設定。
		globalRootSig_ = std::make_shared<RayRootsignature>();
		// パラメーターt0にTLAS(SRV)を設定。
		for (int index = 0; index < SRVCount; ++index)globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, index);
		// パラメーターb0にカメラ用バッファを設定。
		for (int index = 0; index < CBVCount; ++index) {
			globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, index);
		}
		// パラメーターu0に出力用バッファを設定。
		for (int index = 0; index < UAVCount; ++index)globalRootSig_->AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, index);

		// ルートシグネチャを生成。
		globalRootSig_->Build(false, L"GlobalRootSig");

		// ステートオブジェクトの設定を保存しておくようの構造体。
		CD3DX12_STATE_OBJECT_DESC subobjects;
		// ステートオブジェクトの状態を設定。
		subobjects.SetStateObjectType(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

		// シェーダーを読み込む。
		const int SHADER_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_COUNT; ++index) {

			shaderCode_.emplace_back();

			// シェーダーをコンパイルする。
			ShaderStorage::Instance()->LoadShaderForDXC(shaderData_[index].shaderPath_, "lib_6_4", "");

			// シェーダーを読み込む。
			shaderCode_[index].BytecodeLength = static_cast<SIZE_T>(ShaderStorage::Instance()->GetShaderBin(shaderData_[index].shaderPath_).size());
			shaderCode_[index].pShaderBytecode = ShaderStorage::Instance()->GetShaderBin(shaderData_[index].shaderPath_).data();

			// シェーダーの各関数レコードの登録。
			auto dxilLib = subobjects.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
			dxilLib->SetDXILLibrary(&shaderCode_[index]);

			// シェーダーのエントリポイントを登録。
			const int RG_ENTRY_COUNT = static_cast<int>(InputData[index].rayGenEnteryPoint_.size());
			for (int entryPointIndex = 0; entryPointIndex < RG_ENTRY_COUNT; ++entryPointIndex) {

				// 保存する。
				dxilLib->DefineExport(shaderData_[index].rayGenEnteryPoint_[entryPointIndex]);

			}
			const int MS_ENTRY_COUNT = static_cast<int>(InputData[index].missEntryPoint_.size());
			for (int entryPointIndex = 0; entryPointIndex < MS_ENTRY_COUNT; ++entryPointIndex) {

				// 保存する。
				dxilLib->DefineExport(shaderData_[index].missEntryPoint_[entryPointIndex]);

			}
			const int HG_ENTRY_COUNT = static_cast<int>(InputData[index].hitgroupEntryPoint_.size());
			for (int entryPointIndex = 0; entryPointIndex < HG_ENTRY_COUNT; ++entryPointIndex) {

				// 保存する。
				dxilLib->DefineExport(shaderData_[index].hitgroupEntryPoint_[entryPointIndex]);

			}

		}

		// ヒットグループの設定。
		auto hitGroup_ = subobjects.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup_->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// ClosestHitShaderをエントリポイントを保存。
		if (HitGroupMgr::Instance()->GetCHFlag(UseHitGroup)) {
			hitGroup_->SetClosestHitShaderImport(HitGroupMgr::Instance()->GetCH(UseHitGroup));
		}
		// AnyHitShaderのエントリポイントを保存。
		if (HitGroupMgr::Instance()->GetAHFlag(UseHitGroup)) {
			hitGroup_->SetAnyHitShaderImport(HitGroupMgr::Instance()->GetAH(UseHitGroup));
		}
		// IntersectShaderのエントリポイントを保存。
		if (HitGroupMgr::Instance()->GetISFlag(UseHitGroup)) {
			hitGroup_->SetIntersectionShaderImport(HitGroupMgr::Instance()->GetIS(UseHitGroup));
		}
		// ヒットグループ名を保存。
		hitGroup_->SetHitGroupExport(HitGroupMgr::Instance()->hitGroupNames[UseHitGroup]);

		// グローバルルートシグネチャの設定。
		auto rootSig = subobjects.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		rootSig->SetRootSignature(globalRootSig_->GetRootSig().Get());

		// HitGroupのローカルルートシグネチャを設定。
		auto chLocalRootSig = subobjects.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		chLocalRootSig->SetRootSignature(HitGroupMgr::Instance()->GetLocalRootSig(UseHitGroup)->GetRootSig().Get());
		auto chAssocModel = subobjects.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		chAssocModel->AddExport(HitGroupMgr::Instance()->hitGroupNames[UseHitGroup]);
		chAssocModel->SetSubobjectToAssociate(*chLocalRootSig);

		// シェーダーの設定。
		auto shaderConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		shaderConfig->Config(PayloadSize, AttribSize);

		// パイプラインの設定。
		auto pipelineConfig = subobjects.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		pipelineConfig->Config(ReflectionCount);

		// 生成する。
		DirectX12Device::Instance()->raytracingDevice->CreateStateObject(
			subobjects, IID_PPV_ARGS(&stateObject_)
		);
		stateObject_->SetName(L"StateObject");

		// Blasの保持数
		numBlas_ = 0;

	}

	void RayPipeline::BuildShaderTable(int DispatchX, int DispatchY)
	{

		/*===== シェーダーテーブルを構築 =====*/

		// コンテナに登録されているBlasの数。
		int blasRefCount = BlasReferenceVector::Instance()->GetBlasRefCount();

		// Blasの数が0だったら何もしない。
		if (blasRefCount == 0) return;

		// Blasの参照数がパイプラインに含まれているBlasの数を上回ったらシェーダーテーブルを再構築する。
		if (numBlas_ < blasRefCount) {

			// 再構築。
			ConstructionShaderTable(DispatchX, DispatchY);

			numBlas_ = blasRefCount;

		}
		else {

			// 再構築せずにBlasの情報を更新。
			MapHitGroupInfo();

		}

	}

	void RayPipeline::ConstructionShaderTable(int DispatchX, int DispatchY)
	{

		/*===== シェーダーテーブルを構築 =====*/

		// シェーダーテーブルのサイズを計算。
		const auto ShaderRecordAlignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

		// RayGenerationシェーダーではローカルルートシグネチャ未使用。
		raygenRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		raygenRecordSize_ = RoundUp(raygenRecordSize_, ShaderRecordAlignment);

		// Missシェーダーではローカルルートシグネチャ未使用。
		missRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		missRecordSize_ = RoundUp(missRecordSize_, ShaderRecordAlignment);

		// ヒットグループでは、保存されているヒットグループの中から最大のサイズのものでデータを確保する。
		hitgroupRecordSize_ += D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		hitgroupRecordSize_ += GetLargestDataSizeInHitGroup();
		hitgroupRecordSize_ = RoundUp(hitgroupRecordSize_, ShaderRecordAlignment);

		// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める。
		hitgroupCount = HitGroupMgr::Instance()->GetHitGroupCount();
		raygenSize_ = GetRayGenerationCount() * raygenRecordSize_;
		missSize_ = GetMissCount() * missRecordSize_;
		hitGroupSize_ = hitgroupCount * hitgroupRecordSize_;

		// 各テーブルの開始位置にアライメント制約があるので調整する。
		tableAlign_ = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;
		raygenRegion_ = RoundUp(raygenRecordSize_, tableAlign_);
		missRegion_ = RoundUp(missSize_, tableAlign_);
		hitgroupRegion_ = RoundUp(hitGroupSize_, tableAlign_);

		// 生成されたBLASの数。
		const int BLAS_COUNT = BlasReferenceVector::Instance()->GetBlasRefCount();

		// シェーダーテーブルのサイズ。
		tableSize_ = raygenRegion_ + missRegion_ + hitgroupRegion_ * BLAS_COUNT;

		/*========== シェーダーテーブルの構築 ==========*/

		// シェーダーテーブル確保。
		shaderTable_ = CreateBuffer(
			tableSize_, D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_HEAP_TYPE_UPLOAD,
			L"ShaderTable0");
		shaderTalbeMapAddress_ = nullptr;
		shaderTable_->Map(0, nullptr, &shaderTalbeMapAddress_);

		stateObject_.As(&rtsoProps_);

		// シェーダーテーブルを書き込み、レイを設定する。
		WriteShadetTalbeAndSettingRay(DispatchX, DispatchY);

	}

	void RayPipeline::MapHitGroupInfo()
	{

		/*===== HitGroupの情報を転送 =====*/

		uint8_t* pStart = static_cast<uint8_t*>(shaderTalbeMapAddress_);

		// Hit Group 用のシェーダーレコードを書き込み。
		uint8_t* hitgroupStart = pStart + raygenRegion_ + missRegion_;
		{

			uint8_t* pRecord = hitgroupStart;

			pRecord = BlasReferenceVector::Instance()->WriteShaderRecord(pRecord, hitgroupRecordSize_, stateObject_, hitGroupName_);

		}

	}

	void RayPipeline::TraceRay(std::weak_ptr<RaytracingOutput> Output, std::weak_ptr<RaytracingOutput> GBuffer0, std::weak_ptr<RaytracingOutput> GBuffer1, std::weak_ptr<RaytracingOutput> RenderUAV)
	{

		/*===== レイトレーシングを実行 =====*/

		// グローバルルートシグネチャで使うと宣言しているリソースらをセット。
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapMgr::Instance()->GetHeap().Get() };
		DirectX12CmdList::Instance()->cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		DirectX12CmdList::Instance()->cmdList->SetComputeRootSignature(GetGlobalRootSig()->GetRootSig().Get());

		// TLASを設定。
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(Tlas::Instance()->GetDescHeapHandle()));

		// 出力用UAVを設定。
		Output.lock()->SetComputeRootDescriptorTalbe(1);
		GBuffer0.lock()->SetComputeRootDescriptorTalbe(2);
		GBuffer1.lock()->SetComputeRootDescriptorTalbe(3);
		RenderUAV.lock()->SetComputeRootDescriptorTalbe(4);

		// パイプラインを設定。
		DirectX12CmdList::Instance()->cmdList->SetPipelineState1(stateObject_.Get());

		// レイトレーシングを実行。
		DirectX12CmdList::Instance()->cmdList->DispatchRays(&dispatchRayDesc_);




		/*===== コピーコマンドを積む =====*/

		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::UAV(
		Output.lock()->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
		GBuffer0.lock()->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
		GBuffer1.lock()->GetRaytracingOutput().Get()),CD3DX12_RESOURCE_BARRIER::UAV(
		RenderUAV.lock()->GetRaytracingOutput().Get())
		};

		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(3, barrierToUAV);


		auto backBufferIndex = DirectX12::Instance()->swapchain->GetCurrentBackBufferIndex();
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			DirectX12::Instance()->backBuffers[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST),
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);

		Output.lock()->SetResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		DirectX12CmdList::Instance()->cmdList->CopyResource(DirectX12::Instance()->backBuffers[backBufferIndex].Get(), Output.lock()->GetRaytracingOutput().Get());

		Output.lock()->SetResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// レンダーターゲットのリソースバリアをもとに戻す。
		D3D12_RESOURCE_BARRIER endBarriers[] = {

		CD3DX12_RESOURCE_BARRIER::Transition(
		DirectX12::Instance()->backBuffers[backBufferIndex].Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET)

		};

		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(endBarriers), endBarriers);

	}

	UINT RayPipeline::GetLargestDataSizeInHitGroup()
	{

		/*===== 全てのヒットグループの中で最もデータサイズが大きいものを取得する =====*/

		UINT largestDataSize = 0;

		const int HIT_GROUP_COUNT = HitGroupMgr::Instance()->GetHitGroupCount();
		for (int index = 0; index < HIT_GROUP_COUNT; ++index) {

			// データサイズを取得。
			UINT dataSize = 0;
			dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Instance()->GetSRVCount(static_cast<HitGroupMgr::HITGROUP_ID>(index));
			dataSize += sizeof(D3D12_GPU_VIRTUAL_ADDRESS) * HitGroupMgr::Instance()->GetCBVCount(static_cast<HitGroupMgr::HITGROUP_ID>(index));
			dataSize += sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) * HitGroupMgr::Instance()->GetUAVCount(static_cast<HitGroupMgr::HITGROUP_ID>(index));

			// 取得したデータサイズが保存されているデータサイズより小さかったら処理を飛ばす。
			if (dataSize < largestDataSize) continue;

			// 取得したデータサイズを保存する。
			largestDataSize = dataSize;

		}

		return largestDataSize;

	}

	void RayPipeline::WriteShadetTalbeAndSettingRay(int DispatchX, int DispatchY)
	{

		/*===== シェーダーテーブルを書き込み、レイを設定する =====*/

		// 各シェーダーレコードを書き込んでいく。
		uint8_t* pStart = static_cast<uint8_t*>(shaderTalbeMapAddress_);

		// RayGeneration 用のシェーダーレコードを書き込み。
		uint8_t* rgsStart = pStart;
		{
			uint8_t* p = rgsStart;

			const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
			for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

				const int RG_COUNT = static_cast<int>(shaderData_[index].rayGenEnteryPoint_.size());
				for (int rgIndex = 0; rgIndex < RG_COUNT; ++rgIndex) {

					void* id = rtsoProps_->GetShaderIdentifier(shaderData_[index].rayGenEnteryPoint_[rgIndex]);
					p += WriteShaderIdentifier(p, id);

				}

			}

		}

		// Miss Shader 用のシェーダーレコードを書き込み。
		uint8_t* missStart = pStart + raygenRegion_;
		{
			uint8_t* p = missStart;

			const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
			for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

				const int MS_COUNT = static_cast<int>(shaderData_[index].missEntryPoint_.size());
				for (int msIndex = 0; msIndex < MS_COUNT; ++msIndex) {

					void* id = rtsoProps_->GetShaderIdentifier(shaderData_[index].missEntryPoint_[msIndex]);
					p += WriteShaderIdentifier(p, id);

				}

			}

		}

		// Hit Group 用のシェーダーレコードを書き込み。
		uint8_t* hitgroupStart = pStart + raygenRegion_ + missRegion_;
		{

			uint8_t* pRecord = hitgroupStart;

			pRecord = BlasReferenceVector::Instance()->WriteShaderRecord(pRecord, hitgroupRecordSize_, stateObject_, hitGroupName_);

		}

		// レイ発射時の設定を設定。

		// DispatchRays のために情報をセットしておく.
		auto startAddress = shaderTable_->GetGPUVirtualAddress();
		// RayGenerationシェーダーの情報
		auto& shaderRecordRG = dispatchRayDesc_.RayGenerationShaderRecord;
		shaderRecordRG.StartAddress = startAddress;
		shaderRecordRG.SizeInBytes = raygenSize_;
		startAddress += raygenRegion_;
		// Missシェーダーの情報
		auto& shaderRecordMS = dispatchRayDesc_.MissShaderTable;
		shaderRecordMS.StartAddress = startAddress;
		shaderRecordMS.SizeInBytes = missSize_;
		shaderRecordMS.StrideInBytes = missRecordSize_;
		startAddress += missRegion_;
		// HitGroupの情報
		auto& shaderRecordHG = dispatchRayDesc_.HitGroupTable;
		shaderRecordHG.StartAddress = startAddress;
		shaderRecordHG.SizeInBytes = hitGroupSize_;
		shaderRecordHG.StrideInBytes = hitgroupRecordSize_;
		startAddress += hitgroupRegion_;
		// レイの情報
		dispatchRayDesc_.Width = DispatchX;
		dispatchRayDesc_.Height = DispatchY;
		dispatchRayDesc_.Depth = 1;

	}

	UINT RayPipeline::WriteShaderIdentifier(void* dst, const void* shaderId)
	{
		memcpy(dst, shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
	}

	int RayPipeline::GetRayGenerationCount()
	{
		int count = 0;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			count += static_cast<int>(shaderData_[index].rayGenEnteryPoint_.size());

		}

		return count;

	}

	int RayPipeline::GetMissCount()
	{
		int count = 0;

		const int SHADER_DATA_COUNT = static_cast<int>(shaderData_.size());
		for (int index = 0; index < SHADER_DATA_COUNT; ++index) {

			count += static_cast<int>(shaderData_[index].missEntryPoint_.size());

		}

		return count;

	}

	Microsoft::WRL::ComPtr<ID3D12Resource> RayPipeline::CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType, const wchar_t* Name)
	{

		/*===== バッファを生成 =====*/

		D3D12_HEAP_PROPERTIES heapProps{};
		if (HeapType == D3D12_HEAP_TYPE_DEFAULT) {
			heapProps = D3D12_HEAP_PROPERTIES{
			D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
			};
		}
		if (HeapType == D3D12_HEAP_TYPE_UPLOAD) {
			heapProps = D3D12_HEAP_PROPERTIES{
			D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1
			};
		}
		HRESULT hr;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = Size;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc = { 1, 0 };
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = Flags;

		hr = DirectX12Device::Instance()->raytracingDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			InitialState,
			nullptr,
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())
		);
		if (FAILED(hr)) {
			OutputDebugStringA("CreateBuffer failed.\n");
		}
		if (resource != nullptr && Name != nullptr) {
			resource->SetName(Name);
		}
		return resource;

	}

}