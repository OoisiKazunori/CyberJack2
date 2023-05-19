#include "ModelLoader.h"
#include"Helper/OutPutDebugStringAndCheckResult.h"

ModelLoader::ModelLoader()
{
}

std::shared_ptr<ModelInfomation> ModelLoader::Load(std::string fileName)
{
	//(フローチャート)
	//ファイルが存在するかどうか
	//ファイルの拡張子から何のモデルか読み込むか
	//読み込み処理(個別)
	//頂点情報生成
	//(問題点)
	//モデル読み込み時の自由な対応(頂点情報を自由に引き出したい)

	//ファイル読み込み
	std::ifstream file;
	file.open(fileName);
	if (file.fail())
	{
		std::string errorName = fileName + "の読み込みに失敗しました\n";
		FailCheck(errorName);
		static_assert(true);
	}

	//ファイルパス確認
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

	//生成されているか確認
	if (modelData.vertexData.verticesArray.size() <= 0 || modelData.vertexData.indexArray.size() <= 0)
	{
		ErrorCheck(fileName + "の読み込みに失敗しました\n");
		assert(1);
	}
	SucceedCheck(fileName + "の読み込みに成功しました\n");


	//頂点、インデックスバッファ生成
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
	//単体の情報の登録---------------------------------------
	std::vector<KazMath::Vec3<float>>positions;
	std::vector<KazMath::Vec3<float>>normals;
	std::vector<KazMath::Vec2<float>>texcoords;
	std::vector<USHORT> indexArray;
	//単体の情報の登録---------------------------------------

	//モデルの頂点の配列に合わせた情報の組み込み
	std::vector<KazMath::Vec3<float>>vertexPosArray;
	std::vector<KazMath::Vec3<float>>normalArray;
	std::vector<KazMath::Vec2<float>>texcoordArray;


	unsigned int indexCountNum = 0;

	LocalMateriaData materialData;

	//一行ずつ読み込む
	std::string line;
	while (getline(file, line))
	{
		//一行分の文字列をストリームに変換して解析しやすくなる
		std::stringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		std::string key;//ここでvかf等の判断をする
		getline(line_stream, key, ' ');

		//頂点読み込み
		if (key == "v")
		{
			//XYZ読み込み
			KazMath::Vec3<float> position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			positions.emplace_back(position);
		}

		//インデックス読み込み
		if (key == "f")
		{
			std::string index_string;
			int faceIndexCount = 0;


			while (getline(line_stream, index_string, ' '))
			{
				//一行分の文字列をストリームに変換して解析しやすくなる
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


				// インデックスデータの追加
				if (faceIndexCount >= 3) {
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
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

		//UV読み込み
		if (key == "vt")
		{
			KazMath::Vec2<float> texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			/*
			使用したモデリングツールによって、
			v方向の上下が逆の場合があり、
			ここでは読み込み処理で反転させて対応している
			*/
			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}

		//法線読み込み
		if (key == "vn")
		{
			KazMath::Vec3<float> normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			normals.emplace_back(normal);
		}

		//マテリアル情報読み込み
		if (key == "mtllib")
		{
			std::string filename;
			line_stream >> filename;
			materialData = LoadMaterial(fileDir, fileDir + filename);
		}

	}
	file.close();

	//テクスチャ読み込み失敗
	//if (resource[setHandle].mtlHanlde == -1)
	//{
	//	std::string errorName = filename + "の画像読み込みに失敗しました\n";
	//	FailCheck(errorName);
	//	//handle.DeleteHandle(setHandle);
	//	//return -1;
	//}

	/*
	一点につき頂点座標/テクスチャ座標/法線ベクトル
	*/
	/*vertexPosArray.push_back({});
	vertexPosArray[vertexPosArray.size() - 1].pos = { positions[indexPos - 1].x,positions[indexPos - 1].y,positions[indexPos - 1].z };
	vertexPosArray[vertexPosArray.size() - 1].uv = texcoords[indexTexcoord - 1];
	vertexPosArray[vertexPosArray.size() - 1].normal = normals[indexNormal - 1];*/


	////頂点データとインデックスバッファの生成、転送---------------------------------------
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
	////頂点データとインデックスバッファの生成、転送---------------------------------------


	////頂点バッファビューとインデックスバッファビューの設定---------------------------------------
	//resource[setHandle].vertexBufferView = KazBufferHelper::SetVertexBufferView(modelDataBuffers->GetGpuAddress(vertHandle), vertByte, sizeof(vertexPosArray[0]));
	//resource[setHandle].indexBufferView = KazBufferHelper::SetIndexBufferView(modelDataBuffers->GetGpuAddress(indexHandle), indexByte);
	////頂点バッファビューとインデックスバッファビューの設定---------------------------------------


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
