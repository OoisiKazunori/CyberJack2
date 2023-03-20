#include "PolygonRender.h"
#include"../KazLibrary/Helper/KazHelper.h"

PolygonRender::PolygonRender(const std::array<SpriteVertex, 4> &DATA)
{
	anchorPoint = { 0.5f,0.5f };

	gpuBuffer = std::make_unique<CreateGpuBuffer>();

	//データの定義-----------------------------------------------------------------------------------------------------
	//頂点データ
	//KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, anchorPoint);
	KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
	//インデックスデータ
	indices = KazRenderHelper::InitIndciesForPlanePolygon();
	//データの定義-----------------------------------------------------------------------------------------------------

	vertices = DATA;


	vertByte = KazBufferHelper::GetBufferSize<UINT>(vertices.size(), sizeof(SpriteVertex));
	indexByte = KazBufferHelper::GetBufferSize<UINT>(indices.size(), sizeof(unsigned short));


	//バッファ生成-----------------------------------------------------------------------------------------------------
	//頂点バッファ
	vertexBufferHandle = gpuBuffer->CreateBuffer
	(
		KazBufferHelper::SetVertexBufferData(vertByte)
	);
	//インデックスバッファ
	indexBufferHandle = gpuBuffer->CreateBuffer
	(
		KazBufferHelper::SetIndexBufferData(indexByte)
	);
	//定数バッファ
	constBufferHandle = CreateConstBuffer(sizeof(ConstBufferData), typeid(ConstBufferData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DRAW);
	//バッファ生成-----------------------------------------------------------------------------------------------------


	//バッファ転送-----------------------------------------------------------------------------------------------------
	gpuBuffer->TransData(vertexBufferHandle, vertices.data(), vertByte);
	gpuBuffer->TransData(indexBufferHandle, indices.data(), indexByte);
	//バッファ転送-----------------------------------------------------------------------------------------------------


	drawIndexInstanceCommandData = KazRenderHelper::SetDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(gpuBuffer->GetGpuAddress(vertexBufferHandle), vertByte, sizeof(vertices[0])),
		KazBufferHelper::SetIndexBufferView(gpuBuffer->GetGpuAddress(indexBufferHandle), indexByte),
		static_cast<UINT>(indices.size()),
		1
	);
}

void PolygonRender::Draw()
{

	//パイプライン設定-----------------------------------------------------------------------------------------------------
	renderData.pipelineMgr->SetPipeLineAndRootSignature(data.pipelineName);
	//パイプライン設定-----------------------------------------------------------------------------------------------------


	//行列計算-----------------------------------------------------------------------------------------------------
	if (data.transform.Dirty() || data.billBoardDirtyFlag.Dirty() || renderData.cameraMgrInstance->BillboardDirty() || data.motherMat.dirty.Dirty())
	{
		baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
		baseMatWorldData.matScale = KazMath::CaluScaleMatrix(data.transform.scale);
		baseMatWorldData.matTrans = KazMath::CaluTransMatrix(data.transform.pos);
		baseMatWorldData.matRota = KazMath::CaluRotaMatrix(data.transform.rotation);
		//ビルボード行列を掛ける
		if (data.billBoardFlag)
		{
			baseMatWorldData.matWorld *= renderData.cameraMgrInstance->GetMatBillBoard(data.cameraIndex.dirty.Dirty());
		}
		baseMatWorldData.matWorld *= baseMatWorldData.matScale;
		baseMatWorldData.matWorld *= baseMatWorldData.matRota;
		baseMatWorldData.matWorld *= baseMatWorldData.matTrans;
		baseMatWorldData.matWorld *= data.motherMat.mat;

		//親行列を掛ける
		motherMat = baseMatWorldData.matWorld;
	}
	//行列計算-----------------------------------------------------------------------------------------------------


	//バッファの転送-----------------------------------------------------------------------------------------------------
	//行列
	if (data.transform.Dirty() || renderData.cameraMgrInstance->ViewAndProjDirty(data.cameraIndex.id) || renderData.cameraMgrInstance->BillboardDirty() || data.motherMat.dirty.Dirty() || data.cameraIndex.dirty.Dirty())
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


	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------
	SetConstBufferOnCmdList(data.pipelineName);
	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------

	//描画命令-----------------------------------------------------------------------------------------------------
	DrawIndexInstanceCommand(drawIndexInstanceCommandData);
	//描画命令-----------------------------------------------------------------------------------------------------

	data.Record();
}