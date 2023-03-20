#include "LineLevel1.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

LineLevel1::LineLevel1()
{
	initFlag = false;

	for (int i = 0; i < countVec.size(); ++i)
	{
		countVec[i] = 0;
	}
	allFinishFlag = false;
}

void LineLevel1::CalucurateDistance(const KazMath::Vec3<float> &PLAYER_POS, const KazMath::Vec3<float> &ENEMY_POS)
{
	KazMath::Vec3<float> distance(ENEMY_POS - PLAYER_POS);

	distanceValue = distance / rockOnDistance;


	if (isnan<float>(distanceValue.x) || isinf<float>(distanceValue.x))
	{
		distanceValue.x = 0.0f;
	}
	if (isnan<float>(distanceValue.y) || isinf<float>(distanceValue.y))
	{
		distanceValue.y = 0.0f;
	}
	if (isnan<float>(distanceValue.z) || isinf<float>(distanceValue.z))
	{
		distanceValue.z = 0.0f;
	}
}

void LineLevel1::Attack(const KazMath::Vec3<float> &PLAYER_POS, const KazMath::Vec3<float> &ENEMY_POS)
{
	if (!initFlag)
	{
		limitPos.clear();
		line.clear();
		limitPolygon.clear();

#pragma region ゴール座標算出

		//ゴール座標算出を開始----------------------------------------------------------------
		//1.どの面に刺すか決める
		eSurface surface //SURFACE_RIGHT;
			= static_cast<eSurface>(KazMath::Rand<int>(6, 0));

		//2.自分の座標を見て、刺す面は見えている位置かどうか確認する
		bool xMinusFlag = signbit(ENEMY_POS.x);
		bool yMinusFlag = signbit(ENEMY_POS.y);
		bool zMinusFlag = signbit(ENEMY_POS.z);
		//同じ高さにいるかどうか
		bool yEqualFlag = ENEMY_POS.y <= 1.0f && -1.0f <= ENEMY_POS.y;
		//真正面かどうか
		bool frontFlag = ENEMY_POS.x == PLAYER_POS.x;

		//どれがマイナスかでどの面が見えているかどうか分かる
		std::array<eSurface, 3> canLook;
		if (xMinusFlag)
		{
			canLook[0] = SURFACE_RIGHT;
		}
		else
		{
			canLook[0] = SURFACE_LEFT;
		}
		if (yMinusFlag)
		{
			canLook[1] = SURFACE_TOP;
		}
		else
		{
			canLook[1] = SURFACE_BUTTOM;
		}
		if (zMinusFlag)
		{
			canLook[2] = SURFACE_BACK;
		}
		else
		{
			canLook[2] = SURFACE_FRONT;
		}

		//真正面に敵が居る場合は正面しか見えない
		if (frontFlag)
		{
			canLook[0] = SURFACE_NONE;
			canLook[1] = SURFACE_NONE;
			canLook[2] = SURFACE_FRONT;
		}
		//高さが同じなら上下と後ろ面は見えない
		if (yEqualFlag)
		{
			canLook[1] = SURFACE_NONE;
			canLook[2] = SURFACE_FRONT;
		}



		//見える面に刺すかどうか判断する
		bool canLookFlag = false;
		for (int i = 0; i < canLook.size(); i++)
		{
			if (canLook[i] == surface)
			{
				canLookFlag = true;
				break;
			}
		}

		KazMath::Vec3<float> goalPos = ENEMY_POS;

		//ゴール座標から順に制御点にプッシュするための座標を記録する
		std::vector<KazMath::Vec3<float>> endLimitPos;
		if (true)
		{
			//true...プレイヤーから見えている面の場合,false...プレイヤーから見えていない面の場合
			if (canLookFlag)
			{
				//垂直に線を伸ばす...乱数で伸ばす
				KazMath::Vec3<float> dir = firstDir(surface);
				goalPos += dir;
				endLimitPos.push_back(goalPos);
			}
			else
			{
				//垂直に線を伸ばす...乱数で伸ばす
				KazMath::Vec3<float> dir = firstDir(surface);
				goalPos += dir;
				endLimitPos.push_back(goalPos);


				//刺す面から伸ばした方向を記録
				int oldMoveVector = -1;
				switch (surface)
				{
				case SURFACE_NONE:
					break;
				case SURFACE_LEFT:
					oldMoveVector = LINE_MOVE_LEFT;
					break;
				case SURFACE_RIGHT:
					oldMoveVector = LINE_MOVE_RIGHT;
					break;
				case SURFACE_FRONT:
					oldMoveVector = LINE_MOVE_BACK;
					break;
				case SURFACE_BACK:
					oldMoveVector = LINE_MOVE_STRAIGHT;
					break;
				case SURFACE_TOP:
					oldMoveVector = LINE_MOVE_UP;
					break;
				case SURFACE_BUTTOM:
					oldMoveVector = LINE_MOVE_DOWN;
					break;
				default:
					break;
				}


#pragma region 回り道線の処理
				//条件に沿って線を伸ばしていく
				int moveVector = oldMoveVector;
				//伸びた方向の回数の初期化
				std::array<int, 5>limitCount;
				for (int i = 0; i < limitCount.size(); ++i)
				{
					limitCount[i] = 0;
				}

				int countLoop = 0;

				//回り道線の処理開始
				while (1)
				{
					++countLoop;

					if (10 <= countLoop)
					{
						goalPos.z += (ENEMY_POS.z - goalPos.z) / 2.0f;
						break;
					}

					//どの方向に線を伸ばすか指定
					moveVector = CalucurateDirection(moveVector, 5);

					bool yUpLimitFlag = (LINE_MOVE_UP == moveVector && 1 <= limitCount[moveVector]);
					bool yDownLimitFlag = (LINE_MOVE_DOWN == moveVector && 1 <= limitCount[moveVector]);
					//上か下に2回以上伸ばそうとしたらもう一度乱数を出す
					if (yUpLimitFlag || yDownLimitFlag)
					{
						continue;
					}

					if (5 <= moveVector)
					{
						moveVector = 4;
					}
					//伸ばした方向をカウントする
					++limitCount[moveVector];


					//制限された回数以上に線が伸びようとしたらtrueになる
					//true...プレイヤーの方向に線を伸ばす,false...乱数で線を伸ばす
					if (2 <= limitCount[moveVector])
					{
						//KazMath::Vec3<float> dir;
						//余剰分の距離を入れる
						DirectX::XMVECTOR addDistance = { 10.0f,10.0f,10.0f };


						//ゴール座標と敵との距離

						DirectX::XMVECTOR enemyToGoalDistance = DirectX::XMVectorSubtract(ENEMY_POS.ConvertXMVECTOR(), goalPos.ConvertXMVECTOR());
						//敵とプレイヤーの距離
						DirectX::XMVECTOR playerToEnemyDistance = DirectX::XMVectorSubtract(ENEMY_POS.ConvertXMVECTOR(), PLAYER_POS.ConvertXMVECTOR());


						//絶対値-----------------------
						const int x = 0;
						const int y = 0;
						const int z = 0;

						//敵とゴール座標
						enemyToGoalDistance.m128_f32[x] = fabs(enemyToGoalDistance.m128_f32[x]);
						enemyToGoalDistance.m128_f32[y] = fabs(enemyToGoalDistance.m128_f32[y]);
						enemyToGoalDistance.m128_f32[z] = fabs(enemyToGoalDistance.m128_f32[z]);

						//プレイヤーと敵座標
						playerToEnemyDistance.m128_f32[x] = fabs(playerToEnemyDistance.m128_f32[x]);
						playerToEnemyDistance.m128_f32[y] = fabs(playerToEnemyDistance.m128_f32[y]);
						playerToEnemyDistance.m128_f32[z] = fabs(playerToEnemyDistance.m128_f32[z]);
						//絶対値----------------------

						//伸ばす距離
						DirectX::XMVECTOR vec = {};
						/*
						敵の座標より手前に座標を持ってくる
						正し、プレイヤーより線を越えてはいけないので比較する
						*/
						//true...敵との距離の方よりプレイヤーとの距離の方が短い
						for (int axis = 0; axis < 3; ++axis)
						{
							if (playerToEnemyDistance.m128_f32[axis] <= enemyToGoalDistance.m128_f32[axis] + addDistance.m128_f32[axis])
							{
								//プレイヤーと敵との距離内かつ敵より前の座標を配置する
								//どれくらい超えているか確認
								DirectX::XMVECTOR tmp;
								tmp.m128_f32[axis] = playerToEnemyDistance.m128_f32[axis] - enemyToGoalDistance.m128_f32[axis];
								tmp.m128_f32[axis] = fabs(tmp.m128_f32[axis]);

								//敵とゴール座標との距離に超えた距離を引く
								//tmpの軸が0出ない限り、超えた距離を引く
								if (0.1f <= tmp.m128_f32[axis])
								{
									vec.m128_f32[axis] = enemyToGoalDistance.m128_f32[axis] - tmp.m128_f32[axis];
								}
							}
							else
							{
								//越えなければそのまま使う

								vec = DirectX::XMVectorAdd(enemyToGoalDistance, addDistance);
							}
						}

						//どの方向に線を伸ばすか-----------------------
						float dirVec = 0.0f;
						switch (moveVector)
						{
						case LINE_MOVE_LEFT:
							dirVec = vec.m128_f32[x];
							break;
						case LINE_MOVE_RIGHT:
							dirVec = vec.m128_f32[x];
							break;
						case LINE_MOVE_UP:
							dirVec = vec.m128_f32[y];
							break;
						case LINE_MOVE_DOWN:
							dirVec = vec.m128_f32[y];
							break;
						case LINE_MOVE_STRAIGHT:
							dirVec = vec.m128_f32[z];
							break;
						case LINE_MOVE_BACK:
							dirVec = vec.m128_f32[z];
							break;
						default:
							break;
						}

						//プレイヤーの方向に向かせる
						if (xMinusFlag)
						{
							dirVec *= -1;
						}
						if (yMinusFlag)
						{
							dirVec *= -1;
						}
						if (zMinusFlag)
						{
							dirVec *= -1;
						}
						//プレイヤーの方向に向かせる

						goalPos += CalucurateMoveVector(moveVector, dirVec);
						//どの方向に線を伸ばすか-----------------------



					}
					else
					{
						//線を伸ばす、その後記録
						goalPos += CalucurateMoveVector(moveVector, 10);
					}

					//三つの条件いずれかが当てはまればループから抜け出す
					bool nearXFlag = abs(goalPos.x) < abs(ENEMY_POS.x);
					bool nearYFlag = abs(goalPos.y) < abs(ENEMY_POS.y);
					bool nearZFlag = abs(goalPos.z) < abs(ENEMY_POS.z);
					if (nearXFlag || nearZFlag || nearYFlag)
					{
						break;
					}
					else
					{
						endLimitPos.push_back(goalPos);
					}
				}
#pragma endregion
			}
		}
#pragma endregion

		//goalPos = ENEMY_POS;
		//通常処理開始----------------------------------------------------------------
		//1.プレイヤーと敵の距離を算出する
		DirectX::XMVECTOR distance = DirectX::XMVectorSubtract(goalPos.ConvertXMVECTOR(), PLAYER_POS.ConvertXMVECTOR());
		std::array<bool, 3> minusFlags;

		for (int i = 0; i < minusFlags.size(); ++i)
		{
			if (distance.m128_f32[i] < 0)
			{
				minusFlags[i] = true;
				distance.m128_f32[i] = fabs(distance.m128_f32[i]);
			}
			else
			{
				minusFlags[i] = false;
			}
		}

		//(最初の制御点の位置はプレイヤー座標)
		limitPos.push_back(PLAYER_POS.ConvertXMVECTOR());


		//ゴールまでの線を伸ばす処理をループさせる
		int limitRandom = KazMath::Rand<int>(2, 0);
		eLineMove oldVec = LINE_MOVE_NONE;
		while (1)
		{
			//2.2軸以上、残りの距離が0になったら次の制御点を敵の座標にしてループを抜ける
			int count = 0;
			int dontUseVec = LINE_MOVE_NONE;
			for (int i = 0; i < 3; ++i)
			{
				if (distance.m128_f32[i] <= 0)
				{
					++count;
					dontUseVec = i;
				}
			}
			if (2 <= count)
			{
				limitPos.push_back(goalPos.ConvertXMVECTOR());
				initFlag = true;
				break;
			}

			//3.「XYZ軸どちらに伸ばすか」を乱数で算出
			eLineMove eVec = CaluRandom(dontUseVec, oldVec);
			oldVec = eVec;

			++countVec[eVec];


			if (countVec[eVec] <= limitRandom)
			{
				//3.「その軸の残り距離を割る数」を乱数で算出
				DirectX::XMVECTOR dir = { 0.0f,0.0f,0.0f };
				dir.m128_f32[eVec] = distance.m128_f32[eVec] / KazMath::Rand<int>(3, 1);

				//4.伸ばす距離で残り距離を引く
				distance.m128_f32[eVec] -= dir.m128_f32[eVec];

				if (minusFlags[eVec])
				{
					dir.m128_f32[eVec] *= -1;
				}

				//4.一つ前の制御点の座標から足した距離を次の制御点の座標とする
				DirectX::XMVECTOR tmp = limitPos[limitPos.size() - 1];
				limitPos.push_back(DirectX::XMVectorAdd(tmp, dir));
			}
			else
			{
				//5.同じ軸に一定回数以上割ったら、残り距離を0にする
				DirectX::XMVECTOR tmp = limitPos[limitPos.size() - 1];
				DirectX::XMVECTOR dir = { 0.0f, 0.0f, 0.0f };
				dir.m128_f32[eVec] = distance.m128_f32[eVec];
				distance.m128_f32[eVec] -= dir.m128_f32[eVec];
				if (minusFlags[eVec])
				{
					dir.m128_f32[eVec] *= -1;
				}
				limitPos.push_back(DirectX::XMVectorAdd(tmp, dir));
			}
		}


		//ゴール座標から敵までの座標を追加していく
		for (int i = static_cast<int>(endLimitPos.size()) - 1; 0 <= i; --i)
		{
			limitPos.push_back(endLimitPos[i].ConvertXMVECTOR());
		}
		limitPos.push_back(ENEMY_POS.ConvertXMVECTOR());



		//制御点の描画クラス用意----------------------------------------------------------------
		limitPolygon.resize(limitPos.size());
		for (int i = 0; i < limitPolygon.size(); i++)
		{
			limitPolygon[i] = std::make_unique<BoxPolygonRender>();
		}

		line.resize(limitPos.size() - 1);


		float lDistance = PLAYER_POS.Distance(ENEMY_POS);
		float lSpeed = lDistance / 10.0f;
		for (int i = 0; i < line.size(); i++)
		{
			line[i] = std::make_unique<LineEffect>();

			KazMath::Vec3<float> lLimitPos, lLimitPos2;
			lLimitPos = KazMath::Vec3<float>(limitPos[i].m128_f32[0], limitPos[i].m128_f32[1], limitPos[i].m128_f32[2]);
			lLimitPos2 = KazMath::Vec3<float>(limitPos[i + 1].m128_f32[0], limitPos[i + 1].m128_f32[1], limitPos[i + 1].m128_f32[2]);

			KazMath::Vec3<float> startPlayerdistance = lLimitPos - PLAYER_POS;
			KazMath::Vec3<float> endPlayerdistance = lLimitPos2 - PLAYER_POS;

			line[i]->RockOn(lLimitPos, lLimitPos2, startPlayerdistance, endPlayerdistance, lSpeed);
		}


		for (int i = 0; i < limitPolygon.size(); i++)
		{
			limitPolygon[i]->data.transform.pos = KazMath::Vec3<float>(limitPos[i].m128_f32[0], limitPos[i].m128_f32[1], limitPos[i].m128_f32[2]);
			limitPolygon[i]->data.transform.scale = KazMath::Vec3<float>(0.5f, 2.0f, 0.5f);
			limitPolygon[i]->data.color = { 255,0,0,255 };
		}

		int count = static_cast<int>(endLimitPos.size());
		for (int i = static_cast<int>(limitPolygon.size()) - 1; 0 <= i; --i)
		{
			if (count <= 0)
			{
				break;
			}
			limitPolygon[i]->data.transform.scale = KazMath::Vec3<float>(2.0f, 0.5f, 0.5f);
			limitPolygon[i]->data.color = { 0,255,0,255 };
			--count;
		}



		rockOnDistance = ENEMY_POS - PLAYER_POS;
		finishTimer = 0;
		allFinishFlag = false;
	}



	lineReachObjFlag = false;
	alreadyReachedFlag = false;
	line[0]->StartEffect();

}

