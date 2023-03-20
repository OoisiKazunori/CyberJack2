#include "CircleRender.h"

CircleRender::CircleRender()
{
	const USHORT VERT_NUMBER = 300;
	const float RADIUS = 10;


	std::vector<Vertex> lVert(VERT_NUMBER);
	std::vector<USHORT> lIndi(VERT_NUMBER * 3);

	indicesNum = static_cast<UINT>(lIndi.size());
	const float PI_F2 = DirectX::XM_2PI;


	//頂点データ
	for (int i = 0; i < lVert.size(); i++)
	{
		lVert[i].pos.x = (RADIUS * sin((PI_F2 / VERT_NUMBER) * i));
		lVert[i].pos.y = -(RADIUS * cos((PI_F2 / VERT_NUMBER) * i));
		lVert[i].pos.z = 0.0f;

		lVert[i].uv.x = 1;
		lVert[i].uv.y = 1;
	}

	//インデックスデータ
	for (int i = 0; i < VERT_NUMBER; i++)
	{
		lIndi[i * 3] = VERT_NUMBER;	//中心
		lIndi[i * 3 + 1] = static_cast<USHORT>(i);			//再利用
		lIndi[i * 3 + 2] = static_cast<USHORT>(i + 1);		//新しい頂点
	}
	lIndi[lIndi.size() - 1] = 0;

	BUFFER_SIZE VertByte = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVert.size(), sizeof(Vertex));
	BUFFER_SIZE IndexByte = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndi.size(), sizeof(USHORT));


	gpuBuffer = std::make_unique<CreateGpuBuffer>();

	//バッファ生成-----------------------------------------------------------------------------------------------------
	//頂点バッファ
	vertexBufferHandle = gpuBuffer->CreateBuffer
	(
		KazBufferHelper::SetVertexBufferData(VertByte)
	);
	//インデックスバッファ
	indexBufferHandle = gpuBuffer->CreateBuffer
	(
		KazBufferHelper::SetIndexBufferData(IndexByte)
	);
	//定数バッファ
	constBufferHandle = CreateConstBuffer(sizeof(ConstBufferData), typeid(ConstBufferData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DRAW);
	//バッファ生成-----------------------------------------------------------------------------------------------------


	//バッファ転送-----------------------------------------------------------------------------------------------------
	gpuBuffer->TransData(vertexBufferHandle, lVert.data(), VertByte);
	gpuBuffer->TransData(indexBufferHandle, lIndi.data(), IndexByte);
	//バッファ転送-----------------------------------------------------------------------------------------------------

	drawIndexInstanceCommandData = KazRenderHelper::SetDrawIndexInstanceCommandData(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(gpuBuffer->GetGpuAddress(vertexBufferHandle), VertByte, sizeof(lVert[0])),
		KazBufferHelper::SetIndexBufferView(gpuBuffer->GetGpuAddress(indexBufferHandle), IndexByte),
		indicesNum,
		1
	);

	drawInstanceCommandData = KazRenderHelper::SetDrawInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		KazBufferHelper::SetVertexBufferView(gpuBuffer->GetGpuAddress(vertexBufferHandle), VertByte, sizeof(lVert[0])),
		300,
		1
	);
}

