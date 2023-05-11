#pragma once
#include<string>
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include <source_location>

/// <summary>
/// コンピュートシェーダーの実行
/// </summary>
class DispatchComputeShader
{
public:
	struct DispatchData
	{
		UINT x, y, z;
	};

	struct ComputeData
	{
		ComputeData(std::source_location location = std::source_location::current()) :drawCallData(location), isGenerateFlag(false)
		{};

		bool isGenerateFlag;
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
		ShaderOptionData shaderData;
		const DispatchData *dispatchData;
		std::vector<std::shared_ptr<KazBufferHelper::BufferData>>bufferArray;

		//デバック情報
		std::source_location drawCallData;
	};

	void Stack(const ComputeData &STACK_DATA);

	void Update();
	void Compute();

private:

	struct ComputeBufferData
	{
		RESOURCE_HANDLE pipelineHandle, rootsignatureHandle, shaderHandle;
		std::vector<std::shared_ptr<KazBufferHelper::BufferData>>bufferArray;
		const DispatchData *dispatchData;

		ComputeBufferData(const DispatchData *DISPATCH_DATA, std::vector<std::shared_ptr<KazBufferHelper::BufferData>>BUFFER_ARRAY,
			RESOURCE_HANDLE PIPELINE_HANDLE, RESOURCE_HANDLE ROOTSIGNATURE_HANDLE, RESOURCE_HANDLE SHADER_HANDLE) :
			pipelineHandle(PIPELINE_HANDLE), rootsignatureHandle(ROOTSIGNATURE_HANDLE), shaderHandle(SHADER_HANDLE),
			bufferArray(BUFFER_ARRAY), dispatchData(DISPATCH_DATA)

		{
		};
	};


	PipelineDuplicateBlocking piplineBufferMgr;
	ShaderDuplicateBlocking shaderBufferMgr;
	RootSignatureDuplicateBlocking rootSignatureBufferMgr;


	std::vector<ComputeData>computeArray;		//命令受け取り用
	std::vector<ComputeBufferData>generateComputeArray;	//生成用

	//描画に必要なバッファをコマンドリストに積む
	void SetBufferOnCmdList(const  std::vector<std::shared_ptr<KazBufferHelper::BufferData>> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);


	//何処の描画関数から呼び出されたかエラー文を書く
	void ErrorCheck(RESOURCE_HANDLE HANDLE, const std::source_location &DRAW_SOURCE_LOCATION)
	{
		if (HANDLE == -1)
		{
			FailCheck(ErrorMail(DRAW_SOURCE_LOCATION));
			assert(0);
		}
	}

	std::string ErrorMail(const std::source_location &DRAW_SOURCE_LOCATION)
	{
		std::string lFunctionString = DRAW_SOURCE_LOCATION.function_name();
		std::string lFileNameString = DRAW_SOURCE_LOCATION.file_name();
		std::string lColumn = std::to_string(DRAW_SOURCE_LOCATION.column());
		std::string lLine = std::to_string(DRAW_SOURCE_LOCATION.line());

		return lFileNameString + "ファイルの" + lFunctionString + "関数の" + lLine + "行目の" + lColumn + "文字目に書かれている描画クラスで生成された情報に問題があります";
	}
};