void LineLevel1::ReleaseShot()
{
	if (line.size() != 0)
	{
		line[0]->ReleaseEffect();
	}
}

void LineLevel1::Release()
{
	for (int i = 0; i < line.size(); i++)
	{
		line[i].reset();
		limitPolygon[i].reset();
	}
	line.shrink_to_fit();
	line.clear();

	limitPolygon.shrink_to_fit();
	limitPolygon.clear();

	limitPos.shrink_to_fit();
	limitPos.clear();

	initFlag = false;
	lineReachObjFlag = false;

	for (int i = 0; i < countVec.size(); ++i)
	{
		countVec[i] = 0;
	}
}

void LineLevel1::Update()
{
	if (initFlag)
	{
		//ロックオン中の挙動-----------------------
		//敵とプレイヤーの距離　/ ロックオン時の距離　で割合を求める
		for (int i = 0; i < line.size(); ++i)
		{
			line[i]->playerPos = playerPos;
			line[i]->MoveLine(distanceValue);
		}
		//ロックオン中の挙動-----------------------

		line[0]->firstFlag = true;
		line[0]->Update();
		for (int i = 1; i < line.size(); ++i)
		{
			if (line[i - 1]->finishRockOnFlag)
			{
				line[i]->StartEffect();
			}
			if (line[i - 1]->finishReleaseFlag)
			{
				line[i]->ReleaseEffect();
			}


			line[i]->Update();
		}

		int count = 0;
		for (int i = 1; i < line.size(); ++i)
		{
			if (line[i]->finishReleaseFlag)
			{
				++count;
			}
		}

		//リリース時の演出が目標にたどり着いたらフラグを立てる
		if (line[line.size() - 1]->finishReleaseFlag)
		{
			lineReachObjFlag = true;
		}



		//終了処理
		if (line.size() - 1 <= count)
		{
			for (int i = 0; i < line.size(); ++i)
			{
				line[i]->Finalize();
			}

			if (line[0]->FinishFlag())
			{
				initFlag = false;
				allFinishFlag = true;
			}
		}

	}

}

