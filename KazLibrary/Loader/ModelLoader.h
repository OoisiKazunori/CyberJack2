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
#define TINYGLTF_NO_STB_IMAGE_WRITE  // �����o�����g�p���Ȃ����߁B
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


//�o�b�t�@�����p�̍\����
struct MaterialBufferData
{
	DirectX::XMFLOAT3 ambient;//�A���r�G���g
	float pad;
	DirectX::XMFLOAT3 diffuse;//�f�B�t���[�Y
	float pad2;
	DirectX::XMFLOAT3 specular;//�X�y�L�����[
	float alpha;
};


struct VertexData
{
	std::vector<KazMath::Vec3<float>> verticesArray;
	std::vector<KazMath::Vec2<float>> uvArray;
	std::vector<KazMath::Vec3<float>> normalArray;
	std::vector<USHORT>indexArray;
};

struct MaterialData
{
	KazMath::Vec3<float> ambient;//�A���r�G���g
	KazMath::Vec3<float> diffuse;//�f�B�t���[�Y
	KazMath::Vec3<float> specular;//�X�y�L�����[
	KazBufferHelper::BufferData textureBuffer;

	MaterialBufferData GetMaterialData();
};

struct AnimationData
{
	//��
	std::vector<DirectX::XMMATRIX> boneArray;
	//�A�j���[�V��������
};

struct ModelData
{
	//���_���
	VertexData vertexData;
	//�}�e���A�����
	MaterialData materialData;
	//�{�[�����
	AnimationData animationData;

};





/// <summary>
/// OBJ���f���̓ǂݍ���
/// </summary>
class OBJLoader
{
public:
	ModelData Load(std::ifstream &fileName, std::string fileDir);

private:
	std::vector<std::string> fileNameArray;

	struct LocalMateriaData
	{
		std::string name;//�}�e���A����
		KazMath::Vec3<float> ambient;//�A���r�G���g
		KazMath::Vec3<float> diffuse;//�f�B�t���[�Y
		KazMath::Vec3<float> specular;//�X�y�L�����[
		float alpha;//��
		std::string textureFilename;//�e�N�X�`���t�@�C����
		KazBufferHelper::BufferData textureBuffer;

		LocalMateriaData();

		void Delete();
	};

	LocalMateriaData LoadMaterial(const std::string &FILE_NAME, std::string MTL_RESOURE);
};


struct ModelInfomation
{
	ModelData modelData;
	PolygonIndexData vertexBufferData;

	ModelInfomation(const ModelData &model, const PolygonIndexData &vertexBuffer);
};


class GLTFLoader
{
public:
	void Load(std::ifstream &fileName, std::string fileDir);

};



/// <summary>
/// ���f���̓ǂݍ���
/// ����OBJ�̂ݑΉ�
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
	std::shared_ptr<ModelInfomation> Load(std::string fileName, ModelFileType type);
	std::vector<Vertex>GetVertexDataArray(const VertexData &data);


private:


	OBJLoader objLoad;
	GLTFLoader glTFLoad;

	std::vector<std::shared_ptr<ModelInfomation>> m_modelArray;
	PolygonBuffer m_test;
	PolygonIndexData m_Poly;
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