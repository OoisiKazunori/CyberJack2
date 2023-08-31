#include "EnemyHelper.h"

void KazEnemyHelper::GenerateEnemy(std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES, std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> RESPONE_DATA, std::array<int, ENEMY_TYPE_MAX> &ENEMISE_HANDLE)
{
	for (int enemyType = 0; enemyType < RESPONE_DATA.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < RESPONE_DATA[enemyType].size(); ++enemyCount)
		{
			if (RESPONE_DATA[enemyType][enemyCount].layerLevel != -1)
			{
				switch (enemyType)
				{
				case ENEMY_TYPE_VIRUS:
					ENEMIES[enemyType][enemyCount] = std::make_unique<VirusEnemy>(RESPONE_DATA[enemyType][enemyCount].moveID, RESPONE_DATA[enemyType][enemyCount].moveIDparam);
					break;

				default:
					break;
				}
				++ENEMISE_HANDLE[enemyType];
			}
		}
	}
}

void KazEnemyHelper::GenerateEnemy(std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES, std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> RESPONE_DATA, std::array<int, ENEMY_TYPE_MAX> &ENEMISE_HANDLE, std::vector<Sphere *> &ENEMISE_HITBOX)
{
	for (int enemyType = 0; enemyType < RESPONE_DATA.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < RESPONE_DATA[enemyType].size(); ++enemyCount)
		{
			if (RESPONE_DATA[enemyType][enemyCount].layerLevel != -1)
			{
				switch (enemyType)
				{
				case ENEMY_TYPE_VIRUS:
					ENEMIES[enemyType][enemyCount] = std::make_unique<VirusEnemy>(RESPONE_DATA[enemyType][enemyCount].moveID, RESPONE_DATA[enemyType][enemyCount].moveIDparam);
					ENEMISE_HITBOX.emplace_back(&ENEMIES[enemyType][enemyCount]->GetData()->hitBox);
					break;
				default:
					break;
				}
				++ENEMISE_HANDLE[enemyType];
			}
		}
	}
}

void KazEnemyHelper::AddEnemy(const std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES, std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &RESPONE_DATA, std::array<int, ENEMY_TYPE_MAX> &ADD_ENEMISE_HANDLE, int GAME_FLAME, int GAME_STAGE_LEVEL)
{
	for (int enemyType = 0; enemyType < ENEMIES.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < ENEMIES[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = ENEMIES[enemyType][enemyCount] != nullptr && ENEMIES[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				EnemyData *enemyData = ENEMIES[enemyType][enemyCount]->GetData().get();
				//追加で生成するデータを検知したら生成する
				if (enemyData->genarateData.enemyType != -1)
				{
					//生成する敵の種類
					int enemyTypeData = enemyData->genarateData.enemyType;
					//最後に生成して次のハンドル
					int nowHandle = ADD_ENEMISE_HANDLE[enemyTypeData];
					//現在のレイヤーレベルに合わせる
					RESPONE_DATA[enemyTypeData][nowHandle].layerLevel = GAME_STAGE_LEVEL;
					//現在のフレーム数+インターバルフレーム*個数で設定する
					RESPONE_DATA[enemyTypeData][nowHandle].flame = GAME_FLAME;
					RESPONE_DATA[enemyTypeData][nowHandle].generateData = enemyData->genarateData.enemyGenerateData;
					//ハンドルを増やす
					++ADD_ENEMISE_HANDLE[enemyTypeData];

					//追加したら終了処理を入れる
					ENEMIES[enemyType][enemyCount]->GetData()->genarateData.Finalize();
				}
			}
		}
	}
}

void KazEnemyHelper::InitEnemy(const KazMath::Transform3D* arg_playerTransform, std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES, std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> RESPONE_DATA, int GAME_FLAME, int GAME_STAGE_LEVEL, bool DEMO_FLAG)
{
	for (int enemyType = 0; enemyType < RESPONE_DATA.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < RESPONE_DATA[enemyType].size(); ++enemyCount)
		{
			bool enableToUseThisDataFlag = RESPONE_DATA[enemyType][enemyCount].layerLevel != -1;
			bool readyToInitDataFlag = RESPONE_DATA[enemyType][enemyCount].flame == GAME_FLAME &&
				RESPONE_DATA[enemyType][enemyCount].layerLevel == GAME_STAGE_LEVEL;

			if (enableToUseThisDataFlag && readyToInitDataFlag && ENEMIES[enemyType][enemyCount] != nullptr)
			{
				ENEMIES[enemyType][enemyCount]->Init(arg_playerTransform, RESPONE_DATA[enemyType][enemyCount].generateData, DEMO_FLAG);
			}
		}
	}
}

bool KazEnemyHelper::LockOn(Cursor *CURSOR, const std::shared_ptr<IEnemy> &ENEMY, float FONT_SIZE, RESOURCE_HANDLE LOCKON_SOUND)
{
	if (ENEMY == nullptr)
	{
		return false;
	}

	//ロックオン判定
	bool enableToLockOnNumFlag = CURSOR->LockOn();
	bool enableToLockOnEnemyFlag = ENEMY->IsAlive() && !ENEMY->LockedOrNot();
	bool hitFlag = CollisionManager::Instance()->CheckRayAndSphere(CURSOR->hitBox, ENEMY->GetData()->hitBox);
	if (!hitFlag ||
		!enableToLockOnNumFlag ||
		!enableToLockOnEnemyFlag ||
		CURSOR->releaseFlag)
	{
		return false;
	}

	//カーソルのカウント数を増やす
	CURSOR->Hit(ENEMY->GetData()->hitBox.center);
	//敵が当たった情報を書く
	ENEMY->Hit();

	return true;
}