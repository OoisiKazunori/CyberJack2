#include"DebugScene.h"
#include"../Camera/CameraMgr.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Input/ControllerInputManager.h"
#include"../Math/KazMath.h"
#include"../Helper/KazHelper.h"
#include"../Helper/KazBufferHelper.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../Imgui/MyImgui.h"

DebugScene::DebugScene() :bulr({ WIN_X,WIN_Y })
{
	//short texHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "");
	srvHandle = 0;
	buffer = std::make_unique<CreateGpuBuffer>();


	std::vector<MultiRenderTargetData> renderData;
	renderData.push_back(MultiRenderTargetData());
	renderData.push_back(MultiRenderTargetData());
	renderData[0].graphSize = { WIN_X,WIN_Y };
	renderData[0].backGroundColor = BG_COLOR;
	renderData[1].graphSize = { WIN_X,WIN_Y };
	renderData[1].backGroundColor = { 0.0f,0.0f,0.0f };

	std::vector<RESOURCE_HANDLE> handles =
		RenderTargetStatus::Instance()->CreateMultiRenderTarget(renderData, DXGI_FORMAT_R8G8B8A8_UNORM);
	mainHandle = handles[0];
	addHandle = handles[1];
	lumiHandle = RenderTargetStatus::Instance()->CreateRenderTarget({ WIN_X,WIN_Y }, { 0.0f,0.0f,0.0f }, DXGI_FORMAT_R8G8B8A8_UNORM);

	mainRender.data.handleData = mainHandle;
	mainRender.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };

	lumiRender.data.pipelineName = PIPELINE_NAME_SPRITE_LUMI;
	lumiRender.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	lumiRender.data.handleData = mainHandle;
	lumiRender.data.addHandle.handle[0] = addHandle;
	lumiRender.data.addHandle.paramType[0] = GRAPHICS_PRAMTYPE_TEX2;


	addRender.data.handleData = lumiHandle;
	addRender.data.pipelineName = PIPELINE_NAME_ADDBLEND;
	addRender.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };


	//CommandBuffer---------------------------
	std::array<D3D12_INDIRECT_ARGUMENT_DESC, 2> args{};
	args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
	args[0].UnorderedAccessView.RootParameterIndex = 0;
	args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

	//CommandArgument------------------------
	D3D12_COMMAND_SIGNATURE_DESC desc{};
	desc.pArgumentDescs = args.data();
	desc.NumArgumentDescs = static_cast<UINT>(args.size());
	desc.ByteStride = sizeof(IndirectCommand);
	//CommandArgument------------------------

	DirectX12Device::Instance()->dev->CreateCommandSignature(&desc, GraphicsRootSignature::Instance()->GetRootSignature(ROOTSIGNATURE_DATA_DRAW_UAV).Get(), IID_PPV_ARGS(&commandSig));
	//CommandArgument---------------------------

	//Parameter---------------------------
	const UINT constantBufferDataSize = TRIANGLE_RESOURCE_COUNT * sizeof(SceneConstantBuffer);
	paramCBHandle = buffer->CreateBuffer(KazBufferHelper::SetConstBufferData(constantBufferDataSize));
	{
		for (UINT n = 0; n < TRIANGLE_ARRAY_NUM; n++)
		{
			constantBufferData[n].velocity = DirectX::XMFLOAT4(KazMath::Rand<float>(0.01f, 0.02f), 0.0f, 0.0f, 0.0f);
			constantBufferData[n].offset = DirectX::XMFLOAT4(KazMath::Rand<float>(-5.0f, -1.5f), KazMath::Rand<float>(-1.0f, 1.0f), KazMath::Rand<float>(0.0f, 2.0f), 0.0f);
			constantBufferData[n].color = DirectX::XMFLOAT4(KazMath::Rand<float>(1.0f, 0.0f), KazMath::Rand<float>(0.0f, 1.0f), KazMath::Rand<float>(0.0f, 1.0f), 1.0f);

			float m_aspectRatio = static_cast<float>(WIN_X) / static_cast<float>(WIN_Y);
			XMStoreFloat4x4(&constantBufferData[n].projection, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, m_aspectRatio, 0.01f, 20.0f)));
		}

		pointer = buffer->GetMapAddres(paramCBHandle);
		memcpy(pointer, &constantBufferData[0], TRIANGLE_ARRAY_NUM * sizeof(SceneConstantBuffer));

		//Buffer
		cbvSize = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_CBV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = TRIANGLE_ARRAY_NUM;
		srvDesc.Buffer.StructureByteStride = sizeof(SceneConstantBuffer);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		cbvHandle = 0;
		for (UINT frame = 0; frame < FRAME_COUNT; frame++)
		{
			srvDesc.Buffer.FirstElement = frame * TRIANGLE_ARRAY_NUM;
			DescriptorHeapMgr::Instance()->CreateBufferView(cbvSize.startSize + frame, srvDesc, buffer->GetBufferData(paramCBHandle).Get());
			++cbvHandle;
		}
	}
	//Parameter---------------------------


	//VertexBuffer------------------------
	{
		const float TriangleDepth = 0.0f;
		struct Vert
		{
			DirectX::XMFLOAT3 pos;
		};
		std::array<Vert, 3>vert;
		vert[0].pos = { 0.0f, 0.5f, TriangleDepth };
		vert[1].pos = { 0.5f, 0.0f, TriangleDepth };
		vert[2].pos = { -0.5f, 0.0f, TriangleDepth };

		int size = static_cast<int>(vert.size()) * sizeof(Vert);
		RESOURCE_HANDLE handle = buffer->CreateBuffer(KazBufferHelper::SetVertexBufferData(size));
		buffer->TransData(handle, vert.data(), size);

		vertexBufferView = KazBufferHelper::SetVertexBufferView(buffer->GetGpuAddress(handle), size, sizeof(vert[0]));
	}
	//VertexBuffer-------------------------


	//CommandBuffer-------------------------
	{
		std::array<IndirectCommand, TRIANGLE_RESOURCE_COUNT> commands;
		const UINT commandBufferSize = CommandSizePerFrame * FRAME_COUNT;
		commandBufferHandle = buffer->CreateBuffer(KazBufferHelper::SetCommandBufferData(commandBufferSize));
		RESOURCE_HANDLE uplod = buffer->CreateBuffer(KazBufferHelper::SetStructureBuffer(commandBufferSize));

		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGpuAddress(paramCBHandle);
		UINT commandIndex = 0;
		for (UINT frame = 0; frame < FRAME_COUNT; frame++)
		{
			for (UINT n = 0; n < TRIANGLE_ARRAY_NUM; n++)
			{
				commands[commandIndex].cbv = gpuAddress;
				commands[commandIndex].drawArguments.VertexCountPerInstance = 3;
				commands[commandIndex].drawArguments.InstanceCount = 1;
				commands[commandIndex].drawArguments.StartVertexLocation = 0;
				commands[commandIndex].drawArguments.StartInstanceLocation = 0;

				commandIndex++;
				gpuAddress += sizeof(SceneConstantBuffer);
			}
		}
		//CommandBuffer-------------------------

		D3D12_SUBRESOURCE_DATA commandData = {};
		commandData.pData = reinterpret_cast<UINT8 *>(&commands[0]);
		commandData.RowPitch = commandBufferSize;
		commandData.SlicePitch = commandData.RowPitch;
		UpdateSubresources<1>(DirectX12CmdList::Instance()->cmdList.Get(), buffer->GetBufferData(commandBufferHandle).Get(), buffer->GetBufferData(uplod).Get(), 0, 0, 1, &commandData);

		{
			//CommandBuffer,SRV------------------------
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = TRIANGLE_ARRAY_NUM;
			srvDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			for (int i = 0; i < FRAME_COUNT; ++i)
			{
				srvDesc.Buffer.FirstElement = i * TRIANGLE_ARRAY_NUM;
				BufferMemorySize s = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_SRV);
				DescriptorHeapMgr::Instance()->CreateBufferView(s.startSize + i, srvDesc, buffer->GetBufferData(commandBufferHandle).Get());
				++srvHandle;
			}
			//CommandBuffer,SRV------------------------
		}

