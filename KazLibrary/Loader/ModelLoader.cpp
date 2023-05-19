#include "ModelLoader.h"
#include"Helper/OutPutDebugStringAndCheckResult.h"

ModelLoader::ModelLoader()
{
}

std::shared_ptr<ModelInfomation> ModelLoader::Load(std::string fileName)
{
	//(�t���[�`���[�g)
	//�t�@�C�������݂��邩�ǂ���
	//�t�@�C���̊g���q���牽�̃��f�����ǂݍ��ނ�
	//�ǂݍ��ݏ���(��)
	//���_��񐶐�
	//(���_)
	//���f���ǂݍ��ݎ��̎��R�ȑΉ�(���_�������R�Ɉ����o������)

	//�t�@�C���ǂݍ���
	std::ifstream file;
	file.open(fileName);
	if (file.fail())
	{
		std::string errorName = fileName + "�̓ǂݍ��݂Ɏ��s���܂���\n";
		FailCheck(errorName);
		static_assert(true);
	}

	//�t�@�C���p�X�m�F
	string filePass = fileName;
	for (size_t i = filePass.length() - 1; 0 < i; i--)
	{
		if (filePass[i] == '/')
		{
			break;
		}
		else
		{
			filePass.pop_back();
		}
	}

	ModelData modelData;
	ModelFileType type = ModelFileType::OBJ;
	switch (type)
	{
	case ModelLoader::ModelFileType::NONE:
		break;
	case ModelLoader::ModelFileType::OBJ:
		modelData = objLoad.Load(file, filePass);
		break;
	case ModelLoader::ModelFileType::FBX:
		break;
	case ModelLoader::ModelFileType::GLTF:
		break;
	default:
		break;
	}

	//��������Ă��邩�m�F
	if (modelData.vertexData.verticesArray.size() <= 0 || modelData.vertexData.indexArray.size() <= 0)
	{
		ErrorCheck(fileName + "�̓ǂݍ��݂Ɏ��s���܂���\n");
		assert(1);
	}
	SucceedCheck(fileName + "�̓ǂݍ��݂ɐ������܂���\n");


	//���_�A�C���f�b�N�X�o�b�t�@����
	std::vector<Vertex>vertexData = GetVertexDataArray(modelData.vertexData);
	m_Poly.vertBuffer = m_test.GenerateVertexBuffer(vertexData.data(), sizeof(Vertex), vertexData.size());
	m_Poly.indexBuffer = m_test.GenerateIndexBuffer(modelData.vertexData.indexArray);
	m_Poly.index = KazRenderHelper::SetDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(m_Poly.vertBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(vertexData.size(), sizeof(Vertex)), sizeof(vertexData[0])),
		KazBufferHelper::SetIndexBufferView(m_Poly.indexBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(modelData.vertexData.indexArray.size(), sizeof(USHORT))),
		static_cast<UINT>(modelData.vertexData.indexArray.size()),
		1
	);

	m_modelArray.emplace_back(std::make_shared<ModelInfomation>(modelData, m_Poly));

	return m_modelArray.back();
}

std::vector<Vertex> ModelLoader::GetVertexDataArray(const VertexData &data)
{
	std::vector<Vertex>result(data.indexArray.size());

	for (int i = 0; i < result.size(); ++i)
	{
		USHORT index = data.indexArray[i];
		result[index].pos = data.verticesArray[index].ConvertXMFLOAT3();
		result[index].uv = data.uvArray[index].ConvertXMFLOAT2();
		result[index].normal = data.normalArray[index].ConvertXMFLOAT3();
	}

	return result;
}

