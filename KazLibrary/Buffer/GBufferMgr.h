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
		NORMAL
	};
	GBufferMgr();

	/// <summary>
	/// 各バッファの入手
	/// </summary>
	/// <param name="arg_type">バッファの種類</param>
	/// <returns>指定したG-Buffer</returns>
	KazBufferHelper::BufferData GetBuffer(BufferType arg_type);
	std::vector<RESOURCE_HANDLE> GetRenderTarget();

	/// <summary>
	/// 各G-Bufferをコンピュートシェーダーでクリアする用のデータ
	/// </summary>
	DispatchComputeShader::ComputeData ClearData();

	/// <summary>
	/// デバック用の描画確認
	/// </summary>
	void DebugDraw();


private:
	//G-Bufferの格納
	std::vector<KazBufferHelper::BufferData>m_gBufferArray;
	//クリア用のG-Buffer格納場所
	std::vector<KazBufferHelper::BufferData>m_gBufferClearArray;
	//クリア用の情報
	DispatchComputeShader::ComputeData m_computeData;

	//G-Buffer用のレンダーターゲット
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
};