#pragma region ComputeShader


		//InputBuffer-------------------------
		{
			inputHandle = buffer->CreateBuffer(KazBufferHelper::SetStructureBuffer(TRIANGLE_ARRAY_NUM * sizeof(UpdateData), "InputParam"));

			std::array<UpdateData, TRIANGLE_ARRAY_NUM> data;
			for (int i = 0; i < TRIANGLE_ARRAY_NUM; ++i)
			{
				data[i].pos = { 0.0f + static_cast<float>(i) * 10.0f,10.0f,10.0f,0.0f };
				data[i].velocity = { 0.0f,0.0f,0.0f,0.0f };
				data[i].color = { KazMath::Rand<float>(1.0f,0.0f),KazMath::Rand<float>(1.0f,0.0f),KazMath::Rand<float>(1.0f,0.0f),1.0f };
			}

			buffer->TransData(inputHandle, data.data(), TRIANGLE_ARRAY_NUM * sizeof(UpdateData));

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = TRIANGLE_ARRAY_NUM;
			srvDesc.Buffer.StructureByteStride = sizeof(UpdateData);
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			BufferMemorySize s = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_SRV);
			DescriptorHeapMgr::Instance()->CreateBufferView(s.startSize + srvHandle, srvDesc, buffer->GetBufferData(inputHandle).Get());
			++srvHandle;
		}
		//InputBuffer-------------------------


		computeMemSize = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER);

		int uavHandle = 0;

		BUFFER_SIZE outputBufferSize = static_cast<BUFFER_SIZE>(TRIANGLE_ARRAY_NUM * sizeof(OutPutData));
		//BUFFER_SIZE inputBufferSize = static_cast<BUFFER_SIZE>(TRIANGLE_ARRAY_NUM * sizeof(UpdateData));
		//BUFFER_SIZE commonBufferSize = static_cast<BUFFER_SIZE>(TRIANGLE_ARRAY_NUM * sizeof(CommonData));
		//BUFFER_SIZE drawIndirectBufferSize = static_cast<BUFFER_SIZE>(TRIANGLE_ARRAY_NUM * sizeof(IndirectCommand));

		{
			counterBufferHandle = buffer->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(UINT)));
			UINT num = 0;
			buffer->TransData(counterBufferHandle, &num, sizeof(UINT));
		}

		//OutputBuffer---------------------------
		{
			outputMatHandle = buffer->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(outputBufferSize));

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = TRIANGLE_ARRAY_NUM;
			uavDesc.Buffer.StructureByteStride = sizeof(OutPutData);
			uavDesc.Buffer.CounterOffsetInBytes = 0;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			DescriptorHeapMgr::Instance()->CreateBufferView(computeMemSize.startSize + uavHandle, uavDesc, buffer->GetBufferData(outputMatHandle).Get(), buffer->GetBufferData(counterBufferHandle).Get());
		}
		++uavHandle;


		//UpdateBuffer
		{
			updateHandle = buffer->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(UpdateData) * TRIANGLE_ARRAY_NUM));

			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = TRIANGLE_ARRAY_NUM;
			uavDesc.Buffer.StructureByteStride = sizeof(UpdateData);
			uavDesc.Buffer.CounterOffsetInBytes = 0;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

			DescriptorHeapMgr::Instance()->CreateBufferView(computeMemSize.startSize + uavHandle, uavDesc, buffer->GetBufferData(updateHandle).Get(), nullptr);
		}
		++uavHandle;

		commonHandle = buffer->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(CommonData), "CommonData"));
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = 1;
			srvDesc.Buffer.StructureByteStride = sizeof(CommonData);
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			//DescriptorHeapMgr::Instance()->CreateBufferView(cbvSize.startSize + cbvHandle, srvDesc, buffer->GetBufferData(commonHandle).Get());
			++cbvHandle;
		}


		{
			cbvMatHandle = buffer->CreateBuffer(KazBufferHelper::SetConstBufferData(outputBufferSize, "CbvMatHandle"));

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Buffer.NumElements = TRIANGLE_ARRAY_NUM;
			srvDesc.Buffer.StructureByteStride = sizeof(OutPutData);
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			DescriptorHeapMgr::Instance()->CreateBufferView(cbvSize.startSize + cbvHandle, srvDesc, buffer->GetBufferData(cbvMatHandle).Get());
			++cbvHandle;
		}

		//DrawIndirect
		{
			drawCommandHandle = buffer->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(IndirectCommand) * DRAW_CALL, "DrawCommand"));

			std::array<IndirectCommand, DRAW_CALL> lCommands;
			D3D12_GPU_VIRTUAL_ADDRESS cbGpuAddress = buffer->GetGpuAddress(outputMatHandle);
			for (int i = 0; i < lCommands.size(); ++i)
			{
				lCommands[i].cbv = cbGpuAddress;
				lCommands[i].drawArguments.VertexCountPerInstance = 3;
				lCommands[i].drawArguments.InstanceCount = TRIANGLE_ARRAY_NUM;
				lCommands[i].drawArguments.StartVertexLocation = 0;
				lCommands[i].drawArguments.StartInstanceLocation = 0;
			}
			buffer->TransData(drawCommandHandle, &lCommands, sizeof(IndirectCommand) *DRAW_CALL);


			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = 1;
			uavDesc.Buffer.StructureByteStride = sizeof(IndirectCommand);
			uavDesc.Buffer.CounterOffsetInBytes = 0;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			DescriptorHeapMgr::Instance()->CreateBufferView(computeMemSize.startSize + uavHandle, uavDesc, buffer->GetBufferData(drawCommandHandle).Get());
			++uavHandle;

		}
		//DrawInDirect---------------------------
