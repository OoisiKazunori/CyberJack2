#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include<map>

template<typename T>
struct KeyFrame
{
	int frame;
	T value;
};

template<typename T>
struct Animation
{
	int startFrame;
	int endFrame;
	std::vector<KeyFrame<T>>keyFrames;
};

class Bone
{
public:
	static size_t GetSizeWithOutName()
	{
		return sizeof(char) + sizeof(int) + sizeof(KazMath::Vec3<float>) + sizeof(DirectX::XMMATRIX);
	}

	std::string name;
	char parent = -1;	//親ボーン
	std::vector<char>children;	//子ボーン
	int transLayer = 0;	//変形階層
	KazMath::Vec3<float> pos = { 0.0f,0.0f,0.0f };
	DirectX::XMMATRIX invBindMat = DirectX::XMMatrixIdentity();
};

class Skeleton
{
public:
	static const std::string DEFINE_PARENT_BONE_NAME;
	struct BoneAnimation
	{
	public:
		Animation<KazMath::Vec3<float>>posAnim;
		Animation<DirectX::XMVECTOR>rotateAnim;
		Animation<KazMath::Vec3<float>>scaleAnim;
	};
	struct ModelAnimation
	{
		//全てのボーンのアニメーションが終了する時間
		int finishTime;
		//ボーン単位のアニメーション
		std::map<std::string, BoneAnimation>boneAnim;
	};

	//座標系変換を修正するための行列
	DirectX::XMMATRIX coordinateSystemConvertMat = DirectX::XMMatrixIdentity();

	std::vector<Bone>bones;
	std::map<std::string, int>boneIdxTable;
	/*
		アニメーション情報（Animatorからの参照用）
		キーは アニメーション名
	*/
	std::map<std::string, ModelAnimation>animations;

	void CreateBoneTree(const DirectX::XMMATRIX &CoordinateSysConvertMat);
	int GetIndex(const std::string& BoneName);
};

struct VertexData
{
	std::string name;
	std::vector<KazMath::Vec3<float>> verticesArray;
	std::vector<KazMath::Vec2<float>> uvArray;
	std::vector<KazMath::Vec3<float>> normalArray;
	std::vector<KazMath::Vec3<float>> tangentArray;
	std::vector<KazMath::Vec3<float>> binormalArray;
	std::vector<KazMath::Vec4<int>> boneIdx;
	std::vector<KazMath::Vec4<float>> boneWeight;
	std::vector<UINT>indexArray;
};

//バッファ生成用の構造体
struct MaterialBufferData
{
	DirectX::XMFLOAT3 ambient;//アンビエント
	float pad;
	DirectX::XMFLOAT3 diffuse;//ディフューズ
	float pad2;
	DirectX::XMFLOAT3 specular;//スペキュラー
	float alpha;
};

struct MaterialData
{
	KazMath::Vec3<float> ambient;//アンビエント
	KazMath::Vec3<float> diffuse;//ディフューズ
	KazMath::Vec3<float> specular;//スペキュラー
	std::vector<KazBufferHelper::BufferData> textureBuffer;

	MaterialBufferData GetMaterialData();
};

struct ModelMeshData
{
	//頂点情報
	VertexData vertexData;
	//マテリアル情報
	MaterialData materialData;
};

struct ModelInfomation
{
	std::vector<ModelMeshData> modelData;
	RESOURCE_HANDLE modelVertDataHandle;
	std::shared_ptr<Skeleton> skelton;

	ModelInfomation(const std::vector<ModelMeshData>& model, RESOURCE_HANDLE vertHandle, const std::shared_ptr<Skeleton>& skel);
};

class ModelAnimator
{
	static const int MAX_BONE_NUM = 256;

	//対応するスケルトンの参照
	std::weak_ptr<Skeleton>attachSkelton;
	//ボーンのローカル行列
	KazBufferHelper::BufferData boneBuff;
	//ボーン行列（GPU送信用）
	std::array<DirectX::XMMATRIX, MAX_BONE_NUM>boneMatricies;
	//各ボーンのTransform
	std::vector<KazMath::Transform3D>m_boneTransform;

	struct PlayAnimation
	{
		std::string name;	//アニメーション名
		float past = 0.0f;
		bool loop = false;
		bool finish = false;

		PlayAnimation(const std::string& Name, const bool& Loop) :name(Name), loop(Loop) {}
	};

	std::list<PlayAnimation>playAnimations;

	//ComputeShader computeAnimation;


	//ボーンTransform一括計算
	void CalculateTransform(KazMath::Transform3D& BoneTransform, const Skeleton::BoneAnimation& BoneAnim, const float& Frame, bool& FinishFlg);
	void BoneMatrixRecursive(const int& BoneIdx, const float& Past, bool* Finish, Skeleton::ModelAnimation& Anim);

public:
	ModelAnimator() {}
	ModelAnimator(std::weak_ptr<ModelInfomation>Model);
	void Attach(std::weak_ptr<ModelInfomation>Model);


	//単位行列で埋める
	void Reset();

	//指定したアニメーションの始まりの姿勢をセット
	void SetStartPosture(const std::string& AnimationName);

	//指定したアニメーションの終わりの姿勢をセット
	void SetEndPosture(const std::string& AnimationName);

	//アニメーション再生
	void Play(const std::string& AnimationName, const bool& Loop, const bool& Blend, float InitPast = 0.0f);
	//何かしらのアニメーションが現在再生中か
	bool IsPlay()const
	{
		return !playAnimations.empty();
	}

	//指定のアニメーションが現在再生中か
	bool IsPlay(const std::string& AnimationName)
	{
		auto result = std::find_if(playAnimations.begin(), playAnimations.end(), [AnimationName](PlayAnimation& Anim)
			{
				return AnimationName.compare(Anim.name) == 0;
			});
		return result != playAnimations.end();
	}
	//アニメーション更新
	void Update(const float& arg_timeScale);

	//ボーントランスフォームに親設定
	void SetParentTransform(KazMath::Transform3D& arg_parent);

	const KazBufferHelper::BufferData& GetBoneMatBuff() { return boneBuff; }
	KazMath::Transform3D& GetBoneTransform(const std::string& BoneName);
};