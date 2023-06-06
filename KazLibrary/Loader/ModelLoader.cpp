#include "ModelLoader.h"
#include"Helper/OutPutDebugStringAndCheckResult.h"

ModelLoader::ModelLoader()
{
}

std::shared_ptr<ModelInfomation> ModelLoader::Load(std::string fileName, ModelFileType type)
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

	std::vector<ModelMeshData> modelData;

	switch (type)
	{
	case ModelLoader::ModelFileType::NONE:
		break;
	case ModelLoader::ModelFileType::OBJ:
		modelData.emplace_back(objLoad.Load(file, filePass));
		break;
	case ModelLoader::ModelFileType::FBX:
		break;
	case ModelLoader::ModelFileType::GLTF:
		modelData = glTFLoad.Load(file, filePass);
		break;
	default:
		break;
	}

	//生成されているか確認
	if (modelData.back().vertexData.verticesArray.size() <= 0 || modelData.back().vertexData.indexArray.size() <= 0)
	{
		ErrorCheck(fileName + "の読み込みに失敗しました\n");
		assert(1);
	}
	SucceedCheck(fileName + "の読み込みに成功しました\n");



	PolygonMultiMeshedIndexData data;
	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;

	for (const auto &meshData : modelData)
	{
		m_testMultiMeshed.emplace_back();


		std::vector<Vertex>vertexData = GetVertexDataArray(meshData.vertexData, meshData.vertexData.indexArray);

		std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(m_testMultiMeshed.back().GenerateVertexBuffer(vertexData.data(), sizeof(Vertex), vertexData.size()));
		std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(m_testMultiMeshed.back().GenerateIndexBuffer(meshData.vertexData.indexArray));

		m_PolyMultiMeshed.vertBuffer.emplace_back(vertexBuffer);
		m_PolyMultiMeshed.indexBuffer.emplace_back(indexBuffer);
		//頂点情報
		setVertDataArray.emplace_back();
		setVertDataArray.back().numViews = 1;
		setVertDataArray.back().slot = 0;
		setVertDataArray.back().vertexBufferView = KazBufferHelper::SetVertexBufferView(vertexBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(vertexData.size(), sizeof(Vertex)), sizeof(vertexData[0]));

		//インデックス情報
		indexBufferViewArray.emplace_back(
			KazBufferHelper::SetIndexBufferView(
				indexBuffer->bufferWrapper->GetGpuAddress(),
				KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.vertexData.indexArray.size(), sizeof(USHORT))
			)
		);

		//描画コマンド情報
		KazRenderHelper::DrawIndexedInstancedData result;
		result.indexCountPerInstance = static_cast<UINT>(meshData.vertexData.indexArray.size());
		result.instanceCount = 1;
		result.startIndexLocation = 0;
		result.baseVertexLocation = 0;
		result.startInstanceLocation = 0;
		drawCommandDataArray.emplace_back(result);

	}

	m_PolyMultiMeshed.index = KazRenderHelper::SetMultiMeshedDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		setVertDataArray,
		indexBufferViewArray,
		drawCommandDataArray
	);

	m_modelArray.emplace_back(std::make_shared<ModelInfomation>(modelData, m_PolyMultiMeshed));

	return m_modelArray.back();
}

std::vector<Vertex> ModelLoader::GetVertexDataArray(const VertexData &data)
{
	std::vector<Vertex>result(data.indexArray.size());

	for (int i = 0; i < result.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
	}
	return result;
}

std::vector<Vertex> ModelLoader::GetVertexDataArray(const VertexData &data, const std::vector<USHORT> &indexArray)
{
	std::vector<Vertex>result(data.verticesArray.size());
	for (int i = 0; i < data.verticesArray.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
	}
	return result;
}

ModelMeshData OBJLoader::Load(std::ifstream &file, std::string fileDir)
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

	ModelMeshData loadData;
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