ModelData OBJLoader::Load(std::ifstream &file, std::string fileDir)
{
	//�P�̂̏��̓o�^---------------------------------------
	std::vector<KazMath::Vec3<float>>positions;
	std::vector<KazMath::Vec3<float>>normals;
	std::vector<KazMath::Vec2<float>>texcoords;
	std::vector<USHORT> indexArray;
	//�P�̂̏��̓o�^---------------------------------------

	//���f���̒��_�̔z��ɍ��킹�����̑g�ݍ���
	std::vector<KazMath::Vec3<float>>vertexPosArray;
	std::vector<KazMath::Vec3<float>>normalArray;
	std::vector<KazMath::Vec2<float>>texcoordArray;


	unsigned int indexCountNum = 0;

	LocalMateriaData materialData;

	//��s���ǂݍ���
	std::string line;
	while (getline(file, line))
	{
		//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷���Ȃ�
		std::stringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		std::string key;//������v��f���̔��f������
		getline(line_stream, key, ' ');

		//���_�ǂݍ���
		if (key == "v")
		{
			//XYZ�ǂݍ���
			KazMath::Vec3<float> position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			positions.emplace_back(position);
		}

		//�C���f�b�N�X�ǂݍ���
		if (key == "f")
		{
			std::string index_string;
			int faceIndexCount = 0;


			while (getline(line_stream, index_string, ' '))
			{
				//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷���Ȃ�
				std::stringstream index_stream(index_string);
				USHORT indexPos, indexNormal, indexTexcoord;

				index_stream >> indexPos;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);
				index_stream >> indexNormal;


				vertexPosArray.emplace_back(positions[indexPos - 1]);
				texcoordArray.emplace_back(texcoords[indexTexcoord - 1]);
				normalArray.emplace_back(normals[indexNormal - 1]);


				// �C���f�b�N�X�f�[�^�̒ǉ�
				if (faceIndexCount >= 3) {
					// �l�p�`�|���S����4�_�ڂȂ̂ŁA
					// �l�p�`��0,1,2,3�̓� 2,3,0�ŎO�p�`���\�z����
					indexArray.emplace_back(static_cast<USHORT>(indexCountNum - 1));
					indexArray.emplace_back(static_cast<USHORT>(indexCountNum));
					indexArray.emplace_back(static_cast<USHORT>(indexCountNum - 3));
				}
				else
				{
					indexArray.emplace_back(static_cast<USHORT>(indexCountNum));
				}


				faceIndexCount++;
				indexCountNum++;
			}
		}

		//UV�ǂݍ���
		if (key == "vt")
		{
			KazMath::Vec2<float> texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			/*
			�g�p�������f�����O�c�[���ɂ���āA
			v�����̏㉺���t�̏ꍇ������A
			�����ł͓ǂݍ��ݏ����Ŕ��]�����đΉ����Ă���
			*/
			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}

		//�@���ǂݍ���
		if (key == "vn")
		{
			KazMath::Vec3<float> normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			normals.emplace_back(normal);
		}

		//�}�e���A�����ǂݍ���
		if (key == "mtllib")
		{
			std::string filename;
			line_stream >> filename;
			materialData = LoadMaterial(fileDir, fileDir + filename);
		}

	}
	file.close();

	//�e�N�X�`���ǂݍ��ݎ��s
	//if (resource[setHandle].mtlHanlde == -1)
	//{
	//	std::string errorName = filename + "�̉摜�ǂݍ��݂Ɏ��s���܂���\n";
	//	FailCheck(errorName);
	//	//handle.DeleteHandle(setHandle);
	//	//return -1;
	//}

	/*
	��_�ɂ����_���W/�e�N�X�`�����W/�@���x�N�g��
	*/
	/*vertexPosArray.push_back({});
	vertexPosArray[vertexPosArray.size() - 1].pos = { positions[indexPos - 1].x,positions[indexPos - 1].y,positions[indexPos - 1].z };
	vertexPosArray[vertexPosArray.size() - 1].uv = texcoords[indexTexcoord - 1];
	vertexPosArray[vertexPosArray.size() - 1].normal = normals[indexNormal - 1];*/


	////���_�f�[�^�ƃC���f�b�N�X�o�b�t�@�̐����A�]��---------------------------------------
	//int vertByte = KazBufferHelper::GetBufferSize<int>(vertexPosArray.size(), sizeof(Vertex));
	//int indexByte = KazBufferHelper::GetBufferSize<int>(indexArray.size(), sizeof(unsigned short));

	//RESOURCE_HANDLE vertHandle = modelDataBuffers->CreateBuffer
	//(
	//	KazBufferHelper::SetVertexBufferData(vertByte)
	//);
	//RESOURCE_HANDLE indexHandle = modelDataBuffers->CreateBuffer
	//(
	//	KazBufferHelper::SetIndexBufferData(indexByte)
	//);
	//modelDataBuffers->TransData
	//(
	//	vertHandle,
	//	vertexPosArray.data(),
	//	vertByte
	//);
	//modelDataBuffers->TransData
	//(
	//	indexHandle,
	//	indexArray.data(),
	//	indexByte
	//);
	////���_�f�[�^�ƃC���f�b�N�X�o�b�t�@�̐����A�]��---------------------------------------


	////���_�o�b�t�@�r���[�ƃC���f�b�N�X�o�b�t�@�r���[�̐ݒ�---------------------------------------
	//resource[setHandle].vertexBufferView = KazBufferHelper::SetVertexBufferView(modelDataBuffers->GetGpuAddress(vertHandle), vertByte, sizeof(vertexPosArray[0]));
	//resource[setHandle].indexBufferView = KazBufferHelper::SetIndexBufferView(modelDataBuffers->GetGpuAddress(indexHandle), indexByte);
	////���_�o�b�t�@�r���[�ƃC���f�b�N�X�o�b�t�@�r���[�̐ݒ�---------------------------------------


	//resource[setHandle].indexNum = static_cast<UINT>(indexArray.size());
	//resource[setHandle].vertices = positions;
	//resource[setHandle].index = indiKeepData;

	ModelData loadData;
	loadData.vertexData.verticesArray = vertexPosArray;
	loadData.vertexData.uvArray = texcoordArray;
	loadData.vertexData.normalArray = normalArray;
	loadData.vertexData.indexArray = indexArray;

	loadData.materialData.ambient = materialData.ambient;
	loadData.materialData.diffuse = materialData.diffuse;
	loadData.materialData.specular = materialData.specular;
	loadData.materialData.textureBuffer = materialData.textureBuffer;

	return loadData;
}
