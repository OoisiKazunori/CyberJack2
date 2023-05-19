#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"Loader/TextureResourceMgr.h"
#include"../Buffer/Polygon.h"
#include"../Helper/ISinglton.h"
#include<vector>
#include<fstream>
#include<sstream>

struct VertexData
{
	std::vector<KazMath::Vec3<float>> verticesArray;
	std::vector<KazMath::Vec2<float>> uvArray;
	std::vector<KazMath::Vec3<float>> normalArray;
	std::vector<USHORT>indexArray;
};

struct MaterialData
{
	KazMath::Vec3<float> ambient;//アンビエント
	KazMath::Vec3<float> diffuse;//ディフューズ
	KazMath::Vec3<float> specular;//スペキュラー
	KazBufferHelper::BufferData textureBuffer;
};

struct AnimationData
{
	//骨
	std::vector<DirectX::XMMATRIX> boneArray;
	//アニメーション時間
};

struct ModelData
{
	//頂点情報
	VertexData vertexData;
	//マテリアル情報
	MaterialData materialData;
	//ボーン情報
	AnimationData animationData;
};


/// <summary>
/// OBJモデルの読み込み
/// </summary>
class OBJLoader
{
public:
	OBJLoader()
	{};
	ModelData Load(std::ifstream &fileName, std::string fileDir);

private:
	std::vector<std::string> fileNameArray;

	struct LocalMateriaData
	{
		std::string name;//マテリアル名
		KazMath::Vec3<float> ambient;//アンビエント
		KazMath::Vec3<float> diffuse;//ディフューズ
		KazMath::Vec3<float> specular;//スペキュラー
		float alpha;//α
		std::string textureFilename;//テクスチャファイル名
		KazBufferHelper::BufferData textureBuffer;

		LocalMateriaData()
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

	LocalMateriaData LoadMaterial(const std::string &FILE_NAME, std::string MTL_RESOURE)
	{

		std::ifstream file;
		file.open(MTL_RESOURE);
		if (file.fail())
		{
			return LocalMateriaData();
		}

		LocalMateriaData materialLoadData;

		std::string textureFilePass;

		std::string line;
		while (getline(file, line))
		{
			std::istringstream line_stream(line);


			std::string key;
			getline(line_stream, key, ' ');

			//先頭のタブ文字は無視する
			if (key[0] == '\t')
			{
				key.erase(key.begin());
			}


			if (key == "newmtl")
			{
				line_stream >> materialLoadData.name;
			}
			if (key == "Ka")
			{
				line_stream >> materialLoadData.ambient.x;
				line_stream >> materialLoadData.ambient.y;
				line_stream >> materialLoadData.ambient.z;
			}
			if (key == "Kd")
			{
				line_stream >> materialLoadData.diffuse.x;
				line_stream >> materialLoadData.diffuse.y;
				line_stream >> materialLoadData.diffuse.z;
			}
			if (key == "Ks")
			{
				line_stream >> materialLoadData.specular.x;
				line_stream >> materialLoadData.specular.y;
				line_stream >> materialLoadData.specular.z;
			}
			if (key == "map_Kd")
			{
				line_stream >> materialLoadData.textureFilename;
			}
		}

		file.close();

		materialLoadData.textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(FILE_NAME + materialLoadData.textureFilename);
		return materialLoadData;
	}
};


struct ModelInfomation
{
	ModelData modelData;
	PolygonIndexData vertexBufferData;

	ModelInfomation(const ModelData &model, const PolygonIndexData &vertexBuffer) :modelData(model), vertexBufferData(vertexBuffer)
	{
	}
};

/// <summary>
/// モデルの読み込み
/// 現在OBJのみ対応
/// </summary>
class ModelLoader :public ISingleton<ModelLoader>
{
public:

	ModelLoader();
	std::shared_ptr<ModelInfomation> Load(std::string fileName);
	std::vector<Vertex>GetVertexDataArray(const VertexData &data);


private:

	enum class ModelFileType
	{
		NONE,
		OBJ,
		FBX,
		GLTF
	};

	OBJLoader objLoad;

	std::vector<std::shared_ptr<ModelInfomation>> m_modelArray;
	PolygonBuffer m_test;
	PolygonIndexData m_Poly;
};
