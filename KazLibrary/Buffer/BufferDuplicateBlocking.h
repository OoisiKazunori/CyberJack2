#pragma once
#include"../DirectXCommon/Base.h"
#include"../KazLibrary/Helper/HandleMaker.h"
#include"../KazLibrary/Pipeline/Shader.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"

/// <summary>
/// バッファを生成する際に同じ設定の場合に生成せず同じ情報を返す処理
/// </summary>
class PipelineDuplicateBlocking
{
public:
	PipelineDuplicateBlocking()
	{}

	RESOURCE_HANDLE GeneratePipeline(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &DATA);
	RESOURCE_HANDLE GeneratePipeline(const D3D12_COMPUTE_PIPELINE_STATE_DESC &DATA);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetBuffer(RESOURCE_HANDLE HANDLE);

private:
	HandleMaker handle;
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> bufferArray;
	std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC>bufferGenerateDataArray;
};


class ShaderDuplicateBlocking
{
public:
	ShaderDuplicateBlocking()
	{}

	RESOURCE_HANDLE GenerateShader(const ShaderOptionData &DATA);
	Microsoft::WRL::ComPtr<IDxcBlob> GetBuffer(RESOURCE_HANDLE HANDLE);

private:
	HandleMaker handle;
	std::vector<Microsoft::WRL::ComPtr<IDxcBlob>> bufferArray;
	std::vector<ShaderOptionData>generateDataArray;
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
};
