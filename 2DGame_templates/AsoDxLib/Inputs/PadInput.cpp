#include "PadInput.h"
#include <DxLib.h>

PadInput::PadInput() : _lastKey(0), _nowKey(0), _rx(0), _ry(0), _lx(0), _ly(0)
{
}

PadInput::~PadInput()
{
}

void PadInput::Init()
{
}

void PadInput::Update()
{
    // 現在の入力状態を１フレーム前の入力状態として保存してから更新する
    _lastKey = _nowKey;
    _nowKey = GetJoypadInputState(DX_INPUT_PAD1);

    // 左スティックの入力を取得
    GetJoypadAnalogInput(&_lx, &_ly, DX_INPUT_PAD1);

    // 右スティックの入力を取得
    GetJoypadAnalogInputRight(&_rx, &_ry, DX_INPUT_PAD1);
}

void PadInput::DebugDraw() const
{
#ifdef _DEBUG
    DrawFormatString(0, 20, 0xffffff, "rx : %d, ry : %d", _rx, _ry);
    DrawFormatString(0, 40, 0xffffff, "lx : %d, ly : %d", _lx, _ly);
#endif
}

bool PadInput::IsPress(int key) const
{
    bool isNow = _nowKey & key;

    return isNow;
}

bool PadInput::IsTrigger(int key) const
{
    bool isNow = _nowKey & key;
    bool isLast = _lastKey & key;

    return (isNow && !isLast);
}

bool PadInput::IsRelease(int key) const
{
    bool isNow = _nowKey & key;
    bool isLast = _lastKey & key;

    return (!isNow && isLast);
}
