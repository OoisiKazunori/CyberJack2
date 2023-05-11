#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
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

	void Update();
	void Draw();

	struct DrawData
	{
		bool generateFlag;

		//���_���
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;

		//�p�C�v���C�����
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineData;
		std::vector<ShaderOptionData> shaderDataArray;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE pipelineHandle;
		std::vector<RESOURCE_HANDLE> shaderHandleArray;
		RESOURCE_HANDLE rootsignatureHandle;


		//���̑��`��ɕK�v�ȃo�b�t�@���
		std::vector<KazBufferHelper::BufferData> buffer;

		//�f�o�b�N���
		std::source_location drawCallData;

		DrawData() :generateFlag(false), renderTargetHandle(-1)
		{
		};
	};


	/// <summary>
	/// �`����ɃX�^�b�N�������̏�����
	/// �V�[���؂�ւ����ɌĂяo������
	/// </summary>
	void Clear();


	//�`����̃X�^�b�N----------------------------------------

	RESOURCE_HANDLE GetHandle();
	DrawData *StackData(RESOURCE_HANDLE HANDLE);

	//�`����̃X�^�b�N----------------------------------------

private:
	HandleMaker drawHandle;
	std::vector<DrawData> graphicDataArray;

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


	//�`��ɕK�v�ȃo�b�t�@���R�}���h���X�g�ɐς�
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);

	//���_���̃Z�b�g
	void DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData &DATA);
	void DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData &DATA);


	//�����̕`��֐�����Ăяo���ꂽ���G���[��������
	void ErrorCheck(RESOURCE_HANDLE HANDLE, const std::source_location &DRAW_SOURCE_LOCATION)
	{
		if (HANDLE == -1)
		{
			FailCheck(ErrorMail(DRAW_SOURCE_LOCATION));
			assert(0);
		}
	}

	std::string ErrorMail(const std::source_location &DRAW_SOURCE_LOCATION);
};

