#include "ModelLoader.h"
#include"Helper/OutPutDebugStringAndCheckResult.h"
#include"../KazLibrary/Buffer/VertexBufferMgr.h"

ModelLoader::ModelLoader()
{
}

std::shared_ptr<ModelInfomation> ModelLoader::Load(std::string arg_fileDir, std::string arg_fileName)
{
	//多重ロード防止
	for (int i = 0; i < m_modelNameArray.size(); ++i)
	{
		if (m_modelNameArray[i] == arg_fileDir + arg_fileName)
		{
			return m_modelArray[i];
		}
	}


	std::vector<ModelMeshData> modelData;
	modelData = glTFLoad.Load(arg_fileName, arg_fileDir);

	//生成されているか確認
	if (modelData.back().vertexData.verticesArray.size() <= 0 || modelData.back().vertexData.indexArray.size() <= 0)
	{
		ErrorCheck(arg_fileDir + arg_fileName + "の読み込みに失敗しました\n");
		assert(1);
	}
	SucceedCheck(arg_fileDir + arg_fileName + "の読み込みに成功しました\n");

	//多重ロード防止用にモデルの名前登録
	m_modelNameArray.emplace_back(arg_fileDir + arg_fileName);


	std::vector<VertexGenerateData> vertArray;
	m_modelVertexDataArray.emplace_back();
	int dex = 0;
	for (const auto& meshData : modelData)
	{
		std::vector<VertexBufferData>vertexData = GetVertexDataArray(meshData.vertexData, meshData.vertexData.indexArray);
		m_modelVertexDataArray.back().m_vertexDataArray.emplace_back(vertexData);
		//頂点バッファ生成用の情報をスタックする。
		VertexGenerateData vertData(m_modelVertexDataArray.back().m_vertexDataArray[dex].data(), sizeof(VertexBufferData), vertexData.size(), sizeof(vertexData[0]), meshData.vertexData.indexArray);
		vertArray.emplace_back(vertData);
		++dex;
	}
	//頂点バッファ生成
	RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GenerateBuffer(vertArray);
	m_modelArray.emplace_back(std::make_shared<ModelInfomation>(modelData, handle));

	return m_modelArray.back();
}

std::vector<VertexBufferData> ModelLoader::GetVertexDataArray(const VertexData& data)
{
	std::vector<VertexBufferData>result(data.indexArray.size());

	for (int i = 0; i < result.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
		result[i].tangent = data.tangentArray[i].ConvertXMFLOAT3();
		result[i].binormal = data.binormalArray[i].ConvertXMFLOAT3();
	}
	return result;
}

std::vector<VertexBufferData> ModelLoader::GetVertexDataArray(const VertexData& data, const std::vector<UINT>& indexArray)
{
	std::vector<VertexBufferData>result(data.verticesArray.size());

	bool skipFlag = 0 < data.tangentArray.size();

	for (int i = 0; i < data.verticesArray.size(); ++i)
	{
		result[i].pos = data.verticesArray[i].ConvertXMFLOAT3();
		result[i].uv = data.uvArray[i].ConvertXMFLOAT2();
		result[i].normal = data.normalArray[i].ConvertXMFLOAT3();
		if (skipFlag)
		{
			result[i].tangent = data.tangentArray[i].ConvertXMFLOAT3();
			result[i].binormal = data.binormalArray[i].ConvertXMFLOAT3();
		}
		else
		{
			result[i].tangent = {};
			result[i].binormal = {};
		}
	}
	return result;
}

