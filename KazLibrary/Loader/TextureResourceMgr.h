#pragma once
#include"../DirectXCommon/Base.h"
#include<DirectXTex.h>
#include<string>
#include"../Pipeline/Shader.h"
#include<vector>
#include"../Helper/ISinglton.h"
#include"../Pipeline/GraphicsRootSignature.h"
#include"../Buffer/CreateGpuBuffer.h"
#include"../KazLibrary/Math/KazMath.h"

/// <summary>
/// 画像の分割をする際にどの座標からどれくらいのサイズでUV切り取りをするか記録したもの
/// </summary>
struct DivGraphData
{
	std::vector<KazMath::Vec2<int>> divLeftUp;
	KazMath::Vec2<int> divSize;
	RESOURCE_HANDLE handle;
};

/// <summary>
/// 画像の読み込み、画像の管理
/// </summary>
class TextureResourceMgr :public ISingleton<TextureResourceMgr> {
public:

	TextureResourceMgr();
	~TextureResourceMgr();

	/// <summary>
	/// ファイルパスを書いて画像の読み込みを行います
	/// </summary>
	/// <param name="RESOURCE">読み込みたい画像のファイルパス</param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE LoadGraph(std::string RESOURCE);
	
	/// <summary>
	///  ファイルパスを書いて画像を読み込み、分割します
	/// </summary>
	/// <param name="RESOURCE">読み込みたい画像のファイルパス</param>
	/// <param name="DIV_WIDTH_NUM">X軸に画像を分割する数</param>
	/// <param name="DIV_HEIGHT_NUM">Y軸に画像を分割する数</param>
	/// <param name="TEXSIZE_WIDTH">分割する画像のXサイズ</param>
	/// <param name="TEXSIZE_HEIGHT">分割する画像のYサイズ</param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE LoadDivGraph(string RESOURCE,
		int DIV_WIDTH_NUM,
		int DIV_HEIGHT_NUM,
		int TEXSIZE_WIDTH,
		int TEXSIZE_HEIGHT
	);

	/// <summary>
	/// 読み込んだ画像を削除します
	/// </summary>
	/// <param name="HANDLE">削除したい画像のハンドル</param>
	void Release(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// ハンドルからシェーダーリソースをコマンドリストに積ませます
	/// </summary>
	/// <param name="GRAPH_HANDLE">ハンドル</param>
	/// <param name="PARAM">ルートパラム</param>
	/// <param name="TYPE">ルートパラムの種類</param>
	void SetSRV(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE);


	/// <summary>
	/// 画像の情報を渡します
	/// バッファの取得に失敗した場合 WidthとHeightに0が返されます
	/// </summary>
	/// <param name="HANDLE"></param>
	/// <returns></returns>
	D3D12_RESOURCE_DESC GetTextureSize(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// 分割する画像の情報を渡します
	/// バッファの取得に失敗した場合 texSizeのXとYに-1が入ります
	/// </summary>
	/// <param name="HANDLE">ハンドル</param>
	/// <returns>分割する画像の情報</returns>
	DivGraphData GetDivData(RESOURCE_HANDLE HANDLE);

	std::unique_ptr<CreateGpuBuffer> buffers;
private:
	//std::unique_ptr<CreateGpuBuffer> buffers;
	const DirectX::Image *img;

	D3D12_STATIC_SAMPLER_DESC samplerDesc;

	static const int texWidth = 256;
	static const int texDataCount = texWidth * texWidth;
	const int DescriptorMaxNum = 5000;

	DirectX::ScratchImage scratchImg;
	UINT handle;
	UINT setHandle;
	UINT IncreSize;
	DirectX::TexMetadata metadata;

	HRESULT result;

	std::vector<std::string> handleName;

	vector<DivGraphData> divData;
	friend ISingleton<TextureResourceMgr>;
};