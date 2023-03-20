#include "FbxModelResourceMgr.h"
#include"../Loader/TextureResourceMgr.h"
#include"../Helper/KazHelper.h"
#include"../Helper/ResourceFilePass.h"

FbxModelResourceMgr::FbxModelResourceMgr()
{
	fbxManager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);



	fbxImporter = FbxImporter::Create(fbxManager, "");
	handle = std::make_unique<HandleMaker>();

	errorResource = std::make_unique<FbxResourceData>();
}

FbxModelResourceMgr::~FbxModelResourceMgr()
{
	fbxImporter->Destroy();
	for (int i = 0; i < fbxScene.size(); ++i)
	{
		fbxScene[i]->Destroy();
	}
	fbxManager->Destroy();

	modelResource.shrink_to_fit();
	modelResource.clear();
}

RESOURCE_HANDLE FbxModelResourceMgr::LoadModel(const std::string &MODEL_NAME, bool REV_UV_FLAG)
{
	for (int i = 0; i < handleName.size(); i++)
	{
		if (handleName[i] == MODEL_NAME)
		{
			return i;
		}
	}
	revUvFlag = REV_UV_FLAG;

	fbxScene.push_back(FbxScene::Create(fbxManager, "fbxScene"));

	fbxImporter->Initialize(MODEL_NAME.c_str());
	fbxImporter->Import(fbxScene[fbxScene.size() - 1]);

	FbxGeometryConverter converter(fbxManager);
	// ポリゴンを三角形にする
	converter.Triangulate(fbxScene[fbxScene.size() - 1], true);

	//モデルの生成
	Model *model = new Model;
	model->name = MODEL_NAME;

	//Fbxノードの数を取得
	int nodeCount = fbxScene[fbxScene.size() - 1]->GetNodeCount();

	//あらかじめ必要数分のメモリを確保する事でアドレスがずれるのを予防する
	model->nodes.reserve(nodeCount);
	//ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(model, fbxScene[fbxScene.size() - 1]->GetRootNode());


	if (model->vertices.size() == 0)
	{
		string errorName = MODEL_NAME + "の読み込みに失敗しました\n";
		FailCheck(errorName);
		return -1;
	}

	//リソースに保管
	//unsigned int vertByte = KazBufferHelper::GetBufferSize<unsigned short>(model->vertices.size(), sizeof(Model::VertexPosNormalUvSkin));
	int vertByte = static_cast<int>(model->vertices.size()) * static_cast<int>(sizeof(Model::VertexPosNormalUvSkin));
	//unsigned short indexByte = KazBufferHelper::GetBufferSize<unsigned short>(model->indices.size(), sizeof(unsigned short));


	RESOURCE_HANDLE lHandle = handle->GetHandle();

	modelResource.push_back(std::make_shared<FbxResourceData>());
	modelResource[lHandle]->buffers = std::make_unique<CreateGpuBuffer>();
	RESOURCE_HANDLE vertBuffetHandle = modelResource[lHandle]->buffers->CreateBuffer(KazBufferHelper::SetVertexBufferData(vertByte));
	//RESOURCE_HANDLE indexBufferHandle = modelResource[lHandle]->buffers->CreateBuffer(KazBufferHelper::SetIndexBufferData(indexByte));

	//バッファ転送-----------------------------------------------------------------------------------------------------
	modelResource[lHandle]->buffers->TransData(vertBuffetHandle, model->vertices.data(), vertByte);
	//modelResource[lHandle]->buffers->TransData(indexBufferHandle, model->indices.data(), indexByte);
	//バッファ転送-----------------------------------------------------------------------------------------------------

	//バッファビュー設定-----------------------------------------------------------------------------------------------------
	modelResource[lHandle]->vertexBufferView = KazBufferHelper::SetVertexBufferView(modelResource[lHandle]->buffers->GetGpuAddress(vertBuffetHandle), vertByte, sizeof(model->vertices[0]));
	//modelResource[lHandle]->indexBufferView = KazBufferHelper::SetIndexBufferView(modelResource[lHandle]->buffers->GetGpuAddress(indexBufferHandle), indexByte);
	//バッファビュー設定-----------------------------------------------------------------------------------------------------

	modelResource[lHandle]->textureHandle = model->textureHandle;

	modelResource[lHandle]->diffuse = model->diffuse;
	modelResource[lHandle]->ambient = model->ambient;

	modelResource[lHandle]->indicisNum = static_cast<UINT>(model->indices.size());

	modelResource[lHandle]->bone = model->bones;

	modelResource[lHandle]->vertNum = static_cast<UINT>(model->vertices.size());

	modelResource[lHandle]->indexData = model->indices;
	modelResource[lHandle]->vertFloat4Data = model->vertFloat4Data;
	modelResource[lHandle]->vertFloat3Data = model->vertFloat3Data;
	modelResource[lHandle]->vertUvData = model->vertUvData;
	modelResource[lHandle]->faceCountNum = model->faceCountNum;

	modelResource[lHandle]->uvData = model->uvData;
	modelResource[lHandle]->normalData = model->normalData;


	modelResource[modelResource.size() - 1]->mesh = mesh;

	//アニメーションの数を取得
	int animaStackCount = fbxImporter->GetAnimStackCount();
	for (int i = 0; i < animaStackCount; i++)
	{
		FbxAnimStack *animStack = nullptr;
		animStack = fbxScene[fbxScene.size() - 1]->GetSrcObject<FbxAnimStack>(i);

		const char *animStackName = animStack->GetName();
		FbxTakeInfo *takeInfo = fbxScene[fbxScene.size() - 1]->GetTakeInfo(animStackName);
		fbxScene[fbxScene.size() - 1]->SetCurrentAnimationStack(animStack);

		modelResource[lHandle]->startTime.push_back(takeInfo->mLocalTimeSpan.GetStart());
		modelResource[lHandle]->endTime.push_back(takeInfo->mLocalTimeSpan.GetStop());
	}


	handleName.push_back(MODEL_NAME);

	SucceedCheck(MODEL_NAME + "の読み込みに成功しました\n");

	delete model;
	return lHandle;
}

