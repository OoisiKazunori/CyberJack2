#pragma once
#include"../KazLibrary/Render/KazRender.h"

class OldComputer
{
public:
	OldComputer();
	void Init(RESOURCE_HANDLE MOVIE_HANDLE);
	void Update();
	void Draw();

	void SetMonitorTexture(RESOURCE_HANDLE MOVIE_HANDLE);
	void SetTransform(const KazMath::Transform3D &TRANSFORM);

	void BloomMonitor()
	{
		data.screenFlag = 0;
		data.bloomColor = { 1.0f,0.0f,0.0f,0.4f };
	};

	void PortalMode()
	{
		data.screenFlag = 1;
		data.bloomColor = { 1.0f,0.0f,0.0f,0.0f };
	};

private:
	ObjModelRender render;
	RESOURCE_HANDLE dataHandle;
	struct Data
	{
		DirectX::XMFLOAT4 bloomColor;
		UINT screenFlag;
	};
	Data data;
};

