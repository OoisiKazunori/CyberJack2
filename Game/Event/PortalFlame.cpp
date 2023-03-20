#include "PortalFlame.h"

PortalFlame::PortalFlame()
{
	for (int i = 0; i < flame.size(); ++i)
	{
		lineEffectData[i].x = 0.0f;
		lineEffectData[i].y = 1.0f;
		lineEffectData[i].z = 0.0f;
		lineEffectData[i].w = 0.0f;


		flame[i].data.pipelineName = PIPELINE_NAME_LINE_FLASHEFFECT;
		constBufferHandle[i] = flame[i].CreateConstBuffer(sizeof(DirectX::XMFLOAT4), typeid(DirectX::XMFLOAT4).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	}
}

void PortalFlame::Init(const KazMath::Vec3<float> &POS, const KazMath::Vec2<float> &SIZE)
{
	startFlag = false;

	initFlamePos[LEFT_UP] =
	{
		POS.x - SIZE.x / 2.0f,
		POS.y + SIZE.y / 2.0f,
		POS.z
	};
	initFlamePos[LEFT_DOWN] =
	{
		POS.x - SIZE.x / 2.0f,
		POS.y - SIZE.y / 2.0f,
		POS.z
	};
	initFlamePos[RIGHT_UP] =
	{
		POS.x + SIZE.x / 2.0f,
		POS.y + SIZE.y / 2.0f,
		POS.z
	};
	initFlamePos[RIGHT_DOWN] =
	{
		POS.x + SIZE.x / 2.0f,
		POS.y - SIZE.y / 2.0f,
		POS.z
	};

	//上辺
	{
		float lMaxDistance = initFlamePos[RIGHT_UP].Distance(initFlamePos[LEFT_UP]);
		float lDistance = lMaxDistance / static_cast<float>(LINE_CPU_UPDOWN_NUM);
		std::array<KazMath::Vec3<float>, LINE_CPU_UPDOWN_NUM> lDiv;
		for (int i = 0; i < LINE_CPU_UPDOWN_NUM; ++i)
		{
			lDiv[i] = initFlamePos[LEFT_UP];
			lDiv[i].x += lDistance / 2.0f;
			lDiv[i].x += lDistance * static_cast<float>(i);
			upDownMemoryine[LINE_UPVEC][i].Init(LINE_UPVEC, lDiv[i]);

			upDownMemoryine[LINE_UPVEC][i].rate = (lDiv[i].x - initFlamePos[LEFT_UP].x) / lMaxDistance;
		}
	}

	//下辺
	{
		float lMaxDistance = initFlamePos[RIGHT_DOWN].Distance(initFlamePos[LEFT_DOWN]);
		float lDistance = lMaxDistance / static_cast<float>(LINE_CPU_UPDOWN_NUM);
		std::array<KazMath::Vec3<float>, LINE_CPU_UPDOWN_NUM> lDiv;
		for (int i = 0; i < LINE_CPU_UPDOWN_NUM; ++i)
		{
			lDiv[i] = initFlamePos[LEFT_DOWN];
			lDiv[i].x += lDistance / 2.0f;
			lDiv[i].x += lDistance * static_cast<float>(i);
			upDownMemoryine[LINE_DOWNVEC][i].Init(LINE_DOWNVEC, lDiv[i]);

			upDownMemoryine[LINE_DOWNVEC][i].rate = initFlamePos[LEFT_DOWN].Distance(lDiv[i]) / lMaxDistance;
		}
	}

	//左辺
	{
		float lMaxDistance = initFlamePos[LEFT_UP].Distance(initFlamePos[LEFT_DOWN]);
		float lDistance = lMaxDistance / static_cast<float>(LINE_CPU_NUM);
		std::array<KazMath::Vec3<float>, LINE_CPU_NUM> lDiv;
		for (int i = 0; i < LINE_CPU_NUM; ++i)
		{
			lDiv[i] = initFlamePos[LEFT_DOWN];
			lDiv[i].y += lDistance / 2.0f;
			lDiv[i].y += lDistance * static_cast<float>(i);
			sideMemoryine[LINE_LEFTVEC - 2][i].Init(LINE_LEFTVEC, lDiv[i]);

			sideMemoryine[LINE_LEFTVEC - 2][i].rate = initFlamePos[LEFT_DOWN].Distance(lDiv[i]) / lMaxDistance;
		}
	}

	//右辺
	{
		float lMaxDistance = initFlamePos[RIGHT_UP].Distance(initFlamePos[RIGHT_DOWN]);
		float lDistance = lMaxDistance / static_cast<float>(LINE_CPU_NUM);
		std::array<KazMath::Vec3<float>, LINE_CPU_NUM> lDiv;
		for (int i = 0; i < LINE_CPU_NUM; ++i)
		{
			lDiv[i] = initFlamePos[RIGHT_DOWN];
			lDiv[i].y += lDistance / 2.0f;
			lDiv[i].y += lDistance * static_cast<float>(i);
			sideMemoryine[LINE_RIGHTVEC - 2][i].Init(LINE_RIGHTVEC, lDiv[i]);

			sideMemoryine[LINE_RIGHTVEC - 2][i].rate = initFlamePos[RIGHT_DOWN].Distance(lDiv[i]) / lMaxDistance;
		}
	}


	for (int edge = 0; edge < LINE_CPU_UPDOWN_NUM; ++edge)
	{
		upDownMemoryine[LINE_UPVEC][edge].rate = 1.0f - upDownMemoryine[LINE_UPVEC][edge].rate;
	}
	for (int edge = 0; edge < LINE_CPU_NUM; ++edge)
	{
		sideMemoryine[LINE_LEFTVEC - 2][edge].rate = 1.0f - sideMemoryine[LINE_LEFTVEC - 2][edge].rate;
	}

	for (int i = 0; i < flame.size(); ++i)
	{
		flame[i].data.startPos = { 0.0f,0.0f,0.0f };
		flame[i].data.endPos = { 0.0f,0.0f,0.0f };
	}


	for (int i = 0; i < flashTimer.size(); ++i)
	{
		flashTimer[i] = -1;
	}
	flameFlashIndex = 0;
	flashMaxTimer = 60;

	flameTimer = 0;
	flameIndex = 0;
	makeFlameMaxTimer = 10;
	readyToFlashFlag = false;
	basePos = POS;
}

void PortalFlame::Update()
{
	if (startFlag)
	{
		const float lSpeed = 0.3f;
		basePos.z -= lSpeed;


		//ポータル枠の生成の過程を描画する-------------------------
		std::array<std::array<KazMath::Vec3<float>, 2>, 4>lTmpVecPos;
		const int START = 0;
		const int END = 1;
		lTmpVecPos[0][START] = initFlamePos[RIGHT_UP];
		lTmpVecPos[0][END] = initFlamePos[LEFT_UP];
		lTmpVecPos[1][START] = initFlamePos[LEFT_UP];
		lTmpVecPos[1][END] = initFlamePos[LEFT_DOWN];
		lTmpVecPos[2][START] = initFlamePos[LEFT_DOWN];
		lTmpVecPos[2][END] = initFlamePos[RIGHT_DOWN];
		lTmpVecPos[3][START] = initFlamePos[RIGHT_DOWN];
		lTmpVecPos[3][END] = initFlamePos[RIGHT_UP];

		if (makeFlameMaxTimer <= flameTimer)
		{
			++flameIndex;
			flameTimer = 0;
		}
		if (flame.size() <= flameIndex)
		{
			flameIndex = 3;
			flameTimer = makeFlameMaxTimer;
			readyToFlashFlag = true;
		}
		else
		{
			++flameTimer;
		}


		if (readyToFlashFlag)
		{
			if (flashMaxTimer <= flashTimer[flameFlashIndex])
			{
				flashTimer[flameFlashIndex] = -1;
				++flameFlashIndex;
				if (flameFlashIndex < flashTimer.size())
				{
					flashTimer[flameFlashIndex] = 0;
				}
			}

			if (flashTimer.size() <= flameFlashIndex)
			{
				flameFlashIndex = 0;
				flashTimer[flameFlashIndex] = -1;
			}
			else
			{
				++flashTimer[flameFlashIndex];
			}
		}

		//フラッシュ用のデータ
		for (int i = 0; i < flame.size(); ++i)
		{
			if (flashTimer[i] == -1.0f)
			{
				lineEffectData[i].w = -1.0f;
			}
			else
			{
				lineEffectData[i].w = KazMath::ConvertTimerToRate(flashTimer[i], flashMaxTimer);
			}
			flame[i].TransData(&lineEffectData[i], constBufferHandle[i], typeid(DirectX::XMFLOAT4).name());
		}

		//timerから線をどれくらい伸ばすか決める
		{
			KazMath::Vec3<float> lDistance = lTmpVecPos[flameIndex][END] - lTmpVecPos[flameIndex][START];
			flame[flameIndex].data.startPos = lTmpVecPos[flameIndex][START];
			flame[flameIndex].data.endPos = lTmpVecPos[flameIndex][START];
			KazMath::Vec3<float> lAddVel = lDistance * KazMath::ConvertTimerToRate(flameTimer, makeFlameMaxTimer);
			flame[flameIndex].data.endPos = lTmpVecPos[flameIndex][START] + lAddVel;
			KazMath::CheckIsnan(&flame[flameIndex].data.endPos);
			float lMaxRate = lTmpVecPos[flameIndex][END].Distance(lTmpVecPos[flameIndex][START]);
			float lNowRate = flame[flameIndex].data.endPos.Distance(flame[flameIndex].data.startPos);
			float lRate = lNowRate / lMaxRate;

			LineEffectVec vec = LINE_UPVEC;
			switch (flameIndex)
			{
			case 0:
				vec = LINE_UPVEC;
				break;
			case 1:
				vec = LINE_LEFTVEC;
				break;
			case 2:
				vec = LINE_DOWNVEC;
				break;
			case 3:
				vec = LINE_RIGHTVEC;
				break;
			default:
				break;
			}

			//左右の線の数
			if (vec == LINE_LEFTVEC || vec == LINE_RIGHTVEC)
			{
				for (int edge = 0; edge < sideMemoryine[vec - 2].size(); ++edge)
				{
					const bool flameLineCameFlag = sideMemoryine[vec - 2][edge].rate <= lRate;
					if (flameLineCameFlag)
					{
						sideMemoryine[vec - 2][edge].Appear();
					}
				}
			}
			//上下の線の数
			else
			{
				for (int edge = 0; edge < upDownMemoryine[vec].size(); ++edge)
				{
					const bool flameLineCameFlag = upDownMemoryine[vec][edge].rate <= lRate;
					if (flameLineCameFlag)
					{
						upDownMemoryine[vec][edge].Appear();
					}
				}
			}
		}

		//ポータル枠の生成の過程を描画する-------------------------
		float lNowRate = 0.0f;
		{
			KazMath::Vec3<float>lCaluMaxRate;
			float lFlashDisntace = lTmpVecPos[flameFlashIndex][END].Distance(lTmpVecPos[flameFlashIndex][START]);
			float lMaxRate = lCaluMaxRate.Distance(lTmpVecPos[flameFlashIndex][END] - lTmpVecPos[flameFlashIndex][START]);
			float lFlashAddVel = lFlashDisntace * KazMath::ConvertTimerToRate(flashTimer[flameFlashIndex], flashMaxTimer);
			float lCaliNowRate = lMaxRate - lFlashDisntace + lFlashAddVel;
			float lRate = lCaliNowRate / lMaxRate;

			LineEffectVec vec = LINE_UPVEC;
			switch (flameFlashIndex)
			{
			case 0:
				vec = LINE_UPVEC;
				break;
			case 1:
				vec = LINE_LEFTVEC;
				break;
			case 2:
				vec = LINE_DOWNVEC;
				break;
			case 3:
				vec = LINE_RIGHTVEC;
				break;
			default:
				break;
			}

			//左右の線の数
			if (vec == LINE_LEFTVEC || vec == LINE_RIGHTVEC)
			{
				for (int edge = 0; edge < sideMemoryine[vec - 2].size(); ++edge)
				{
					const bool flameFlashCameFlag = sideMemoryine[vec - 2][edge].rate <= lRate;
					if (flameFlashCameFlag)
					{
						sideMemoryine[vec - 2][edge].FlashLight();
					}
				}
			}
			//上下の線の数
			else
			{
				for (int edge = 0; edge < upDownMemoryine[vec].size(); ++edge)
				{
					const bool flameFlashCameFlag = upDownMemoryine[vec][edge].rate <= lRate;
					if (flameFlashCameFlag)
					{
						upDownMemoryine[vec][edge].FlashLight();
					}
				}
			}
			lNowRate = lCaliNowRate;
		}


		//Z軸を動かす-------------------------
		for (int i = 0; i < upDownMemoryine.size(); ++i)
		{
			for (int edge = 0; edge < upDownMemoryine[i].size(); ++edge)
			{
				upDownMemoryine[i][edge].basePos.z = basePos.z;
			}
		}
		for (int i = 0; i < sideMemoryine.size(); ++i)
		{
			for (int edge = 0; edge < sideMemoryine[i].size(); ++edge)
			{
				sideMemoryine[i][edge].basePos.z = basePos.z;
			}
		}
		for (int i = 0; i < flame.size(); ++i)
		{
			flame[i].data.startPos.z = basePos.z;
			flame[i].data.endPos.z = basePos.z;
		}
		//Z軸を動かす-------------------------


		//メモリ線
		for (int i = 0; i < sideMemoryine.size(); ++i)
		{
			for (int edge = 0; edge < sideMemoryine[i].size(); ++edge)
			{
				sideMemoryine[i][edge].Update();
			}
		}
		for (int i = 0; i < upDownMemoryine.size(); ++i)
		{
			for (int edge = 0; edge < upDownMemoryine[i].size(); ++edge)
			{
				upDownMemoryine[i][edge].Update();
			}
		}

		if (flameFlashIndex != prevFlameFlashIndex)
		{
			LineEffectVec vec = LINE_UPVEC;
			switch (flameFlashIndex)
			{
			case 0:
				vec = LINE_UPVEC;
				break;
			case 1:
				vec = LINE_LEFTVEC;
				break;
			case 2:
				vec = LINE_DOWNVEC;
				break;
			case 3:
				vec = LINE_RIGHTVEC;
				break;
			default:
				break;
			}


			//左右の線の数
			if (vec == LINE_LEFTVEC || vec == LINE_RIGHTVEC)
			{
				//現在のフレームの光の位置の割合が0.0f以下なら初期化する
				for (int edge = 0; edge < sideMemoryine[vec - 2].size(); ++edge)
				{
					sideMemoryine[vec - 2][edge].InitFlash();
				}
			}
			else
			{
				//現在のフレームの光の位置の割合が0.0f以下なら初期化する
				for (int edge = 0; edge < upDownMemoryine[vec].size(); ++edge)
				{
					upDownMemoryine[vec][edge].InitFlash();
				}
			}
		}
		prevFlameFlashIndex = flameFlashIndex;
	}
}

void PortalFlame::Draw()
{
	if (startFlag)
	{
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, L"Draw PortalLine");
		for (int i = 0; i < flame.size(); ++i)
		{
			flame[i].Draw();
		}

		for (int i = 0; i < sideMemoryine.size(); ++i)
		{
			for (int edge = 0; edge < sideMemoryine[i].size(); ++edge)
			{
				sideMemoryine[i][edge].Draw();
			}
		}

		for (int i = 0; i < upDownMemoryine.size(); ++i)
		{
			for (int edge = 0; edge < upDownMemoryine[i].size(); ++edge)
			{
				upDownMemoryine[i][edge].Draw();
			}
		}
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());
	}
}

void PortalFlame::Start()
{
	startFlag = true;
}

bool PortalFlame::Flame()
{
	return readyToFlashFlag;
}
