#include "Input.h"
#include <DxLib.h>

Input::Input() :
	_lastKey(0),
	_nowKey(0),
	_rx(0), _ry(0),
	_lx(0), _ly(0)
{
}

Input::~Input()
{
}

void Input::Init()
{
}

void Input::Update()
{
	// 現在の入力状態を１フレーム前の入力状態として保存してから更新する
	_lastKey = _nowKey;
	_nowKey = GetJoypadInputState(DX_INPUT_PAD1);

	// 左スティックの入力を取得
	GetJoypadAnalogInput(&_lx, &_ly, DX_INPUT_PAD1);

	// 右スティックの入力を取得
	GetJoypadAnalogInputRight(&_rx, &_ry, DX_INPUT_PAD1);
}

void Input::Draw()
{
#ifdef _DEBUG
	DrawFormatString(0, 20, 0xffffff, "rx : %d, ry : %d", _rx, _ry);
	DrawFormatString(0, 40, 0xffffff, "lx : %d, ly : %d", _lx, _ly);
#endif
}

bool Input::IsPress(int key)
{
	bool isNow = _nowKey & key;
	bool isLast = _lastKey & key;

	return (isNow && isLast);
}

bool Input::IsTrigger(int key)
{
	bool isNow = _nowKey & key;
	bool isLast = _lastKey & key;

	return (isNow && !isLast);
}

bool Input::IsRelease(int key)
{
	bool isNow = _nowKey & key;
	bool isLast = _lastKey & key;

	return (!isNow && isLast);
}
