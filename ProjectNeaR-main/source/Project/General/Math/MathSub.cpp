#include "MathSub.h"

int MathSub::ClampInt(int value, int min, int max)
{
    //”ÍˆÍ“à‚Éû‚Ü‚Á‚Ä‚¢‚é‚È‚ç‚»‚Ì‚Ü‚Ü•Ô‚·
    int clampValue = value;
    //Å¬‚æ‚è¬‚³‚¢‚È‚çÅ¬‚É‡‚í‚¹‚é
    if (clampValue < min)
    {
        clampValue = min;
    }
    //Å‘å‚æ‚è‘å‚«‚¢‚È‚çÅ‘å‚É‡‚í‚¹‚é
    if (clampValue > max)
    {
        clampValue = max;
    }
    return clampValue;
}

float MathSub::ClampFloat(float value, float min, float max)
{
    //”ÍˆÍ“à‚Éû‚Ü‚Á‚Ä‚¢‚é‚È‚ç‚»‚Ì‚Ü‚Ü•Ô‚·
    float clampValue = value;
    //Å¬‚æ‚è¬‚³‚¢‚È‚çÅ¬‚É‡‚í‚¹‚é
    if (clampValue < min)
    {
        clampValue = min;
    }
    //Å‘å‚æ‚è‘å‚«‚¢‚È‚çÅ‘å‚É‡‚í‚¹‚é
    if (clampValue > max)
    {
        clampValue = max;
    }
    return clampValue;
}

float MathSub::Lerp(float start, float end, float t)
{
    float result = start * (1.0f - t) + end * t;
    return result;
}

float MathSub::Min(float a, float b)
{
    float min = a;
    if (a > b)
    {
        min = b;
    }
    return min;
}

float MathSub::Min(float a, float b,float c)
{
    float min = a;
    if (min > b)
    {
        min = b;
    }
    if (min > c)
    {
        min = c;
    }
    return min;
}


float MathSub::Max(float a, float b)
{
    float max = a;
    if (a < b)
    {
        max = b;
    }
    return max;
}

float MathSub::Max(float a, float b, float c)
{
    float max = a;
    if (max < b)
    {
        max = b;
    }
    if (max < c)
    {
        max = c;
    }
    return max;
}


float MathSub::Sign(float num)
{
    if (num == 0.0f)
    {
        return 0.0f;
    }
    else if (num > 0.0f)
    {
        return 1.0f;
    }
    else
    {
        return -1.0f;
    }
}
