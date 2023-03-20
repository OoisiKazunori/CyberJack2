#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"

class LineFlashLight
{
public:
	LineFlashLight();
	void Init(const std::vector<KazMath::Vec3<float>*> &POS_ARRAY, int TIMER);
	void Finalize();
	void Update();
	void Draw();

	bool IsFinish();
private:
	std::vector<KazMath::Vec3<float>*>posArray;//OBJ‚ª‚Ç‚Ì•ûŒü‚É’H‚Á‚Äi‚ñ‚Å‚¢‚­‚©”»’f‚·‚éˆ×‚ÌÀ•W‚ğ“Z‚ß‚½”z—ñ

	float speed;
	int lineIndex;

	BoxPolygonRender boxR;

	void CheckWhereToGo();
	float CaluDisntace();

	float *nowPos = nullptr;
	float *nextPos = nullptr;
	bool finishFlag;

	bool useXPosFlag;
	bool useYPosFlag;
	bool useZPosFlag;

};