std::vector<ModelMeshData> GLTFLoader::Load(std::string fileName, std::string fileDir)
{
	//std::string filepass("Resource/Test/Plane/plane.glb");
	//std::string Ext(".glb");
	std::string FileDir(fileDir);
	std::string filepass(FileDir + fileName);
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

	auto MakePathExt = [](const std::string& ext)
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
	catch (const Microsoft::glTF::GLTFException& ex)
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
	std::vector<DirectX::XMMATRIX> worldMat;
	//ノードの読み込み
	for (const auto& gltfNode : doc.nodes.Elements())
	{
	}

	std::vector<MaterialData> modelMaterialDataArray;
	//マテリアル情報の読み込み
	for (const auto& material : doc.materials.Elements())
	{
		modelMaterialDataArray.emplace_back();

		auto texID = material.metallicRoughness.baseColorTexture.textureId;
		//テクスチャの取得
		if (!texID.empty())
		{
			auto& texture = doc.textures.Get(texID);
			auto& image = doc.images.Get(texture.imageId);
			if (!image.uri.empty())
			{
				std::string textureFilePass(FileDir + image.uri);
				//テクスチャ読み込み
				modelMaterialDataArray.back().textureBuffer.emplace_back(TextureResourceMgr::Instance()->LoadGraphBuffer(textureFilePass));
				modelMaterialDataArray.back().textureBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
			}
			else
			{
				modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(GRAPHICS_PRAMTYPE_DATA));
			}
		}
		else
		{
			modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(GRAPHICS_PRAMTYPE_DATA));
		}

		//法線マップの取得
		texID = material.normalTexture.textureId;
		if (!texID.empty())
		{
			auto& texture = doc.textures.Get(texID);
			auto& image = doc.images.Get(texture.imageId);
			if (!image.uri.empty())
			{
				std::string textureFilePass(FileDir + image.uri);
				//テクスチャ読み込み
				modelMaterialDataArray.back().textureBuffer.emplace_back(TextureResourceMgr::Instance()->LoadGraphBuffer(textureFilePass));
				modelMaterialDataArray.back().textureBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
			}
			//何もない場合は透明なテクスチャを送る
			else
			{
				modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(GRAPHICS_PRAMTYPE_DATA2));
			}
		}
		else
		{
			modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(GRAPHICS_PRAMTYPE_DATA2));
		}

		//メタルネスの取得
		texID = material.metallicRoughness.metallicRoughnessTexture.textureId;
		if (!texID.empty())
		{
			auto& texture = doc.textures.Get(texID);
			auto& image = doc.images.Get(texture.imageId);
			if (!image.uri.empty())
			{
				std::string textureFilePass(FileDir + image.uri);
				//テクスチャ読み込み
				modelMaterialDataArray.back().textureBuffer.emplace_back(TextureResourceMgr::Instance()->LoadGraphBuffer(textureFilePass));
				modelMaterialDataArray.back().textureBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
			}
			//何もない場合は透明なテクスチャを送る
			else
			{
				modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(GRAPHICS_PRAMTYPE_DATA3));
			}
		}
		else
		{
			modelMaterialDataArray.back().textureBuffer.emplace_back(LoadErrorTex(GRAPHICS_PRAMTYPE_DATA3));
		}
	}

	//モデル一つ分のメッシュの塊
	std::vector<ModelMeshData> meshData;
	//メッシュの読み込み
	for (const auto& meshes : doc.meshes.Elements())
	{
		//メッシュの名前を保存
		const auto meshName = meshes.name;

		//アクセサーの情報を保存(アクセサーはbufferViewがどのような振る舞いをするか定義されたもの、バイナリーの情報を読み取る為に必要)
		Microsoft::glTF::IndexedContainer<const Microsoft::glTF::Accessor> accsessor(doc.accessors);

		//プリミティブにはmeshのレンダリングに必要なジオメトリ情報を持つ
		for (const auto& primitive : meshes.primitives)
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
			auto& idPos = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION);
			auto& accPos = accsessor.Get(idPos);
			auto& vertPos = resourceReader->ReadBinaryData<float>(doc, accPos);
			//頂点情報の入手---------------------------------------

			//法線情報の入手---------------------------------------
			auto& idNormal = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_NORMAL);
			auto& accNormal = accsessor.Get(idNormal);
			auto& normal = resourceReader->ReadBinaryData<float>(doc, accNormal);
			//法線情報の入手---------------------------------------

			//UV座標情報の入手---------------------------------------
			auto& idUV = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TEXCOORD_0);
			auto& accUV = accsessor.Get(idUV);
			auto& uv = resourceReader->ReadBinaryData<float>(doc, accUV);
			//UV座標情報の入手---------------------------------------


			//上で手に入れた情報を元に一つのメッシュ情報を追加---------------------------------------

			const auto VERT_MAX_COUNT = accPos.count;
			for (int vertCount = 0; vertCount < VERT_MAX_COUNT; ++vertCount)
			{
				const int TRIANGLE_VERT_NUM = 3;
				const int UV_VERT_NUM = 2;

				//三角面になるようにインデックスを決める
				KazMath::Vec3<int>vertIndex(GetVertIndex(vertCount, TRIANGLE_VERT_NUM));
				KazMath::Vec3<int>uvIndex(GetVertIndex(vertCount, UV_VERT_NUM));

				vertexInfo.verticesArray.emplace_back(KazMath::Vec3<float>(vertPos[vertIndex.x], vertPos[vertIndex.y], vertPos[vertIndex.z]));
				vertexInfo.normalArray.emplace_back(KazMath::Vec3<float>(normal[vertIndex.x], normal[vertIndex.y], normal[vertIndex.z]));
				vertexInfo.uvArray.emplace_back(KazMath::Vec2<float>(uv[uvIndex.x], uv[uvIndex.y]));
			}


			//タンジェント空間の計算...ポリゴンの表面を中心に、UV.xの向き=+X軸、UV.yの向き=+Y軸、法線の向き＝+Z軸とした空間
			//(参考サイト)https://coposuke.hateblo.jp/entry/2020/12/21/144327)
			if (primitive.HasAttribute(Microsoft::glTF::ACCESSOR_TANGENT))
			{
				auto& tangentID = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TANGENT);
				auto& accTangent = accsessor.Get(tangentID);
				auto& vertTangent = resourceReader->ReadBinaryData<float>(doc, accTangent);

				for (int i = 0; i < vertexInfo.verticesArray.size(); ++i)
				{
					const int TRIANGLE_VERT_NUM = 3;
					KazMath::Vec3<int>vertIndex(GetVertIndex(i, TRIANGLE_VERT_NUM));
					//タンジェント空間のU(tangent)V(binormal)座標を得る。
					KazMath::Vec3<float>tangent(vertTangent[vertIndex.x], vertTangent[vertIndex.y], vertTangent[vertIndex.z]);
					//法線と接戦の外積から得られる従法線(binormal)を得る
					KazMath::Vec3<float>binNormal(vertexInfo.normalArray[i].Cross(tangent));
					vertexInfo.tangentArray.emplace_back(tangent);
					vertexInfo.binormalArray.emplace_back(binNormal);
				}
			}



			//アクセサーからインデックス情報の(???)を取得
			auto accIndex(accsessor.Get(primitive.indicesAccessorId));
			//インデックスデータを読み取る
			std::vector<uint16_t>indices = resourceReader->ReadBinaryData<uint16_t>(doc, accIndex);
			const auto INDEX_MAX_COUNT = accIndex.count;
			for (int indexCount = 0; indexCount < INDEX_MAX_COUNT; indexCount += 3)
			{
				vertexInfo.indexArray.emplace_back(static_cast<UINT>(indices[indexCount + 1]));
				vertexInfo.indexArray.emplace_back(static_cast<UINT>(indices[indexCount + 0]));
				vertexInfo.indexArray.emplace_back(static_cast<UINT>(indices[indexCount + 2]));
			}

			//メッシュ情報の追加
			vertexInfo.name = meshName;

			meshData.emplace_back();
			meshData.back().vertexData = vertexInfo;
			//マテリアルがあるのかどうか(現在はAlbedoのみ対応)
			if (doc.materials.Has(primitive.materialId))
			{
				int materialIndex = static_cast<int>(doc.materials.GetIndex(primitive.materialId));
				meshData.back().materialData = modelMaterialDataArray[materialIndex];
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

void GLTFLoader::PrintDocumentInfo(const Microsoft::glTF::Document& document)
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

	for (const auto& extension : document.extensionsUsed)
	{
		std::cout << "Extension Used: " << extension << "\n";
	}

	if (!document.extensionsUsed.empty())
	{
		std::cout << "\n";
	}

	for (const auto& extension : document.extensionsRequired)
	{
		std::cout << "Extension Required: " << extension << "\n";
	}

	if (!document.extensionsRequired.empty())
	{
		std::cout << "\n";
	}
}

