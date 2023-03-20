#include"FbxModelRender.h"
#include"../Helper/KazBufferHelper.h"
#include"../Buffer/DescriptorHeapMgr.h"

FbxModelRender::FbxModelRender()
{
	gpuBuffer = std::make_unique<CreateGpuBuffer>();
	constBufferHandle[0] = CreateConstBuffer(sizeof(ConstBufferData), typeid(ConstBufferData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DRAW);
	constBufferHandle[1] = CreateConstBuffer(sizeof(ConstBufferDataSkin), typeid(ConstBufferDataSkin).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_SKINING);

	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);


	ConstBufferDataSkin *constMap = nullptr;
	gpuBuffer->GetBufferData(constBufferHandle[1])->Map(0, nullptr, (void **)&constMap);
	for (int i = 0; i < MAX_BONES; i++)
	{
		constMap->bones[i] = DirectX::XMMatrixIdentity();
	}
	gpuBuffer->GetBufferData(constBufferHandle[1])->Unmap(0, nullptr);

	removeSkining = false;
}

void FbxModelRender::Draw(bool DRAE_FLAG)
{
	if (data.handle.flag.Dirty())
	{
		resourceData = FbxModelResourceMgr::Instance()->GetResourceData(data.handle.handle);
		//drawIndexInstanceCommandData = KazRenderHelper::SetDrawIndexInstanceCommandData(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, resourceData->vertexBufferView, resourceData->indexBufferView, resourceData->indicisNum, 1);
		drawInstanceCommandData = KazRenderHelper::SetDrawInstanceCommandData(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, resourceData->vertexBufferView, resourceData->vertNum, 1);
	}


	if (data.handle.handle != -1)
	{
		if (resourceData->startTime.size() != 0 && !data.stopAnimationFlag)
		{
			if (data.isPlayFlag)
			{
				currentTime += frameTime;
				if (currentTime > resourceData->endTime[data.animationNumber])
				{
					currentTime = resourceData->endTime[data.animationNumber];
				}
			}
			else if (data.isReverseFlag)
			{
				currentTime -= frameTime;
				if (currentTime <= resourceData->startTime[data.animationNumber])
				{
					currentTime = resourceData->startTime[data.animationNumber];
				}
			}
			else
			{
				currentTime = resourceData->startTime[data.animationNumber];
			}
		}

		//パイプライン設定-----------------------------------------------------------------------------------------------------
		renderData.pipelineMgr->SetPipeLineAndRootSignature(data.pipelineName);
		//パイプライン設定-----------------------------------------------------------------------------------------------------


		//行列計算-----------------------------------------------------------------------------------------------------
		if (data.transform.Dirty() || data.motherMat.dirty.Dirty())
		{
			CaluMat();
		}
		//行列計算-----------------------------------------------------------------------------------------------------

		if (!DRAE_FLAG)
		{
			return;
		}

		//バッファの転送-----------------------------------------------------------------------------------------------------
		//行列
		if (renderData.cameraMgrInstance->ViewAndProjDirty(data.cameraIndex.id) || data.transform.Dirty() || data.motherMat.dirty.Dirty() || data.cameraIndex.dirty.Dirty() || data.colorData.Dirty())
		{
			ConstBufferData constMap;
			constMap.world = baseMatWorldData.matWorld;
			constMap.view = renderData.cameraMgrInstance->GetViewMatrix(data.cameraIndex.id);
			constMap.viewproj = renderData.cameraMgrInstance->GetPerspectiveMatProjection(data.cameraIndex.id);
			constMap.color = data.colorData.ConvertColorRateToXMFLOAT4();
			constMap.mat = constMap.world * constMap.view * constMap.viewproj;
			TransData(&constMap, constBufferHandle[0], typeid(constMap).name());
		}


		if ((data.isPlayFlag || data.isReverseFlag) && !removeSkining)
		{
			ConstBufferDataSkin *lConstMap = nullptr;
			gpuBuffer->GetBufferData(constBufferHandle[1])->Map(0, nullptr, (void **)&lConstMap);

			if (resourceData->bone.size() != 0)
			{
				for (int i = 0; i < resourceData->bone.size(); i++)
				{
					FbxSkin *bonePtr = FbxModelResourceMgr::Instance()->boneSkinArray[data.handle.handle];
					FbxAMatrix lFbxCurrentPose = bonePtr->GetCluster(i)->GetLink()->EvaluateGlobalTransform(currentTime);
					DirectX::XMMATRIX lMatCurrentPose;
					KazMath::ConvertMatrixFromFbx(&lMatCurrentPose, lFbxCurrentPose);

					if (resourceData->startTime.size() == 0)
					{
						lConstMap->bones[i] = DirectX::XMMatrixIdentity();
					}
					else
					{
						lConstMap->bones[i] = resourceData->bone[i].invInitialPose * lMatCurrentPose;
					}
				}
			}
			else if (resourceData->bone.size() != 0)
			{
				for (int i = 0; i < resourceData->bone.size(); i++)
				{
					FbxSkin *bonePtr = FbxModelResourceMgr::Instance()->boneSkinArray[data.handle.handle];
					FbxAMatrix lFbxCurrentPose = bonePtr->GetCluster(i)->GetLink()->EvaluateGlobalTransform(currentTime);
					DirectX::XMMATRIX lMatCurrentPose;
					KazMath::ConvertMatrixFromFbx(&lMatCurrentPose, lFbxCurrentPose);

					if (resourceData->startTime.size() == 0)
					{
						lConstMap->bones[i] = DirectX::XMMatrixIdentity();
					}
					else
					{
						lConstMap->bones[i] = resourceData->bone[i].invInitialPose * lMatCurrentPose;
					}
				}
			}
			gpuBuffer->GetBufferData(constBufferHandle[1])->Unmap(0, nullptr);
		}
		else if (resourceData->bone.size() != 0 && !removeSkining)
		{
			ConstBufferDataSkin *lConstMap = nullptr;
			gpuBuffer->GetBufferData(constBufferHandle[1])->Map(0, nullptr, (void **)&lConstMap);

			for (int i = 0; i < resourceData->bone.size(); i++)
			{
				DirectX::XMMATRIX lMatCurrentPose;
				FbxAMatrix lFbxCurrentPose = resourceData->bone[i].fbxSkin->GetCluster(i)->GetLink()->EvaluateGlobalTransform(currentTime);
				KazMath::ConvertMatrixFromFbx(&lMatCurrentPose, lFbxCurrentPose);
				if (resourceData->startTime.size() == 0)
				{
					lConstMap->bones[i] = DirectX::XMMatrixIdentity();
				}
				else
				{
					lConstMap->bones[i] = resourceData->bone[i].invInitialPose * lMatCurrentPose;
				}
			}

			gpuBuffer->GetBufferData(constBufferHandle[1])->Unmap(0, nullptr);
		}


		SetConstBufferOnCmdList(data.pipelineName, data.removeMaterialFlag);

		if (!data.removeMaterialFlag)
		{
			for (int i = 0; i < resourceData->textureHandle.size(); i++)
			{
				if (resourceData->textureHandle[i] != -1)
				{
					D3D12_GPU_DESCRIPTOR_HANDLE lGpuDescHandleSRV = DescriptorHeapMgr::Instance()->GetGpuDescriptorView(resourceData->textureHandle[i]);
					int lParam = KazRenderHelper::SetBufferOnCmdList(GraphicsRootSignature::Instance()->GetRootParam(renderData.pipelineMgr->GetRootSignatureName(data.pipelineName)), GRAPHICS_RANGE_TYPE_SRV_DESC, GRAPHICS_PRAMTYPE_TEX);
					renderData.cmdListInstance->cmdList->SetGraphicsRootDescriptorTable(lParam, lGpuDescHandleSRV);
				}
			}
		}

		//DrawIndexInstanceCommand(drawIndexInstanceCommandData);
		DrawInstanceCommand(drawInstanceCommandData);
	}

	data.Record();
}

void FbxModelRender::CaluMat()
{
	baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
	baseMatWorldData.matScale = KazMath::CaluScaleMatrix(data.transform.scale);
	baseMatWorldData.matTrans = KazMath::CaluTransMatrix(data.transform.pos);
	baseMatWorldData.matRota = KazMath::CaluRotaMatrix(data.transform.rotation);

	baseMatWorldData.matWorld = DirectX::XMMatrixIdentity();
	baseMatWorldData.matWorld *= baseMatWorldData.matScale;
	baseMatWorldData.matWorld *= baseMatWorldData.matRota;
	baseMatWorldData.matWorld *= baseMatWorldData.matTrans;
	baseMatWorldData.matWorld *= data.motherMat.mat;

	//親行列を掛ける
	motherMat = baseMatWorldData.matWorld;
}

void FbxModelRender::ReleaseSkining()
{
	removeSkining = true;
	Release(constBufferHandle[1]);
}
