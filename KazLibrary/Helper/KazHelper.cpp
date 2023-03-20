#include "KazHelper.h"
#include"../Camera/CameraMgr.h"

bool KazHelper::DirtyFlag(DirectX::XMFLOAT3 VALUE, DirectX::XMFLOAT3 OLD_VALUE)
{
	if (VALUE.x != OLD_VALUE.x ||
		VALUE.y != OLD_VALUE.y ||
		VALUE.z != OLD_VALUE.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool KazHelper::DirtyFlag(DirectX::XMFLOAT2 VALUE, DirectX::XMFLOAT2 OLD_VALUE)
{
	if (VALUE.x != OLD_VALUE.x ||
		VALUE.y != OLD_VALUE.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool KazHelper::DirtyFlag(float VALUE, float OLD_VALUE)
{
	if (VALUE != OLD_VALUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool KazHelper::IsInScreen(const KazMath::Vec3<float> &POS)
{
	KazMath::Vec3<float> screenPos = KazMath::ConvertWorldPosToScreenPos(POS, CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());

	bool outFlag = true;
	if (screenPos.x <= static_cast<float>(0))
	{
		outFlag = false;
	}
	if (screenPos.y <= static_cast<float>(0))
	{
		outFlag = false;
	}
	if (static_cast<float>(WIN_X) <= screenPos.x)
	{
		outFlag = false;
	}
	if (static_cast<float>(WIN_Y) <= screenPos.y)
	{
		outFlag = false;
	}
	return outFlag;
}

KazMath::Vec3<float> KazHelper::GetScreenPos(const KazMath::Vec3<float> &POS, const KazMath::Vec2<float> &LEFTUP_POS)
{
	KazMath::Vec3<float>screenPos = KazMath::ConvertWorldPosToScreenPos(POS, CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());

	KazMath::Vec3<float> leftUpPos = KazMath::ConvertScreenPosToWorldPos(KazMath::Vec3<float>(LEFTUP_POS.x, LEFTUP_POS.y, screenPos.z), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());

	return leftUpPos;
}

KazMath::Vec3<float> KazHelper::GetRightDownStagePos(const KazMath::Vec3<float> &POS, const KazMath::Vec2<float> &RIGHT_POS)
{
	KazMath::Vec3<float>screenPos = KazMath::ConvertWorldPosToScreenPos(POS, CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());

	KazMath::Vec3<float> rightDown =
		KazMath::ConvertScreenPosToWorldPos(KazMath::Vec3<float>(RIGHT_POS.x,RIGHT_POS.y, screenPos.z), CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());

	return rightDown;
}

int KazHelper::GetDigit(int NUM)
{
	if (NUM == 0)return 1;

	int result = 0;
	while (NUM != 0)
	{
		NUM /= 10;
		result++;
	}
	return result;
}

std::vector<int> KazHelper::CountNumber(int TIME, int ZERO)
{
	int haveZero = 0;

	//0–„‚ß‚·‚é
	if (ZERO != -1)
	{
		haveZero = ZERO;
	}
	else
	{
		haveZero = GetDigit(TIME);
	}

	std::vector<int> Number(haveZero);

	int tmp = TIME;
	for (int i = 0; tmp > 0; i++)
	{
		int result = tmp % 10;
		Number[i] = result;
		tmp /= 10;
	}
	for (int i = 0; i < Number.size(); i++)
	{
		if (Number[i] == 0)
		{
			Number[i] = 0;
		}
	}
	std::reverse(Number.begin(), Number.end());
	return Number;
}

std::vector<char> KazHelper::CovertStringToChar(std::string STRING)
{
	std::vector<char>result;
	for (int i = 0; i < STRING.size(); ++i)
	{
		result.push_back(STRING[i]);
	}
	result.push_back('\0');


	return result;
}


void KazHelper::ConvertStringToWchar_t(std::string STRING, wchar_t *WCHAR_STRING, size_t ARRAY_SIZE)
{
	MultiByteToWideChar(CP_ACP, 0, STRING.c_str(), -1, WCHAR_STRING, static_cast<INT>(ARRAY_SIZE));
}