OBJLoader::LocalMateriaData OBJLoader::LoadMaterial(const std::string &FILE_NAME, std::string MTL_RESOURE)
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

OBJLoader::LocalMateriaData::LocalMateriaData()
{
	ambient = { 0.3f,0.3f,0.3f };
	diffuse = { 0.0f,0.0f,0.0f };
	specular = { 0.0f,0.0f,0.0f };
	alpha = 1.0;
}

void OBJLoader::LocalMateriaData::Delete()
{
	ambient = { 0.0f,0.0f,0.0f };
	diffuse = { 0.0f,0.0f,0.0f };
	specular = { 0.0f,0.0f,0.0f };
	alpha = 0.0;
}

std::vector<ModelMeshData> GLTFLoader::Load(std::ifstream &fileName, std::string fileDir)
{
	//std::string filepass("Resource/Test/Plane/plane.glb");
	//std::string Ext(".glb");
	std::string FileDir("Resource/Test/glTF/");
	std::string filepass(FileDir + "sponza.gltf");
	std::string Ext(".gltf");


	//GLTFSDKから引用---------------------------------------
	//https://github.com/microsoft/glTF-SDK/blob/master/GLTFSDK.Samples/Deserialize/Source/main.cpp
	auto modelFilePath = std::experimental::filesystem::path(filepass);
	if (modelFilePath.is_relative())
	{
		auto current = std::experimental::filesystem::current_path();
		current /= modelFilePath;
		current.swap(modelFilePath);
	}
	auto reader = std::make_unique<StreamReader>(modelFilePath.parent_path());
	auto stream = reader->GetInputStream(modelFilePath.filename().string());

	std::string manifest;

	auto MakePathExt = [](const std::string &ext)
	{
		return "." + ext;
	};

	std::unique_ptr<Microsoft::glTF::GLTFResourceReader> resourceReader;

	if (Ext == MakePathExt(Microsoft::glTF::GLTF_EXTENSION))
	{
		auto gltfResourceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(reader));

		std::stringstream manifestStream;

		// Read the contents of the glTF file into a string using a std::stringstream
		manifestStream << stream->rdbuf();
		manifest = manifestStream.str();

		resourceReader = std::move(gltfResourceReader);
	}
	else if (Ext == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
	{
		auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(reader), std::move(stream));

		manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

		resourceReader = std::move(glbResourceReader);
	}

	assert(resourceReader);

	Microsoft::glTF::Document doc;
	try
	{
		doc = Microsoft::glTF::Deserialize(manifest);
	}
	catch (const Microsoft::glTF::GLTFException &ex)
	{
		std::stringstream ss;
		ss << "Microsoft::glTF::Deserialize failed: ";
		ss << ex.what();
		throw std::runtime_error(ss.str());
	}

	PrintDocumentInfo(doc);
	PrintResourceInfo(doc, *resourceReader);

	//https://github.com/microsoft/glTF-SDK/blob/master/GLTFSDK.Samples/Deserialize/Source/main.cpp
	//GLTFSDKから引用---------------------------------------


	std::vector<Microsoft::glTF::Node>node;
	//ノードの読み込み
	for (const auto &gltfNode : doc.nodes.Elements())
	{
		gltfNode.rotation;
		gltfNode.scale;
		gltfNode.translation;
		bool debug = false;
	}

	std::vector<MaterialData> modelMaterialDataArray;
	//マテリアル情報の読み込み
	for (const auto &material : doc.materials.Elements())
	{
		modelMaterialDataArray.emplace_back();

		auto texID = material.metallicRoughness.baseColorTexture.textureId;
		//テクスチャの取得
		if (!texID.empty())
		{
			auto &texture = doc.textures.Get(texID);
			auto &image = doc.images.Get(texture.imageId);
			if (!image.uri.empty())
			{
				std::string textureFilePass(FileDir + image.uri);
				//テクスチャ読み込み
				modelMaterialDataArray.back().textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(textureFilePass);
				modelMaterialDataArray.back().textureBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA;
			}
		}

		//法線マップの取得
		if (!texID.empty())
		{
		}
	}

	//モデル一つ分のメッシュの塊
	std::vector<ModelMeshData> meshData;
	//メッシュの読み込み
	for (const auto &meshes : doc.meshes.Elements())
	{
		//メッシュの名前を保存
		const auto meshName = meshes.name;

		//アクセサーの情報を保存(アクセサーはbufferViewがどのような振る舞いをするか定義されたもの、バイナリーの情報を読み取る為に必要)
		Microsoft::glTF::IndexedContainer<const Microsoft::glTF::Accessor> accsessor(doc.accessors);

		//プリミティブにはmeshのレンダリングに必要なジオメトリ情報を持つ
		for (const auto &primitive : meshes.primitives)
		{
			//メッシュ一個分の情報
			VertexData vertexInfo;
			/*
			1...Attribute(頂点情報)から引数に指定した名前を元にアクセサーを取り出す為のIDを手に入れる。
			2...IDからアクセサーを取り出す
			3...ファイルの情報とアクセサーからバイナリーに入っている指定の情報を取り出す。
			頂点、法線、UV等、上のやり方でデータを取り出す。
			アクセサーでbufferviewとその奥の階層にあるbufferの振る舞いを見る事が出来るので、バイナリー内の指定の情報を探索する事が出来る。
			*/
			//頂点情報の入手---------------------------------------
			auto &idPos = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION);
			auto &accPos = accsessor.Get(idPos);
			auto &vertPos = resourceReader->ReadBinaryData<float>(doc, accPos);
			//頂点情報の入手---------------------------------------

			//法線情報の入手---------------------------------------
			auto &idNormal = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_NORMAL);
			auto &accNormal = accsessor.Get(idNormal);
			auto &normal = resourceReader->ReadBinaryData<float>(doc, accNormal);
			//法線情報の入手---------------------------------------

			//UV座標情報の入手---------------------------------------
			auto &idUV = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TEXCOORD_0);
			auto &accUV = accsessor.Get(idUV);
			auto &uv = resourceReader->ReadBinaryData<float>(doc, accUV);
			//UV座標情報の入手---------------------------------------


			//上で手に入れた情報を元に一つのメッシュ情報を追加---------------------------------------

			const auto VERT_MAX_COUNT = accPos.count;
			for (int vertCount = 0; vertCount < VERT_MAX_COUNT; ++vertCount)
			{
				const int TRIANGLE_VERT_NUM = 3;
				const int UV_VERT_NUM = 2;

				//三角面になるようにインデックスを決める
				KazMath::Vec3<int>vertIndex(TRIANGLE_VERT_NUM * vertCount, TRIANGLE_VERT_NUM * vertCount + 1, TRIANGLE_VERT_NUM * vertCount + 2);
				KazMath::Vec2<int>uvIndex(UV_VERT_NUM * vertCount, UV_VERT_NUM * vertCount + 1);

				vertexInfo.verticesArray.emplace_back(KazMath::Vec3<float>(vertPos[vertIndex.x], vertPos[vertIndex.y], vertPos[vertIndex.z]));
				vertexInfo.normalArray.emplace_back(KazMath::Vec3<float>(normal[vertIndex.x], normal[vertIndex.y], normal[vertIndex.z]));
				vertexInfo.uvArray.emplace_back(KazMath::Vec2<float>(uv[uvIndex.x], uv[uvIndex.y]));
			}

			//アクセサーからインデックス情報の(???)を取得
			auto accIndex(accsessor.Get(primitive.indicesAccessorId));
			//インデックスデータを読み取る
			std::vector<uint16_t>indices = resourceReader->ReadBinaryData<uint16_t>(doc, accIndex);
			const auto INDEX_MAX_COUNT = accIndex.count;
			for (int indexCount = 0; indexCount < INDEX_MAX_COUNT; indexCount += 3)
			{
				vertexInfo.indexArray.emplace_back(static_cast<USHORT>(indices[indexCount + 1]));
				vertexInfo.indexArray.emplace_back(static_cast<USHORT>(indices[indexCount + 0]));
				vertexInfo.indexArray.emplace_back(static_cast<USHORT>(indices[indexCount + 2]));
			}

			//メッシュ情報の追加
			vertexInfo.name = meshName;

			meshData.emplace_back();
			meshData.back().vertexData = vertexInfo;
			//マテリアルがあるのかどうか(現在はAlbedoのみ対応)
			if (doc.materials.Has(primitive.materialId))
			{
				int materialIndex = static_cast<int>(doc.materials.GetIndex(primitive.materialId));
				meshData.back().materialData.textureBuffer = modelMaterialDataArray[materialIndex].textureBuffer;
			}
			else
			{
				bool debug = false;
			}

			//上で手に入れた情報を元に一つのメッシュ情報を追加---------------------------------------
		}
	}

	return meshData;
}

