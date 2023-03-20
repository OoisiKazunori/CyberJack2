#include "Sprite3DRender.h"
#include"../Helper/OutPutDebugStringAndCheckResult.h"
#include"../Helper/KazRenderHelper.h"
#include"../Helper/KazHelper.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../RenderTarget/RenderTargetStatus.h"

Sprite3DRender::Sprite3DRender(const KazMath::Vec2<float> ANCHOR_POINT)
{
	anchorPoint = ANCHOR_POINT;
	gpuBuffer = std::make_unique<CreateGpuBuffer>();



	//データの定義-----------------------------------------------------------------------------------------------------
	//頂点データ
	KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, anchorPoint.ConvertXMFLOAT2());
	KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
	//インデックスデータ
	indices = KazRenderHelper::InitIndciesForPlanePolygon();
	//データの定義-----------------------------------------------------------------------------------------------------


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
		KazBufferHelper::SetIndexBufferView(gpuBuffer->GetGpuAddress(indexBufferHandle), indexByte), static_cast<UINT>(indices.size()),
		1
	);

}

Sprite3DRender::~Sprite3DRender()
{
}

void Sprite3DRender::Draw()
{
	//パイプライン設定-----------------------------------------------------------------------------------------------------
	renderData.pipelineMgr->SetPipeLineAndRootSignature(data.pipelineName);
	//パイプライン設定-----------------------------------------------------------------------------------------------------


	//行列計算-----------------------------------------------------------------------------------------------------
	if (data.transform.Dirty() || data.billBoardDirtyFlag.Dirty() || renderData.cameraMgrInstance->BillboardDirty() || (data.billBoardFlag && renderData.cameraMgrInstance->BillboardDirty()) || data.motherMat.dirty.Dirty())
	{
		baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
		baseMatWorldData.matScale = KazMath::CaluScaleMatrix(data.transform.scale);
		baseMatWorldData.matTrans = KazMath::CaluTransMatrix(data.transform.pos);
		baseMatWorldData.matRota = KazMath::CaluRotaMatrix(data.transform.rotation);


		baseMatWorldData.matWorld *= baseMatWorldData.matScale;
		baseMatWorldData.matWorld *= baseMatWorldData.matRota;
		//ビルボード行列を掛ける
		if (data.billBoardFlag)
		{
			baseMatWorldData.matWorld *= renderData.cameraMgrInstance->GetMatBillBoard(data.cameraIndex.id);
		}
		baseMatWorldData.matWorld *= baseMatWorldData.matTrans;
		baseMatWorldData.matWorld *= data.motherMat.mat;

	}
	//行列計算-----------------------------------------------------------------------------------------------------
	motherMat = baseMatWorldData.matWorld;


	//頂点データの書き換えとUVの書き換え-----------------------------------------------------------------------------------------------------

	//読み込んだテクスチャのサイズ
	//読み込んだ画像のサイズを合わせる
	if (data.handleData.flag.Dirty() || data.transform.scaleDirtyFlag.Dirty())
	{
		if (DescriptorHeapMgr::Instance()->GetType(data.handleData.handle) != DESCRIPTORHEAP_MEMORY_TEXTURE_RENDERTARGET)
		{

			KazMath::Vec2<float> lTmpSize = {};
			lTmpSize = { data.transform.scale.x, data.transform.scale.y };


			texSize.x = static_cast<int>(renderData.shaderResourceMgrInstance->GetTextureSize(data.handleData.handle).Width);
			texSize.y = static_cast<int>(renderData.shaderResourceMgrInstance->GetTextureSize(data.handleData.handle).Height);


			KazMath::Vec2<float> lLeftUp, lRightDown;
			lLeftUp = { 0.0f,0.0f };
			lRightDown = { 1.0f,1.0f };

			std::array<KazMath::Vec2<float>, 4>lVert;
			//サイズ変更
			lVert = KazRenderHelper::ChangePlaneScale(lLeftUp, lRightDown, lTmpSize, anchorPoint, texSize);

			for (int i = 0; i < lVert.size(); i++)
			{
				vertices[i].pos = { lVert[i].x,lVert[i].y,0.0f };
			}
			KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
		}
		//レンダーターゲットのテクスチャサイズ
		else
		{
			//サイズをDirectX::XMFLOAT3からDirectX::XMFLOAT2に直す
			KazMath::Vec2<float> lTmpSize = { data.transform.scale.x, data.transform.scale.y };

			texSize.x = static_cast<int>(RenderTargetStatus::Instance()->GetBufferData(data.handleData.handle)->GetDesc().Width);
			texSize.y = static_cast<int>(RenderTargetStatus::Instance()->GetBufferData(data.handleData.handle)->GetDesc().Height);

			KazMath::Vec2<float> lLeftUp, lRightDown;
			lLeftUp = { 0.0f,0.0f };
			lRightDown = { 1.0f,1.0f };

			//サイズ変更
			std::array<KazMath::Vec2<float>, 4>lVert = KazRenderHelper::ChangePlaneScale(lLeftUp, lRightDown, lTmpSize, anchorPoint, texSize);
			for (int i = 0; i < lVert.size(); i++)
			{
				vertices[i].pos = { lVert[i].x,lVert[i].y,0.0f };
			}
		}
	}

	//UV切り取り
	if (data.handleData.flag.Dirty() || data.animationHandle.flag.Dirty())
	{
		KazMath::Vec2<int> lDivSize = renderData.shaderResourceMgrInstance->GetDivData(data.handleData.handle).divSize;
		KazMath::Vec2<float> lTmpSize = { data.transform.scale.x, data.transform.scale.y };

		const bool lIsItSafeToUseAnimationHandleFlag = KazHelper::IsitInAnArray(data.animationHandle.handle, renderData.shaderResourceMgrInstance->GetDivData(data.handleData.handle).divLeftUp.size());
		const bool IsItSafeToUseDivDataFlag = (lDivSize.x != -1 && lDivSize.y != -1);

		if (IsItSafeToUseDivDataFlag && lIsItSafeToUseAnimationHandleFlag)
		{
			KazMath::Vec2<int> lDivLeftUpPos = renderData.shaderResourceMgrInstance->GetDivData(data.handleData.handle).divLeftUp[data.animationHandle.handle];


			KazRenderHelper::VerticesCut(lDivSize, lDivLeftUpPos, &vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, lTmpSize, anchorPoint);
			KazRenderHelper::UVCut(lDivLeftUpPos, lDivSize, texSize, &vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
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
	if (data.handleData.flag.Dirty() || data.transform.scaleDirtyFlag.Dirty() || data.animationHandle.flag.Dirty() || lFlipDirtyFlag)
	{
		gpuBuffer->TransData(vertexBufferHandle, vertices.data(), vertByte);
	}
	//頂点データの書き換えとUVの書き換え-----------------------------------------------------------------------------------------------------


	//バッファの転送-----------------------------------------------------------------------------------------------------
	//行列
	if (data.transform.Dirty() || renderData.cameraMgrInstance->ViewAndProjDirty(data.cameraIndex.id) || renderData.cameraMgrInstance->BillboardDirty() || data.colorData.Dirty() || data.cameraIndex.dirty.Dirty() || data.motherMat.dirty.Dirty())
	{
		ConstBufferData lConstMap;
		lConstMap.world = baseMatWorldData.matWorld;
		lConstMap.view = renderData.cameraMgrInstance->GetViewMatrix(data.cameraIndex.id);
		lConstMap.viewproj = renderData.cameraMgrInstance->GetPerspectiveMatProjection(data.cameraIndex.id);
		lConstMap.color = data.colorData.ConvertColorRateToXMFLOAT4();
		lConstMap.mat = lConstMap.world * lConstMap.view * lConstMap.viewproj;

		TransData(&lConstMap, constBufferHandle, typeid(lConstMap).name());
	}
	//バッファの転送-----------------------------------------------------------------------------------------------------


	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------
	SetConstBufferOnCmdList(data.pipelineName);
	renderData.shaderResourceMgrInstance->SetSRV(data.handleData.handle, GraphicsRootSignature::Instance()->GetRootParam(renderData.pipelineMgr->GetRootSignatureName(data.pipelineName)), GRAPHICS_PRAMTYPE_TEX);
	//バッファをコマンドリストに積む-----------------------------------------------------------------------------------------------------

	//追加のテクスチャを送る---------------------------------------------------------------
	for (int i = 0; i < data.addHandle.handle.size(); ++i)
	{
		renderData.shaderResourceMgrInstance->SetSRV(data.addHandle.handle[i], GraphicsRootSignature::Instance()->GetRootParam(renderData.pipelineMgr->GetRootSignatureName(data.pipelineName)), data.addHandle.paramType[i]);
	}
	//追加のテクスチャを送る---------------------------------------------------------------


	//描画命令-----------------------------------------------------------------------------------------------------
	DrawIndexInstanceCommand(drawIndexInstanceCommandData);
	//描画命令-----------------------------------------------------------------------------------------------------




	//DirtyFlagの更新-----------------------------------------------------------------------------------------------------
	data.Record();
	//DirtyFlagの更新-----------------------------------------------------------------------------------------------------
}

DirectX::XMMATRIX Sprite3DRender::GetMotherMatrix()
{
	return motherMat;
}
