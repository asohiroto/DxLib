#include "MyMath.h"
#include <random>
#include <cmath>
float MyMath::GetRandF(float min, float max)
{
    // 非決定的な乱数生成器
    static std::random_device rd;
    // メルセンヌ・ツイスタ法による擬似乱数生成器
    static std::mt19937 mt(rd());
    // minからmaxの範囲で一様分布の乱数を生成
    std::uniform_real_distribution<float> dist(min, max);
    return dist(mt);
}

int MyMath::GetRand(int min, int max)
{
    // 非決定的な乱数生成器
    static std::random_device rd;
    // メルセンヌ・ツイスタ法による擬似乱数生成器
    static std::mt19937 mt(rd());
    // minからmaxの範囲で一様分布の乱数を生成
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

bool MyMath::IsRand()
{
    return GetRand(0, 1);
}