void GLTFLoader::PrintDocumentInfo(const Microsoft::glTF::Document &document)
{
	// Asset Info
	std::cout << "Asset Version:    " << document.asset.version << "\n";
	std::cout << "Asset MinVersion: " << document.asset.minVersion << "\n";
	std::cout << "Asset Generator:  " << document.asset.generator << "\n";
	std::cout << "Asset Copyright:  " << document.asset.copyright << "\n\n";

	// Scene Info
	std::cout << "Scene Count: " << document.scenes.Size() << "\n";

	if (document.scenes.Size() > 0U)
	{
		std::cout << "Default Scene Index: " << document.GetDefaultScene().id << "\n\n";
	}
	else
	{
		std::cout << "\n";
	}

	// Entity Info
	std::cout << "Node Count:     " << document.nodes.Size() << "\n";
	std::cout << "Camera Count:   " << document.cameras.Size() << "\n";
	std::cout << "Material Count: " << document.materials.Size() << "\n\n";

	// Mesh Info
	std::cout << "Mesh Count: " << document.meshes.Size() << "\n";
	std::cout << "Skin Count: " << document.skins.Size() << "\n\n";

	// Texture Info
	std::cout << "Image Count:   " << document.images.Size() << "\n";
	std::cout << "Texture Count: " << document.textures.Size() << "\n";
	std::cout << "Sampler Count: " << document.samplers.Size() << "\n\n";

	// Buffer Info
	std::cout << "Buffer Count:     " << document.buffers.Size() << "\n";
	std::cout << "BufferView Count: " << document.bufferViews.Size() << "\n";
	std::cout << "Accessor Count:   " << document.accessors.Size() << "\n\n";

	// Animation Info
	std::cout << "Animation Count: " << document.animations.Size() << "\n\n";

	for (const auto &extension : document.extensionsUsed)
	{
		std::cout << "Extension Used: " << extension << "\n";
	}

	if (!document.extensionsUsed.empty())
	{
		std::cout << "\n";
	}

	for (const auto &extension : document.extensionsRequired)
	{
		std::cout << "Extension Required: " << extension << "\n";
	}

	if (!document.extensionsRequired.empty())
	{
		std::cout << "\n";
	}
}

