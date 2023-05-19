#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"Loader/TextureResourceMgr.h"
#include<vector>
#include<fstream>
#include<sstream>

struct VertexData
{
	std::vector<KazMath::Vec3<float>> verticesArray;
	std::vector<KazMath::Vec2<float>> uvArray;
	std::vector<KazMath::Vec3<float>> normalArray;
};

struct MaterialData
{
	KazMath::Vec3<float> ambient;//�A���r�G���g
	KazMath::Vec3<float> diffuse;//�f�B�t���[�Y
	KazMath::Vec3<float> specular;//�X�y�L�����[
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
	OBJLoader()
	{};
	ModelData Load(std::string fileName);

private:
	std::vector<std::string> fileNameArray;

	struct LocalMateriaData
	{
		std::string name;//�}�e���A����
		DirectX::XMFLOAT3 ambient;//�A���r�G���g
		DirectX::XMFLOAT3 diffuse;//�f�B�t���[�Y
		DirectX::XMFLOAT3 specular;//�X�y�L�����[
		float alpha;//��
		std::string textureFilename;//�e�N�X�`���t�@�C����

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

		RESOURCE_HANDLE lHandle = -1;

		std::string line;
		while (getline(file, line))
		{
			std::istringstream line_stream(line);


			std::string key;
			getline(line_stream, key, ' ');

			//�擪�̃^�u�����͖�������
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
				lHandle = TextureResourceMgr::Instance()->LoadGraph(FILE_NAME + materialLoadData.textureFilename);
			}
		}

		file.close();

		return materialLoadData;
	}
};



/// <summary>
/// ���f���̓ǂݍ���
/// ����OBJ�̂ݑΉ�
/// </summary>
class ModelLoader
{
public:

	ModelLoader();
	std::shared_ptr<ModelData> Load();


private:

	enum class ModelFileType
	{
		NONE,
		OBJ,
		FBX,
		GLTF
	};

	OBJLoader objLoad;

	std::vector<std::shared_ptr<ModelData>> m_modelArray;
};
