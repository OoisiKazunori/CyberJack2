#include "ObjResourceMgr.h"
#include"../Loader/TextureResourceMgr.h"
#include"../Helper/KazHelper.h"

ObjResourceMgr::ObjResourceMgr()
{
	modelDataBuffers = std::make_unique<CreateGpuBuffer>();
}

ObjResourceMgr::~ObjResourceMgr()
{
}

RESOURCE_HANDLE ObjResourceMgr::LoadModel(std::string RESOURCE)
{
	//ファイル読み込み
	ifstream file;
	file.open(RESOURCE);
	if (file.fail())
	{
		string errorName = RESOURCE + "の読み込みに失敗しました\n";
		FailCheck(errorName);

		return -1;
	}

	//生成ずみかどうか
	for (int i = 0; i < name.size(); i++)
	{
		if (RESOURCE == name[i])
		{
			return static_cast<RESOURCE_HANDLE>(i);
		}
	}

	RESOURCE_HANDLE setHandle = handle.GetHandle();

	//名前の登録
	name.push_back(RESOURCE);
	resource.push_back({});

	vector<Vertex> vert;
	vector<USHORT> indi;
	vector<UINT> indiKeepData;
	vector<DirectX::XMFLOAT4>positions;
	vector<DirectX::XMFLOAT3>normals;
	vector<DirectX::XMFLOAT2>texcoords;

	unsigned int indexCountNum = 0;

	string filePass = RESOURCE;
	for (size_t i = RESOURCE.length() - 1; 0 < i; i--)
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
	string filename;

	//一行ずつ読み込む
	string line;
	while (getline(file, line))
	{
		//一行分の文字列をストリームに変換して解析しやすくなる
		istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;//ここでvかf等の判断をする
		getline(line_stream, key, ' ');

		if (key == "v") {
			//XYZ読み込み
			DirectX::XMFLOAT4 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			positions.emplace_back(position);
		}

		if (key == "f") {
			string index_string;
			int faceIndexCount = 0;
			while (getline(line_stream, index_string, ' ')) {
				//一行分の文字列をストリームに変換して解析しやすくなる
				istringstream index_stream(index_string);
				USHORT indexPos, indexNormal, indexTexcoord;

				index_stream >> indexPos;
				index_stream.seekg(1, ios_base::cur);
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);
				index_stream >> indexNormal;

				resource[resource.size() - 1].posKey.push_back(indexPos);

				/*
				一点につき頂点座標/テクスチャ座標/法線ベクトル
				*/
				vert.push_back({});
				vert[vert.size() - 1].pos = { positions[indexPos - 1].x,positions[indexPos - 1].y,positions[indexPos - 1].z };
				vert[vert.size() - 1].uv = texcoords[indexTexcoord - 1];
				vert[vert.size() - 1].normal = normals[indexNormal - 1];

				// インデックスデータの追加
				if (faceIndexCount >= 3) {
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
					indi.emplace_back(static_cast<USHORT>(indexCountNum - 1));
					indi.emplace_back(static_cast<USHORT>(indexCountNum));
					indi.emplace_back(static_cast<USHORT>(indexCountNum - 3));

					indiKeepData.emplace_back(static_cast<UINT>(indexCountNum - 1));
					indiKeepData.emplace_back(static_cast<UINT>(indexCountNum));
					indiKeepData.emplace_back(static_cast<UINT>(indexCountNum - 3));
				}
				else
				{
					indi.emplace_back(static_cast<USHORT>(indexCountNum));
					indiKeepData.emplace_back(static_cast<UINT>(indexCountNum));
				}


				faceIndexCount++;
				indexCountNum++;
			}
		}

		if (key == "vt") {
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

		if (key == "vn") {
			DirectX::XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			normals.emplace_back(normal);
		}

		if (key == "mtllib")
		{
			line_stream >> filename;
			resource[setHandle].mtlHanlde = LoadMaterial(filePass, &resource[setHandle], filePass + filename);
		}

	}
	file.close();

	//テクスチャ読み込み失敗
	if (resource[setHandle].mtlHanlde == -1)
	{
		string errorName = filename + "の画像読み込みに失敗しました\n";
		FailCheck(errorName);
		//handle.DeleteHandle(setHandle);
		//return -1;
	}


	/*
	頂点情報{座標,uv,法線}
	マテリアル
	*/
	if (vert.size() <= 0)
	{
		ErrorCheck(RESOURCE + "の読み込みに失敗しました\n");
		return -1;
	}
	if (indi.size() <= 0)
	{
		ErrorCheck(RESOURCE + "の読み込みに失敗しました\n");
		return -1;
	}

	SucceedCheck(RESOURCE + "の読み込みに成功しました\n");

	//頂点データとインデックスバッファの生成、転送---------------------------------------
	int vertByte = KazBufferHelper::GetBufferSize<int>(vert.size(), sizeof(Vertex));
	int indexByte = KazBufferHelper::GetBufferSize<int>(indi.size(), sizeof(unsigned short));

	RESOURCE_HANDLE vertHandle = modelDataBuffers->CreateBuffer
	(
		KazBufferHelper::SetVertexBufferData(vertByte)
	);
	RESOURCE_HANDLE indexHandle = modelDataBuffers->CreateBuffer
	(
		KazBufferHelper::SetIndexBufferData(indexByte)
	);
	modelDataBuffers->TransData
	(
		vertHandle,
		vert.data(),
		vertByte
	);
	modelDataBuffers->TransData
	(
		indexHandle,
		indi.data(),
		indexByte
	);
	//頂点データとインデックスバッファの生成、転送---------------------------------------


	//頂点バッファビューとインデックスバッファビューの設定---------------------------------------
	resource[setHandle].vertexBufferView = KazBufferHelper::SetVertexBufferView(modelDataBuffers->GetGpuAddress(vertHandle), vertByte, sizeof(vert[0]));
	resource[setHandle].indexBufferView = KazBufferHelper::SetIndexBufferView(modelDataBuffers->GetGpuAddress(indexHandle), indexByte);
	//頂点バッファビューとインデックスバッファビューの設定---------------------------------------


	resource[setHandle].indexNum = static_cast<UINT>(indi.size());
	resource[setHandle].vertices = positions;
	resource[setHandle].index = indiKeepData;

	return setHandle;
}

