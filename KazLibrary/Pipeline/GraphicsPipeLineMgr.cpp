#include "GraphicsPipeLineMgr.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Helper/KazHelper.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

GraphicsPipeLineMgr::GraphicsPipeLineMgr()
{
	geo = false;
}

GraphicsPipeLineMgr::~GraphicsPipeLineMgr()
{
}

void GraphicsPipeLineMgr::RegisterInputLayOutWithData(InputLayOutData INPUT_LAY_OUT, InputLayOutNames NAME)
{
	if (IsitSafe(NAME, inputLayOutRegisterData.size()))
	{
		inputLayOutRegisterData[NAME] = INPUT_LAY_OUT;
	}
	else
	{
		FailCheck("危険:InputLayOutが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::RegisterVertexShaderWithData(string SHADER_FILE, LPCSTR ENTRY_POINT, LPCSTR SHADER_MODEL, VertexShaderNames NAME)
{
	if (IsitSafe(NAME, vertexShaderRegisterData.size()))
	{
		Shader vsShader;
		vsShader.CompileShader(SHADER_FILE, ENTRY_POINT, SHADER_MODEL, SHADER_TYPE_VERTEX);
		vertexShaderRegisterData[NAME] = vsShader.GetShaderData(SHADER_TYPE_VERTEX);
	}
	else
	{
		FailCheck("危険:VertexShaderNamesが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::RegisterPixcelShaderWithData(string SHADER_FILE, LPCSTR ENTRY_POINT, LPCSTR SHADER_MODEL, PixelShaderNames NAME)
{
	if (IsitSafe(NAME, pixcelShaderRegisterData.size()))
	{
		Shader psShader;
		psShader.CompileShader(SHADER_FILE, ENTRY_POINT, SHADER_MODEL, SHADER_TYPE_PIXEL);
		pixcelShaderRegisterData[NAME] = psShader.GetShaderData(SHADER_TYPE_PIXEL);
	}
	else
	{
		FailCheck("危険:PixcelShaderNamesが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::RegisterGeometoryShaderWithData(string SHADER_FILE, LPCSTR ENTRY_POINT, LPCSTR SHADER_MODEL, GeometoryShaderNames NAME)
{
	if (IsitSafe(NAME, geometoryShaderRegisterData.size()))
	{
		Shader gsShader;
		gsShader.CompileShader(SHADER_FILE, ENTRY_POINT, SHADER_MODEL, SHADER_TYPE_GEOMETORY);
		geometoryShaderRegisterData[NAME] = gsShader.GetShaderData(SHADER_TYPE_GEOMETORY);
	}
	else
	{
		FailCheck("危険:GeometryShaderNamesが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::RegisterPipeLineDataWithData(D3D12_GRAPHICS_PIPELINE_STATE_DESC PIPELINE_DATA, PipeLineDataNames NAME)
{
	if (IsitSafe(NAME, pipeLineDataRegisterData.size()))
	{
		pipeLineDataRegisterData[NAME] = PIPELINE_DATA;
	}
	else
	{
		FailCheck("危険:PipeLineDataRegisterDataが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::RegisterComputePipeLineDataWithData(D3D12_COMPUTE_PIPELINE_STATE_DESC PIPELINE_DATA, ComputePipeLineDataNames NAME)
{
	if (IsitSafe(NAME, computePipeLineDataRegisterData.size()))
	{
		computePipeLineDataRegisterData[NAME] = PIPELINE_DATA;
	}
	else
	{
		FailCheck("危険:computePipeLineDataRegisterDataが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::CreatePipeLine(InputLayOutNames INPUT_LAYOUT_NAME, VertexShaderNames VERTEX_SHADER_NAME, PixelShaderNames PIXCEL_SHADER_NAME, PipeLineDataNames PIPELINE_DATA_NAME, RootSignatureMode ROOTSIGNATURE, PipeLineNames PIPELINE_NAME, GeometoryShaderNames GEOMETORY_SHADER_NAME)
{
	//パイプラインデータの代入
	D3D12_GRAPHICS_PIPELINE_STATE_DESC grahicsPipeLine;
	grahicsPipeLine = pipeLineDataRegisterData[PIPELINE_DATA_NAME];


	//インプットレイアウトの代入
	grahicsPipeLine.InputLayout.pInputElementDescs = inputLayOutRegisterData[INPUT_LAYOUT_NAME].inputLayOut;
	grahicsPipeLine.InputLayout.NumElements = inputLayOutRegisterData[INPUT_LAYOUT_NAME].size;

	//Shaderの代入
	grahicsPipeLine.VS = CD3DX12_SHADER_BYTECODE(vertexShaderRegisterData[VERTEX_SHADER_NAME]->GetBufferPointer(), vertexShaderRegisterData[VERTEX_SHADER_NAME]->GetBufferSize());
	grahicsPipeLine.PS = CD3DX12_SHADER_BYTECODE(pixcelShaderRegisterData[PIXCEL_SHADER_NAME]->GetBufferPointer(), pixcelShaderRegisterData[PIXCEL_SHADER_NAME]->GetBufferSize());



	if (GEOMETORY_SHADER_NAME != SHADER_GEOMETORY_NONE)
	{
		grahicsPipeLine.GS = CD3DX12_SHADER_BYTECODE(geometoryShaderRegisterData[GEOMETORY_SHADER_NAME]->GetBufferPointer(), geometoryShaderRegisterData[GEOMETORY_SHADER_NAME]->GetBufferSize());
	}

	//ルートシグネチャの設定
	rootSignatureName[PIPELINE_NAME] = ROOTSIGNATURE;
	grahicsPipeLine.pRootSignature = GraphicsRootSignature::Instance()->GetRootSignature(ROOTSIGNATURE).Get();


	//パイプラインの生成
	if (IsitSafe(PIPELINE_NAME, pipeLineRegisterData.size()))
	{
		HRESULT lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&grahicsPipeLine, IID_PPV_ARGS(&pipeLineRegisterData[PIPELINE_NAME]));
		if (lResult != S_OK)
		{
			assert(0);
		}
	}
	else
	{
		FailCheck("危険:Pipelineが登録できませんでした");
	}
}

bool GraphicsPipeLineMgr::SetPipeLineAndRootSignature(PipeLineNames PIPELINE_NAME)
{
	if (IsitSafe(PIPELINE_NAME, pipeLineRegisterData.size()) && pipeLineRegisterData[PIPELINE_NAME].Get() != nullptr)
	{
		GraphicsRootSignature::Instance()->SetRootSignature(rootSignatureName[PIPELINE_NAME]);
		DirectX12CmdList::Instance()->cmdList->SetPipelineState(pipeLineRegisterData[PIPELINE_NAME].Get());
		DescriptorHeapMgr::Instance()->SetDescriptorHeap();
		return true;
	}
	else
	{
		FailCheck("危険:PipeLineが存在しない為、コマンドリストに積めませんでした");
		return false;
	}
}

bool GraphicsPipeLineMgr::SetComputePipeLineAndRootSignature(ComputePipeLineNames PIPELINE_NAME)
{
	//if (IsitSafe(PIPELINE_NAME, computePipeLineRegisterData.size()) && computePipeLineRegisterData[PIPELINE_NAME].Get() != nullptr)
	//{
	//	GraphicsRootSignature::Instance()->SetComputeRootSignature(computeRootSignatureName[PIPELINE_NAME]);
	//	DirectX12CmdList::Instance()->cmdList->SetPipelineState(computePipeLineRegisterData[PIPELINE_NAME].Get());
	//	return true;
	//}
	//else
	//{
	//	FailCheck("危険:ComputePipeLineが存在しない為、コマンドリストに積めませんでした");
	//	return false;
	//}

	if (IsitSafe(PIPELINE_NAME, computePipeLineRegisterData.size()) && computePipeLineRegisterData[PIPELINE_NAME].Get() != nullptr)
	{
		DirectX12CmdList::Instance()->cmdList->SetComputeRootSignature(rootSignature[PIPELINE_NAME].Get());
		DirectX12CmdList::Instance()->cmdList->SetPipelineState(computePipeLineRegisterData[PIPELINE_NAME].Get());
		return true;
	}
	else
	{
		FailCheck("危険:ComputePipeLineが存在しない為、コマンドリストに積めませんでした");
		return false;
	}
}

//bool GraphicsPipeLineMgr::SetComputePipeLineAndRootSignature2(ComputePipeLineNames PIPELINE_NAME)
//{
//	if (IsitSafe(PIPELINE_NAME, computePipeLineRegisterData.size()) && computePipeLineRegisterData[PIPELINE_NAME].Get() != nullptr)
//	{
//		DirectX12CmdList::Instance()->cmdList->SetComputeRootSignature(rootSignature[PIPELINE_NAME].Get());
//		DirectX12CmdList::Instance()->cmdList->SetPipelineState(computePipeLineRegisterData[PIPELINE_NAME].Get());
//		return true;
//	}
//	else
//	{
//		FailCheck("危険:ComputePipeLineが存在しない為、コマンドリストに積めませんでした");
//		return false;
//	}
//}

RootSignatureMode GraphicsPipeLineMgr::GetRootSignatureName(PipeLineNames PIPELINE_NAME)
{
	return rootSignatureName[PIPELINE_NAME];
}

void GraphicsPipeLineMgr::CreatePipeLine(
	std::vector<D3D12_INPUT_ELEMENT_DESC> INPUT_LAYOUT_NAME,
	const ShaderOptionData &VERTEX_SHADER_NAME,
	const ShaderOptionData &PIXEL_SHADER_NAME,
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC &PIPELINE_DATA_NAME,
	const RootSignatureDataTest &ROOTSIGNATURE,
	PipeLineNames PIPELINE_NAME,
	const ShaderOptionData &GEOMETORY_SHADER_NAME
)
{
	//パイプラインデータの代入
	D3D12_GRAPHICS_PIPELINE_STATE_DESC grahicsPipeLine = PIPELINE_DATA_NAME;

	//インプットレイアウトの代入
	grahicsPipeLine.InputLayout.pInputElementDescs = INPUT_LAYOUT_NAME.data();
	grahicsPipeLine.InputLayout.NumElements = static_cast<UINT>(INPUT_LAYOUT_NAME.size());

	Shader lShader;
	IDxcBlob *lVS = lShader.CompileShader(VERTEX_SHADER_NAME);
	IDxcBlob *lPS = lShader.CompileShader(PIXEL_SHADER_NAME);
	//Shaderの代入
	grahicsPipeLine.VS = CD3DX12_SHADER_BYTECODE(lVS->GetBufferPointer(), lVS->GetBufferSize());
	grahicsPipeLine.PS = CD3DX12_SHADER_BYTECODE(lPS->GetBufferPointer(), lPS->GetBufferSize());

	if (GEOMETORY_SHADER_NAME.fileName != "")
	{
		IDxcBlob *lGS = lShader.CompileShader(GEOMETORY_SHADER_NAME);
		grahicsPipeLine.GS = CD3DX12_SHADER_BYTECODE(lGS->GetBufferPointer(), lGS->GetBufferSize());
	}

	//ルートシグネチャの設定
	rootSignature[PIPELINE_NAME] = GraphicsRootSignature::Instance()->CreateRootSignature(ROOTSIGNATURE, ROOTSIGNATURE_GRAPHICS).Get();
	grahicsPipeLine.pRootSignature = rootSignature[PIPELINE_NAME].Get();

	//パイプラインの生成
	if (IsitSafe(PIPELINE_NAME, pipeLineRegisterData.size()))
	{
		HRESULT lResult = DirectX12Device::Instance()->dev->CreateGraphicsPipelineState(&grahicsPipeLine, IID_PPV_ARGS(&pipeLineRegisterData[PIPELINE_NAME]));
		assert(lResult == S_OK);
	}
	else
	{
		FailCheck("危険:Pipelineが登録できませんでした");
	}
}

void GraphicsPipeLineMgr::CreateComputePipeLine(
	const ShaderOptionData &COMPUTE_SHADER,
	const D3D12_COMPUTE_PIPELINE_STATE_DESC &PIPELINE_DATA_NAME,
	const RootSignatureDataTest &ROOTSIGNATURE,
	ComputePipeLineNames PIPELINE_NAME)
{
	//パイプラインデータの代入
	D3D12_COMPUTE_PIPELINE_STATE_DESC lComputePipeLine = PIPELINE_DATA_NAME;

	Shader lShader;
	IDxcBlob *lCS = lShader.CompileShader(COMPUTE_SHADER);
	//Shaderの代入
	lComputePipeLine.CS = CD3DX12_SHADER_BYTECODE(lCS->GetBufferPointer(), lCS->GetBufferSize());

	//ルートシグネチャの設定
	rootSignature[PIPELINE_NAME] = GraphicsRootSignature::Instance()->CreateRootSignature(ROOTSIGNATURE, ROOTSIGNATURE_COMPUTE).Get();
	lComputePipeLine.pRootSignature = rootSignature[PIPELINE_NAME].Get();

	//パイプラインの生成
	if (IsitSafe(PIPELINE_NAME, pipeLineRegisterData.size()))
	{
		HRESULT lResult = DirectX12Device::Instance()->dev->CreateComputePipelineState(&lComputePipeLine, IID_PPV_ARGS(&computePipeLineRegisterData[PIPELINE_NAME]));
		assert(lResult == S_OK);
	}
	else
	{
		FailCheck("危険:Pipelineが登録できませんでした");
	}
}

template <typename T>
inline bool GraphicsPipeLineMgr::IsitSafe(T NAME, size_t SIZE)
{
	if (0 <= NAME && NAME < SIZE)
	{
		return true;
	}
	else
	{
		return false;
	}
}
