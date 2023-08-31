#pragma once
#include<memory>
#include<array>
#include"../Game/Enemy/EnemyData.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Enemy/VirusEnemy.h"
#include"../Game/UI/Cursor.h"


namespace KazEnemyHelper
{
	static const int ENEMY_TYPE_MAX = 20;				//レイヤーレベルの最大数
	static const int ENEMY_NUM_MAX = 100;				//1レイヤーレベルに登場する敵の最大数
	static const int STAGE_NUM_MAX = 3;				//1レイヤーレベルに登場する敵の最大数

	void GenerateEnemy(
		std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES,
		std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> RESPONE_DATA,
		std::array<int, ENEMY_TYPE_MAX> &ENEMISE_HANDLE
	);

	void GenerateEnemy(
		std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES,
		std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> RESPONE_DATA,
		std::array<int, ENEMY_TYPE_MAX> &ENEMISE_HANDLE,
		std::vector<Sphere*> &ENEMISE_HITBOX
	);

	void AddEnemy(
		const std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES,
		std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &RESPONE_DATA,
		std::array<int, ENEMY_TYPE_MAX> &ADD_ENEMISE_HANDLE,
		int GAME_FLAME,
		int GAME_STAGE_LEVEL
	);

	void InitEnemy(
		const KazMath::Transform3D* arg_playerTransform,
		std::array<std::array<std::shared_ptr<IEnemy>, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> &ENEMIES,
		std::array<std::array<ResponeData, ENEMY_NUM_MAX>, ENEMY_TYPE_MAX> RESPONE_DATA,
		int GAME_FLAME,
		int GAME_STAGE_LEVEL,
		bool DEMO_FLAG = false
	);

	bool LockOn(Cursor *CURSOR, const std::shared_ptr<IEnemy> &ENEMY, float FONT_SIZE, RESOURCE_HANDLE LOCKON_SOUND);

	enum CameraDir
	{
		CAMERA_FRONT,
		CAMERA_BACK,
		CAMERA_LEFT,
		CAMERA_RIGHT,
	};
	struct ForceCameraData
	{
		int flame;
		CameraDir dir;
		ForceCameraData() :flame(-1)
		{
		};
	};
}
