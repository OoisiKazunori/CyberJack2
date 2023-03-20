#pragma once

/// <summary>
/// 継承したクラスをシングルトンにします
/// </summary>
/// <typeparam name="_T">継承するクラス</typeparam>
template <typename _T>
class ISingleton 
{
protected:
	ISingleton() {}
	virtual ~ISingleton() {}
	ISingleton(const ISingleton &r) {}
	ISingleton &operator=(const ISingleton &r) {}

public:
	static _T *Instance()
	{
		static _T inst;
		return &inst;
	};
};