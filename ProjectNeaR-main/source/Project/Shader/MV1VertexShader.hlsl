#include "VertexShader.h"

#define USESPC true

// 頂点シェーダーの入力
struct VS_INPUT
{
	float3 Position        : POSITION ;			// 座標( ローカル空間 )
	float3 Normal          : NORMAL0 ;			// 法線( ローカル空間 )
#if USESPC
    float4 Specular : COLOR1; // スペキュラカラー
#endif
	float4 Diffuse         : COLOR0 ;			// ディフューズカラー
	float4 TexCoords0      : TEXCOORD0 ;		// テクスチャ座標
	float4 TexCoords1      : TEXCOORD1 ;		// サブテクスチャ座標
    int4 BlendIndex : BLENDINDICES0;
	float4 BlendWeight : BLENDWEIGHT0;
};

// 頂点シェーダーの出力
struct VS_OUTPUT
{
	float4 Diffuse         : COLOR0 ;		// ディフューズカラー
#if USESPC
	float4 Specular        : COLOR1 ;		// スペキュラカラー
#endif
	float4 TexCoords0_1    : TEXCOORD0 ;	// xy:テクスチャ座標 zw:サブテクスチャ座標
	float3 VPosition       : TEXCOORD1 ;	// 座標( ビュー空間 )
	float3 VNormal         : TEXCOORD2 ;	// 法線( ビュー空間 )

	float2 Fog             : TEXCOORD5 ;	// フォグパラメータ( x )  高さフォグパラメータ( y )

	float4 Position        : SV_POSITION ;	// 座標( プロジェクション空間 )
    float3 WorldPosition	: POSITION0;//もとのワールド座標
    float4 PositionD		: POSITION1; //デプス用
} ;

#ifdef SKINMESH
#define LOCAL_WORLD_MAT         lLocalWorldMatrix
#else
#define LOCAL_WORLD_MAT         g_Base.LocalWorldMatrix
#endif

#define VERTICAL_FOG_OUT        VSOutput.VerticalFog

