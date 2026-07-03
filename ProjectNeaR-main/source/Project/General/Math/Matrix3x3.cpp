#include "Matrix3x3.h"

Matrix3x3::Matrix3x3()
{
    IdentityMat3x3();
}

//単位行列
Matrix3x3 IdentityMat3x3()
{
    Matrix3x3 identity = {};//要素をすべて0にする
    identity.mat[0][0] = 1.0f;
    identity.mat[1][1] = 1.0f;
    identity.mat[2][2] = 1.0f;
    return identity;
}

//平行移動行列
Matrix3x3 TranslateMat3x3(float x, float y)
{
    Matrix3x3 translate = {};
    translate = IdentityMat3x3();//単位行列にする
    translate.mat[0][2] = x;
    translate.mat[1][2] = y;
    return translate;
}

//回転行列
Matrix3x3 RotateMat3x3(float angle)
{
    Matrix3x3 rotate = {};
    rotate = IdentityMat3x3();//単位行列にする
    //[cos][-sin]
    //[sin][cos]
    rotate.mat[0][0] = cosf(angle);
    rotate.mat[0][1] = -sinf(angle);
    rotate.mat[1][0] = sinf(angle);
    rotate.mat[1][1] = cosf(angle);
    return rotate;
}

//２つの行列の乗算を返す
Matrix3x3 Matrix3x3::MultipleMat3x3(const Matrix3x3& left, const Matrix3x3& right)
{
    //これに乗算結果を入れていく
    Matrix3x3 mul = {};
    for (int k = 0;k <= 2;++k)
    {
        for (int j = 0;j <= 2;++j)
        {
            for (int i = 0;i <= 2;++i)
            {
                mul.mat[k][j] += left.mat[k][i] * right.mat[i][j];
            }
        }
    }
    //mul.mat[0][0] = 
    //    left.mat[0][0] * right.mat[0][0] + 
    //    left.mat[0][1] * right.mat[1][0] + 
    //    left.mat[0][2] * right.mat[2][0];
    //mul.mat[0][1] =
    //    left.mat[0][0] * right.mat[0][1] +
    //    left.mat[0][1] * right.mat[1][1] +
    //    left.mat[0][2] * right.mat[2][1];
    //...
    return mul;
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& right)
{
    return MultipleMat3x3(*this, right);
}

//ベクトルに対して行列乗算を適用し、結果のベクトルを返す
Vector2 Matrix3x3::MultipleVec2(const Matrix3x3& m, const Vector2& v)
{
    Vector2 vec = {};
    vec.x = m.mat[0][0] * v.x + m.mat[0][1] * v.y + m.mat[0][2] * 1.0f;
    vec.y = m.mat[1][0] * v.x + m.mat[1][1] * v.y + m.mat[1][2] * 1.0f;
    return vec;
}

Vector2 Matrix3x3::operator*(const Vector2& v)
{
    return MultipleVec2(*this, v);
}


///とある点を中心に回転して、その回転後の座標を返します
Matrix3x3 RotatePositionMatrix3x3(const Vector2& center, float angle)
{
    //中心を原点に平行移動させる
    //原点中心に回転
    //中心をもとの座標に戻す
    Matrix3x3 rotaMat = TranslateMat3x3(center.x, center.y) *//元の座標
        RotateMat3x3(angle) *//回転
        TranslateMat3x3(-center.x, -center.y);//原点に戻す
    return rotaMat;
}