const std::shared_ptr<FbxResourceData> &FbxModelResourceMgr::GetResourceData(RESOURCE_HANDLE HANDLE)
{
	if (KazHelper::IsitInAnArray(HANDLE, modelResource.size()))
	{
		return modelResource[HANDLE];
	}
	else
	{
		return errorResource;
	}
}

void FbxModelResourceMgr::ParseNodeRecursive(Model *MODEL, FbxNode *FBX_NODE, Node *PARENT)
{
	//モデルにノードを追加
	MODEL->nodes.emplace_back();
	Node &node = MODEL->nodes.back();
	//ノード名を取得
	std::string name = FBX_NODE->GetName();

	//Fbxノードのローカル移動情報
	FbxDouble3 rotation = FBX_NODE->LclRotation.Get();
	FbxDouble3 scaling = FBX_NODE->LclScaling.Get();
	FbxDouble3 translation = FBX_NODE->LclTranslation.Get();

	//形式変換して代入
	node.rotation = FbxDoubleToXMVECTOR(rotation);
	node.scaling = FbxDoubleToXMVECTOR(scaling);
	node.translation = FbxDoubleToXMVECTOR(translation);

	//スケール、回転、平行移動の計算
	DirectX::XMMATRIX matScaling, matRotatin, matTranslation;
	matScaling = DirectX::XMMatrixScalingFromVector(node.scaling);
	matRotatin = DirectX::XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTranslation = DirectX::XMMatrixTranslationFromVector(node.translation);

	//ローカル変換行列の計算
	node.transform = DirectX::XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotatin;
	node.transform *= matTranslation;

	//グローバル変換行列の計算
	node.globalTransform = node.transform;
	if (PARENT)
	{
		node.parent = PARENT;
		//親の変形を乗算
		node.globalTransform *= PARENT->globalTransform;
	}

	//FBXノードの情報を解析してノードに記録
	//FBXノードのメッシュ情報を解析
	FbxNodeAttribute *fbxNodeAttribute = FBX_NODE->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			MODEL->meshNode = &node;
			mesh = FBX_NODE->GetMesh();
			ParseMesh(MODEL, FBX_NODE);
		}
	}

	//子ノードに対して再帰呼び出し
	for (int i = 0; i < FBX_NODE->GetChildCount(); i++)
	{
		ParseNodeRecursive(MODEL, FBX_NODE->GetChild(i), &node);
	}

}

