#pragma once
#include<array>
#include<vector>
#include"../KazLibrary/Render/KazRender.h"

class MountainObject
{
public:
	MountainObject();
	void Init(const KazMath::Transform3D &TRANSFORM, RESOURCE_HANDLE HANDLE, const int *CAMERA_ID_PTR);
	void Update();
	void Draw();

private:
	struct FloorObjData
	{
		std::array<ObjModelRender, 2> objRender;
		KazMath::Vec3<float>initScale;
	};
	FloorObjData floorObjectRender;

	float scaleRate;
	bool reversValueFlag;
	const int *cameraId;
};

