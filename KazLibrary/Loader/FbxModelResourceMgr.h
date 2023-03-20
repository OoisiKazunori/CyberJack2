#pragma once
#pragma warning(push,0)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include"fbxsdk.h"
#pragma warning(pop)
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../Helper/HandleMaker.h"
#include"../Buffer/CreateGpuBuffer.h"

struct WeightSet
{
	UINT index;
	float weight;
};

struct VertexUv
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};


/// <summary>
/// ノード一つ分の情報
/// </summary>
struct Node
{
	/// <summary>
	/// 名前
	/// </summary>
	std::string name;

	DirectX::XMFLOAT2 pad;
	//ローカルスケール
	DirectX::XMVECTOR scaling = { 1.0f,1.0f,1.0f,0.0f };
	//ローカル回転角
	DirectX::XMVECTOR rotation = { 0.0f,0.0f,0.0f,0.0f };
	//ローカル移動
	DirectX::XMVECTOR translation = { 0.0f,0.0f,0.0f,1.0f };
	//ローカル変形行列
	DirectX::XMMATRIX transform;
	//グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	//親ノード
	Node *parent = nullptr;
};

/// <summary>
/// 読み取った情報を流し込み先
/// </summary>
class Model
{
public:
	static const int MAX_BONE_INDICES = 4;

	/// <summary>
	/// ボーン構造体
	/// </summary>
	struct Bone
	{
		//名前
		std::string name;
		//初期姿勢の逆行列
		DirectX::XMMATRIX invInitialPose;
		//クラスター(FBXのボーン情報)
		FbxSkin* fbxSkin;

		Bone(const std::string &NAME)
		{
			this->name = NAME;
		}
	};
	FbxMesh *mesh;
private:
	struct VertexPosNormalUvSkin
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 uv;
		UINT boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};	

	std::string name;
	std::vector<Node>nodes;
	std::vector<Bone>bones;


	Node *meshNode = nullptr;
	std::vector<VertexPosNormalUvSkin>vertices;
	std::vector<DirectX::XMFLOAT4>vertFloat4Data;
	std::vector<DirectX::XMFLOAT3>vertFloat3Data;
	std::vector<DirectX::XMFLOAT2>uvData;
	std::vector<DirectX::XMFLOAT3>normalData;
	std::vector<VertexUv>vertUvData;
	std::vector<UINT>indices;


	DirectX::XMFLOAT3 ambient = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 diffuse = { 1.0f,1.0f,1.0f };
	std::vector<RESOURCE_HANDLE> textureHandle;
	UINT faceCountNum;
public:
	friend class FbxModelResourceMgr;

	std::vector<Bone> &GetBone()
	{
		return bones;
	};
};


struct FbxResourceData
{
public:
	std::unique_ptr<CreateGpuBuffer> buffers;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	std::vector<RESOURCE_HANDLE> textureHandle;
	UINT indicisNum;
	DirectX::XMFLOAT3 ambient;
	DirectX::XMFLOAT3 diffuse;
	std::vector<Model::Bone> bone;

	std::vector<FbxTime> startTime;
	std::vector<FbxTime> endTime;

	UINT vertNum;
	std::vector<DirectX::XMFLOAT4>vertFloat4Data;
	std::vector<DirectX::XMFLOAT3>vertFloat3Data;
	std::vector<DirectX::XMFLOAT2>uvData;
	std::vector<DirectX::XMFLOAT3>normalData;

	std::vector<VertexUv>vertUvData;
	std::vector<UINT>indexData;

	FbxMesh *mesh;

	UINT faceCountNum;
	FbxResourceData() :indicisNum(0), ambient({ -1.0f,-1.0f,-1.0f }), diffuse({ -1.0f,-1.0f,-1.0f }), vertNum(0)
	{
	}
};

class FbxModelResourceMgr:public ISingleton<FbxModelResourceMgr>
{
public:
	FbxModelResourceMgr();
	~FbxModelResourceMgr();

	RESOURCE_HANDLE LoadModel(const std::string &MODEL_NAME, bool REV_UV_FLAG = false);

	const std::shared_ptr<FbxResourceData> &GetResourceData(RESOURCE_HANDLE HANDLE);

	std::vector<FbxSkin *>boneSkinArray;
private:
	FbxManager *fbxManager;
	FbxImporter *fbxImporter;
	std::vector<FbxScene *>fbxScene;

	std::vector<std::shared_ptr<FbxResourceData>>modelResource;
	std::shared_ptr<FbxResourceData> errorResource;
	std::unique_ptr<HandleMaker> handle;


	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name="MODEL">読み込み先のモデルオブジェクト</param>
	/// <param name="FBX_NODE">解析対象のノード</param>
	void ParseNodeRecursive(Model *MODEL, FbxNode *FBX_NODE, Node *PARENT = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="MODEL">読み込み先モデルオブジェクト</param>
	/// <param name="FBX_NODE">解析対象のノード</param>
	void ParseMesh(Model *MODEL, FbxNode *FBX_NODE);

	//頂点座標読み取り
	void ParseMeshVertices(Model *MODEL, FbxMesh *FBX_MESH);
	//面情報読み取り
	void ParseMeshFaces(Model *MODEL, FbxMesh *FBX_MESH);
	//マテリアル読み取り
	void ParseMaterial(Model *MODEL, FbxNode *FBX_NODE);
	//テクスチャ読み取り
	void LoadTexture(Model *MODEL, const std::string &FULL_PATH);
	//スキニング情報の読み取り
	void ParseBone(Model *MODEL, FbxMesh *FBX_MESH);

	void ParseFaces(Model *MODEL, FbxMesh *FBX_MESH);

	DirectX::XMVECTOR FbxDoubleToXMVECTOR(const FbxDouble3 &DOUBLE_3);

	std::string ExtractFileName(const std::string &PATH);

	std::vector<std::string> handleName;


	bool revUvFlag;

	FbxMesh *mesh;


};