void LineLevel1::Draw()
{
	if (initFlag)
	{
		for (int i = 0; i < limitPolygon.size(); i++)
		{
			//limitPolygon[i]->Draw();
		}

		for (int i = 0; i < line.size(); i++)
		{
			line[i]->Draw();
		}
	}
}

KazMath::Vec3<float> LineLevel1::CalucurateMoveVector(const int &RANDM_NUM, const float &LENGTH)
{
	KazMath::Vec3<float> vel{};
	EnumLineMove moveType = static_cast<EnumLineMove>(RANDM_NUM);

	switch (moveType)
	{
	case LINE_MOVE_LEFT:
		vel = KazMath::Vec3<float>(-LENGTH, 0.0f, 0.0f);
		break;
	case LINE_MOVE_RIGHT:
		vel = KazMath::Vec3<float>(LENGTH, 0.0f, 0.0f);
		break;
	case LINE_MOVE_UP:
		vel = KazMath::Vec3<float>(0.0f, LENGTH, 0.0f);
		break;
	case LINE_MOVE_DOWN:
		vel = KazMath::Vec3<float>(0.0f, -LENGTH, 0.0f);
		break;
	case LINE_MOVE_STRAIGHT:
		vel = KazMath::Vec3<float>(0.0f, 0.0f, -LENGTH);
		break;
	case LINE_MOVE_BACK:
		vel = KazMath::Vec3<float>(0.0f, 0.0f, LENGTH);
		break;
	default:
		break;
	}

	return vel;
}

