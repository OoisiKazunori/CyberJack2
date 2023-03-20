#include "RegisterNameWithData.h"
#include"../Helper/KazHelper.h"

RegisterNameWithData::RegisterNameWithData()
{
}

RegisterNameWithData::~RegisterNameWithData()
{
}

bool RegisterNameWithData::RegisterName(std::string NAME)
{
	for (int i = 0; i < name.size(); i++)
	{
		if (name[i] == NAME)
		{
			std::string dangerString = "ŠëŒ¯:" + NAME + "‚ð“o˜^‚µ‚æ‚¤‚Æ‚µ‚Ü‚µ‚½‚ªA“¯‚¶–¼‘O‚ª‚ ‚Á‚½ˆ×“o˜^‚Å‚«‚Ü‚¹‚ñ‚Å‚µ‚½";
			ErrorCheck(dangerString.c_str());
			return false;
		}
	}

	name.push_back(NAME);
	return true;
}

bool RegisterNameWithData::UnRegisterName(std::string NAME)
{
	for (int i = 0; i < name.size(); i++)
	{
		if (name[i] == NAME)
		{
			name.erase(name.begin() + i);
			return true;
		}
	}

	std::string dangerString = "ŠëŒ¯:" + NAME + "‚ðíœ‚µ‚æ‚¤‚Æ‚µ‚Ü‚µ‚½‚ªA–³‚©‚Á‚½ˆ×íœ‚ÉŽ¸”s‚µ‚Ü‚µ‚½";
	ErrorCheck(dangerString.c_str());
	return false;
}

int RegisterNameWithData::IsNameExists(std::string NAME)
{
	for (int i = 0; i < name.size(); i++)
	{
		if (name[i] == NAME)
		{
			return i;
		}
	}
	std::string dangerString = "ŠëŒ¯:" + NAME + "‚ª“o˜^‚³‚ê‚Ä‚¢‚Ü‚¹‚ñ";
	ErrorCheck(dangerString.c_str());
	return -1;
}

void RegisterNameWithData::UnRegisterName(int HANDLE)
{
	if (KazHelper::IsitInAnArray(HANDLE, name.size()))
	{
		name[HANDLE] = "";
	}
}
