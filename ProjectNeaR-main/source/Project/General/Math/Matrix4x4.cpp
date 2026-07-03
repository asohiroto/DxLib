#include "Matrix4x4.h"
#include <cmath>
#include <DxLib.h>

Matrix4x4::Matrix4x4():
    mat{}
{
}
//単位行列
Matrix4x4 Matrix4x4::IdentityMat4x4()
{
    Matrix4x4 identity = {};//要素をすべて0にする
    identity.mat[0][0] = 1.0f;
    identity.mat[1][1] = 1.0f;
    identity.mat[2][2] = 1.0f;
    identity.mat[3][3] = 1.0f;
    return identity;
}

//平行移動
Matrix4x4 Matrix4x4::TranslateMat4x4(float x, float y, float z)
{
    Matrix4x4 translate = {};
    translate = IdentityMat4x4();//単位行列にする
    translate.mat[3][0] = x;
    translate.mat[3][1] = y;
    translate.mat[3][2] = z;
    return translate;
}

//X軸回転
Matrix4x4 Matrix4x4::RotateXMat4x4(float angle)
{
    Matrix4x4 rotaX{};
    rotaX = IdentityMat4x4();//単位行列
    rotaX.mat[1][1] = cosf(angle);
    rotaX.mat[1][2] = -sinf(angle);
    rotaX.mat[2][1] = sinf(angle);
    rotaX.mat[2][2] = cosf(angle);
    return rotaX;
}

//Y軸回転
Matrix4x4 Matrix4x4::RotateYMat4x4(float angle)
{
    Matrix4x4 rotaY{};
    rotaY = IdentityMat4x4();//単位行列
    rotaY.mat[0][0] = cosf(angle);
    rotaY.mat[0][2] = sinf(angle);
    rotaY.mat[2][0] = -sinf(angle);
    rotaY.mat[2][2] = cosf(angle);
    return rotaY;
}

//Z軸回転
Matrix4x4 Matrix4x4::RotateZMat4x4(float angle)
{
    Matrix4x4 rotaZ{};
    rotaZ = IdentityMat4x4();//単位行列
    rotaZ.mat[0][0] = cosf(angle);
    rotaZ.mat[0][1] = -sinf(angle);
    rotaZ.mat[1][0] = sinf(angle);
    rotaZ.mat[1][1] = cosf(angle);
    return rotaZ;
}

//マトリクス同士の乗算
Matrix4x4 Matrix4x4::MultipleMat4x4(const Matrix4x4& left, const Matrix4x4& right)
{
    //これに乗算結果を入れていく
    Matrix4x4 mul = {};
    mul.mat[0][0] = left.mat[0][0] * right.mat[0][0] + left.mat[0][1] * right.mat[1][0] + left.mat[0][2] * right.mat[2][0] + left.mat[0][3] * right.mat[3][0];
    mul.mat[0][1] = left.mat[0][0] * right.mat[0][1] + left.mat[0][1] * right.mat[1][1] + left.mat[0][2] * right.mat[2][1] + left.mat[0][3] * right.mat[3][1];
    mul.mat[0][2] = left.mat[0][0] * right.mat[0][2] + left.mat[0][1] * right.mat[1][2] + left.mat[0][2] * right.mat[2][2] + left.mat[0][3] * right.mat[3][2];
    mul.mat[0][3] = left.mat[0][0] * right.mat[0][3] + left.mat[0][1] * right.mat[1][3] + left.mat[0][2] * right.mat[2][3] + left.mat[0][3] * right.mat[3][3];

    mul.mat[1][0] = left.mat[1][0] * right.mat[0][0] + left.mat[1][1] * right.mat[1][0] + left.mat[1][2] * right.mat[2][0] + left.mat[1][3] * right.mat[3][0];
    mul.mat[1][1] = left.mat[1][0] * right.mat[0][1] + left.mat[1][1] * right.mat[1][1] + left.mat[1][2] * right.mat[2][1] + left.mat[1][3] * right.mat[3][1];
    mul.mat[1][2] = left.mat[1][0] * right.mat[0][2] + left.mat[1][1] * right.mat[1][2] + left.mat[1][2] * right.mat[2][2] + left.mat[1][3] * right.mat[3][2];
    mul.mat[1][3] = left.mat[1][0] * right.mat[0][3] + left.mat[1][1] * right.mat[1][3] + left.mat[1][2] * right.mat[2][3] + left.mat[1][3] * right.mat[3][3];

    mul.mat[2][0] = left.mat[2][0] * right.mat[0][0] + left.mat[2][1] * right.mat[1][0] + left.mat[2][2] * right.mat[2][0] + left.mat[2][3] * right.mat[3][0];
    mul.mat[2][1] = left.mat[2][0] * right.mat[0][1] + left.mat[2][1] * right.mat[1][1] + left.mat[2][2] * right.mat[2][1] + left.mat[2][3] * right.mat[3][1];
    mul.mat[2][2] = left.mat[2][0] * right.mat[0][2] + left.mat[2][1] * right.mat[1][2] + left.mat[2][2] * right.mat[2][2] + left.mat[2][3] * right.mat[3][2];
    mul.mat[2][3] = left.mat[2][0] * right.mat[0][3] + left.mat[2][1] * right.mat[1][3] + left.mat[2][2] * right.mat[2][3] + left.mat[2][3] * right.mat[3][3];

    mul.mat[3][0] = left.mat[3][0] * right.mat[0][0] + left.mat[3][1] * right.mat[1][0] + left.mat[3][2] * right.mat[2][0] + left.mat[3][3] * right.mat[3][0];
    mul.mat[3][1] = left.mat[3][0] * right.mat[0][1] + left.mat[3][1] * right.mat[1][1] + left.mat[3][2] * right.mat[2][1] + left.mat[3][3] * right.mat[3][1];
    mul.mat[3][2] = left.mat[3][0] * right.mat[0][2] + left.mat[3][1] * right.mat[1][2] + left.mat[3][2] * right.mat[2][2] + left.mat[3][3] * right.mat[3][2];
    mul.mat[3][3] = left.mat[3][0] * right.mat[0][3] + left.mat[3][1] * right.mat[1][3] + left.mat[3][2] * right.mat[2][3] + left.mat[3][3] * right.mat[3][3];
    return mul;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& right)
{
    return MultipleMat4x4(*this, right);
}