void FbxModelResourceMgr::ParseMesh(Model *MODEL, FbxNode *FBX_NODE)
{
	//ノードのメッシュを取得
	FbxMesh *fbxMesh = FBX_NODE->GetMesh();

	//頂点座標読み取り
	//ParseMeshVertices(MODEL, fbxMesh);
	//面を構成するデータの読み取り
	//ParseMeshFaces(MODEL, fbxMesh);
	ParseFaces(MODEL, fbxMesh);
	//マテリアルの読み取り
	ParseMaterial(MODEL, FBX_NODE);
	//ボーンの読み取り
	ParseBone(MODEL, fbxMesh);
}

void FbxModelResourceMgr::ParseMeshVertices(Model *MODEL, FbxMesh *FBX_MESH)
{
	auto &vertices = MODEL->vertices;

	//頂点座標データの数
	const int controlPointsCount = FBX_MESH->GetControlPointsCount();

	//必要数だけ頂点データ配列を確保
	Model::VertexPosNormalUvSkin vert{};
	MODEL->vertices.resize(controlPointsCount, vert);

	//FBXメッシュの頂点座標配列を取得
	FbxVector4 *pCoord = FBX_MESH->GetControlPoints();

	for (int i = 0; i < controlPointsCount; i++)
	{
		Model::VertexPosNormalUvSkin &vertex = vertices[i];

		//座標のコピー
		vertex.pos.x = (float)pCoord[i][0];
		vertex.pos.y = (float)pCoord[i][1];
		vertex.pos.z = (float)pCoord[i][2];
	}
}

void FbxModelResourceMgr::ParseMeshFaces(Model *MODEL, FbxMesh *FBX_MESH)
{
	auto &vertices = MODEL->vertices;
	auto &indices = MODEL->indices;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);
	//面の数
	const int polygonCount = FBX_MESH->GetPolygonCount();
	//UVデータの数
	const int textureUVCount = FBX_MESH->GetTextureUVCount();

	//UV名リスト
	FbxStringList uvNames;
	FBX_MESH->GetUVSetNames(uvNames);

	//面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++)
	{
		//面を構成する頂点の数を取得(3なら三角形ポリゴン)
		const int polygonSize = FBX_MESH->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1頂点ずつ処理
		for (int j = 0; j < polygonSize; j++)
		{
			int index = FBX_MESH->GetPolygonVertex(i, j);
			assert(index >= 0);

			//頂点法線読み込み
			Model::VertexPosNormalUvSkin &vertex = vertices[index];
			FbxVector4 normal;
			if (FBX_MESH->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			//テクスチャUV読み込み
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;
				//0番決め打ちで読み込み
				if (FBX_MESH->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = static_cast<float>(uvs[0]);
					vertex.uv.y = static_cast<float>(uvs[1]);
				}
			}

			//インデックス配列に頂点インデックス追加
			//3頂点目までなら
			if (j < 3)
			{
				//1点追加し、他の2点と三角形を構築する
				indices.push_back(static_cast<UINT>(index));
			}
			//4頂点目
			else
			{
				//3点追加し、
				//四角形の0,1,2,3の内,2,3,0で三角形を構築する
				UINT index2 = indices[indices.size() - 1];
				UINT index3 = static_cast<UINT>(index);
				UINT index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}
		}
	}

}

