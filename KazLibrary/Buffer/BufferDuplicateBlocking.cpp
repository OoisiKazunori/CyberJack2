#include "BufferDuplicateBlocking.h"
#include"DirectXCommon/DirectX12Device.h"
#include"DirectXCommon/DirectX12CmdList.h"

RESOURCE_HANDLE PipelineDuplicateBlocking::GeneratePipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &DATA)
{
	//重複チェック
	for (int i = 0; i < bufferGenerateDataArray.size(); ++i)
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
	HRESULT lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&DATA, IID_PPV_ARGS(&bufferArray[lHandle]));
	if (lResult != S_OK)
	{
		return -1;
	}
	return lHandle;
}

RESOURCE_HANDLE PipelineDuplicateBlocking::GeneratePipeline(const D3D12_COMPUTE_PIPELINE_STATE_DESC &DATA)
{
	//重複チェック
	for (int i = 0; i < bufferGenerateDataArray.size(); ++i)
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
	HRESULT lResult = DirectX12Device::Instance()->dev->CreateComputePipelineState(&DATA, IID_PPV_ARGS(&bufferArray[lHandle]));
	if (lResult != S_OK)
	{
		return -1;
	}
	return lHandle;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return bufferArray[HANDLE];
}

RESOURCE_HANDLE ShaderDuplicateBlocking::GenerateShader(const ShaderOptionData &DATA)
{
	for (int i = 0; i < generateDataArray.size(); ++i)
	{
		if (generateDataArray[i].fileName == DATA.fileName &&
			generateDataArray[i].entryPoint == DATA.entryPoint &&
			generateDataArray[i].shaderModel == DATA.shaderModel)
		{
			return i;
		}
	}
	generateDataArray.emplace_back(DATA);

	Shader lShader;
	bufferArray.emplace_back(lShader.CompileShader(DATA));
	return handle.GetHandle();
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return bufferArray[HANDLE];
}

RESOURCE_HANDLE RootSignatureDuplicateBlocking::GenerateRootSignature(const RootSignatureDataTest &DATA)
{
	//重複チェック
	for (int i = 0; i < dataForDuplicateBlocking.size(); ++i)
	{
		//設定されたルートシグネチャの数が違う場合は再検索
		if (dataForDuplicateBlocking[i].rangeArray.size() != DATA.rangeArray.size())
		{
			continue;
		}

		bool lContinueFlag = false;
		//設定されている種類が違う場合は再検査
		for (int duplicateIndex = 0; duplicateIndex < dataForDuplicateBlocking[i].rangeArray.size(); ++duplicateIndex)
		{
			if (dataForDuplicateBlocking[i].rangeArray[duplicateIndex].rangeType != DATA.rangeArray[duplicateIndex].rangeType)
			{
				lContinueFlag = true;
				break;
			}
		}
		if (lContinueFlag)
		{
			continue;
		}

		return i;
	}
	dataForDuplicateBlocking.emplace_back(DATA);

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

	//パイプラインとパラムデータの保管
	rootSignatureArray.emplace_back(lRootSignature, lParamArrayData);
	return handle.GetHandle();
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignatureDuplicateBlocking::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return rootSignatureArray[HANDLE].buffer;
}

const std::vector<RootSignatureParameter> &RootSignatureDuplicateBlocking::GetRootParam(RESOURCE_HANDLE ROOTSIGNATURE_HANDLE)
{
	return rootSignatureArray[ROOTSIGNATURE_HANDLE].rootParamDataArray;
}
