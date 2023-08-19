#pragma once
#include<string>
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include <source_location>

struct DispatchData
{
	UINT x, y, z;
};

class ComputeShader
{
public:
	//パイプライン、ルートシグネチャー生成
	ComputeShader();
	void Generate(const ShaderOptionData& arg_shader, std::vector<KazBufferHelper::BufferData> arg_extraBuffer);
	void Generate(const ShaderOptionData& arg_shader, const RootSignatureDataTest& arg_rootsignature);
	void Compute(const DispatchData &arg_dispatch);


	std::vector<KazBufferHelper::BufferData>m_extraBufferArray;
private:
	//描画に必要なバッファをコマンドリストに積む
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData>& BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);
	PipelineDuplicateBlocking m_piplineBuffer;
	ShaderDuplicateBlocking m_shaderBuffer;
	RootSignatureDuplicateBlocking m_rootSignatureBuffer;
	bool m_initFlag;
};