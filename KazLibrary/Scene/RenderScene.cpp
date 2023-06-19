#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Buffer/GBufferMgr.h"
#include"../KazLibrary/Render/DrawFunc.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	boxData = boxBuffer.GenerateBoxBuffer(1.0f);

	//G-Buffer生成
	GBufferMgr::Instance();


	RESOURCE_HANDLE handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "hamster.obj");
	ObjResourceMgr::Instance()->GetResourceData(handle);
	model = ModelLoader::Instance()->Load("Resource/Test/glTF/hamster.obj");

	//フォワードレンダリングで描画するモデル
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvmain", "ps_6_4", SHADER_TYPE_PIXEL);

		//描画
		drawSponza = DrawFuncData::SetDrawGLTFIndexMaterialData(*model, lData);
		//その他バッファ
		drawSponza.extraBufferArray.emplace_back(KazBufferHelper::BufferData(KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT3))));
		drawSponza.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawSponza.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawSponza.extraBufferArray.back().bufferSize = sizeof(DirectX::XMFLOAT3);
	}

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

	CoordinateSpaceMatData transData(transformArray[0].GetMat(), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
	drawSponza.extraBufferArray[0].bufferWrapper->TransData(&transData, sizeof(CoordinateSpaceMatData));
	DirectX::XMFLOAT3 dir = lightVec.ConvertXMFLOAT3();
	drawSponza.extraBufferArray[1].bufferWrapper->TransData(&dir, sizeof(DirectX::XMFLOAT3));

	//法線描画
	//compute.Update();
}

void RenderScene::Draw()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	rasterizeRenderer.ObjectRender(drawSponza);


	rasterizeRenderer.Sort();
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
