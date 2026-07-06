#include"Input.h"
#include<DxLib.h>

void Input::Update()
{
	_lastPad = _nowPad;
	_nowPad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	GetJoypadAnalogInputRight(&_rx, &_ry, DX_INPUT_PAD1);
}

bool Input::IsPress(int key)
{
	return (_nowPad & key);
}

bool Input::IsTrigger(int key)
{
	bool isNow = (_nowPad & key);
	bool isLast = (_lastPad & key);

	return (!isLast && isNow);
}

bool Input::IsRelease(int key)
{
	bool isNow = (_nowPad & key);
	bool isLast = (_lastPad & key);

	return (!isNow && isLast);
}