void CircleRender::Draw()
{
	//パイプライン設定-----------------------------------------------------------------------------------------------------
	if (data.fillFlag)
	{
		renderData.pipelineMgr->SetPipeLineAndRootSignature(data.pipelineName);
	}
	else
	{
		renderData.pipelineMgr->SetPipeLineAndRootSignature(PIPELINE_NAME_COLOR_LINE);
	}
	//パイプライン設定-----------------------------------------------------------------------------------------------------

	//行列計算-----------------------------------------------------------------------------------------------------
	if (data.transform.Dirty() || data.change3DDirtyFlag.Dirty() || renderData.cameraMgrInstance->BillboardDirty(data.cameraIndex.id))
	{
		if (data.change3DFlag)
		{
			baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
			baseMatWorldData.matScale = KazMath::CaluScaleMatrix(data.transform.scale);
			baseMatWorldData.matTrans = KazMath::CaluTransMatrix(data.transform.pos);
			baseMatWorldData.matRota = KazMath::CaluRotaMatrix(data.transform.rotation);

			//ワールド行列の計算
			baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
			baseMatWorldData.matWorld *= baseMatWorldData.matScale;
			baseMatWorldData.matWorld *= baseMatWorldData.matRota;
			//ビルボード行列を掛ける
			if (data.billBoardFlag)
			{
				baseMatWorldData.matWorld *= renderData.cameraMgrInstance->GetMatBillBoard(data.cameraIndex.id);
			}
			baseMatWorldData.matWorld *= baseMatWorldData.matTrans;
		}
		else
		{
			baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
			baseMatWorldData.matScale = KazMath::CaluScaleMatrix(data.transform.scale);
			baseMatWorldData.matTrans = KazMath::CaluTransMatrix(data.transform.pos);

			//ワールド行列の計算
			baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
			baseMatWorldData.matWorld *= baseMatWorldData.matScale;
			baseMatWorldData.matWorld *= baseMatWorldData.matTrans;

		}
	}
	//行列計算-----------------------------------------------------------------------------------------------------

	//円の拡縮-------------------------
	if (data.radiusDirtyFlag.Dirty())
	{
		const int VERT_NUMBER = 300;
		const float RADIUS = data.radius;

		std::vector<Vertex> lVert(VERT_NUMBER);

		float PI_F2 = DirectX::XM_2PI;
		//頂点データ
		for (int i = 0; i < lVert.size(); i++)
		{
			lVert[i].pos.x = (RADIUS * sin((PI_F2 / VERT_NUMBER) * i));
			lVert[i].pos.y = -(RADIUS * cos((PI_F2 / VERT_NUMBER) * i));
			lVert[i].pos.z = 0.0f;

			lVert[i].uv.x = 1.0f;
			lVert[i].uv.y = 1.0f;
		}
		BUFFER_SIZE lVertByte = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVert.size(), sizeof(Vertex));


		if (data.fillFlag)
		{
			lVert[lVert.size() - 1] = lVert[0];
		}
		gpuBuffer->TransData(vertexBufferHandle, lVert.data(), lVertByte);
	}
	//円の拡縮-------------------------



	//バッファの転送-----------------------------------------------------------------------------------------------------

	if (data.colorData.Dirty() || data.transform.Dirty() || data.change3DDirtyFlag.Dirty() || (renderData.cameraMgrInstance->ViewAndProjDirty(data.cameraIndex.id) && data.change3DFlag) || data.cameraIndex.dirty.Dirty() || renderData.cameraMgrInstance->BillboardDirty(data.cameraIndex.id))
	{
		//行列
		if (data.change3DFlag)
		{
			constMap.world = baseMatWorldData.matWorld;
			constMap.view = renderData.cameraMgrInstance->GetViewMatrix(data.cameraIndex.id);
			constMap.viewproj = renderData.cameraMgrInstance->GetPerspectiveMatProjection(data.cameraIndex.id);
			constMap.color = data.colorData.ConvertColorRateToXMFLOAT4();
			constMap.mat = constMap.world * constMap.view * constMap.viewproj;
		}
		else
		{
			constMap.world = baseMatWorldData.matWorld;
			constMap.view = DirectX::XMMatrixIdentity();
			constMap.viewproj = renderData.cameraMgrInstance->GetOrthographicMatProjection();
			constMap.color = data.colorData.ConvertColorRateToXMFLOAT4();
			constMap.mat = constMap.world * constMap.viewproj;
		}
		TransData(&constMap, constBufferHandle, typeid(constMap).name());
	}
	//バッファの転送-----------------------------------------------------------------------------------------------------


	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------
	SetConstBufferOnCmdList(data.pipelineName);
	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------

	//描画命令-----------------------------------------------------------------------------------------------------
	if (data.fillFlag)
	{
		DrawIndexInstanceCommand(drawIndexInstanceCommandData);
	}
	else
	{
		DrawInstanceCommand(drawInstanceCommandData);
	}
	//描画命令-----------------------------------------------------------------------------------------------------

	data.Record();

}
