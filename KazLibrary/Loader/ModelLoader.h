#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"Loader/TextureResourceMgr.h"
#include"../Buffer/Polygon.h"
#include"../Helper/ISinglton.h"
#include<vector>
#include<fstream>
#include<sstream>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE  // 書き出しを使用しないため。
#pragma push_macro("min")
#pragma push_macro("max")
#pragma push_macro("snprintf")
#undef min
#undef max
#undef snprintf
#pragma warning(push,0)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#pragma warning(push,0)
#pragma warning(disable : 4996)
#pragma warning(push,0)
#pragma warning(disable : 2039)
#include<GLTFSDK/GLTF.h>
#include<GLTFSDK/GLTFResourceReader.h>
#include<GLTFSDK/GLTFResourceWriter.h>
#include<GLTFSDK/Deserialize.h>
#include<GLTFSDK/GLBResourceReader.h>
#include<GLTFSDK/GLBResourceWriter.h>
#include<GLTFSDK/IStreamReader.h>
#pragma warning(pop)
#pragma warning(pop)
#pragma warning(pop)
#pragma pop_macro("min")
#pragma pop_macro("max")
#pragma pop_macro("snprintf")


#include <filesystem> // C++17 standard header file name
#include <fstream>
#include <iostream>
#if _MSC_VER > 1922 && !defined(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif

#include <experimental/filesystem>


//バッファ生成用の構造体
struct MaterialBufferData
{
	DirectX::XMFLOAT3 ambient;//アンビエント
	float pad;
	DirectX::XMFLOAT3 diffuse;//ディフューズ
	float pad2;
	DirectX::XMFLOAT3 specular;//スペキュラー
	float alpha;
};

struct VertexBufferData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
};

struct VertexData
{
	std::string name;
	std::vector<KazMath::Vec3<float>> verticesArray;
	std::vector<KazMath::Vec2<float>> uvArray;
	std::vector<KazMath::Vec3<float>> normalArray;
	std::vector<KazMath::Vec3<float>> tangentArray;
	std::vector<KazMath::Vec3<float>> binormalArray;
	std::vector<USHORT>indexArray;
};

struct MaterialData
{
	KazMath::Vec3<float> ambient;//アンビエント
	KazMath::Vec3<float> diffuse;//ディフューズ
	KazMath::Vec3<float> specular;//スペキュラー
	std::vector<KazBufferHelper::BufferData> textureBuffer;

	MaterialBufferData GetMaterialData();
};

struct AnimationData
{
	//骨
	std::vector<DirectX::XMMATRIX> boneArray;
	//アニメーション時間
};

struct ModelMeshData
{
	//頂点情報
	VertexData vertexData;
	//マテリアル情報
	MaterialData materialData;
};

enum MaterialEnum
{
	MATERIAL_TEXTURE_NONE = -1,
	MATERIAL_TEXTURE_ALBEDO,
	MATERIAL_TEXTURE_NORMAL,
	MATERIAL_TEXTURE_MAX,
};



/// <summary>
/// OBJモデルの読み込み
/// </summary>
class OBJLoader
{
public:
	ModelMeshData Load(std::ifstream &fileName, std::string fileDir);

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

		LocalMateriaData();

		void Delete();
	};

	LocalMateriaData LoadMaterial(const std::string &FILE_NAME, std::string MTL_RESOURE);
};


struct ModelInfomation
{
	std::vector<ModelMeshData> modelData;
	RESOURCE_HANDLE modelVertDataHandle;

	ModelInfomation(const std::vector<ModelMeshData> &model, RESOURCE_HANDLE vertHandle);
};


class GLTFLoader
{
public:
	std::vector<ModelMeshData> Load(std::ifstream &fileName, std::string fileDir);


private:
	// Uses the Document class to print some basic information about various top-level glTF entities
	void PrintDocumentInfo(const Microsoft::glTF::Document &document);

	// Uses the Document and GLTFResourceReader classes to print information about various glTF binary resources
	void PrintResourceInfo(const Microsoft::glTF::Document &document, const Microsoft::glTF::GLTFResourceReader &resourceReader);

	void PrintInfo(const std::experimental::filesystem::path &path);

	KazMath::Vec3<int> GetVertIndex(int vertCount, int vecMaxNum)
	{
		//三角面になるようにインデックスを決める
		return KazMath::Vec3<int>(vecMaxNum * vertCount, vecMaxNum * vertCount + 1, vecMaxNum * vertCount + 2);
	}
};



/// <summary>
/// モデルの読み込み
/// 現在OBJのみ対応
/// </summary>
class ModelLoader :public ISingleton<ModelLoader>
{
public:
	enum class ModelFileType
	{
		NONE,
		OBJ,
		FBX,
		GLTF
	};

	ModelLoader();
	std::shared_ptr<ModelInfomation> Load(std::string fileName);
	std::vector<VertexBufferData>GetVertexDataArray(const VertexData &data);
	std::vector<VertexBufferData>GetVertexDataArray(const VertexData &data, const std::vector<USHORT> &indexArray);


private:

	OBJLoader objLoad;
	GLTFLoader glTFLoad;

	std::vector<std::shared_ptr<ModelInfomation>> m_modelArray;
	std::vector<std::vector<VertexBufferData>>vertexDataArray;
};

class StreamReader : public Microsoft::glTF::IStreamReader
{
public:
	StreamReader(std::experimental::filesystem::path pathBase) : m_pathBase(std::move(pathBase)) { }

	std::shared_ptr<std::istream> GetInputStream(const std::string &filename) const override
	{
		auto streamPath = m_pathBase / std::experimental::filesystem::u8path(filename);
		auto stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);
		if (!stream || !(*stream))
		{
			throw std::runtime_error("Unable to create valid input stream.");
		}
		return stream;
	}

private:
	std::experimental::filesystem::path m_pathBase;
};