#pragma endregion
	}

	seed = 0;
}

DebugScene::~DebugScene()
{
}

void DebugScene::Init()
{
}

void DebugScene::Finalize()
{
}

void DebugScene::Update()
{
	CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f });


	//ComputeShader------------------------
	//GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_TEST);

	UINT num = 0;
	buffer->TransData(counterBufferHandle, &num, sizeof(UINT));

	{
		computeMemSize = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER);
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(computeMemSize.startSize + 0));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(computeMemSize.startSize + 1));
	}

	//Common
	{
		std::array<CommonData, 1> commonData;
		for (int i = 0; i < commonData.size(); ++i)
		{
			commonData[i].cameraMat = CameraMgr::Instance()->GetViewMatrix();
			commonData[i].projectionMat = CameraMgr::Instance()->GetPerspectiveMatProjection();
			commonData[i].increSize = sizeof(OutPutData);
			commonData[i].gpuAddress = buffer->GetGpuAddress(outputMatHandle);
			commonData[i].emittPos = { 0.0f,0.0f,0.0f,0.0f };
			commonData[i].seed = seed;
		}
		seed = KazMath::Rand<int>(100, 0);
		buffer->TransData(commonHandle, commonData.data(), sizeof(CommonData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffer->GetGpuAddress(commonHandle));
	}

	DirectX12CmdList::Instance()->cmdList->Dispatch(TRIANGLE_ARRAY_NUM, 1, 1);

}