void FbxModelResourceMgr::ParseMaterial(Model *MODEL, FbxNode *FBX_NODE)
{
	const int materialCount = FBX_NODE->GetMaterialCount();
	if (materialCount > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial *material = FBX_NODE->GetMaterial(0);
		//テクスチャ読み込みしたかどうか
		bool textureLoaded = false;

		if (material)
		{
			//FbxSurfaceLamvertクラスかどうかを調べる
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert *lambert = static_cast<FbxSurfaceLambert *>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				MODEL->ambient.x = (float)ambient.Get()[0];
				MODEL->ambient.y = (float)ambient.Get()[1];
				MODEL->ambient.z = (float)ambient.Get()[2];

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				MODEL->diffuse.x = (float)diffuse.Get()[0];
				MODEL->diffuse.y = (float)diffuse.Get()[1];
				MODEL->diffuse.z = (float)diffuse.Get()[2];
			}

			//ディヒューズテクスチャを取り出す
			const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (diffuseProperty.IsValid())
			{
				const FbxFileTexture *texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
				if (texture)
				{
					const char *filePath = texture->GetRelativeFileName();
					//ファイルパスからファイル名抽出
					std::string path_str(filePath);
					std::string name = ExtractFileName(path_str);

					//テクスチャ読み込み
					LoadTexture(MODEL, name);
					textureLoaded = true;
				}
			}
		}

		//テクスチャが無い場合
		//if (!textureLoaded)
	//	{
			//RESOURCE_HANDLE lHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "white1x1.png");
			//MODEL->textureHandle.push_back(lHandle);
//		}
	}
}

void FbxModelResourceMgr::LoadTexture(Model *MODEL, const std::string &FULL_PATH)
{
	// ファイル名を取得
	std::string file_path = FULL_PATH;

	// ファイル分解
	char buffer[256];
	ZeroMemory(buffer, sizeof(char) * 256);
	memcpy(buffer, file_path.c_str(), sizeof(char) * 256);

	char *file_name = buffer;
	std::string rename = file_name;
	std::string rename2 = MODEL->name;

	//階層がどれくらい深いか見る
	int countPath = 0;
	for (int i = 0; i < rename2.size(); ++i)
	{
		if (rename2[i] == '/')
		{
			++countPath;
		}
	}
	//モデルがある場所のファイルパスを検索する
	std::string filePass;
	int nowCountPass = 0;
	for (int i = 0; i < rename2.size(); ++i)
	{
		if (rename2[i] == '/')
		{
			++nowCountPass;
		}
		if (countPath <= nowCountPass)
		{
			filePass = rename2.substr(0, i + 1);
			break;
		}
	}

	//画像読み込み
	RESOURCE_HANDLE lHandle = TextureResourceMgr::Instance()->LoadGraph(filePass + FULL_PATH);
	MODEL->textureHandle.push_back(lHandle);
}

DirectX::XMVECTOR FbxModelResourceMgr::FbxDoubleToXMVECTOR(const FbxDouble3 &DOUBLE_3)
{
	DirectX::XMVECTOR result;
	result =
	{
		static_cast<float>(DOUBLE_3[0]),
		static_cast<float>(DOUBLE_3[1]),
		static_cast<float>(DOUBLE_3[2]),
		0.0f
	};

	return result;
}

std::string FbxModelResourceMgr::ExtractFileName(const std::string &PATH)
{
	size_t pos1;

	std::string search = "\\";
	//区切り文字'￥￥'が出てくる一番最後の部分を検索
	pos1 = PATH.rfind(search);
	if (pos1 != std::string::npos)
	{
		return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1);
	}

	//区切り文字'/'が出てくる一番最後の部分を検索
	pos1 = PATH.rfind('/');
	if (pos1 != std::string::npos)
	{
		return PATH.substr(pos1 + 1, PATH.size() - pos1 - 1);
	}

	//パスを含まないファイル名
	return PATH;
}

