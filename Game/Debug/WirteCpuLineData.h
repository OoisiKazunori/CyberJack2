#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Helper/ISinglton.h"
#include"../Game/Debug/ParameterMgr.h"
#include"../KazLibrary/Math/KazMath.h"

struct WriteData
{
	std::array<KazMath::Vec3<float>*, 4> startPosData;
	std::array<KazMath::Vec3<float>*, 4> endPosData;
};

class WirteCpuLineData:public ISingleton<WirteCpuLineData>
{
public:
	WirteCpuLineData();

	void Write();
	void Load();

	void UpdataData(int ID,int LINE_NUM, KazMath::Vec3<float> *START_POS, KazMath::Vec3<float> *END_POS);

	bool importFlag;
private:
	ParameterMgr stageParamLoader;

	static const int LINE_MAX_NUM = 3;

	static const int line = 7;
	static const int DATA_MAX = (line * 2) * 2 + line * 2;
	std::array<WriteData, DATA_MAX>data;
};

