#pragma once
class MathSub
{
public:
    /// <summary>
    /// ある値が一定範囲内に収まるようにクランプする
    /// </summary>
    /// <param name="value">クランプする値</param>
    /// <param name="min">最小</param>
    /// <param name="max">最大</param>
    /// <returns>int型</returns>
    static int ClampInt(int value, int min, int max);
    /// <summary>
    /// ある値が一定範囲内に収まるようにクランプする
    /// </summary>
    /// <param name="value">クランプする値</param>
    /// <param name="min">最小</param>
    /// <param name="max">最大</param>
    /// <returns>float型</returns>
    static float ClampFloat(float value, float min, float max);
    /// <summary>
    /// 線形補間
    /// </summary>
    /// <param name="start">最初</param>
    /// <param name="end">最後</param>
    /// <param name="t">補間の割合</param>
    /// <returns></returns>
    static float Lerp(float start, float end, float t);

    /// <summary>
	/// 最小値を返す
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    static float Min(float a, float b);

    static float Min(float a, float b, float c);

    /// <summary>
    /// 最大値を返す
    /// </summary>
    /// <param name="a"></param>
    /// <param name="b"></param>
    /// <returns></returns>
    static float Max(float a, float b);

    static float Max(float a, float b, float c);

    /// <summary>
    /// 値の符号を取得
    /// </summary>
    /// <param name="num"></param>
    /// <returns></returns>
    static float Sign(float num);
};

