#include "Msg.h"

bool Msg::CheckMessage()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) 
	{
		return false;
	}
	else
	{
		return true;
	}
}
