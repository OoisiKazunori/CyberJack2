#include "BufferDuplicateBlocking.h"
#include"DirectXCommon/DirectX12Device.h"
#include"DirectXCommon/DirectX12CmdList.h"

RESOURCE_HANDLE PipelineDuplicateBlocking::GeneratePipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &DATA, const PipelineDuplicateData &CHECK_DATA)
{
	//重複チェック
	for (int i = 0; i < duplicateDataArray.size(); ++i)
	{
		//比較する際に重要になる要素
		/*
		ルートシグネチャーの設定、シェーダーの設定、ブレンドモード
		*/
		if (!IsSameRootSignatureGenerated(CHECK_DATA.rootsignature, duplicateDataArray[i].rootsignature))
		{
			continue;
		}
		if (CHECK_DATA.shaderOption != duplicateDataArray[i].shaderOption)
		{
			continue;
		}
		if (CHECK_DATA.blendMode != duplicateDataArray[i].blendMode)
		{
			continue;
		}
		//重複を確認したら返す
		return i;
	}


	//重複がなければ生成
	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	HRESULT lResult = S_FALSE;
	//末尾追加
	if (bufferArray.size() <= outputHandle)
	{
		bufferArray.emplace_back();
		duplicateDataArray.emplace_back(CHECK_DATA);
		pipelineCount.emplace_back(outputHandle);
		lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&DATA, IID_PPV_ARGS(&bufferArray[outputHandle]));
	}
	//ガベレージコレクション
	else
	{
		duplicateDataArray[outputHandle] = CHECK_DATA;
		pipelineCount[outputHandle].shaderHandle = outputHandle;
		lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&DATA, IID_PPV_ARGS(&bufferArray[outputHandle]));
	}

	if (lResult != S_OK)
	{
		return -1;
	}
	return outputHandle;
}

RESOURCE_HANDLE PipelineDuplicateBlocking::GeneratePipeline(const D3D12_COMPUTE_PIPELINE_STATE_DESC &DATA)
{
	//重複チェック
	for (int i = 0; i < 0; ++i)
	{
		bool lCheckFlag = false;
		//bufferGenerateDataArray[i].VS.pShaderBytecode == DATA.VS.pShaderBytecode;

		//重複を確認したら返す
		if (lCheckFlag)
		{
			return i;
		}
	}

	//重複がなければ生成
	RESOURCE_HANDLE lHandle = handle.GetHandle();
	bufferArray.emplace_back();
	pipelineCount.emplace_back(lHandle);
	HRESULT lResult = DirectX12Device::Instance()->dev->CreateComputePipelineState(&DATA, IID_PPV_ARGS(&bufferArray[lHandle]));
	if (lResult != S_OK)
	{
		return -1;
	}
	return lHandle;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	pipelineCount[HANDLE].Call();
	return bufferArray[HANDLE];
}

void PipelineDuplicateBlocking::Update()
{
	for (auto &obj : pipelineCount)
	{
		//一度も使われていないデータは消去する。
		if (obj.IsNotCall())
		{
			RESOURCE_HANDLE releaseHandle = obj.shaderHandle;
			handle.DeleteHandle(releaseHandle);
			bufferArray[releaseHandle].Reset();
			pipelineCount[releaseHandle].Finalize();
			duplicateDataArray[releaseHandle].Finalize();
		}
		obj.Reset();
	}
}

RESOURCE_HANDLE ShaderDuplicateBlocking::GenerateShader(const ShaderOptionData &DATA)
{
	//重複確認
	for (int i = 0; i < generateDataArray.size(); ++i)
	{
		if (generateDataArray[i] == DATA)
		{
			shaderCount[i].Call();
			return i;
		}
	}

	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	//末尾追加
	if (bufferArray.size() <= outputHandle)
	{
		//ID追加
		generateDataArray.emplace_back(DATA);
		//シェーダー生成
		Shader lShader;
		bufferArray.emplace_back(lShader.CompileShader(DATA));
		//呼び出し確認用
		shaderCount.emplace_back(outputHandle);
	}
	//ガベレージコレクション
	else
	{
		//ID追加
		generateDataArray[outputHandle] = DATA;
		//シェーダー生成
		Shader lShader;
		bufferArray[outputHandle] = lShader.CompileShader(DATA);
		//呼び出し確認用
		shaderCount[outputHandle].shaderHandle = outputHandle;
	}
	return outputHandle;
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return bufferArray[HANDLE];
}