namespace DrawFunc
{
	struct PipelineGenerateData
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
		std::vector<ShaderOptionData>shaderDataArray;
	};
	struct DrawFuncBufferData
	{
		DrawFuncBufferData(const KazBufferHelper::BufferResourceData &DATA, GraphicsRootParamType ROOT_PARAM, GraphicsRangeType RANGE_TYPE) :
			resourceData(DATA), rootParam(ROOT_PARAM), rangeType(RANGE_TYPE)
		{};

		KazBufferHelper::BufferResourceData resourceData;
		GraphicsRootParamType rootParam;
		GraphicsRangeType rangeType;
	};


	//DrawFunc���g�p����ۂɕK�v�ȃf�[�^
	struct DrawCallData
	{
		DrawCallData(DrawingByRasterize *CALL_DATA_PTR) :
			callDataPtr(CALL_DATA_PTR)
		{
		};
		//���X�^���C�U�`��Ăяo��
		DrawingByRasterize *callDataPtr;

		//���_���
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;

		//�p�C�v���C�����
		PipelineGenerateData pipelineData;

		//���̑��K�v�ȃo�b�t�@�̐ݒ�
		std::vector<DrawFuncBufferData>bufferResourceDataArray;
	};


	//�P�F�̃|���S���\��(�C���f�b�N�X����)
	static DrawCallData SetDrawPolygonIndexData(DrawingByRasterize *CALL_DATA_PTR, const KazRenderHelper::DrawIndexInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData(CALL_DATA_PTR);
		//���_���
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		//�s����
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX)),
			GRAPHICS_PRAMTYPE_DATA,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		//�F���
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4)),
			GRAPHICS_PRAMTYPE_DATA2,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);		//�p�C�v���C�����̃Z�b�g
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//�s����̂�
	static DrawCallData SetTransformData(DrawingByRasterize *CALL_DATA_PTR, const KazRenderHelper::DrawIndexInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData(CALL_DATA_PTR);
		//���_���
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		//�s����
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX)),
			GRAPHICS_PRAMTYPE_DATA,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};


	//�P�F�̃|���S���\��(�C���f�b�N�X�Ȃ�)
	static DrawCallData SetDrawPolygonData(DrawingByRasterize *CALL_DATA_PTR, const KazRenderHelper::DrawInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData(CALL_DATA_PTR);
		//���_���
		lDrawCallData.drawInstanceCommandData = VERTEX_DATA;
		//�s����
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX)),
			GRAPHICS_PRAMTYPE_DATA,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		//�F���
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4)),
			GRAPHICS_PRAMTYPE_DATA2,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		//�p�C�v���C�����̃Z�b�g
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};


	//�N���X���g�p�����`��֐�
	class KazRender
	{
	public:
		KazRender(const DrawCallData &INIT_DATA, std::source_location location = std::source_location::current()) :
			callData(INIT_DATA.callDataPtr),
			handle(callData->GetHandle())
		{
			//�o�b�t�@�̐���
			DrawingByRasterize::DrawData *lData = callData->StackData(handle);

			//���_���̃Z�b�g
			lData->drawInstanceCommandData = INIT_DATA.drawInstanceCommandData;
			lData->drawIndexInstanceCommandData = INIT_DATA.drawIndexInstanceCommandData;

			//�p�C�v���C���̐ݒ�
			lData->pipelineData = INIT_DATA.pipelineData.desc;
			lData->shaderDataArray = INIT_DATA.pipelineData.shaderDataArray;

			//���̑��K�v�ȃo�b�t�@�̃Z�b�g
			for (int i = 0; i < INIT_DATA.bufferResourceDataArray.size(); ++i)
			{
				lData->buffer.emplace_back(INIT_DATA.bufferResourceDataArray[i].resourceData);
				lData->buffer[i].rangeType = INIT_DATA.bufferResourceDataArray[i].rangeType;
				lData->buffer[i].rootParamType = INIT_DATA.bufferResourceDataArray[i].rootParam;
			}

			//�f�o�b�N�p�̏��̃Z�b�g
			lData->drawCallData = location;
		};


		//�悭�g�������͊֐��ɓZ�߂�----------------------------------------

		//�p�ӂ����o�b�t�@�̃A�N�Z�X
		DrawingByRasterize::DrawData *GetDrawData()
		{
			return callData->StackData(handle);
		}

		void DrawCall(const KazMath::Transform3D &TRANSFORM, const KazMath::Color &COLOR, int CAMERA_INDEX, const DirectX::XMMATRIX &MOTHER_MAT)
		{
			//�X�^�b�N�p�̃o�b�t�@���Ăяo���A�����ɓ����Ă���o�b�t�@���g�p���ē]������
			DrawingByRasterize::DrawData *lData = GetDrawData();
			//�s��
			DirectX::XMMATRIX lMat =
				KazMath::CaluWorld(TRANSFORM, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f }) *
				CameraMgr::Instance()->GetViewMatrix() *
				CameraMgr::Instance()->GetPerspectiveMatProjection();
			lData->buffer[0].bufferWrapper->TransData(&lMat, sizeof(DirectX::XMMATRIX));
			//�F
			DirectX::XMFLOAT4 lColor = COLOR.ConvertColorRateToXMFLOAT4();
			lData->buffer[1].bufferWrapper->TransData(&lColor, sizeof(DirectX::XMFLOAT4));
		};

		void DrawTexPlane(const KazMath::Transform3D &TRANSFORM, const KazMath::Color &COLOR, int CAMERA_INDEX, const DirectX::XMMATRIX &MOTHER_MAT)
		{
			//�X�^�b�N�p�̃o�b�t�@���Ăяo���A�����ɓ����Ă���o�b�t�@���g�p���ē]������
			DrawingByRasterize::DrawData *lData = GetDrawData();
			//�s��
			DirectX::XMMATRIX lMat =
				KazMath::CaluWorld(TRANSFORM, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f }) *
				//CameraMgr::Instance()->GetViewMatrix() *
				CameraMgr::Instance()->GetOrthographicMatProjection();
			lData->buffer[0].bufferWrapper->TransData(&lMat, sizeof(DirectX::XMMATRIX));
		};

		//�悭�g�������͊֐��ɓZ�߂�----------------------------------------

	private:
		DrawingByRasterize *callData;
		RESOURCE_HANDLE handle;
	};
}