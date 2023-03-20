#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include"../Math/KazMath.h"
#include"../Helper/DirtyFlag.h"
#include"../Render/RenderData.h"
#include"../KazLibrary/Math/KazMath.h"

class Sprite3DRender : public IRender
{
public:
	Sprite3DRender(const KazMath::Vec2<float> ANCHOR_POINT = { 0.5f,0.5f });
	~Sprite3DRender();
	void Draw();

	DirectX::XMMATRIX GetMotherMatrix();
	Sprite3DData data;
private:
	std::array<SpriteVertex, 4> vertices;
	std::array<USHORT,6> indices;
	KazMath::Vec2<float> anchorPoint;
	KazMath::Vec2<int> texSize;


	DirectX::XMMATRIX motherMat;
	RESOURCE_HANDLE vertexBufferHandle;
	RESOURCE_HANDLE indexBufferHandle;
	RESOURCE_HANDLE constBufferHandle;

	UINT vertByte;
	UINT indexByte;

};