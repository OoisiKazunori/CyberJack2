#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Helper/Compute.h"

/// <summary>
/// G-Buffer�̊Ǘ��N���X
/// </summary>
class GBufferMgr :public ISingleton<GBufferMgr>
{
public:
	enum BufferType
	{
		NONE = -1,
		ALBEDO,
		NORMAL,
		R_M_S_ID,
		WORLD
	};
	GBufferMgr();

	/// <summary>
	/// �e�o�b�t�@�̓���
	/// </summary>
	/// <param name="arg_type">�o�b�t�@�̎��</param>
	/// <returns>�w�肵��G-Buffer</returns>
	KazBufferHelper::BufferData GetBuffer(BufferType arg_type);
	std::vector<RESOURCE_HANDLE> GetRenderTarget();
	std::vector<DXGI_FORMAT> GetRenderTargetFormat();

	/// <summary>
	/// �eG-Buffer���R���s���[�g�V�F�[�_�[�ŃN���A����p�̃f�[�^
	/// </summary>
	DispatchComputeShader::ComputeData ClearData();

	/// <summary>
	/// �f�o�b�N�p�̕`��m�F
	/// </summary>
	void DebugDraw();


private:
	//G-Buffer�̊i�[
	std::vector<KazBufferHelper::BufferData>m_gBufferArray;
	//�N���A�p��G-Buffer�i�[�ꏊ
	std::vector<KazBufferHelper::BufferData>m_gBufferClearArray;
	//�N���A�p�̏��
	DispatchComputeShader::ComputeData m_computeData;

	//G-Buffer�p�̃����_�[�^�[�Q�b�g
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;
};