void GLTFLoader::PrintResourceInfo(const Microsoft::glTF::Document& document, const Microsoft::glTF::GLTFResourceReader& resourceReader)
{
	// Use the resource reader to get each mesh primitive's position data
	for (const auto& mesh : document.meshes.Elements())
	{
		std::cout << "Mesh: " << mesh.id << "\n";

		for (const auto& meshPrimitive : mesh.primitives)
		{
			std::string accessorId;

			if (meshPrimitive.TryGetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION, accessorId))
			{
				const Microsoft::glTF::Accessor& accessor = document.accessors.Get(accessorId);

				const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
				const auto dataByteLength = data.size() * sizeof(float);

				std::cout << "MeshPrimitive: " << dataByteLength << " bytes of position data\n";
			}
		}

		std::cout << "\n";
	}

	// Use the resource reader to get each image's data
	for (const auto& image : document.images.Elements())
	{
		std::string filename;

		if (image.uri.empty())
		{
			assert(!image.bufferViewId.empty());

			auto& bufferView = document.bufferViews.Get(image.bufferViewId);
			auto& buffer = document.buffers.Get(bufferView.bufferId);

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

void GLTFLoader::PrintInfo(const std::experimental::filesystem::path& path)
{
	// Pass the absolute path, without the filename, to the stream reader
	auto streamReader = std::make_unique<StreamReader>(path.parent_path());

	std::experimental::filesystem::path pathFile = path.filename();
	std::experimental::filesystem::path pathFileExt = pathFile.extension();

	std::string manifest;

	auto MakePathExt = [](const std::string& ext)
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
	catch (const Microsoft::glTF::GLTFException& ex)
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

ModelInfomation::ModelInfomation(const std::vector<ModelMeshData>& model, RESOURCE_HANDLE vertHandle) :modelData(model), modelVertDataHandle(vertHandle)
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