void ShaderDuplicateBlocking::Update()
{
	for (auto &obj : shaderCount)
	{
		//一度も使われていないデータは消去する。
		if (obj.IsNotCall())
		{
			RESOURCE_HANDLE releaseHandle = obj.shaderHandle;
			handle.DeleteHandle(releaseHandle);
			bufferArray[releaseHandle].Reset();
			shaderCount[releaseHandle].Finalize();
			generateDataArray[releaseHandle].fileName = "";
		}
		obj.Reset();
	}
}

RESOURCE_HANDLE RootSignatureDuplicateBlocking::GenerateRootSignature(const RootSignatureDataTest &DATA)
{
	//重複チェック
	for (int i = 0; i < dataForDuplicateBlocking.size(); ++i)
	{
		if (IsSameRootSignatureGenerated(DATA, dataForDuplicateBlocking[i]))
		{
			callDataArray[i].Call();
			return i;
		}
	}

	std::vector<CD3DX12_ROOT_PARAMETER> lRootparamArray(DATA.rangeArray.size());

	int lCountDescriptorNum = 0;
	//デスクリプタヒープの設定が必要な数を計算する
	for (int i = 0; i < DATA.rangeArray.size(); ++i)
	{
		const bool L_THIS_TYPE_IS_DESCRIPTOR_FLAG = DATA.rangeArray[i].rangeType % 2 == 0;
		if (L_THIS_TYPE_IS_DESCRIPTOR_FLAG)
		{
			++lCountDescriptorNum;
		}
	}
	std::vector<CD3DX12_DESCRIPTOR_RANGE> lDescRangeRangeArray(lCountDescriptorNum);

#pragma region ルートパラメーターを設定する
	GraphicsRootSignature::ParamData lViewData;
	UINT lDescriptorArrayNum = 0;

	std::vector<RootSignatureParameter> lParamArrayData;
	for (int i = 0; i < DATA.rangeArray.size(); i++)
	{
		switch (DATA.rangeArray[i].rangeType)
		{
		case GRAPHICS_RANGE_TYPE_CBV_VIEW:
			//ルートパラムの設定
			lRootparamArray[i].InitAsConstantBufferView(lViewData.cbv, 0, D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.cbv;
			break;

		case GRAPHICS_RANGE_TYPE_CBV_DESC:
			//ルートパラムの設定
			lDescRangeRangeArray[lDescriptorArrayNum].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, lViewData.cbv);
			lRootparamArray[i].InitAsDescriptorTable(1, &lDescRangeRangeArray[lDescriptorArrayNum], D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.cbv;
			break;

		case GRAPHICS_RANGE_TYPE_SRV_VIEW:
			//ルートパラムの設定
			lRootparamArray[i].InitAsShaderResourceView(lViewData.srv, 0, D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.srv;
			break;

		case GRAPHICS_RANGE_TYPE_SRV_DESC:
			//ルートパラムの設定
			lDescRangeRangeArray[lDescriptorArrayNum].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, lViewData.srv);
			lRootparamArray[i].InitAsDescriptorTable(1, &lDescRangeRangeArray[lDescriptorArrayNum], D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.srv;
			break;


		case GRAPHICS_RANGE_TYPE_UAV_VIEW:
			//ルートパラムの設定
			lRootparamArray[i].InitAsUnorderedAccessView(lViewData.uav, 0, D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.uav;
			break;

		case GRAPHICS_RANGE_TYPE_UAV_DESC:
			//ルートパラムの設定
			lDescRangeRangeArray[lDescriptorArrayNum].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, lViewData.uav);
			lRootparamArray[i].InitAsDescriptorTable(1, &lDescRangeRangeArray[lDescriptorArrayNum], D3D12_SHADER_VISIBILITY_ALL);
			++lViewData.uav;
			break;
		case -1:
			break;
		}

#pragma region ルートパラメーター自動設定を行う為の設定
		RootSignatureParameter lData;
		lData.range = DATA.rangeArray[i].rangeType;
		lData.paramData.type = DATA.rangeArray[i].dataType;

		const bool L_THIS_TYPE_IS_DESCRIPTOR_FLAG = DATA.rangeArray[i].rangeType % 2 == 0;
		if (L_THIS_TYPE_IS_DESCRIPTOR_FLAG)
		{
			lData.type = GRAPHICS_ROOTSIGNATURE_TYPE_DESCRIPTORTABLE;
			++lDescriptorArrayNum;
		}
		else
		{
			lData.type = GRAPHICS_ROOTSIGNATURE_TYPE_VIEW;
		}
		lData.paramData.param = lViewData.TotalNum() - 1;
		lParamArrayData.push_back(lData);
#pragma endregion
	}

#pragma endregion

#pragma region バージョン自動判定でのシリアライズ
	D3D12_ROOT_SIGNATURE_FLAGS lRootSignatureType = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC lRootSignatureDesc;
	lRootSignatureDesc.Init_1_0(
		static_cast<UINT>(lRootparamArray.size()),
		lRootparamArray.data(),
		static_cast<UINT>(DATA.samplerArray.size()),
		DATA.samplerArray.data(),
		lRootSignatureType);

	Microsoft::WRL::ComPtr<ID3DBlob> lRootSigBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> lErrorBlob = nullptr;
	HRESULT lResult = D3DX12SerializeVersionedRootSignature(
		&lRootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&lRootSigBlob,
		&lErrorBlob);

	if (lResult != S_OK)
	{
		assert(0);
	}
#pragma endregion


	//ルートシグネチャの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> lRootSignature;
	lResult = DirectX12Device::Instance()->dev->CreateRootSignature(
		0,
		lRootSigBlob->GetBufferPointer(),
		lRootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&lRootSignature)
	);
	if (lResult != S_OK)
	{
		return -1;
	}

	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	//ルートシグネチャーとパラムデータの保管
	if (rootSignatureArray.size() <= outputHandle)
	{
		rootSignatureArray.emplace_back(lRootSignature, lParamArrayData);
		callDataArray.emplace_back(outputHandle);
		dataForDuplicateBlocking.emplace_back(DATA);
	}
	else
	{
		rootSignatureArray[outputHandle] = RootSignatureData(lRootSignature, lParamArrayData);
		callDataArray[outputHandle].shaderHandle = outputHandle;
		dataForDuplicateBlocking[outputHandle] = DATA;
	}

	return outputHandle;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignatureDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	callDataArray[HANDLE].Call();
	return rootSignatureArray[HANDLE].buffer;
}

const std::vector<RootSignatureParameter> &RootSignatureDuplicateBlocking::GetRootParam(RESOURCE_HANDLE ROOTSIGNATURE_HANDLE)
{
	return rootSignatureArray[ROOTSIGNATURE_HANDLE].rootParamDataArray;
}

void RootSignatureDuplicateBlocking::Update()
{
	for (auto &obj : callDataArray)
	{
		//一度も使われていないデータは消去する。
		if (obj.IsNotCall())
		{
			RESOURCE_HANDLE releaseHandle = obj.shaderHandle;
			handle.DeleteHandle(releaseHandle);
			rootSignatureArray[releaseHandle].buffer.Reset();
			rootSignatureArray[releaseHandle].rootParamDataArray.clear();
			dataForDuplicateBlocking[releaseHandle].rangeArray.clear();
			dataForDuplicateBlocking[releaseHandle].samplerArray.clear();
			callDataArray[releaseHandle].Finalize();
		}
		obj.Reset();
	}
}