void FbxModelResourceMgr::ParseBone(Model *MODEL, FbxMesh *FBX_MESH)
{
	//スキニング情報読み込み
	FbxSkin *fbxSkin = static_cast<FbxSkin *>(FBX_MESH->GetDeformer(0, FbxDeformer::eSkin));
	//スキニング情報が無ければ終了
	if (fbxSkin == nullptr)
	{
		for (int i = 0; i < MODEL->vertices.size(); i++)
		{
			MODEL->vertices[i].boneIndex[0] = 0;
			MODEL->vertices[i].boneWeight[0] = 1.0f;
		}
		return;
	}
	//ボーン配列の参照
	vector<Model::Bone> &bones = MODEL->bones;

	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);



	//初期姿勢行列の読み込み--------------------------------------------------
	for (int i = 0; i < clusterCount; i++)
	{
		//Fbxボーン情報
		FbxCluster *fbxCluster = fbxSkin->GetCluster(i);

		//ボーン自体の名前を取得
		const char *boneName = fbxCluster->GetLink()->GetName();

		//新しくボーンを追加し、追加したボーンの参照を得る
		bones.emplace_back(Model::Bone(boneName));
		Model::Bone &bone = bones.back();

		//自作ボーンとfbxのボーンを紐付ける
		bone.fbxSkin = static_cast<FbxSkin *>(FBX_MESH->GetDeformer(0, FbxDeformer::eSkin));

		//Fbxから初期姿勢行列を取得する
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//DirectX::XMMATRIX型に変換
		DirectX::XMMATRIX initialPose;
		KazMath::ConvertMatrixFromFbx(&initialPose, fbxMat);

		//初期姿勢行列の逆行列を得る
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
	}
}