// main関数
VS_OUTPUT main( VS_INPUT VSInput )
{
	VS_OUTPUT	VSOutput ;
	int4		lBoneFloatIndex ;
	float4		lLocalWorldMatrix[ 3 ] ;
	float4		lLocalPosition ;
	float4		lWorldPosition ;
	float4		lViewPosition ;
	float3		lWorldNrm ;
	float3		lWorldTan ;
	float3		lWorldBin ;
	float3		lViewNrm ;
	float3		lViewTan ;
	float3		lViewBin ;
	float		lVerticalFogY ;
	float		lFogDensity ;

	// ローカル座標のセット
	lLocalPosition.xyz = VSInput.Position ;
	lLocalPosition.w = 1.0f ;
	
	// 座標計算( ローカル→ビュー→プロジェクション )
	lWorldPosition.x = dot( lLocalPosition, LOCAL_WORLD_MAT[ 0 ] ) ;
	lWorldPosition.y = dot( lLocalPosition, LOCAL_WORLD_MAT[ 1 ] ) ;
	lWorldPosition.z = dot( lLocalPosition, LOCAL_WORLD_MAT[ 2 ] ) ;
	lWorldPosition.w = 1.0f ;
    VSOutput.WorldPosition = lWorldPosition.xyz;

	lViewPosition.x = dot( lWorldPosition, g_Base.ViewMatrix[ 0 ] ) ;
	lViewPosition.y = dot( lWorldPosition, g_Base.ViewMatrix[ 1 ] ) ;
	lViewPosition.z = dot( lWorldPosition, g_Base.ViewMatrix[ 2 ] ) ;
	lViewPosition.w = 1.0f ;

	VSOutput.Position.x = dot( lViewPosition, g_Base.ProjectionMatrix[ 0 ] ) ;
	VSOutput.Position.y = dot( lViewPosition, g_Base.ProjectionMatrix[ 1 ] ) ;
	VSOutput.Position.z = dot( lViewPosition, g_Base.ProjectionMatrix[ 2 ] ) ;
	VSOutput.Position.w = dot( lViewPosition, g_Base.ProjectionMatrix[ 3 ] ) ;
    VSOutput.PositionD = VSOutput.Position;
   
	
	
	// 座標( ビュー空間 )を保存
	VSOutput.VPosition = lViewPosition.xyz;
	
	// 法線を計算
	lWorldNrm.x = dot( VSInput.Normal, LOCAL_WORLD_MAT[ 0 ].xyz ) ;
	lWorldNrm.y = dot( VSInput.Normal, LOCAL_WORLD_MAT[ 1 ].xyz ) ;
	lWorldNrm.z = dot( VSInput.Normal, LOCAL_WORLD_MAT[ 2 ].xyz ) ;

	lViewNrm.x = dot( lWorldNrm, g_Base.ViewMatrix[ 0 ].xyz ) ;
	lViewNrm.y = dot( lWorldNrm, g_Base.ViewMatrix[ 1 ].xyz ) ;
	lViewNrm.z = dot( lWorldNrm, g_Base.ViewMatrix[ 2 ].xyz ) ;

	// 法線( ビュー空間 )を保存
	VSOutput.VNormal = lViewNrm;

	// ディフューズカラーをセット
	VSOutput.Diffuse  = g_Base.DiffuseSource  > 0.5f ? VSInput.Diffuse  : g_Common.Material.Diffuse ;
	
	// スペキュラカラーをセット
#if USESPC
	VSOutput.Specular = ( g_Base.SpecularSource > 0.5f ? VSInput.Specular : g_Common.Material.Specular ) * g_Base.MulSpecularColor ;
#endif
	// 頂点座標変換 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 終了 )

	VSOutput.Fog.x = 1.0f;

	VSOutput.Fog.y = 1.0f ;
	if( g_Common.VerticalFog.Mode == 1 || g_Common.VerticalFog.Mode == 2 )
	{
		if( g_Common.VerticalFog.Density < 0.0 )
		{
			lVerticalFogY = lWorldPosition.y - g_Common.VerticalFog.DensityStart;
			lFogDensity = -g_Common.VerticalFog.Density ;
		}
		else
		{
			lVerticalFogY = g_Common.VerticalFog.DensityStart - lWorldPosition.y ;
			lFogDensity = g_Common.VerticalFog.Density ;
		}
		if( lVerticalFogY > 0.0f )
		{
			if( g_Common.VerticalFog.Mode == 1 )
			{
				// 指数フォグ計算 1.0f / pow( e, 距離 * density )
				VSOutput.Fog.y = 1.0f / pow( abs( g_Common.VerticalFog.E ), lVerticalFogY * lFogDensity ) ;
			}
			else
			if( g_Common.VerticalFog.Mode == 2 )
			{
				// 指数フォグ２計算 1.0f / pow( e, ( 距離 * density ) * ( 距離 * density ) )
				VSOutput.Fog.y = 1.0f / pow( abs( g_Common.VerticalFog.E ), ( lVerticalFogY * lFogDensity ) * ( lVerticalFogY * lFogDensity ) ) ;
			}
		}
	}
	else
	if( g_Common.VerticalFog.Mode == 3 )
	{
		// 線形フォグ計算
		VSOutput.Fog.y = lWorldPosition.y * g_Common.VerticalFog.LinearDiv + g_Common.VerticalFog.LinearAdd ;
	}

	// テクスチャ座標のセット
    VSOutput.TexCoords0_1.x = VSInput.TexCoords0.x;
    VSOutput.TexCoords0_1.y = VSInput.TexCoords0.y;

    VSOutput.TexCoords0_1.z = VSInput.TexCoords1.z;
    VSOutput.TexCoords0_1.w = VSInput.TexCoords1.w;

	return VSOutput ;
}

