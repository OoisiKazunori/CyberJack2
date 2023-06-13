#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	boxData = boxBuffer.GenerateBoxBuffer(1.0f);

	{
		gBuffer[0] = KazBufferHelper::SetUAVTexBuffer(1280, 720, "G-Buffer_Albedo");
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = 1280 * 720;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, gBuffer[0].bufferWrapper->GetBuffer().Get());
		gBuffer[0].bufferWrapper->CreateViewHandle(view);
		gBuffer[0].elementNum = 1280 * 720;
		gBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	}
	{
		gBuffer[1] = KazBufferHelper::SetUAVTexBuffer(1280, 720, "G-Buffer_Normal");
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = 1280 * 720;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, gBuffer[1].bufferWrapper->GetBuffer().Get());
		gBuffer[1].bufferWrapper->CreateViewHandle(view);
		gBuffer[1].elementNum = 1280 * 720;
		gBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	}
	{
		finalGBuffer = KazBufferHelper::SetUAVTexBuffer(1280, 720, "G-Buffer_Final");
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = 1280 * 720;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, finalGBuffer.bufferWrapper->GetBuffer().Get());
		finalGBuffer.bufferWrapper->CreateViewHandle(view);
		finalGBuffer.elementNum = 1280 * 720;
		finalGBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		finalGBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	}
	//ワールド座標、ラフネス、メタルネス、スぺキュラ、オブジェクトが反射するか屈折するか(インデックス)、Albedo、法線、カメラ座標(定数バッファでも可能)


	//G-Bufferに書き込む予定のオブジェクト
	//{
	//	DrawFunc::PipelineGenerateData lData;
	//	lData.desc = DrawFuncPipelineData::SetTest();
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);


	//	testRArray[0] = std::make_unique<DrawFunc::KazRender>(
	//		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxData.index, lData)
	//		);

	//	gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	//	testRArray[0]->GetDrawData()->buffer.emplace_back(gBuffer[0]);
	//}

	RESOURCE_HANDLE handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "hamster.obj");
	ObjResourceMgr::Instance()->GetResourceData(handle);
	model = ModelLoader::Instance()->Load("Resource/Test/glTF/hamster.obj");

	//フォワードレンダリングで描画するモデル
	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvmain", "ps_6_4", SHADER_TYPE_PIXEL);

		//testRArray[1] = std::make_unique<DrawFunc::KazRender>(
		//	DrawFunc::SetDrawGLTFIndexMaterialData(&rasterizeRenderer, *model, lData)
		//	);

		////ライト用の情報
		//testRArray[1]->GetDrawData()->buffer.emplace_back(KazBufferHelper::BufferData(KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT3))));
		//testRArray[1]->GetDrawData()->buffer.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		//testRArray[1]->GetDrawData()->buffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//testRArray[1]->GetDrawData()->buffer.emplace_back(model->modelData[0].materialData.textureBuffer);
		//testRArray[1]->GetDrawData()->buffer.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		//testRArray[1]->GetDrawData()->buffer.back().rootParamType = GRAPHICS_PRAMTYPE_TEX;

		//MaterialBufferData data = model->modelData[0].materialData.GetMaterialData();
		//testRArray[1]->GetDrawData()->buffer[1].bufferWrapper->TransData(&data, sizeof(MaterialBufferData));

		//gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		//gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		//testRArray[1]->GetDrawData()->buffer.emplace_back(gBuffer[0]);
		//testRArray[1]->GetDrawData()->buffer.emplace_back(gBuffer[1]);

		//描画
		drawSponza = DrawFunc::SetDrawGLTFIndexMaterialData(*model, lData);
		//その他バッファ
		drawSponza.extraBufferArray.emplace_back(KazBufferHelper::BufferData(KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT3))));
		drawSponza.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawSponza.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawSponza.extraBufferArray.back().bufferSize = sizeof(DirectX::XMFLOAT3);
	}

	//G-Bufferの描画確認用の板ポリ
	//{
	//	DrawFunc::PipelineGenerateData lData;
	//	lData.desc = DrawFuncPipelineData::SetTex();
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

	//	planeData = texBuffer.GeneratePlaneTexBuffer(
	//		{
	//			1.0f,
	//			1.0f
	//		},
	//		{
	//			static_cast<int>(gBuffer[0].bufferWrapper->GetBuffer()->GetDesc().Width),
	//			static_cast<int>(gBuffer[0].bufferWrapper->GetBuffer()->GetDesc().Height)
	//		}
	//		);

	//	testRArray[2] = std::make_unique<DrawFunc::KazRender>(
	//		DrawFunc::SetTransformData(&rasterizeRenderer, planeData.index, lData)
	//		);

	//	gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	//	//Albedo用のG-Bufferを生成
	//	testRArray[2]->GetDrawData()->buffer.emplace_back(
	//		gBuffer[0]
	//	);
	//}

	//法線描画用
	//{
	//	DrawFunc::PipelineGenerateData lData;
	//	lData.desc = DrawFuncPipelineData::SetTex();
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

	//	normalGBufferRender = std::make_unique<DrawFunc::KazRender>(
	//		DrawFunc::SetTransformData(&rasterizeRenderer, planeData.index, lData)
	//		);

	//	gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	//	//Albedo用のG-Bufferを生成
	//	normalGBufferRender->GetDrawData()->buffer.emplace_back(
	//		gBuffer[1]
	//	);
	//}

	//最終合成
	//{
	//	DrawFunc::PipelineGenerateData lData;
	//	lData.desc = DrawFuncPipelineData::SetTex();
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	//	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

	//	finalGBufferRender = std::make_unique<DrawFunc::KazRender>(
	//		DrawFunc::SetTransformData(&rasterizeRenderer, planeData.index, lData)
	//		);

	//	finalGBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	//	//Albedo用のG-Bufferを生成
	//	finalGBufferRender->GetDrawData()->buffer.emplace_back(
	//		finalGBuffer
	//	);
	//}

	transformArray[0].pos = { 0.0f,0.0f,0.0f };
	transformArray[1].pos = { 10.0f,0.0f,0.0f };
	transformArray[1].scale = { 1.0f,1.0f,1.0f };

	transformArray[2].pos = { 1280.0f,720.0f,0.0f };
	transformArray[2].scale = { 0.25f,0.25f,0.0f };
	transformArray[3].pos = { 1280.0f,525.0f,0.0f };
	transformArray[3].scale = { 0.25f,0.25f,0.0f };
	transformArray[4].pos = { 1280.0f,300.0f,0.0f };
	transformArray[4].scale = { 0.25f,0.25f,0.0f };


	colorArray[0] = { 155,155,155,255 };
	colorArray[1] = { 155,0,0,155 };
	colorArray[2] = { 0,155,0,55 };

	texFlag = true;


	//clearGBuffer.SetBuffer(testRArray[0]->GetDrawData()->buffer[2], GRAPHICS_PRAMTYPE_DATA);


	//ライティングパス
	{
		DispatchComputeShader::ComputeData computeData;
		//設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		computeData.desc = desc;
		//シェーダーのパス
		computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "DefferdRenderLightingPass.hlsl", "CSLightingPass", "cs_6_4", SHADER_TYPE_COMPUTE);

		//ディスパッチのアドレス
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		computeData.dispatchData = &dispatchData;

		gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		finalGBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		//セットするバッファ
		computeData.bufferArray =
		{
			finalGBuffer,
			gBuffer[0],
			gBuffer[1]
		};

		//積む
		compute.Stack(computeData);
	}

	//クリア処理
	{
		DispatchComputeShader::ComputeData computeData;
		//設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		computeData.desc = desc;
		//シェーダーのパス
		computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "ClearGBuffer.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE);

		//ディスパッチのアドレス
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		computeData.dispatchData = &dispatchData;

		gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//セットするバッファ
		computeData.bufferArray =
		{
			gBuffer[0],
			gBuffer[1]
		};

		//積む
		compute.Stack(computeData);
	}



}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
	camera.Init({});
	lightVec = { 0.0f,1.0f,0.0f };
}

