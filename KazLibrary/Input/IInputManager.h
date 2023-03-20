#pragma once
#include"../DirectXCommon/Base.h"

/// <summary>
/// “ü—Í‚Ì’ŠÛƒNƒ‰ƒX
/// </summary>
class IInputManager 
{
public:
	IInputManager() = default;
	~IInputManager() = default;

	virtual void InputLog() = 0;
	virtual bool InputTrigger(int BUTTON) = 0;
	virtual bool InputState(int BUTTON) = 0;
	virtual bool InputRelease(int BUTTON) = 0;
};

