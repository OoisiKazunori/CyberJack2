#pragma once
#include"../Game/UI/Letter.h"
#include<array>

class AttackLog
{
public:
	AttackLog();
	void Init(const KazMath::Vec2<float> &POS, bool TIMEOVER_FLAG = true, int LOG_MAX = -1);
	void Finalize();
	void Update();
	void Draw();

	void WriteLog(const std::string &STRING, float FONT_SIZE);
private:
	struct StringData
	{
		String log;
		int logIndex;
	};
	std::array<StringData, 15>logStringArray;
	int logArrayIndex;
	int logDataIndex;
	KazMath::Vec2<float>pos;

	bool useTimeOverFlag;
	int logMaxNum;
};