void DebugScene::Draw()
{

	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	{
		//RESOURCE_HANDLE commandBuffHandle = commandBufferHandle;
		RESOURCE_HANDLE commandBuffHandle = drawCommandHandle;


		int num = RenderTargetStatus::Instance()->bbIndex;
		RenderTargetStatus::Instance()->ChangeBarrier(
			RenderTargetStatus::Instance()->backBuffers[num].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);

		//Clear-------------------------
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
		rtvH = RenderTargetStatus::Instance()->rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += num * DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(RenderTargetStatus::Instance()->heapDesc.Type);
		DirectX12CmdList::Instance()->cmdList->OMSetRenderTargets(1, &rtvH, false, &RenderTargetStatus::Instance()->gDepth.dsvH[RenderTargetStatus::Instance()->handle]);
		RenderTargetStatus::Instance()->gDepth.Clear(RenderTargetStatus::Instance()->handle);
		RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);
		//Clear-------------------------


		RenderTargetStatus::Instance()->PrepareToChangeBarrier(mainHandle);
		RenderTargetStatus::Instance()->ClearRenderTarget(mainHandle);


		RenderTargetStatus::Instance()->ChangeBarrier(
			buffer->GetBufferData(commandBuffHandle).Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
		);

		GraphicsPipeLineMgr::Instance()->SetPipeLineAndRootSignature(PIPELINE_NAME_GPUPARTICLE);
		DirectX12CmdList::Instance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		DirectX12CmdList::Instance()->cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);

		//PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, L"Cull invisible triangles");
		DirectX12CmdList::Instance()->cmdList->ExecuteIndirect
		(
			commandSig.Get(),
			DRAW_CALL,
			buffer->GetBufferData(commandBuffHandle).Get(),
			0,
			buffer->GetBufferData(counterBufferHandle).Get(),
			0
		);
		//PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

		RenderTargetStatus::Instance()->ChangeBarrier(
			buffer->GetBufferData(commandBuffHandle).Get(),
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);


		bg.Draw();


		RenderTargetStatus::Instance()->PrepareToChangeBarrier(lumiHandle, mainHandle);
		RenderTargetStatus::Instance()->ClearRenderTarget(lumiHandle);
		lumiRender.Draw();
		RenderTargetStatus::Instance()->PrepareToCloseBarrier(lumiHandle);
		RenderTargetStatus::Instance()->SetDoubleBufferFlame();

		mainRender.Draw();

		addRender.data.handleData = bulr.BlurImage(lumiHandle);
		addRender.Draw();


		RenderTargetStatus::Instance()->ChangeBarrier(
			RenderTargetStatus::Instance()->backBuffers[num].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		//DrawIndirect------------------------

		RenderTargetStatus::Instance()->SwapResourceBarrier();
	}
}