int LineLevel1::CalucurateDirection(const int &MOVE_VEC, const int &MOVE_VEC_MAX_NUM)
{
	int random = KazMath::Rand<int>(MOVE_VEC_MAX_NUM, 0);
	while (1)
	{
		//左右交互に制御点が置かれようとしたらフラグを立てる
		bool lineGoBackFlag =
			(random == LINE_MOVE_LEFT && MOVE_VEC == LINE_MOVE_RIGHT) ||
			(random == LINE_MOVE_RIGHT && MOVE_VEC == LINE_MOVE_LEFT);

		//上下交互に制御点が置かれようとしたらフラグを立てる
		bool lineGoUpFlag =
			(random == LINE_MOVE_UP && MOVE_VEC == LINE_MOVE_DOWN) ||
			(random == LINE_MOVE_DOWN && MOVE_VEC == LINE_MOVE_UP);

		//前後交互に制御点が置かれようとしたらフラグを立てる
		bool lineGoFrontFlag =
			(random == LINE_MOVE_STRAIGHT && MOVE_VEC == LINE_MOVE_BACK) ||
			(random == LINE_MOVE_BACK && MOVE_VEC == LINE_MOVE_STRAIGHT);

		//前の値と一緒だった場合
		bool lineSameFlag = (random == MOVE_VEC);

		//true...乱数を入れ直し,false...被りなしを確認、ループを抜ける
		if (lineGoBackFlag || lineGoUpFlag || lineSameFlag || lineGoFrontFlag)
		{
			random = KazMath::Rand<int>(MOVE_VEC_MAX_NUM, 0);
		}
		else
		{
			break;
		}
	}
	return random;
}

