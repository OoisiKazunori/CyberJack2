#pragma once
#include<string>
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include <source_location>

/// <summary>
/// �R���s���[�g�V�F�[�_�[�̎��s
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

		//�f�o�b�N���
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


	std::vector<ComputeData>computeArray;		//���ߎ󂯎��p
	std::vector<ComputeBufferData>generateComputeArray;	//�����p

	//�`��ɕK�v�ȃo�b�t�@���R�}���h���X�g�ɐς�
	void SetBufferOnCmdList(const  std::vector<std::shared_ptr<KazBufferHelper::BufferData>> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);


	//�����̕`��֐�����Ăяo���ꂽ���G���[��������
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

		return lFileNameString + "�t�@�C����" + lFunctionString + "�֐���" + lLine + "�s�ڂ�" + lColumn + "�����ڂɏ�����Ă���`��N���X�Ő������ꂽ���ɖ�肪����܂�";
	}
};