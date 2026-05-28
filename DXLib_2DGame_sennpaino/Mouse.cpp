#include"DXLib.h"
#include"Mouse.h"

namespace
{
	int lastMouse = 0;
	int nowMouse = 0;
}

namespace Mouse
{
	void Mouse::Update()
	{
		lastMouse = nowMouse;		// 前の入力を現在の入力にする
		nowMouse = GetMouseInput(); // 最新の入力を取得 
	}

	bool Mouse::IsPress()
	{
		return true;
	}

	bool Mouse::IsTriggerRight()
	{
		bool isNow = (nowMouse & MOUSE_INPUT_RIGHT);   // 今押されているか
		bool isLast = (lastMouse & MOUSE_INPUT_RIGHT); // １フレーム前に押されていたか

		if (isNow && !isLast)
		{
			return true;
		}
		return false;

	}

	bool Mouse::IsTriggerLeft()
	{
		bool isNow = (nowMouse & MOUSE_INPUT_LEFT);   // 今押されているか
		bool isLast = (lastMouse & MOUSE_INPUT_LEFT); // １フレーム前に押されていたか

		if (isNow && !isLast)
		{
			return true;
		}
		return false;
	}

	bool Mouse::IsRelease()
	{
		return true;
	}
}