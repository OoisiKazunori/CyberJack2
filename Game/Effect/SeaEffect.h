#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

class SeaEffect :public ISingleton<SeaEffect>
{

public:

	bool m_isOldSeaEffect;
	bool m_isSeaEffect;
	int m_seaID;

};

