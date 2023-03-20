#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/KazRender.h"

class GoalBoxPortalStringEffect
{
public:
	GoalBoxPortalStringEffect();

	void Init(const KazMath::Vec3<float> &POS);
	void Update();
	void Draw();

	void Start();
private:
	bool startFlag;//開始

	KazMath::Vec3<float>basePos;//基準の位置
	static const int STRING_TEXTURE_ARRAY_NUM = 100;	//表示するスプライトの総数
	std::array<Sprite3DRender, STRING_TEXTURE_ARRAY_NUM> stringRender;//表示するスプライト達

	void InitStringEffect(int INDEX);
};
