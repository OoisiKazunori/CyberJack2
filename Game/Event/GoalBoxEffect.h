#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../Game/Interface/IOperationObject.h"
#include"../KazLibrary/Render/KazRender.h"
#include<array>
#include<memory>

class GoalBoxEffect
{
public:
	GoalBoxEffect();

	void Init(DirectX::XMMATRIX *ADRESS);
	void Update();
	void Draw();

	void Appear();
	void Disappear();

	static const int LIGHT_MAX_NUM = 8;
	std::array<Sprite3DRenderPtr, LIGHT_MAX_NUM> lightRender;
private:
	DirectX::XMMATRIX *motherPtr;
	std::array<RESOURCE_HANDLE, LIGHT_MAX_NUM> uvHandle;
	GoalLightData uvData;

	int drawHandle;
	bool disappearFlag;
};

