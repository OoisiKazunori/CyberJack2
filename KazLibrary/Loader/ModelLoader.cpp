#include "ModelLoader.h"
#include"Helper/OutPutDebugStringAndCheckResult.h"

ModelLoader::ModelLoader()
{
}

std::shared_ptr<ModelData> ModelLoader::Load()
{
	//(フローチャート)
	//ファイルが存在するかどうか
	//ファイルの拡張子から何のモデルか読み込むか
	//読み込み処理(個別)
	//頂点情報生成
	//(問題点)
	//モデル読み込み時の自由な対応(頂点情報を自由に引き出したい)

	ModelFileType type = ModelFileType::OBJ;
	switch (type)
	{
	case ModelLoader::ModelFileType::NONE:
		break;
	case ModelLoader::ModelFileType::OBJ:
		m_modelArray.emplace_back(std::make_shared<ModelData>(objLoad.Load("")));
		break;
	case ModelLoader::ModelFileType::FBX:
		break;
	case ModelLoader::ModelFileType::GLTF:
		break;
	default:
		break;
	}

	return m_modelArray.back();
}

ModelData OBJLoader::Load(std::string fileName)
{
	//ファイル読み込み
	std::ifstream file;
	file.open(fileName);
	if (file.fail())
	{
		std::string errorName = fileName + "の読み込みに失敗しました\n";
		FailCheck(errorName);
		static_assert(true);
	}

	//名前の登録
	std::vector<DirectX::XMFLOAT3>positions;
	std::vector<DirectX::XMFLOAT3>normals;
	std::vector<DirectX::XMFLOAT2>texcoords;
	std::vector<USHORT> indexArray;

	unsigned int indexCountNum = 0;

	LocalMateriaData materialData;

	std::string filePass = fileName;
	for (size_t i = fileName.length() - 1; 0 < i; i--)
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
	std::string filename;

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
			DirectX::XMFLOAT3 position{};
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
			DirectX::XMFLOAT2 texcoord{};
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
			DirectX::XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			normals.emplace_back(normal);
		}

		//マテリアル情報読み込み
		if (key == "mtllib")
		{
			line_stream >> filename;
			materialData = LoadMaterial(filePass, filePass + filename);
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


	if (positions.size() <= 0 || indexArray.size() <= 0)
	{
		ErrorCheck(fileName + "の読み込みに失敗しました\n");
		static_assert(true);
	}
	SucceedCheck(fileName + "の読み込みに成功しました\n");



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
	return loadData;
}
