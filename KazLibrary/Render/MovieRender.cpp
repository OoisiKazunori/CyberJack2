#include "MovieRender.h"
#include "Sprite2DRender.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Helper/KazHelper.h"

MovieRender::MovieRender()
{
	anchorPoint = { 0.5f,0.5f };

	gpuBuffer = std::make_unique<CreateGpuBuffer>();


	//データの定義-----------------------------------------------------------------------------------------------------
	//頂点データ
	KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, anchorPoint.ConvertXMFLOAT2());
	KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
	//インデックスデータ
	indices = KazRenderHelper::InitIndciesForPlanePolygon();
	//データの定義-----------------------------------------------------------------------------------------------------


	vertByte = KazBufferHelper::GetBufferSize<UINT>(vertices.size(), sizeof(SpriteVertex));
	indexByte = KazBufferHelper::GetBufferSize<UINT>(indices.size(), sizeof(sizeof(unsigned short)));


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

	drawIndexInstanceCommandData = KazRenderHelper::SetDrawIndexInstanceCommandData(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(gpuBuffer->GetGpuAddress(vertexBufferHandle), vertByte, sizeof(vertices[0])),
		KazBufferHelper::SetIndexBufferView(gpuBuffer->GetGpuAddress(indexBufferHandle), indexByte),
		static_cast<UINT>(indices.size()),
		1);

}

MovieRender::~MovieRender()
{
}

void MovieRender::Draw()

{
	//パイプライン設定-----------------------------------------------------------------------------------------------------
	renderData.pipelineMgr->SetPipeLineAndRootSignature(data.pipelineName);
	//パイプライン設定-----------------------------------------------------------------------------------------------------



	//行列計算-----------------------------------------------------------------------------------------------------
	if (data.transform.Dirty())
	{
		baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
		baseMatWorldData.matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(data.transform.rotation));
		baseMatWorldData.matWorld *= DirectX::XMMatrixTranslationFromVector(data.transform.pos.ConvertXMVECTOR());
	}
	//行列計算-----------------------------------------------------------------------------------------------------



	//頂点データの書き換えとUVの書き換え-----------------------------------------------------------------------------------------------------

	//読み込んだテクスチャのサイズ
	//読み込んだ画像のサイズを合わせる
	//if (data.handleData.flag.Dirty() || data.transform.scaleDirtyFlag.Dirty())
	{
		KazMath::Vec2<float> lTmpSize = data.transform.scale;

		texSize.x = static_cast<int>(data.buff->GetDesc().Width);
		texSize.y = static_cast<int>(data.buff->GetDesc().Height);

		KazMath::Vec2<float> lLeftUp, lRightDown;
		lLeftUp = { 0.0f,0.0f };
		lRightDown = { 1.0f,1.0f };

		//サイズ変更
		std::array<KazMath::Vec2<float>, 4>tmp = KazRenderHelper::ChangePlaneScale(lLeftUp, lRightDown, lTmpSize, anchorPoint, texSize);
		for (int i = 0; i < tmp.size(); i++)
		{
			vertices[i].pos = { tmp[i].x,-tmp[i].y,0.0f };
		}
	}

	//X軸にUVを反転
	if (data.flip.xDirtyFlag.Dirty())
	{
		KazRenderHelper::FlipXUv(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
	}
	//Y軸にUVを反転
	if (data.flip.yDirtyFlag.Dirty())
	{
		KazRenderHelper::FlipYUv(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
	}


	bool lFlipDirtyFlag = data.flip.xDirtyFlag.Dirty() || data.flip.yDirtyFlag.Dirty();
	//頂点データに何か変更があったら転送する
	if (data.handleData.flag.Dirty() || data.transform.scaleDirtyFlag.Dirty() || lFlipDirtyFlag)
	{
		gpuBuffer->TransData(vertexBufferHandle, vertices.data(), vertByte);
	}
	//頂点データの書き換えとUVの書き換え-----------------------------------------------------------------------------------------------------


	//バッファの転送-----------------------------------------------------------------------------------------------------
	//行列
	if (data.transform.Dirty() || renderData.cameraMgrInstance->ViewAndProjDirty() || data.colorData.Dirty())
	{
		ConstBufferData lConstMap;
		lConstMap.world = baseMatWorldData.matWorld;
		lConstMap.view = DirectX::XMMatrixIdentity();
		lConstMap.viewproj = renderData.cameraMgrInstance->GetOrthographicMatProjection();
		lConstMap.color = data.colorData.ConvertColorRateToXMFLOAT4();
		lConstMap.mat = lConstMap.world * lConstMap.viewproj;
		TransData(&lConstMap, constBufferHandle, typeid(lConstMap).name());
	}
	//バッファの転送-----------------------------------------------------------------------------------------------------


	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------
	SetConstBufferOnCmdList(data.pipelineName);


	if (data.handleData.handle != -1)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = DescriptorHeapMgr::Instance()->GetGpuDescriptorView(data.handleData.handle);
		int param = KazRenderHelper::SetBufferOnCmdList(GraphicsRootSignature::Instance()->GetRootParam(renderData.pipelineMgr->GetRootSignatureName(data.pipelineName)), GRAPHICS_RANGE_TYPE_SRV_DESC, GRAPHICS_PRAMTYPE_TEX);
		renderData.cmdListInstance->cmdList->SetGraphicsRootDescriptorTable(param, gpuDescHandleSRV);
	}

	for (int i = 0; i < data.addHandle.handle.size(); ++i)
	{
		renderData.shaderResourceMgrInstance->SetSRV(data.addHandle.handle[i], GraphicsRootSignature::Instance()->GetRootParam(renderData.pipelineMgr->GetRootSignatureName(data.pipelineName)), data.addHandle.paramType[i]);
	}
	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------


	

	//描画命令-----------------------------------------------------------------------------------------------------
	DrawIndexInstanceCommand(drawIndexInstanceCommandData);
	//描画命令-----------------------------------------------------------------------------------------------------


	//DirtyFlagの更新-----------------------------------------------------------------------------------------------------
	data.Record();
	//DirtyFlagの更新-----------------------------------------------------------------------------------------------------
}
