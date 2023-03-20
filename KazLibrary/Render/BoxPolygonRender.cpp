#include "BoxPolygonRender.h"

BoxPolygonRender::BoxPolygonRender(bool INSTANCE_FLAG, int INSTANCE_NUM)
{
	gpuBuffer = std::make_unique<CreateGpuBuffer>();

	const float lSize = 1.0f;

	Vertex lVertices[] =
	{
		//x,y,z	法線	u,v
	//前
	{{-lSize, -lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{-lSize,  lSize, -lSize},{}, {0.0f,0.0f}},		//左上
	{{ lSize, -lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{ lSize,  lSize, -lSize}, {}, {1.0f,0.0f}},		//右上

	//後
	{{-lSize,-lSize,  lSize}, {}, {0.0f,1.0f}},		//左下
	{{-lSize, lSize,  lSize}, {},{0.0f,0.0f}},		//左上
	{{ lSize,-lSize,  lSize},{},{1.0f,1.0f}},		//右下
	{{ lSize, lSize,  lSize},{}, {1.0f,0.0f}},		//右上

	//左
	{{-lSize,-lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{-lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},		//左上
	{{-lSize, lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{-lSize, lSize,  lSize}, {}, {1.0f,0.0f}},		//右上

	//右
	{{lSize,-lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},		//左上
	{{lSize, lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{lSize, lSize,  lSize}, {}, {1.0f,0.0f}},		//右上

	//下
	{{ lSize, -lSize, lSize}, {}, {0.0f,1.0f}},		//左下
	{{ lSize, -lSize,-lSize}, {}, {0.0f,0.0f}},		//左上
	{{-lSize, -lSize, lSize}, {}, {1.0f,1.0f}},		//右下
	{{-lSize, -lSize,-lSize}, {}, {1.0f,0.0f}},	//右上

	//上
	{{ lSize, lSize, lSize}, {}, {0.0f,1.0f}},			//左下
	{{ lSize, lSize,-lSize}, {}, {0.0f,0.0f}},			//左上
	{{-lSize, lSize, lSize}, {}, {1.0f,1.0f}},			//右下
	{{-lSize, lSize,-lSize}, {}, {1.0f,0.0f}}			//右上
	};

	const unsigned short lIndices[] =
	{
		//前
		0,1,2,		//三角形1つ目
		2,1,3,		//三角形2つ目
		//後
		4,6,5,		//三角形3つ目
		6,7,5,		//三角形4つ目
		//左
		8,9,10,		//三角形1つ目
		10,9,11,	//三角形2つ目
		//右
		12,14,13,	//三角形1つ目
		13,14,15,	//三角形2つ目
		//下
		16,18,17,	//三角形1つ目
		17,18,19,	//三角形2つ目
		//上
		20,21,22,	//三角形3つ目
		22,21,23	//三角形4つ目
	};

	UINT lIndexLEN = sizeof(lIndices) / sizeof(lIndices[0]);

	UINT lVertByte = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(sizeof(lVertices) / sizeof(lVertices[0]), sizeof(Vertex));
	UINT lIndexByte = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndexLEN, sizeof(Vertex));

	//法線ベクトル
	for (unsigned int i = 0; i < lIndexLEN / 3; i++)
	{
		USHORT index[3];

		//三角形のインデックスを取り出して、一時的な変数に入れる
		index[0] = lIndices[i * 3 + 0];
		index[1] = lIndices[i * 3 + 1];
		index[2] = lIndices[i * 3 + 2];

		//三角形を構築する頂点座標をベクトルに代入
		DirectX::XMVECTOR p0 = XMLoadFloat3(&lVertices[index[0]].pos);
		DirectX::XMVECTOR p1 = XMLoadFloat3(&lVertices[index[1]].pos);
		DirectX::XMVECTOR p2 = XMLoadFloat3(&lVertices[index[2]].pos);

		//p0-p1,p0-p2ベクトルを計算(ベクトルの減算)
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p1, p0);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		DirectX::XMVECTOR normal = DirectX::XMVector3Cross(v1, v2);

		//正規化(長さを1にする)
		normal = DirectX::XMVector3Normalize(normal);

		//求めた法線を頂点データを代入
		XMStoreFloat3(&lVertices[index[0]].normal, normal);
		XMStoreFloat3(&lVertices[index[1]].normal, normal);
		XMStoreFloat3(&lVertices[index[2]].normal, normal);
	}


	RESOURCE_HANDLE lVertBufferHandle = gpuBuffer->CreateBuffer(KazBufferHelper::SetVertexBufferData(lVertByte));
	RESOURCE_HANDLE lIndexBufferHandle = gpuBuffer->CreateBuffer(KazBufferHelper::SetIndexBufferData(lIndexByte));


	instanceFlag = INSTANCE_FLAG;
	if (!instanceFlag)
	{
		//定数バッファ
		constBufferHandle = CreateConstBuffer(sizeof(ConstBufferData), typeid(ConstBufferData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DRAW);
	}


	std::array<Vertex, 24> vertVec;
	std::array<unsigned short, 36> indiVec;
	for (int i = 0; i < 24; i++)
	{
		vertVec[i].pos = lVertices[i].pos;
		vertVec[i].uv = lVertices[i].uv;
		vertVec[i].normal = lVertices[i].normal;
	}
	for (int i = 0; i < 36; i++)
	{
		indiVec[i] = lIndices[i];
	}

	gpuBuffer->TransData(lVertBufferHandle, vertVec.data(), lVertByte);
	gpuBuffer->TransData(lIndexBufferHandle, indiVec.data(), lIndexByte);


	drawIndexInstanceCommandData = KazRenderHelper::SetDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(gpuBuffer->GetGpuAddress(lVertBufferHandle), lVertByte, sizeof(lVertices[0])),
		KazBufferHelper::SetIndexBufferView(gpuBuffer->GetGpuAddress(lIndexBufferHandle), lIndexByte),
		lIndexLEN,
		INSTANCE_NUM
	);
}

BoxPolygonRender::~BoxPolygonRender()
{
}

void BoxPolygonRender::Draw()
{
	//パイプライン設定-----------------------------------------------------------------------------------------------------
	renderData.pipelineMgr->SetPipeLineAndRootSignature(data.pipelineName);
	//パイプライン設定-----------------------------------------------------------------------------------------------------


	if (!instanceFlag)
	{
		//行列計算-----------------------------------------------------------------------------------------------------
		if (data.transform.Dirty() || data.motherMat.dirty.Dirty())
		{
			baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
			baseMatWorldData.matScale = KazMath::CaluScaleMatrix(data.transform.scale);
			baseMatWorldData.matTrans = KazMath::CaluTransMatrix(data.transform.pos);
			baseMatWorldData.matRota = KazMath::CaluRotaMatrix(data.transform.rotation);

			//ワールド行列の計算
			baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
			baseMatWorldData.matWorld *= baseMatWorldData.matScale;
			baseMatWorldData.matWorld *= baseMatWorldData.matRota;
			baseMatWorldData.matWorld *= baseMatWorldData.matTrans;
			baseMatWorldData.matWorld *= data.motherMat.mat;
			//親行列を掛ける
			motherMatrix = baseMatWorldData.matWorld;
		}
		//行列計算-----------------------------------------------------------------------------------------------------



		//バッファの転送-----------------------------------------------------------------------------------------------------
		//行列
		if (data.transform.Dirty() || renderData.cameraMgrInstance->ViewAndProjDirty(data.cameraIndex.id) || data.color.Dirty() || data.motherMat.dirty.Dirty() || data.cameraIndex.dirty.Dirty())
		{
			ConstBufferData constMap;
			constMap.world = baseMatWorldData.matWorld;
			constMap.view = renderData.cameraMgrInstance->GetViewMatrix(data.cameraIndex.id);
			constMap.viewproj = renderData.cameraMgrInstance->GetPerspectiveMatProjection(data.cameraIndex.id);
			constMap.color = data.color.ConvertColorRateToXMFLOAT4();
			constMap.mat = constMap.world * constMap.view * constMap.viewproj;
			TransData(&constMap, constBufferHandle, typeid(ConstBufferData).name());
		}
		//バッファの転送-----------------------------------------------------------------------------------------------------
	}

	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------
	SetConstBufferOnCmdList(data.pipelineName, false);
	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------

	//描画命令-------------------------
	DrawIndexInstanceCommand(drawIndexInstanceCommandData);
	//描画命令-------------------------

	data.Record();
}

DirectX::XMMATRIX BoxPolygonRender::GetMotherMatrix()
{
	return motherMatrix;
}
