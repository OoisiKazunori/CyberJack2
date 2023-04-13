#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	std::array<SpriteVertex, 4>lVerticesArray;
	std::array<USHORT, 6> lIndicesArray;

	BUFFER_SIZE lVertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVerticesArray.size(), sizeof(SpriteVertex));
	BUFFER_SIZE lIndexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndicesArray.size(), sizeof(USHORT));

	KazBufferHelper::BufferResourceData lBufferData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		"CopyCounterBuffer"
	);
	uavMatBuffer.CreateBuffer(lBufferData);

	//パーティクルの頂点情報
	GenerateRect(lVerticesArray, lIndicesArray);

	//テクスチャ付きパーティクルのパイプライン
	GeneratePipeline();

	//コマンドシグネチャ生成開始
	{
		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> args;
		//UAV
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[args.size() - 1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
		args[args.size() - 1].UnorderedAccessView.RootParameterIndex = 0;
		//インデックスあり
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[args.size() - 1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;


		InitDrawIndexedExcuteIndirect data;
		data.vertexBufferView = KazBufferHelper::SetVertexBufferView(vertexBuffer->GetGpuAddress(), lVertBuffSize, sizeof(lVerticesArray[0]));
		data.indexBufferView = KazBufferHelper::SetIndexBufferView(indexBuffer->GetGpuAddress(), lIndexBuffSize);
		data.indexNum = static_cast<UINT>(6);
		data.elementNum = 1;
		data.texHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "tex.png");
		data.updateView = uavMatBuffer.GetBuffer()->GetGPUVirtualAddress();
		data.rootsignature = rootSignatureArray;
		data.argument = args;

		gpuParticleRender = std::make_unique<DrawExcuteIndirect>(data);
	}
}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
	camera.Init({});
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


	//カメラ転送
	{
		OutputData data;
		data.color = { 1.0f,1.0f,1.0f,1.0f };
		data.mat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		uavMatBuffer.TransData(&data, sizeof(OutputData));
	}
}

void RenderScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	//描画
	gpuParticleRender->Draw(PIPELINE_NAME_GPUPARTICLE_TEXCOLOR, nullptr);

}

int RenderScene::SceneChange()
{
	return 0;
}
