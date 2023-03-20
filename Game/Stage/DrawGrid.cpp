#include "DrawGrid.h"

DrawGrid::DrawGrid(const KazMath::Color &FOG_COLOR) :flashColorArray({ KazMath::Color(255, 255, 255, 255), KazMath::Color(255, 255, 255, 255) }), initFlag(false)
{
	for (int i = 0; i < gridLineRender.size(); ++i)
	{
		gridLineRender[i].data.colorData = { 255,255,255,255 };
		gridLineRender[i].data.pipelineName = PIPELINE_NAME_FOG_LINE_MULTIPASS;

		{
			FogData lData;
			RESOURCE_HANDLE lHandle = gridLineRender[i].CreateConstBuffer(sizeof(FogData), typeid(FogData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
			DirectX::XMFLOAT3 colorRate = FOG_COLOR.ConvertColorRateToXMFLOAT3();
			lData.fogdata = { colorRate.x,colorRate.y,colorRate.z,0.0006f };
			gridLineRender[i].TransData(&lData, lHandle, typeid(lData).name());
		}

		{
			gridLineBloomBufferHandle[i] = gridLineRender[i].CreateConstBuffer(sizeof(BloomData), typeid(BloomData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
			BloomData lData;
			lData.luminanceColor = { 0.0f,0.0f,0.0f,0.0f };
			gridLineRender[i].TransData(&lData, gridLineBloomBufferHandle[i], typeid(BloomData).name());
		}
	}
}

void DrawGrid::Init(bool USE_FOR_FLOOR_FLAG, float SPACE, float BASE_POS, const int *CAMERA_ID_PTR, bool STOP_FLAG, bool FLASH_FLAG, std::array<KazMath::Color, 2>FLASH_COLOR, bool APPEAR_FLAG)
{
	if (USE_FOR_FLOOR_FLAG)
	{
		InitFloor(SPACE, BASE_POS);
	}
	else
	{
		InitWall(SPACE, BASE_POS);
	}

	int lLineRIndex = 0;
	for (int i = 0; i < gridFloorZLinePos.size(); ++i)
	{
		gridLineRender[lLineRIndex].data.startPos = gridFloorZLinePos[i][0].pos;
		gridLineRender[lLineRIndex].data.endPos = gridFloorZLinePos[i][1].pos;
		++lLineRIndex;
	}
	for (int i = 0; i < gridFloorXLinePos.size(); ++i)
	{
		gridLineRender[lLineRIndex].data.startPos = gridFloorXLinePos[i][0].pos;
		gridLineRender[lLineRIndex].data.endPos = gridFloorXLinePos[i][1].pos;
		++lLineRIndex;
	}

	cameraIndex = CAMERA_ID_PTR;
	if (STOP_FLAG)
	{
		vel = -5.0f;
	}
	else
	{
		vel = 0.0f;
	}

	flashFlag = FLASH_FLAG;
	flashColorArray = FLASH_COLOR;
	initFlag = true;


	for (int i = 0; i < lightEffectInitFlagArray.size(); ++i)
	{
		lightEffectInitFlagArray[i] = false;
	}
	timer = 0;


	appearFlag = false;
	if (APPEAR_FLAG)
	{
		appearAlpha = 0;
	}
	else
	{
		appearAlpha = 255;
	}
}

void DrawGrid::Finalize()
{
	for (int i = 0; i < lightEffectArray.size(); ++i)
	{
		for (int j = 0; j < lightEffectArray[i].size(); ++j)
		{
			lightEffectArray[i][j].Finalize();
		}
	}
}

void DrawGrid::Update(float Y_POS, bool USE_FLASHLINE_FLAG)
{
	if (!initFlag)
	{
		return;
	}

#pragma region Grid
	const float lVelZ = vel;

	for (int i = 0; i < gridFloorZLinePos.size(); ++i)
	{
		gridFloorZLinePos[i][0].pos.z += lVelZ;
		gridFloorZLinePos[i][1].pos.z += lVelZ;

		if (gridFloorZLinePos[i][0].pos.z <= -8000.0f)
		{
			gridFloorZLinePos[i][0].pos.z = -8000.0f + static_cast<float>(gridFloorZLinePos.size()) * 300.0f;
			gridFloorZLinePos[i][1].pos.z = gridFloorZLinePos[i][0].pos.z;
		}
	}


	int lineRIndex = 0;
	for (int i = 0; i < gridFloorZLinePos.size(); ++i)
	{
		if (Y_POS != -1.0f)
		{
			gridFloorZLinePos[i][0].pos.y = Y_POS;
			gridFloorZLinePos[i][1].pos.y = Y_POS;
		}
		gridLineRender[lineRIndex].data.startPos = gridFloorZLinePos[i][0].pos;
		gridLineRender[lineRIndex].data.endPos = gridFloorZLinePos[i][1].pos;
		++lineRIndex;
	}
	for (int i = 0; i < gridFloorXLinePos.size(); ++i)
	{
		if (Y_POS != -1.0f)
		{
			gridFloorXLinePos[i][0].pos.y = Y_POS;
			gridFloorXLinePos[i][1].pos.y = Y_POS;
		}

		gridLineRender[lineRIndex].data.startPos = gridFloorXLinePos[i][0].pos;
		gridLineRender[lineRIndex].data.endPos = gridFloorXLinePos[i][1].pos;
		++lineRIndex;
	}
#pragma endregion


	if (reversValueFlag)
	{
		flashRate += 1.0f / 60.0f;
	}
	else
	{
		flashRate += -(1.0f / 30.0f);
	}

	if (flashRate <= 0.0f)
	{
		flashRate = 0.0f;
		reversValueFlag = true;
	}
	else if (1.0f <= flashRate)
	{
		flashRate = 1.0f;
		reversValueFlag = false;
	}


	if (appearFlag)
	{
		appearAlpha += 5;
		if (255 < appearAlpha)
		{
			appearAlpha = 255;
		}
	}



#pragma region LightEffect

	KazMath::Vec3<float> lVel = {};
	KazMath::Vec3<float> lVel1 = {};
	if (floorFlag)
	{
		lVel1 = KazMath::Vec3<float>(-10.0f, 0.0f, 0.0f);
		lVel = KazMath::Vec3<float>(0.0f, 0.0f, -10.0f);
	}
	else
	{
		lVel1 = KazMath::Vec3<float>(0.0f, 10.0f, 0.0f);
		lVel = KazMath::Vec3<float>(0.0f, -10.0f, 0.0f);
	}

	for (int i = 0; i < gridFloorZLinePos.size(); ++i)
	{
		for (int limitPointIndex = 0; limitPointIndex < lightEffectGridFloorLineStartPos[i].size(); limitPointIndex++)
		{
			lightEffectGridFloorLineStartPos[i][limitPointIndex] = gridFloorZLinePos[i][0].pos + KazMath::Vec3<float>(static_cast<float>(limitPointIndex) * -10.0f, 0.0f, 0.0f);
			lightEffectGridFloorLineEndPos[i][limitPointIndex] = gridFloorZLinePos[i][1].pos + lVel1 * static_cast<float>(limitPointIndex);
		}
	}

	for (int i = static_cast<int>(gridFloorZLinePos.size()); i < gridFloorZLinePos.size() + gridFloorXLinePos.size(); ++i)
	{
		for (int limitPointIndex = 0; limitPointIndex < lightEffectGridFloorLineStartPos[i].size(); limitPointIndex++)
		{
			lightEffectGridFloorLineStartPos[i][limitPointIndex] = gridFloorXLinePos[i - LINE_MAX/2][0].pos + KazMath::Vec3<float>(0.0f, 0.0f, static_cast<float>(limitPointIndex) * -10.0f);
			lightEffectGridFloorLineEndPos[i][limitPointIndex] = gridFloorXLinePos[i - LINE_MAX/2][1].pos + lVel * static_cast<float>(limitPointIndex);
		}
	}


	//åıê¸ÇégÇÌÇ»Ç¢èÍçáèàóùÇîÚÇŒÇ∑
	if (!USE_FLASHLINE_FLAG)
	{
		return;
	}


	if (maxTimer <= timer)
	{
		maxTimer = KazMath::Rand<int>(70, 60);
		int countTrue = 0;
		while (countTrue < lightEffectArray.size())
		{
			lightEffectIndex = KazMath::Rand<int>(static_cast<int>(lightEffectArray.size() - 1), 0);
			if (lightEffectArray[lightEffectIndex][0].IsFinish() && lightEffectInitFlagArray[lightEffectIndex])
			{
				break;
			}
			lightEffectInitFlagArray[lightEffectIndex] = true;
			++countTrue;
		}
		timer = 0;
	}
	else
	{
		++timer;
	}

	for (int squareIndex = 0; squareIndex < lightEffectArray[lightEffectIndex].size(); ++squareIndex)
	{
		if (lightEffectArray[lightEffectIndex][squareIndex].IsFinish())
		{
			std::vector<KazMath::Vec3<float> *>lPosArray;
			lPosArray.push_back(&lightEffectGridFloorLineStartPos[lightEffectIndex][squareIndex]);
			lPosArray.push_back(&lightEffectGridFloorLineEndPos[lightEffectIndex][squareIndex]);

			lightEffectArray[lightEffectIndex][squareIndex].Init(lPosArray, 60 * 10);
		}
	}

	for (int i = 0; i < lightEffectArray.size(); ++i)
	{
		for (int squareIndex = 0; squareIndex < lightEffectArray[i].size(); ++squareIndex)
		{
			if (lightEffectArray[i][squareIndex].IsFinish() && lightEffectInitFlagArray[i])
			{
				std::vector<KazMath::Vec3<float> *>lPosArray;
				lPosArray.push_back(&lightEffectGridFloorLineStartPos[i][squareIndex]);
				lPosArray.push_back(&lightEffectGridFloorLineEndPos[i][squareIndex]);
				lightEffectArray[i][squareIndex].Init(lPosArray, 60 * 10);
			}
		}
	}


	for (int i = 0; i < lightEffectArray.size(); ++i)
	{
		for (int squareIndex = 0; squareIndex < lightEffectArray[i].size(); ++squareIndex)
		{
			lightEffectArray[i][squareIndex].Update();
		}
	}

#pragma endregion

}

void DrawGrid::Draw()
{
	if (!initFlag)
	{
		return;
	}


	for (int i = 0; i < gridLineRender.size(); ++i)
	{
		if (flashFlag)
		{
			KazMath::Color nowColor(0, 0, 0, 255);
			nowColor.color = (flashColorArray[1].color - flashColorArray[0].color);
			KazMath::Vec4<float> tmpFloatColor = flashColorArray[0].color.Float() + nowColor.color.Float() * flashRate;
			gridLineRender[i].data.colorData.color = tmpFloatColor.Int();
		}
		else
		{
			gridLineRender[i].data.colorData.color = flashColorArray[0].color;
		}

		gridLineRender[i].data.colorData.color.a = appearAlpha;
		gridLineRender[i].data.cameraIndex.id = *cameraIndex;
		gridLineRender[i].Draw();
	}

	for (int i = 0; i < lightEffectArray.size(); ++i)
	{
		for (int squareIndex = 0; squareIndex < lightEffectArray[i].size(); ++squareIndex)
		{
			lightEffectArray[i][squareIndex].Draw();
		}
	}
}

void DrawGrid::InitFloor(float SPACE, float BASE_POS)
{
	floorFlag = true;
	const float L_SPACE = SPACE;
	const float L_Y = BASE_POS;
	const float L_X = -10000.0f;
	for (int i = 0; i < gridFloorZLinePos.size(); ++i)
	{
		float index = static_cast<float>(i);
		gridFloorZLinePos[i][0].pos = { L_X,L_Y,-700.0f + index * L_SPACE };
		gridFloorZLinePos[i][1] = gridFloorZLinePos[i][0];
		gridFloorZLinePos[i][1].pos.x *= -1;
	}

	for (int i = 0; i < gridFloorXLinePos.size(); ++i)
	{
		float index = static_cast<float>(i);
		gridFloorXLinePos[i][0].pos = { L_X + index * L_SPACE,L_Y,-10000.0f };
		gridFloorXLinePos[i][1] = gridFloorXLinePos[i][0];
		gridFloorXLinePos[i][1].pos.z *= -1;
	}
}

void DrawGrid::InitWall(float SPACE, float BASE_POS)
{
	floorFlag = false;
	const float L_SPACE = SPACE;
	const float L_Y = -150.0f;
	const float L_X = BASE_POS;
	for (int i = 0; i < gridFloorZLinePos.size(); ++i)
	{
		float index = static_cast<float>(i);
		gridFloorZLinePos[i][0].pos = { L_X,10000.0f,-700.0f + index * L_SPACE };
		gridFloorZLinePos[i][1] = gridFloorZLinePos[i][0];
		gridFloorZLinePos[i][1].pos.y *= -1;
	}

	for (int i = 0; i < gridFloorXLinePos.size(); ++i)
	{
		float index = static_cast<float>(i);
		gridFloorXLinePos[i][0].pos = { L_X,L_Y + index * L_SPACE,-8000.0f };
		gridFloorXLinePos[i][1] = gridFloorXLinePos[i][0];
		gridFloorXLinePos[i][1].pos.z *= -1;
	}
}
