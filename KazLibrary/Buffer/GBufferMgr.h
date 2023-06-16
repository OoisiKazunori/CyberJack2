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

	/// <summary>
	/// 各バッファの入手
	/// </summary>
	/// <param name="arg_type">バッファの種類</param>
	/// <returns>指定したG-Buffer</returns>
	KazBufferHelper::BufferData GetBuffer(BufferType arg_type);
	std::vector<RESOURCE_HANDLE> GetRenderTarget();
	std::vector<DXGI_FORMAT> GetRenderTargetFormat();

	/// <summary>
	/// 各G-Bufferをコンピュートシェーダーでクリアする用のデータ
	/// </summary>
	DispatchComputeShader::ComputeData ClearData();

	/// <summary>
	/// デバック用の描画確認
	/// </summary>
	void DebugDraw();


private:
	//G-Buffer用のレンダーターゲット
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;
};

