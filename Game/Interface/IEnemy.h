#pragma once
#include"../Game/Interface/IOperationObject.h"
#include"../KazLibrary/Collision/CollisionManager.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../Enemy/EnemyData.h"
#include"../Game/Effect/MeshParticleEmitter.h"

/// <summary>
/// 敵の抽象クラス
/// </summary>
class IEnemy :public IOperationObject
{
public:

	/// <summary>
	/// 確実に初期化したいデータを入れる
	/// </summary>
	IEnemy();
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~IEnemy()
	{
	};

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="POS">初期座標</param>
	virtual void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG = false) = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 敵を死亡させます
	/// </summary>
	void Dead();

	/// <summary>
	/// 死亡演出
	/// </summary>
	void DeadEffect(KazMath::Vec3<float> *POS, KazMath::Vec3<float> *ROTATION, int *ALPHA);

	//死亡判定と処理
	bool ProcessingOfDeath(EnemyDeathType TYPE);
	bool ProcessingOfDeathFbx(EnemyDeathType TYPE);


	/// <returns>敵の情報</returns>
	const std::unique_ptr<EnemyData> &GetData();

	void DeadSound();
	void ShotSound();
	void InitModel(const KazMath::Transform3D &TRANSFORM, const std::string &MODEL_FILEPASS, float HITBOX_RADIUS, EnemyModelType MODEL_TYPE, bool REV_UV_FLAG = false, bool GENERATE_PARTICLE_FLAG = true);

	//攻撃を確認する用の関数
	virtual void DebugShot() { debugShotFlag = true; };

	void SetLight(const KazMath::Vec3<float> LIGHT_DIR, bool OBJ_FLAG)
	{
		if (!OBJ_FLAG)
		{
			DirectX::XMFLOAT3 dir = LIGHT_DIR.ConvertXMFLOAT3();
			iEnemy_FbxModelRender->TransData(&dir, fbxLightHandle, typeid(DirectX::XMFLOAT3).name());
		}
		else
		{
			DirectX::XMFLOAT3 dir = LIGHT_DIR.ConvertXMFLOAT3();
			iEnemy_ObjModelRender->TransData(&dir, objLightHandle, typeid(DirectX::XMFLOAT3).name());
		}
	};

	void ReleaseLight()
	{
		iEnemy_ObjModelRender->Release(objLightHandle);
		iEnemy_FbxModelRender->Release(fbxLightHandle);
	}

	//追加で描画したいものがあったら再定義する
	virtual void DrawIn() {};

	//共通処理--------------------------------------------
	void OnInit(bool MESH_PARTICLE_FLAG)
	{
		iEnemy_EnemyStatusData->meshParticleFlag = MESH_PARTICLE_FLAG;
	}
	void OnUpdate()
	{
		switch (modelType)
		{
		case ENEMY_MODEL_NONE:
			break;
		case ENEMY_MODEL_OBJ:
			//if (!ProcessingOfDeath(DEATH_ROLL))
			//{
			//	//登場処理
			//	if (iEnemy_ObjModelRender->data.colorData.color.a < 255)
			//	{
			//		iEnemy_ObjModelRender->data.colorData.color.a += 5;
			//	}
			//	else
			//	{
			//		iEnemy_ObjModelRender->data.colorData.color.a = 255;
			//	}
			//}
			break;
		case ENEMY_MODEL_FBX:
			//ProcessingOfDeathFbx(DEATH_SINK);
			break;
		case ENEMY_MODEL_MESHPARTICLE:
			iEnemy_MeshModelRender->Update();
			break;
		default:
			break;
		}
	};
	//敵のモデル描画
	void OnDraw()
	{
		if (1.0f <= iEnemy_ObjModelRender->data.colorData.color.a)
		{
			switch (modelType)
			{
			case ENEMY_MODEL_NONE:
				break;
			case ENEMY_MODEL_OBJ:
				iEnemy_ObjModelRender->Draw();
				break;
			case ENEMY_MODEL_FBX:
				iEnemy_FbxModelRender->Draw();
				break;
			case ENEMY_MODEL_MESHPARTICLE:
				iEnemy_MeshModelRender->Draw();
				break;
			default:
				break;
			}
			DrawIn();
			LockOnWindow(*iEnemy_EnemyStatusData->hitBox.center);
		}
	};
	//共通処理--------------------------------------------



	std::unique_ptr<EnemyData> iEnemy_EnemyStatusData;		//敵の状態を保存するデータ
	ObjModelRenderPtr iEnemy_ObjModelRender;				//敵の描画
	FbxModelRenderPtr iEnemy_FbxModelRender;				//敵の描画
	std::unique_ptr<MeshParticleEmitter> iEnemy_MeshModelRender;
	KazMath::Vec3<float> lerpPos;
	float yVel;

protected:
	bool initDeadSoundFlag;
	bool demoFlag;
	bool debugShotFlag;
private:
	int deadSoundHandle;
	int shotSoundHandle;

	DirtyFlag<short>hpDirtyFlag;
	bool hitFlag;

	RESOURCE_HANDLE fbxLightHandle;
	RESOURCE_HANDLE objLightHandle;

	KazMath::Vec3<float>initDeadRotaVel;
	KazMath::Vec3<float>initDeadYVel;

	EnemyModelType modelType;
};
