#pragma once
#include"../DirectXCommon/Base.h"

template<class T>
class DirtyFlag
{
public:
	DirtyFlag(T *TARGET, bool SEND_DATA_FIRST_TIME = true) : target(TARGET), oldTarget(*TARGET)
	{
		sendDataFlag = true;
		sendDataFlag = SEND_DATA_FIRST_TIME;
	};

	bool Dirty()
	{
		bool tmp = sendDataFlag;
		sendDataFlag = false;
		return (oldTarget != *target) || tmp;
	}
	void Record()
	{
		oldTarget = *target;
	};

private:
	T *target;
	T oldTarget;

	bool sendDataFlag;
};




class DirtySet
{
public:

	~DirtySet()
	{
	}

	DirtySet(DirectX::XMMATRIX &DATA)
	{
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				dirtyFlag.push_back(&DATA.r[y].m128_f32[x]);
			}
		}
	};


	bool Dirty()
	{
		//‚Ç‚Ì’l‚ª­‚µ‚Å‚à1Flame‘O‚Ì’l‚Æ”ä‚×‚Ä•Ï‚í‚Á‚Ä‚¢‚½‚çtrue
		bool isValueDiffrendFlag = false;
		for (int i = 0; i < dirtyFlag.size(); i++)
		{
			if (dirtyFlag[i].Dirty())
			{
				isValueDiffrendFlag = true;
			}
		}
		return isValueDiffrendFlag;
	};


	void Record()
	{
		for (int i = 0; i < dirtyFlag.size(); i++)
		{
			dirtyFlag[i].Record();
		}
	}


private:
	std::vector<DirtyFlag<float>>dirtyFlag;
};