//ベクトルに行列を乗算したベクトルを返す
Vector3 Matrix4x4::MultipleVec3(const Matrix4x4& m, const Vector3& v)
{
    Vector3 vec = {};
    vec.x = m.mat[0][0] * v.x + m.mat[0][1] * v.y + m.mat[0][2] * v.z + m.mat[0][3] * 1.0f;
    vec.y = m.mat[1][0] * v.x + m.mat[1][1] * v.y + m.mat[1][2] * v.z + m.mat[1][3] * 1.0f;
    vec.z = m.mat[2][0] * v.x + m.mat[2][1] * v.y + m.mat[2][2] * v.z + m.mat[2][3] * 1.0f;
    return vec;
}

Vector3 Matrix4x4::operator*(const Vector3& v)
{
    return MultipleVec3(*this, v);
}

//ある座標を中心にY軸回転した座標
Matrix4x4 Matrix4x4::RotateYPositionMatrix4x4(const Position3& center, float angle)
{
    //中心を原点に平行移動させる
     //原点中心に回転
     //中心をもとの座標に戻す
    Matrix4x4 rotaMat = TranslateMat4x4(center.x, center.y, center.z) *//元の座標
        RotateYMat4x4(angle) *//回転
        TranslateMat4x4(-center.x, -center.y, -center.z);//原点に戻す
    return rotaMat;
}


//ある座標を中心にX軸回転した座標
Matrix4x4 Matrix4x4::RotateXPositionMatrix4x4(const Position3& center, float angle)
{
    //中心を原点に平行移動させる
     //原点中心に回転
     //中心をもとの座標に戻す
    Matrix4x4 rotaMat = TranslateMat4x4(center.x, center.y, center.z) *//元の座標
        RotateXMat4x4(angle) *//回転
        TranslateMat4x4(-center.x, -center.y, -center.z);//原点に戻す
    return rotaMat;
}

Matrix4x4 Matrix4x4::RotateZPositionMatrix4x4(const Position3& center, float angle)
{
    //中心を原点に平行移動させる
    //原点中心に回転
    //中心をもとの座標に戻す
    Matrix4x4 rotaMat = TranslateMat4x4(center.x, center.y, center.z) *//元の座標
        RotateZMat4x4(angle) *//回転
        TranslateMat4x4(-center.x, -center.y, -center.z);//原点に戻す
    return rotaMat;
}
//ある座標を中心にX軸回転してY軸回転した座標
Matrix4x4 Matrix4x4::RotateXYPositionMatrix4x4(const Position3& center, float hAngle, float vAngle)
{
    //中心を原点に平行移動させる
     //原点中心に回転
     //中心をもとの座標に戻す
    Matrix4x4 rotaMat = TranslateMat4x4(-center.x, -center.y, -center.z) *//元の座標
        RotateYMat4x4(hAngle) *//回転
        RotateXMat4x4(vAngle) *//回転
        TranslateMat4x4(center.x, center.y, center.z);//原点に戻す
    return rotaMat;
}

Matrix4x4 Matrix4x4::ScaleMatrix4x4(const Vector3& scale)
{
    Matrix4x4 rM = IdentityMat4x4();
    rM.mat[0][0] = scale.x;
    rM.mat[1][1] = scale.y;
    rM.mat[2][2] = scale.z;

    return rM;
}

MATRIX Matrix4x4::ToDxLibMATRIX()
{
    //DXライブラリのマトリクスに変換する
    MATRIX rM = {};
    for (int i = 0; i < 4;++i)
    {
        for (int j = 0; j < 4;++j)
        {
            rM.m[i][j] = this->mat[i][j];
        }
    }
    return rM;
}

Matrix4x4 Matrix4x4::ToMatrix4x4(MATRIX dxMat)
{
    //DXライブラリのマトリクスを自作マトリクスに変換する
    Matrix4x4 rM = {};
    for (int i = 0; i < 4;++i)
    {
        for (int j = 0; j < 4;++j)
        {
            rM.mat[i][j] = dxMat.m[i][j];
        }
    }
    return rM;
}

Matrix4x4 Matrix4x4::LookAt(const Vector3& targetDir, const Vector3 up)
{
    //単位行列
    Matrix4x4 rM = IdentityMat4x4();
    //向きたい方向横方向ベクトルを出す
    Vector3 rightDir = up.Cross(targetDir);
    //向きたい方向の上方向ベクトルを出す
    Vector3 upDir = targetDir.Cross(rightDir);
    //行列を作成する
    //X軸
    rM.mat[0][0] = rightDir.x;
    rM.mat[0][1] = upDir.x;
    rM.mat[0][2] = targetDir.x;
    //Y軸
    rM.mat[1][0] = rightDir.y;
    rM.mat[1][1] = upDir.y;
    rM.mat[1][2] = targetDir.y;
    //Z軸
    rM.mat[2][0] = rightDir.z;
    rM.mat[2][1] = upDir.z;
    rM.mat[2][2] = targetDir.z;
    //行列を返す
    return rM;
}