void GLTFLoader::PrintResourceInfo(const Microsoft::glTF::Document &document, const Microsoft::glTF::GLTFResourceReader &resourceReader)
{
	// Use the resource reader to get each mesh primitive's position data
	for (const auto &mesh : document.meshes.Elements())
	{
		std::cout << "Mesh: " << mesh.id << "\n";

		for (const auto &meshPrimitive : mesh.primitives)
		{
			std::string accessorId;

			if (meshPrimitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION, accessorId))
			{
				const Microsoft::glTF::Accessor &accessor = document.accessors.Get(accessorId);

				const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
				const auto dataByteLength = data.size() * sizeof(float);

				std::cout << "MeshPrimitive: " << dataByteLength << " bytes of position data\n";
			}
		}

		std::cout << "\n";
	}

	// Use the resource reader to get each image's data
	for (const auto &image : document.images.Elements())
	{
		std::string filename;

		if (image.uri.empty())
		{
			assert(!image.bufferViewId.empty());

			auto &bufferView = document.bufferViews.Get(image.bufferViewId);
			auto &buffer = document.buffers.Get(bufferView.bufferId);

			filename += buffer.uri; //NOTE: buffer uri is empty if image is stored in GLB binary chunk
		}
		else if (Microsoft::glTF::IsUriBase64(image.uri))
		{
			filename = "Data URI";
		}
		else
		{
			filename = image.uri;
		}

		auto data = resourceReader.ReadBinaryData(document, image);

		std::cout << "Image: " << image.id << "\n";
		std::cout << "Image: " << data.size() << " bytes of image data\n";

		if (!filename.empty())
		{
			std::cout << "Image filename: " << filename << "\n\n";
		}
	}
}

