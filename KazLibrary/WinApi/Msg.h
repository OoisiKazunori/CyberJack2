#pragma once
#include<Windows.h>


class Msg 
{
public:
	bool CheckMessage();

private:
	MSG msg{};
};

