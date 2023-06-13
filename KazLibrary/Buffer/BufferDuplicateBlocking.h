#pragma once
#include"../DirectXCommon/Base.h"
#include"../KazLibrary/Helper/HandleMaker.h"
#include"../KazLibrary/Pipeline/Shader.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

//いくつ使われていたか確認する為の情報
struct CallCounterData
{
	RESOURCE_HANDLE shaderHandle;
	int callNum;

	CallCounterData(RESOURCE_HANDLE handle) :shaderHandle(handle), callNum(1)
	{
	};
	void Finalize()
	{
		shaderHandle = -1;
		callNum = 0;
	}

	bool IsNotCall()
	{
		return callNum <= 0 && shaderHandle != -1;
	}
	void Call()
	{
		++callNum;
	}
	void Reset()
	{
		callNum = 0;
	}
};

static bool IsSameRootSignatureGenerated(RootSignatureDataTest NEW_ROOTSIGNATURE, RootSignatureDataTest REFERENCE)
{
	//設定されたルートシグネチャの数が違う場合は再検索
	if (REFERENCE.rangeArray.size() != NEW_ROOTSIGNATURE.rangeArray.size())
	{
		return false;
	}

	bool lContinueFlag = false;
	//設定されている種類が違う場合は再検査
	for (int duplicateIndex = 0; duplicateIndex < REFERENCE.rangeArray.size(); ++duplicateIndex)
	{
		if (REFERENCE.rangeArray[duplicateIndex].rangeType != NEW_ROOTSIGNATURE.rangeArray[duplicateIndex].rangeType)
		{
			lContinueFlag = true;
			break;
		}
	}
	if (lContinueFlag)
	{
		return false;
	}

	return true;
};

/// <summary>
/// バッファを生成する際に同じ設定の場合に生成せず同じ情報を返す処理
/// </summary>
class PipelineDuplicateBlocking
{
public:
	PipelineDuplicateBlocking()
	{}

	//重複確認用
	struct PipelineDuplicateData
	{
		RootSignatureDataTest rootsignature;
		std::vector<ShaderOptionData> shaderOption;
		DrawFuncPipelineData::PipelineBlendModeEnum blendMode;

		PipelineDuplicateData(RootSignatureDataTest ROOTSIGNATURE, std::vector<ShaderOptionData> OPTION, DrawFuncPipelineData::PipelineBlendModeEnum blend) :
			rootsignature(ROOTSIGNATURE), shaderOption(OPTION), blendMode(blend)
		{};

		void Finalize()
		{
			shaderOption.clear();
			blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;
			rootsignature.rangeArray.clear();
			rootsignature.samplerArray.clear();
		}
	};

	RESOURCE_HANDLE GeneratePipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &DATA, const PipelineDuplicateData &CHECK_DATA);
	RESOURCE_HANDLE GeneratePipeline(const D3D12_COMPUTE_PIPELINE_STATE_DESC &DATA);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetBuffer(RESOURCE_HANDLE HANDLE);

	void Update();

private:
	HandleMaker handle;
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> bufferArray;
	std::vector<PipelineDuplicateData>duplicateDataArray;

	std::vector<CallCounterData>pipelineCount;

};


class ShaderDuplicateBlocking
{
public:
	ShaderDuplicateBlocking()
	{}

	RESOURCE_HANDLE GenerateShader(const ShaderOptionData &DATA);
	Microsoft::WRL::ComPtr<IDxcBlob> GetBuffer(RESOURCE_HANDLE HANDLE);

	void Update();

private:

	HandleMaker handle;
	std::vector<Microsoft::WRL::ComPtr<IDxcBlob>> bufferArray;
	std::vector<ShaderOptionData>generateDataArray;

	std::vector<CallCounterData>shaderCount;
};

class RootSignatureDuplicateBlocking
{
public:
	RootSignatureDuplicateBlocking()
	{}

	RESOURCE_HANDLE GenerateRootSignature(const RootSignatureDataTest &DATA);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetBuffer(RESOURCE_HANDLE HANDLE);
	const std::vector<RootSignatureParameter> &GetRootParam(RESOURCE_HANDLE ROOTSIGNATURE_HANDLE);


	RootSignatureDataTest GetGenerateData(std::vector<KazBufferHelper::BufferData>bufferArray)
	{
		RootSignatureDataTest lRootSignatureGenerateData;
		//ルートシグネチャの生成
		for (int i = 0; i < bufferArray.size(); ++i)
		{
			lRootSignatureGenerateData.rangeArray.emplace_back
			(
				bufferArray[i].rangeType,
				bufferArray[i].rootParamType
			);
		}
		return lRootSignatureGenerateData;
	}

	void Update();

private:
	HandleMaker handle;

	struct RootSignatureData
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> buffer;
		std::vector<RootSignatureParameter> rootParamDataArray;

		RootSignatureData(Microsoft::WRL::ComPtr<ID3D12RootSignature>BUFFER, std::vector<RootSignatureParameter> PARAM_DATA) :
			buffer(BUFFER), rootParamDataArray(PARAM_DATA)
		{
		};

	};
	std::vector<RootSignatureData> rootSignatureArray;

	//重複阻止用のデータ
	std::vector<RootSignatureDataTest>dataForDuplicateBlocking;

	std::vector<CallCounterData>callDataArray;
};
