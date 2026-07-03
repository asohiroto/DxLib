//サンプラーステート（テクスチャをどうサンプリングするか指定する）
SamplerState smp : register(s0);
//テクスチャ（シェーダーで扱う画像データ）
Texture2D tex : register(t0);
// 定数バッファ (CPU → GPU へ送る値)
cbuffer ConstantBuffer : register(b4)
{
    float shakeStrength; //揺れ（グリッチの強さ）
    float blockScale;    //ノイズブロックの大きさ
    float noiseSpeed;    //ノイズの時間変化の速さ
    int state;           //シェーダーの動作状態（通常/グリッチ/モノクロ）
}
cbuffer ConstantBuffer2 : register(b5)
{
    float justRate; //経過時間
    float dummy1;
    float dummy2;
    float dummy3;
}

//モノクロ化のための輝度係数
#define R_LUMINANCE 0.298912
#define G_LUMINANCE 0.586611
#define B_LUMINANCE 0.114478

//シェーダーの状態定数
#define STATE_NORMAL 0              //通常
#define STATE_GLITCH 1 << 0         //グリッチ効果
#define STATE_MONOCHROME 1 << 1     //モノクロ
#define STATE_JUSTAVOID 1 << 2      //ジャスト回避
#define STATE_GLITCHNOCOLOR 1 << 3  //色ずれなしグリッジ

//疑似乱数を生成する関数
float random(float2 seeds)
{
    return frac(sin(dot(seeds, float2(12.9898, 78.233))) * 43758.5453);
}

//ブロックノイズ（座標を整数化してノイズを生成）
float blockNoise(float2 seeds)
{
    return random(floor(seeds));
}

//-1.0 ～ 1.0 の範囲の乱数を生成
float noiserandom(float2 seeds)
{
    return -1.0 + 2.0 * blockNoise(seeds);
}

//ピクセルシェーダーの入力構造体
struct PS_INPUT
{   
    float4 pos      : SV_Position;  //ピクセルのスクリーン座標
    float4 color    : COLOR0;       //頂点カラー
    float2 uv       : TEXCOORD0;    //テクスチャ座標
};

//ピクセルシェーダーの出力構造体
struct PS_OUTPUT
{
    float4 color    : SV_TARGET0;  //出力カラー
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    bool isUseGlitch = (state & STATE_GLITCH) && !(blockScale <= 0.0 && noiseSpeed <= 0.0 && shakeStrength <= 0.0);
    bool isUseGray = (state & STATE_MONOCHROME);
    bool isUseJust = (state & STATE_JUSTAVOID);
    bool isUseGlitchNoColor = (state & STATE_GLITCHNOCOLOR);
    
    // 元のテクスチャ色を取得
    float4 color = tex.Sample(smp, input.uv);

    //状態がグリッチ
    if (isUseGlitch || isUseGlitchNoColor)
    {
        
        //グリッチ効果
        float2 gv = input.uv; //グリッチ用のUV座標

        //ノイズを計算
        float noise = blockNoise(input.uv.y * blockScale);

        //横方向に少しランダム成分を追加
        noise += random(input.uv.x);

        //ランダム値を生成（揺れ用）
        float2 randomvalue = noiserandom(float2(input.uv.y, shakeStrength * noiseSpeed));

        //X方向のUVを揺らしてグリッチ効果を作成
        gv.x += randomvalue * sin(sin(1)) * sin(-sin(noise)*0.5) * frac(shakeStrength);

        if (isUseGlitchNoColor)
        {
            //全ての色を同じだけ動かす
            color.r = tex.Sample(smp, gv + float2(0.009, 0)).r;
            color.g = tex.Sample(smp, gv + float2(0.009, 0)).g;
            color.b = tex.Sample(smp, gv + float2(0.009, 0)).b;
            color.a = 1.0;
            //色ズレ後のcolorが白い領域なら透明にする
            if (color.r > 0.99 && color.g > 0.99 && color.b > 0.99)
            {
                discard;
            }
        }
        else
        {
           //色ズレ
            color.r = tex.Sample(smp, gv + float2(0.006, 0)).r; //赤を右にずらす
            color.g = tex.Sample(smp, gv).g; //緑はそのまま
            color.b = tex.Sample(smp, gv - float2(0.008, 0)).b; //青を左にずらす
            color.a = 1.0;
        }
    }
  
    //モノクロ化
    if (isUseGray)
    {
        float4 beforeColor = color; //グリッチ処理後の色を保持

        //モノクロ化
        //輝度を計算してRGBを均一化
        float luminance = dot(color.rgb, float3(R_LUMINANCE, G_LUMINANCE, B_LUMINANCE));
        color = float4(luminance, luminance, luminance, 1.0);

        //元の色とモノクロ色をブレンド（完全なモノクロではなく9割モノクロ）
        color = lerp(beforeColor, color, 0.9);

        //色味を微妙に調整（青み・緑みを少し強調）
        color.g *= 1.07;
        color.b *= 1.04;
        
        //UV
        float2 uv = input.uv;
    }
    
    //状態がモノクロまたはジャスト回避
    //画面の端を暗く
    if (isUseJust || isUseGray)
    {
        float2 uv = input.uv;
 
        float d = length(uv - 0.5);
        
        //明るさ
        float rate = justRate;
        if (isUseGray)
        {
            rate = 1.0;
        }
        float lerpRate = saturate(rate);
        
        float4 vignetteColor = lerp(color, float4(0.0, 0.0, 0.0, 1.0), lerpRate);

        float vignetteDistance = pow(saturate(d * 1.5), 2);
        
        color = lerp(color, vignetteColor, vignetteDistance);
    }
   
    //最終的なカラーを出力
    output.color = color;
    return output;
}
