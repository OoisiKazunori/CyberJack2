#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Math/KazMath.h"
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

/// <summary>
/// �X�^�b�N���ꂽ�������Ƀ��X�^���C�Y�ŕ`�悷��
/// </summary>
class DrawingByRasterize
{
public:
	DrawingByRasterize();

	/// <summary>
	/// �`����ɃX�^�b�N�������̏�����
	/// �V�[���؂�ւ����ɌĂяo������
	/// </summary>
	void Clear();


	//�`����̃X�^�b�N----------------------------------------

	RESOURCE_HANDLE GetHandle();
	DrawFuncData::DrawData* StackData(RESOURCE_HANDLE HANDLE);
	//�`����̃X�^�b�N----------------------------------------

	//�V�V�X�e���ł̕`�施��
	void ObjectRender(const DrawFuncData::DrawCallData& DRAW_DATA);
	void Sort();
	void Render();

private:
	HandleMaker drawHandle;
	std::vector<DrawFuncData::DrawData> graphicDataArray;

	//�p�C�v���C���̏��----------------------------------------

	PipelineDuplicateBlocking piplineBufferMgr;
	ShaderDuplicateBlocking shaderBufferMgr;
	RootSignatureDuplicateBlocking rootSignatureBufferMgr;

	//�p�C�v���C���̏��----------------------------------------

	//�����_�[�^�[�Q�b�g���----------------------------------------
	RESOURCE_HANDLE prevRenderTargetHandle;
	struct RenderTargetClearData
	{
		RESOURCE_HANDLE handle;
		bool isOpenFlag;
		bool clearFlag;
	};
	std::vector<RenderTargetClearData>renderTargetClearArray;
	//�����_�[�^�[�Q�b�g���----------------------------------------


	//�V�����`�施�߂̎󂯎��
	//�`����
	std::vector<DrawFuncData::DrawData> renderInfomationForDirectX12Array;
	//�`�施�߂̎󂯎��
	std::list<DrawFuncData::DrawCallData> kazCommandList;


	//�`��ɕK�v�ȃo�b�t�@���R�}���h���X�g�ɐς�
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData>& BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);

	//���_���̃Z�b�g
	void MultiMeshedDrawIndexInstanceCommand(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& DATA, const std::vector<std::vector<KazBufferHelper::BufferData>>& MATERIAL_BUFFER, std::vector<RootSignatureParameter> ROOT_PARAM);
	void DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData& DATA);
	void DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData& DATA);

	void DrawExecuteIndirect(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& DATA, const Microsoft::WRL::ComPtr<ID3D12CommandSignature>&arg_commandSignature,const DrawFuncData::ExcuteIndirectArgumentData &arg_argmentData);

	//�����̕`��֐�����Ăяo���ꂽ���G���[��������
	void ErrorCheck(RESOURCE_HANDLE HANDLE, const std::source_location& DRAW_SOURCE_LOCATION)
	{
		if (HANDLE == -1)
		{
			FailCheck(ErrorMail(DRAW_SOURCE_LOCATION));
			assert(0);
		}
	}

	std::string ErrorMail(const std::source_location& DRAW_SOURCE_LOCATION);
};