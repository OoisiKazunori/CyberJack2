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

	std::vector<RESOURCE_HANDLE> GetRenderTarget();
	std::vector<DXGI_FORMAT> GetRenderTargetFormat();


	/// <summary>
	/// �f�o�b�N�p�̕`��m�F
	/// </summary>
	void DebugDraw();


private:
	//G-Buffer�p�̃����_�[�^�[�Q�b�g
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;
};