void RenderScene::PreInit()
{
}

void RenderScene::Finalize()
{
}

void RenderScene::Input()
{
}

void RenderScene::Update()
{
	camera.Update({}, {}, true);
	CameraMgr::Instance()->Camera(camera.GetEyePos(), camera.GetTargetPos(), { 0.0f,1.0f,0.0f });

	DirectX::XMMATRIX mat(
		transformArray[0].GetMat() *
		CameraMgr::Instance()->GetViewMatrix() *
		CameraMgr::Instance()->GetPerspectiveMatProjection()
	);
	drawSponza.extraBufferArray[0].bufferWrapper->TransData(&mat, sizeof(DirectX::XMMATRIX));
	DirectX::XMFLOAT3 dir = lightVec.ConvertXMFLOAT3();
	drawSponza.extraBufferArray[1].bufferWrapper->TransData(&dir, sizeof(DirectX::XMFLOAT3));


	//描画命令
	if (KeyBoradInputManager::Instance()->InputState(DIK_SPACE))
	{
		rasterizeRenderer.ObjectRender(drawSponza);
	}

	rasterizeRenderer.Sort();
	//compute.Update();
}

void RenderScene::Draw()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	//testRArray[0]->DrawCall(transformArray[0], colorArray[0], 0, motherMat);
	//testRArray[1]->DrawOBJ(transformArray[1], 1.0f, 0, motherMat);
	//testRArray[2]->DrawTexPlane(transformArray[2], colorArray[2], 0, motherMat);

	//normalGBufferRender->DrawTexPlane(transformArray[3], colorArray[2], 0, motherMat);
	//finalGBufferRender->DrawTexPlane(transformArray[4], colorArray[2], 0, motherMat);

	//compute.Compute();
	rasterizeRenderer.Render();

	ImGui::Begin("Light");
	ImGui::DragFloat("VecX", &lightVec.x);
	ImGui::DragFloat("VecY", &lightVec.y);
	ImGui::DragFloat("VecZ", &lightVec.z);
	ImGui::End();
}

int RenderScene::SceneChange()
{
	return 0;
}