int LineLevel1::CalucurateDirection2(const int &MOVE_VEC, const int &MOVE_VEC_MAX_NUM)
{
	int random = KazMath::Rand<int>(MOVE_VEC_MAX_NUM, 0);
	while (true)
	{
		//左右交互に制御点が置かれようとしたらフラグを立てる
		bool lineGoBackFlag =
			random == LINE_MOVE_LEFT && MOVE_VEC == LINE_MOVE_RIGHT ||
			random == LINE_MOVE_RIGHT && MOVE_VEC == LINE_MOVE_LEFT;

		//上下交互に制御点が置かれようとしたらフラグを立てる
		bool lineGoUpFlag =
			random == LINE_MOVE_UP && MOVE_VEC == LINE_MOVE_DOWN ||
			random == LINE_MOVE_DOWN && MOVE_VEC == LINE_MOVE_UP;

		//前の値と一緒だった場合
		bool lineSameFlag = (random == MOVE_VEC);

		//true...乱数を入れ直し,false...被りなしを確認、ループを抜ける
		if (lineGoBackFlag || lineGoUpFlag || lineSameFlag)
		{
			random = KazMath::Rand<int>(MOVE_VEC_MAX_NUM, 0);
		}
		else
		{
			break;
		}
	}
	return random;
}


eLineMove LineLevel1::CaluRandom(int DONT_USE, int OLD_NUM)
{
	eLineMove eVec;
	while (1)
	{
		int random = KazMath::Rand<int>(LINE_MOVE_Z + 1, LINE_MOVE_X);
		if (DONT_USE != random && OLD_NUM != random)
		{
			eVec = static_cast<eLineMove>(random);
			break;
		}
	}

	return eVec;
}

KazMath::Vec3<float> LineLevel1::firstDir(eSurface SURFACE)
{
	KazMath::Vec3<float> dir = { 0.0f,0.0f,0.0f };
	float vec = KazMath::Rand<float>(30.0f, 10.0f);
	switch (SURFACE)
	{
	case SURFACE_NONE:
		break;
	case SURFACE_LEFT:
		dir.x += -vec;
		break;
	case SURFACE_RIGHT:
		dir.x += vec;
		break;
	case SURFACE_FRONT:
		dir.z += -vec;
		break;
	case SURFACE_BACK:
		dir.z += vec;
		break;
	case SURFACE_TOP:
		dir.y += vec;
		break;
	case SURFACE_BUTTOM:
		dir.y += -vec;
		break;
	default:
		break;
	}

	return dir;
}
