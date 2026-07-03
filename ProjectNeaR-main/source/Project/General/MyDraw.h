#pragma once
#include<DxLib.h>
#include<cassert>
#include<array>
#include"Math/MyMath.h"
namespace MyDraw {
	inline void DrawRotaGraph(int x, int y, float scale, float angle, int handle, int psH, bool alpha, int secondH = -1) {
		std::array<DxLib::VERTEX2DSHADER, 4> vertices = {};
		//4頂点に必要な情報を代入していく
		//必要な情報→座標、頂点カラー、UV
		//ただし、rhwは必ず1.0にする必要があります。

		for (auto& v : vertices) {
			v.rhw = 1.0f;//これは必ず1
			v.dif = { 255,255,255,255 };//これは白色にしておく
		}

		int w, h;
		DxLib::GetGraphSize(handle, &w, &h);

		//並べ方はZの字になるように
		//頂点情報(左上)
		vertices[0].pos.x = x - w * 0.5f * scale;
		vertices[0].pos.y = y - h * 0.5f * scale;
		vertices[0].u = 0.0f;
		vertices[0].v = 0.0f;

		//頂点情報(右上)
		vertices[1].pos.x = x + w * 0.5f * scale;
		vertices[1].pos.y = y - h * 0.5f * scale;
		vertices[1].u = 1.0f;
		vertices[1].v = 0.0f;

		//頂点情報(左下)
		vertices[2].pos.x = x - w * 0.5f * scale;
		vertices[2].pos.y = y + h * 0.5f * scale;
		vertices[2].u = 0.0f;
		vertices[2].v = 1.0f;

		//頂点情報(右下)
		vertices[3].pos.x = x + w * 0.5f * scale;
		vertices[3].pos.y = y + h * 0.5f * scale;
		vertices[3].u = 1.0f;
		vertices[3].v = 1.0f;

		//テクスチャを指定
		DxLib::SetUseTextureToShader(0, handle);
		if (secondH >= 0) {
			DxLib::SetUseTextureToShader(1, secondH);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		//シェーダを指定
		DxLib::SetUsePixelShader(psH);

		DxLib::DrawPrimitive2DToShader(vertices.data(), static_cast<int>(vertices.size()),
			DX_PRIMTYPE_TRIANGLESTRIP);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	inline void DrawGraph(int x, int y, int handle, int psH, bool alpha, int secondH = -1) {
		std::array<DxLib::VERTEX2DSHADER, 4> vertices = {};
		//4頂点に必要な情報を代入していく
		//必要な情報→座標、頂点カラー、UV
		//ただし、rhwは必ず1.0にする必要があります。

		for (auto& v : vertices) {
			v.rhw = 1.0f;//これは必ず1
			v.dif = { 255,255,255,255 };//これは白色にしておく
		}

		int w, h;
		DxLib::GetGraphSize(handle, &w, &h);

		//並べ方はZの字になるように
		//頂点情報(左上)
		vertices[0].pos.x = static_cast<float>(x);
		vertices[0].pos.y = static_cast<float>(y);
		vertices[0].u = 0.0f;
		vertices[0].v = 0.0f;

		//頂点情報(右上)
		vertices[1].pos.x = static_cast<float>(x + w);
		vertices[1].pos.y = static_cast<float>(y);
		vertices[1].u = 1.0f;
		vertices[1].v = 0.0f;

		//頂点情報(左下)
		vertices[2].pos.x = static_cast<float>(x);
		vertices[2].pos.y = static_cast<float>(y + h);
		vertices[2].u = 0.0f;
		vertices[2].v = 1.0f;

		//頂点情報(右下)
		vertices[3].pos.x = static_cast<float>(x + w);
		vertices[3].pos.y = static_cast<float>(y + h);
		vertices[3].u = 1.0f;
		vertices[3].v = 1.0f;

		//テクスチャを指定
		DxLib::SetUseTextureToShader(0, handle);
		if (secondH >= 0) {
			DxLib::SetUseTextureToShader(1, secondH);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		//シェーダを指定
		DxLib::SetUsePixelShader(psH);

		DxLib::DrawPrimitive2DToShader(vertices.data(), static_cast<int>(vertices.size()),
			DX_PRIMTYPE_TRIANGLESTRIP);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	inline void DrawPS(int x, int y, float scale, float angle, const int cBuffH, int handle, int psH)
	{
		//更新
		UpdateShaderConstantBuffer(cBuffH);
		//4番にセット
		SetShaderConstantBuffer(cBuffH, DX_SHADERTYPE_PIXEL, 4);
		//画像をノイズ入りで描画
		MyDraw::DrawRotaGraph(x, y, scale, angle, handle, psH, true);
		// シェーダーをオフに
		DxLib::SetUsePixelShader(-1);
	}
	inline void DrawPS(int x, int y, float scale, float angle, const int cBuffH1, const int cBuffH2, int handle, int psH)
	{
		//更新
		UpdateShaderConstantBuffer(cBuffH1);
		UpdateShaderConstantBuffer(cBuffH2);
		//4番にセット
		SetShaderConstantBuffer(cBuffH1, DX_SHADERTYPE_PIXEL, 4);
		//5番にセット
		SetShaderConstantBuffer(cBuffH2, DX_SHADERTYPE_PIXEL, 5);
		//画像をノイズ入りで描画
		MyDraw::DrawRotaGraph(x, y, scale, angle, handle, psH, true);
		// シェーダーをオフに
		DxLib::SetUsePixelShader(-1);
	}
	
	//3D円描画
	inline void Draw3DCircle(Vector3 pos, float radius, int segmentCount = 36, unsigned int color = GetColor(255, 255, 255))
	{
		float angleStep = MyMath::TwoPI_F / segmentCount;

		for (int i = 0; i < segmentCount; ++i) {
			float angle1 = angleStep * i;
			float angle2 = angleStep * (i + 1);

			float x1 = pos.x + cosf(angle1) * radius;
			float z1 = pos.z + sinf(angle1) * radius;

			float x2 = pos.x + cosf(angle2) * radius;
			float z2 = pos.z + sinf(angle2) * radius;

			// Y座標は固定（XZ平面上の円）
			DrawLine3D(VECTOR{ x1, pos.y, z1 }, VECTOR{ x2, pos.y, z2 }, color);
		}
	}
}