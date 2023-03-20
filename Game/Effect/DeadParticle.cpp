#include"DeadParticle.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"

DeadParticle::DeadParticle(const ResouceBufferHelper::BufferData &ADDRESS, int VERT_NUM, const GPUParticleRender *RENDER_PTR, float PARTICLE_SCALE)
{
	PARTICLE_MAX_NUM = VERT_NUM;

	data.vertData = ADDRESS;
	data.triagnleData.x = 650;
	data.triagnleData.y = 70;
	data.triagnleData.z = 50;
	data.triagnleData.w = 100;
	
	meshParticle = std::make_unique<MeshParticle>(data, 0);


	//バッファ生成-------------------------
	initComputeHelper.SetBuffer(meshParticle->GetBuffer(), GRAPHICS_PRAMTYPE_DATA);

	outputInitBufferHandle = initComputeHelper.CreateBuffer
	(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(OutputInitData) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA2,
		sizeof(OutputInitData),
		PARTICLE_MAX_NUM
	);


	updateComputeHelper.SetBuffer(initComputeHelper.GetBufferData(outputInitBufferHandle), GRAPHICS_PRAMTYPE_DATA);

	commonBufferHandle = updateComputeHelper.CreateBuffer
	(
		sizeof(CommonMoveData),
		GRAPHICS_RANGE_TYPE_CBV_VIEW,
		GRAPHICS_PRAMTYPE_DATA3,
		1
	);

	outputBufferHandle = updateComputeHelper.SetBuffer(RENDER_PTR->GetStackBuffer(), GRAPHICS_PRAMTYPE_DATA2);
	//バッファ生成-------------------------



	//初期化処理--------------------------------------------
	initComputeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_DEADPARTICLE_INIT, { static_cast<UINT>(PARTICLE_MAX_NUM / 1000), 1, 1 });


	texHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "Circle.png");

	startFlag = false;

	float lScale = PARTICLE_SCALE;
	scaleRotaMat = KazMath::CaluScaleMatrix({ lScale,lScale,lScale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });

	timer = KazMath::ConvertSecondToFlame(5);
}

void DeadParticle::Init(const DirectX::XMMATRIX *MAT)
{
	startFlag = true;
	motherMat = MAT;
}

void DeadParticle::Update(int ALPHA)
{
	if (!startFlag)
	{
		return;
	}
	--timer;
	if (timer <= 0)
	{
		//startFlag = false;
	}

	constBufferData.scaleRotateBillboardMat = scaleRotaMat * CameraMgr::Instance()->GetMatBillBoard();
	constBufferData.viewProjection = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	constBufferData.motherMat = *motherMat;

	updateComputeHelper.TransData(commonBufferHandle, &constBufferData, sizeof(CommonMoveData));

	updateComputeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_DEADPARTICLE_UPDATE, { 80,1,1 });
}

void DeadParticle::Draw()
{
	if (!startFlag)
	{
		return;
	}
}