void DebugScene::Input()
{
	KeyBoradInputManager *input = KeyBoradInputManager::Instance();

#pragma region Camera
	debugCameraMove = { 0,0,0 };
	float debugSpeed = 1.0f;
	if (input->InputState(DIK_D))
	{
		debugCameraMove.x = -debugSpeed;
	}
	if (input->InputState(DIK_A))
	{
		debugCameraMove.x = debugSpeed;
	}
	if (input->InputState(DIK_W))
	{
		debugCameraMove.y = debugSpeed;
	}
	if (input->InputState(DIK_S))
	{
		debugCameraMove.y = -debugSpeed;
	}

	if (input->InputState(DIK_RIGHTARROW))
	{
		angle.x += debugSpeed;
	}
	if (input->InputState(DIK_LEFTARROW))
	{
		angle.x += -debugSpeed;
	}

	if (input->InputState(DIK_UPARROW))
	{
		angle.y += debugSpeed;
	}
	if (input->InputState(DIK_DOWNARROW))
	{
		angle.y += -debugSpeed;
	}

	//eyePos.x = 0.0f;
	//eyePos.y = 0.0f;
	//eyePos.z = -5.0f;

	eyePos = KazMath::CaluEyePosForDebug(eyePos, debugCameraMove, angle);
	targetPos = KazMath::CaluTargetPosForDebug(eyePos, angle.x);

	//eyePos = { 42.0f,0.0f,-148.0f };
	//targetPos = { 42.0f,0.0f,-143.0f };

#pragma endregion

}

int DebugScene::SceneChange()
{
	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_BACK))
	{
		return 0;
	}
	else
	{
		return SCENE_NONE;
	}
}
