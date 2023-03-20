#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/HandleMaker.h"
#include"../Helper/ISinglton.h"
#include<fstream>
#include<sstream>
#include"../Buffer/CreateGpuBuffer.h"

struct materia
{
	std::string name;//マテリアル名
	DirectX::XMFLOAT3 ambient;//アンビエント影響度
	DirectX::XMFLOAT3 diffuse;//ディフューズ影響度
	DirectX::XMFLOAT3 specular;//スペキュラー影響度
	float alpha;//α
	std::string textureFilename;//テクスチャファイル名

	materia()
	{
		ambient = { 0.3f,0.3f,0.3f };
		diffuse = { 0.0f,0.0f,0.0f };
		specular = { 0.0f,0.0f,0.0f };
		alpha = 1.0;
	}

	void Delete()
	{
		ambient = { 0.0f,0.0f,0.0f };
		diffuse = { 0.0f,0.0f,0.0f };
		specular = { 0.0f,0.0f,0.0f };
		alpha = 0.0;
	}
};


/// <summary>
/// .Objのモデルを描画するのに必要なデータが入ったもの
/// </summary>
struct ObjResourceData 
{
	//vector<Vertex> vert;
	//vector<unsigned short> indexNum;

	materia material;
	RESOURCE_HANDLE mtlHanlde;
	
	std::vector<unsigned short> posKey;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	UINT indexNum;

	std::vector<DirectX::XMFLOAT4>vertices;
	std::vector<UINT>index;
};



/// <summary>
/// .objに必要なデータを保管し、objを描画する際にデータを使い回すクラス
/// </summary>
class ObjResourceMgr :public ISingleton<ObjResourceMgr>
{
public:
	ObjResourceMgr();
	~ObjResourceMgr();

	/// <summary>
	/// .objのモデルを読み込みます
	/// .objと.pngと.mtlの名前は統一して下さい
	/// </summary>
	/// <param name="RESOURCE">.objのデータがあるファイルパス</param>
	/// <param name="TEX_RESOURCE">.mtlの画像データがあるファイルパス</param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE LoadModel(std::string RESOURCE);

	/// <summary>
	/// 指定のモデルデータを削除します
	/// </summary>
	/// <param name="HANDLE">指定のハンドル</param>
	void Release(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// 指定のハンドルからリソースを取得します
	/// </summary>
	/// <param name="HANDLE">指定のハンドル</param>
	ObjResourceData GetResourceData(RESOURCE_HANDLE HANDLE);

private:
	HandleMaker handle;

	std::vector<ObjResourceData> resource;
	std::vector<std::string> name;

	std::unique_ptr<CreateGpuBuffer> modelDataBuffers;


	RESOURCE_HANDLE LoadMaterial(const std::string &filename, ObjResourceData *resource, std::string MTL_RESOURCE);

};