void FbxModelResourceMgr::ParseFaces(Model *MODEL, FbxMesh *FBX_MESH)
{
	//FBXメッシュの頂点座標配列を取得
	FbxVector4 *pCoord = FBX_MESH->GetControlPoints();

	auto &indices = MODEL->indices;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);
	//面の数
	const int polygonCount = FBX_MESH->GetPolygonCount();
	//UVデータの数
	const int textureUVCount = FBX_MESH->GetTextureUVCount();

	//UV名リスト
	FbxStringList uvNames;
	FBX_MESH->GetUVSetNames(uvNames);

	//頂点座標データの数
	const int controlPointsCount = FBX_MESH->GetControlPointsCount();
	std::vector<DirectX::XMFLOAT4>vertPos(controlPointsCount);
	for (int i = 0; i < controlPointsCount; ++i)
	{
		vertPos[i].x = (float)pCoord[i][0];
		vertPos[i].y = (float)pCoord[i][1];
		vertPos[i].z = (float)pCoord[i][2];
	}

	MODEL->faceCountNum = controlPointsCount;

	//重複あり頂点情報
	std::vector<Model::VertexPosNormalUvSkin> vertices;
	std::vector<UINT> indexData;
	std::vector<DirectX::XMFLOAT2> uvData;
	std::vector<DirectX::XMFLOAT3> normalData;
	for (int i = 0; i < polygonCount; i++)
	{
		//面を構成する頂点の数を取得(3なら三角形ポリゴン)
		const int polygonSize = FBX_MESH->GetPolygonSize(i);
		for (int j = 0; j < polygonSize; j++)
		{
			int index = FBX_MESH->GetPolygonVertex(i, j);
			vertices.push_back({});
			Model::VertexPosNormalUvSkin &vertex = vertices[vertices.size() - 1];
			//座標のコピー
			vertex.pos.x = vertPos[index].x;
			vertex.pos.y = vertPos[index].y;
			vertex.pos.z = vertPos[index].z;

			MODEL->vertFloat4Data.push_back(vertPos[index]);
			MODEL->vertFloat3Data.push_back({ vertPos[index].x,vertPos[index].y,vertPos[index].z });
	
			//UV
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;
				//0番決め打ちで読み込み
				if (FBX_MESH->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = (float)uvs[0];

					if (revUvFlag)
					{
						vertex.uv.y = 1.0f - (float)uvs[1];
					}
					else
					{
						vertex.uv.y = (float)uvs[1];
					}
					uvData.push_back({ vertex.uv.x, vertex.uv.y });
				}
			}

			//Normal
			FbxVector4 normal;
			if (FBX_MESH->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];

				normalData.push_back(vertex.normal);
			}

			indexData.push_back(index);

	
			VertexUv vertUvData;
			vertUvData.pos = { vertPos[index].x,vertPos[index].y,vertPos[index].z };
			vertUvData.uv = { vertex.uv.x,vertex.uv.y };
			MODEL->vertUvData.push_back(vertUvData);
		}
	}
	MODEL->vertices = vertices;
	MODEL->indices = indexData;
	MODEL->uvData = uvData;
	MODEL->normalData = normalData;

	FbxSkin *fbxSkin = static_cast<FbxSkin *>(FBX_MESH->GetDeformer(0, FbxDeformer::eSkin));
	boneSkinArray.push_back(fbxSkin);

	if (fbxSkin == nullptr)return;

	//スキンウェイト読み取り--------------------------------------------------
	//二次元配列(ジャグ配列)
	//list、頂点が影響を受けるボーンの全リスト
	//vector、それを全頂点分
	vector<list<WeightSet>> weightLists(MODEL->vertices.size());
	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//Fbxのボーン情報
		FbxCluster *fbxCluster = fbxSkin->GetCluster(i);

		//このボーンに影響を受ける頂点の数
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

		//このボーンに影響を受ける頂点の配列
		int *controlPointIndices = fbxCluster->GetControlPointIndices();
		double *controlPointWeights = fbxCluster->GetControlPointWeights();

		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点番号
			int vertIndex = controlPointIndices[j];
			//スキンウェイト
			float weight = (float)controlPointWeights[j];
			//その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加
			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}
	//ボーン単位にどの頂点に影響を与えるか、そのweightはどれくらいかを一つの配列に組み込む

	//スキンウェイトの整理--------------------------------------------------
	//頂点配列書き換え用の参照
	auto &vertData = MODEL->vertices;
	//各頂点についての処理
	for (int weightIndex = 0; weightIndex < weightLists.size(); ++weightIndex)
	{
		for (int i = 0; i < indexData.size(); ++i)
		{
			//インデックスとウェイトのインデックスが合っていない場合書き込まない
			if (static_cast<int>(indexData[i]) != weightIndex)continue;

			//頂点のウェイトから最も大きい4つを選択
			auto &weightList = weightLists[weightIndex];
			//大小比較用のラムダ式を指定して降順にソート
			weightList.sort([](auto const &lhs, auto const &rhs)
				{
					return lhs.weight > rhs.weight;
				}
			);

			int weightArrayIndex = 0;
			//降順ソート済みのウェイトリストから
			for (auto &weightSet : weightList)
			{
				//頂点データに書き込み
				vertData[i].boneIndex[weightArrayIndex] = weightSet.index;
				vertData[i].boneWeight[weightArrayIndex] = weightSet.weight;

				if (++weightArrayIndex >= Model::MAX_BONE_INDICES)
				{
					float weight = 0.0f;
					//二個目以降のウェイトを合計
					for (int j = 0; j < Model::MAX_BONE_INDICES; j++)
					{
						weight += vertData[i].boneWeight[j];
					}

					//合計で1.0f(100％)になるように調整
					vertData[i].boneWeight[0] = 1.0f - weight;
					break;
				}
			}
		}
	}

}