void GLTFLoader::PrintInfo(const std::experimental::filesystem::path &path)
{
	// Pass the absolute path, without the filename, to the stream reader
	auto streamReader = std::make_unique<StreamReader>(path.parent_path());

	std::experimental::filesystem::path pathFile = path.filename();
	std::experimental::filesystem::path pathFileExt = pathFile.extension();

	std::string manifest;

	auto MakePathExt = [](const std::string &ext)
	{
		return "." + ext;
	};

	std::unique_ptr<Microsoft::glTF::GLTFResourceReader> resourceReader;

	// If the file has a '.gltf' extension then create a GLTFResourceReader
	if (pathFileExt == MakePathExt(Microsoft::glTF::GLTF_EXTENSION))
	{
		auto gltfStream = streamReader->GetInputStream(pathFile.u8string()); // Pass a UTF-8 encoded filename to GetInputString
		auto gltfResourceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(streamReader));

		std::stringstream manifestStream;

		// Read the contents of the glTF file into a string using a std::stringstream
		manifestStream << gltfStream->rdbuf();
		manifest = manifestStream.str();

		resourceReader = std::move(gltfResourceReader);
	}

	// If the file has a '.glb' extension then create a GLBResourceReader. This class derives
	// from GLTFResourceReader and adds support for reading manifests from a GLB container's
	// JSON chunk and resource data from the binary chunk.
	if (pathFileExt == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
	{
		auto glbStream = streamReader->GetInputStream(pathFile.u8string()); // Pass a UTF-8 encoded filename to GetInputString
		auto glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(streamReader), std::move(glbStream));

		manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

		resourceReader = std::move(glbResourceReader);
	}

	if (!resourceReader)
	{
		throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
	}

	Microsoft::glTF::Document document;

	try
	{
		document = Microsoft::glTF::Deserialize(manifest);
	}
	catch (const Microsoft::glTF::GLTFException &ex)
	{
		std::stringstream ss;

		ss << "Microsoft::glTF::Deserialize failed: ";
		ss << ex.what();

		throw std::runtime_error(ss.str());
	}

	std::cout << "### glTF Info - " << pathFile << " ###\n\n";

	PrintDocumentInfo(document);
	PrintResourceInfo(document, *resourceReader);
}

ModelInfomation::ModelInfomation(const std::vector<ModelMeshData> &model, const PolygonMultiMeshedIndexData &vertexBuffer) :modelData(model), vertexBufferData(vertexBuffer)
{
}

MaterialBufferData MaterialData::GetMaterialData()
{
	MaterialBufferData material;
	material.diffuse = { diffuse.x,diffuse.y,diffuse.z };
	material.ambient = { ambient.x,ambient.y,ambient.z };
	material.specular = { specular.x,specular.y,specular.z };
	material.alpha = 1.0f;
	return material;
}