#include "LineFlashLight.h"
#include"../KazLibrary/Helper/KazHelper.h"

LineFlashLight::LineFlashLight() :finishFlag(true)
{
	RESOURCE_HANDLE lHandle = boxR.CreateConstBuffer(sizeof(DirectX::XMFLOAT4), "XMFLOAT4", GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	DirectX::XMFLOAT4 colorData = { 1.0f,0.0f,0.0f,0.0f };
	boxR.TransData(&colorData, lHandle, "XMFLOAT4");
}

void LineFlashLight::Init(const std::vector<KazMath::Vec3<float>*> &POS_ARRAY, int TIMER)
{
	posArray = POS_ARRAY;

	//全長の算出
	float distance = 0.0f;
	for (int i = 0; i < posArray.size(); ++i)
	{
		if (i + 1 < posArray.size())
		{
			distance += posArray[i + 1]->Distance(*posArray[i]);
		}
		else
		{
			break;
		}
	}
	//1Flameのスピード
	speed = distance / static_cast<float>(TIMER);

	lineIndex = 0;
	boxR.data.transform.pos = *posArray[lineIndex];
	boxR.data.transform.scale = { 5.0f,5.0f,5.0f };
	boxR.data.color = { 0,255,0,255 };
	finishFlag = false;
	boxR.data.pipelineName = PIPELINE_NAME_COLOR_MULTITEX;
}

void LineFlashLight::Finalize()
{
	finishFlag = true;
}

void LineFlashLight::Update()
{
	if (!finishFlag)
	{
		//ボックスをどの方向に向かわせるか決める
		CheckWhereToGo();


		if (KazHelper::IsitInAnArray(lineIndex, posArray.size()))
		{
			if (useXPosFlag)
			{
				boxR.data.transform.pos.y = posArray[lineIndex]->y;
				boxR.data.transform.pos.z = posArray[lineIndex]->z;
			}
			else if (useYPosFlag)
			{
				boxR.data.transform.pos.x = posArray[lineIndex]->x;
				boxR.data.transform.pos.z = posArray[lineIndex]->z;
			}
			else if (useZPosFlag)
			{
				boxR.data.transform.pos.x = posArray[lineIndex]->x;
				boxR.data.transform.pos.y = posArray[lineIndex]->y;
			}
		}


		//ボックスを動かす
		if (nowPos != nullptr)
		{
			*nowPos += speed;
		}

		bool lIsReachToGoalDistanceFlag = false;
		float lDistance = CaluDisntace();

		lIsReachToGoalDistanceFlag = lDistance <= 0.0f;

		//線を超えたら次の線に行く
		if (nowPos != nullptr && nextPos != nullptr && lIsReachToGoalDistanceFlag)
		{
			++lineIndex;

			int lLoopCount = 0;

			//一度に線を越えた回数が100回以下&&線を越えなかった時ループを終える
			while (lDistance <= 0.0f)
			{
				//線を超えた範囲にボックスがいるので超えた分を修正する
				if (lDistance <= -0.1f)
				{
					float subDistance = posArray[lineIndex]->Distance(boxR.data.transform.pos);
					boxR.data.transform.pos = *posArray[lineIndex];
					CheckWhereToGo();
					lDistance = CaluDisntace();
					*nowPos += subDistance;
				}
				//ぴったりぐらいの位置にボックスがいる場合はなにもしない
				else
				{
					//最後の制御点なら終了する
					if (!KazHelper::IsitInAnArray(lineIndex + 1, posArray.size()))
					{
						finishFlag = true;
						break;
					}
					break;
				}

				//最後の制御点なら終了する
				if (!KazHelper::IsitInAnArray(lineIndex + 1, posArray.size()))
				{
					finishFlag = true;
					break;
				}

				++lLoopCount;

				//無限ループ防止
				if (100 <= lLoopCount)
				{
					assert(0);
				}
			}
		}
	}
}

void LineFlashLight::Draw()
{
	if (!finishFlag)
	{
		boxR.Draw();
	}
}

bool LineFlashLight::IsFinish()
{
	return finishFlag;
}

void LineFlashLight::CheckWhereToGo()
{
	if (KazHelper::IsitInAnArray(lineIndex + 1, posArray.size()))
	{
		KazMath::Vec3<float> lDistance = *posArray[lineIndex + 1] - *posArray[lineIndex];


		useXPosFlag = false;
		useYPosFlag = false;
		useZPosFlag = false;

		if (0.1f <= abs(lDistance.x))
		{
			//X軸の距離が- && 現在のスピードが-ではない場合に-方向に進ませる
			if (signbit(lDistance.x) && !signbit(speed))
			{
				speed *= -1.0f;
			}
			//X軸の距離が-ではない && 現在のスピードが-の場合+方向に進ませる
			else if (!signbit(lDistance.x) && signbit(speed))
			{
				speed *= -1.0f;
			}
			nowPos = &boxR.data.transform.pos.x;
			nextPos = &posArray[lineIndex + 1]->x;

			useXPosFlag = true;
		}
		else if (0.1f <= abs(lDistance.y))
		{
			//Y軸の距離が- && 現在のスピードが-ではない場合に-方向に進ませる
			if (signbit(lDistance.y) && !signbit(speed))
			{
				speed *= -1.0f;
			}
			//Y軸の距離が-ではない && 現在のスピードが-の場合+方向に進ませる
			else if (!signbit(lDistance.y) && signbit(speed))
			{
				speed *= -1.0f;
			}
			nowPos = &boxR.data.transform.pos.y;
			nextPos = &posArray[lineIndex + 1]->y;

			useYPosFlag = true;
		}
		else if (0.1f <= abs(lDistance.z))
		{
			//Z軸の距離が- && 現在のスピードが-ではない場合に-方向に進ませる
			if (signbit(lDistance.z) && !signbit(speed))
			{
				speed *= -1.0f;
			}
			//Z軸の距離が-ではない && 現在のスピードが-の場合+方向に進ませる
			else if (!signbit(lDistance.z) && signbit(speed))
			{
				speed *= -1.0f;
			}
			nowPos = &boxR.data.transform.pos.z;
			nextPos = &posArray[lineIndex + 1]->z;

			useZPosFlag = true;
		}
	}
}

float LineFlashLight::CaluDisntace()
{
	if (signbit(*nextPos))
	{
		return *nowPos - *nextPos;
	}
	else
	{
		return *nextPos - *nowPos;
	};
}
