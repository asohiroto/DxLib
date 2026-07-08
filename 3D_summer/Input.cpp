#include "Input.h"
#include<DxLib.h>

Input::Input() :
	_lastPad(0),
	_nowPad(0),
	_rx(0),
	_ry(0),
	_lx(0),
	_ly(0), 
	_usePadNum()
{

}

Input::~Input()
{

}

void Input::Init(int padNum)
{
	_usePadNum = padNum;
}

void Input::Update()
{
	// 入力状況の更新
	_lastPad = _nowPad;
	_nowPad = GetJoypadInputState(_usePadNum);

	// 入力値の取得
	GetJoypadAnalogInputRight(&_rx, &_ry, _usePadNum);
	GetJoypadAnalogInput(&_lx, &_ly, _usePadNum);
}

bool Input::IsTrigger(int key)
{
	bool isNow = _nowPad & key;
	bool isLast = _lastPad & key;

	return (!isLast && isNow);
}

bool Input::IsPress(int key)
{
	return (_nowPad & key);
}

bool Input::IsRelease(int key)
{
	bool isNow = _nowPad & key;
	bool isLast = _lastPad & key;

	return (isLast && !isNow);
}