void ObjResourceMgr::Release(RESOURCE_HANDLE HANDLE)
{
	if (KazHelper::IsitInAnArray(HANDLE, resource.size()))
	{
		handle.DeleteHandle(HANDLE);
		name[HANDLE] = "noName";
		//resource[HANDLE].vert.shrink_to_fit();
		//resource[HANDLE].indexNum.shrink_to_fit();
		resource[HANDLE].mtlHanlde = -1;
		resource[HANDLE].material.Delete();
	}
}

ObjResourceData ObjResourceMgr::GetResourceData(RESOURCE_HANDLE HANDLE)
{
	if (KazHelper::IsitInAnArray(HANDLE, resource.size()))
	{
		return resource[HANDLE];
	}
	else
	{
		ObjResourceData errorResource;
		errorResource.mtlHanlde = -1;
		//resource.indexNum.resize(0);
		//resource.vert.resize(0);
		return errorResource;
	}
}

RESOURCE_HANDLE ObjResourceMgr::LoadMaterial(const std::string &FILE_NAME, ObjResourceData *RESOURCE, std::string MTL_RESOURE)
{

	ifstream file;
	file.open(MTL_RESOURE);
	if (file.fail())
	{
		return -1;
	}


	RESOURCE_HANDLE lHandle = -1;

	string line;
	while (getline(file, line))
	{
		istringstream line_stream(line);


		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視する
		if (key[0] == '\t')
		{
			key.erase(key.begin());
		}


		if (key == "newmtl")
		{
			line_stream >> RESOURCE->material.name;
		}
		if (key == "Ka")
		{
			line_stream >> RESOURCE->material.ambient.x;
			line_stream >> RESOURCE->material.ambient.y;
			line_stream >> RESOURCE->material.ambient.z;
		}
		if (key == "Kd")
		{
			line_stream >> RESOURCE->material.diffuse.x;
			line_stream >> RESOURCE->material.diffuse.y;
			line_stream >> RESOURCE->material.diffuse.z;
		}
		if (key == "Ks")
		{
			line_stream >> RESOURCE->material.specular.x;
			line_stream >> RESOURCE->material.specular.y;
			line_stream >> RESOURCE->material.specular.z;
		}
		if (key == "map_Kd")
		{
			line_stream >> RESOURCE->material.textureFilename;
			lHandle = TextureResourceMgr::Instance()->LoadGraph(FILE_NAME + RESOURCE->material.textureFilename);
		}
	}

	file.close();

	return lHandle;
}
