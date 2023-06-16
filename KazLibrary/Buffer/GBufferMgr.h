#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Helper/Compute.h"

/// <summary>
/// G-Bufferの管理クラス
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
	void SetCameraPos(DirectX::XMFLOAT3 arg_pos)
	{
		m_cameraPosBuffer.bufferWrapper->TransData(&arg_pos,sizeof(DirectX::XMFLOAT3));
	};
	const KazBufferHelper::BufferData &GetEyePosBuffer()
	{
		return m_cameraPosBuffer;
	}


	/// <summary>
	/// デバック用の描画確認
	/// </summary>
	void DebugDraw();


private:
	//G-Buffer用のレンダーターゲット
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;

	KazBufferHelper::BufferData m_cameraPosBuffer;
};

