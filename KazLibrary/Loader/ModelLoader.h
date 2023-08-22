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
#include<map>

#include"Animation/AnimationData.h"

struct CoordinateSpaceMatData
{
	DirectX::XMMATRIX m_world;
	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_projective;
	DirectX::XMMATRIX m_rotaion;

	CoordinateSpaceMatData(DirectX::XMMATRIX arg_worldMat, DirectX::XMMATRIX arg_viewMat, DirectX::XMMATRIX arg_projectiveMat) :
		m_world(arg_worldMat), m_view(arg_viewMat), m_projective(arg_projectiveMat)
	{};
};

struct VertexBufferData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
};

struct VertexBufferAnimationData
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMINT4 boneNo;
	DirectX::XMFLOAT4 weight;
};

struct AnimationData
{
	//骨
	std::vector<DirectX::XMMATRIX> boneArray;
	//アニメーション時間
};

enum MaterialEnum
{
	MATERIAL_TEXTURE_NONE = -1,
	MATERIAL_TEXTURE_ALBEDO,
	MATERIAL_TEXTURE_NORMAL,
	MATERIAL_TEXTURE_METALNESS_ROUGHNESS,
	MATERIAL_TEXTURE_EMISSIVE,
	MATERIAL_TEXTURE_MAX,
};

class GLTFLoader
{
public:
	std::vector<ModelMeshData> Load(std::string fileName, std::string fileDir, Skeleton* skelton);


private:
	// Uses the Document class to print some basic information about various top-level glTF entities
	void PrintDocumentInfo(const Microsoft::glTF::Document& document);

	// Uses the Document and GLTFResourceReader classes to print information about various glTF binary resources
	void PrintResourceInfo(const Microsoft::glTF::Document& document, const Microsoft::glTF::GLTFResourceReader& resourceReader);

	void PrintInfo(const std::experimental::filesystem::path& path);

	KazMath::Vec3<int> GetVertIndex(int vertCount, int vecMaxNum)
	{
		//三角面になるようにインデックスを決める
		return KazMath::Vec3<int>(vecMaxNum * vertCount, vecMaxNum * vertCount + 1, vecMaxNum * vertCount + 2);
	}



	KazBufferHelper::BufferData LoadErrorTex(GraphicsRootParamType arg_type)
	{
		std::string errorFilePass("Resource/Error/MaterialErrorTex.png");
		KazBufferHelper::BufferData buffer(TextureResourceMgr::Instance()->LoadGraphBuffer(errorFilePass));
		buffer.rootParamType = arg_type;
		buffer.rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		return buffer;
	};

	void LoadMaterialTexture(MaterialData* arg_material, std::string arg_fileDir, std::string arg_id, const Microsoft::glTF::Document& arg_doc, GraphicsRootParamType arg_rootParam);
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
	std::shared_ptr<ModelInfomation> Load(std::string arg_fileDir, std::string arg_fileName);
	std::vector<VertexBufferData>GetVertexDataArray(const VertexData& data);
	std::vector<VertexBufferData>GetVertexDataArray(const VertexData& data, const std::vector<UINT>& indexArray);
	std::vector<VertexBufferAnimationData>GetVertexAnimationDataArray(const VertexData& data, const std::vector<UINT>& indexArray);


private:
	GLTFLoader glTFLoad;

	std::vector<std::shared_ptr<ModelInfomation>> m_modelArray;

	struct MeshVertex
	{
		std::vector<std::vector<VertexBufferData>>m_vertexDataArray;
		std::vector<std::vector<VertexBufferAnimationData>>m_vertexAnimationDataArray;
	};
	std::vector<MeshVertex>m_modelVertexDataArray;
	std::vector<std::string> m_modelNameArray;
};

class StreamReader : public Microsoft::glTF::IStreamReader
{
public:
	StreamReader(std::experimental::filesystem::path pathBase) : m_pathBase(std::move(pathBase)) { }

	std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override
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