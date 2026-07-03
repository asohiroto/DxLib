#include "TitleUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/CharaStatus.h"
#include "../../../General/Math/MyMath.h"
#include "../../../General/MyDraw.h"
#include "../../../General/Game.h"
#include "../../../General/ShaderPostProcess.h"
#include "../../../General/Fader.h"
#include "../../../General/Input.h"
#include "../../../General/LoadingManager.h"
#include <DxLib.h>

namespace
{
	//ハンドル
	const std::wstring kTitleNameHandlePath = L"Title/TitleName";
	const std::wstring kTitleNameShadowHandlePath = L"Title/TitleNameShadow";
	const std::wstring kTitleBackHandlePath = L"Title/TitleBack";
	const std::wstring kGearHandlePath = L"Title/Gear";
	const std::wstring kClockHandlePath = L"Title/Time";
	
	//揺れるフレーム
	constexpr int kShakeStartFrame = 200;
	constexpr int kShakeFrame = 20;

	//シェーダパラメータ
	constexpr float kBlockScale = 0.03f;
	constexpr float kNoiseSpeed = 1.0f;
	constexpr float kShakeStrength = 0.5f;

	//タイトルが少しずれているので補正
	constexpr float kTitleNameOffsetX = 6.0f;

	//タイトルが完成するまでのフレーム
	constexpr int kTitleMoveStartFrame = 80;
	constexpr int kTitleCompleteFrame = 120;
	//ギアが回りだすフレーム
	constexpr int kGearStartFrame = 130;

	//時計完成のlerp率
	constexpr float kTitleCompleteLerpRate = 0.1f;

	//時計初期位置
	const Vector2 kClockStartPos = { Game::kScreenCenterX ,-1000.0f };
	//時計目的地
	const Vector2 kClockFinishPos = { Game::kScreenCenterX, Game::kScreenCenterY };
	//時計大きさ
	constexpr double kClockScale = 0.7;

	//歯車位置
	const Vector2 kGear1StartPos = { - 500.0f , -500.0f };
	const Vector2 kGear2StartPos = { Game::kScreenWidth + 400.0f, Game::kScreenHeight + 530.0f};
	const Vector2 kGear3StartPos = { - 600.0f, Game::kScreenHeight + 100.0f};
	const Vector2 kGear4StartPos = { Game::kScreenWidth + 700.0f, -800.0f};
	//歯車目的地
	const Vector2 kGear1FinishPos = { Game::kScreenCenterX - 200.0f , Game::kScreenCenterY - 30.0f };
	const Vector2 kGear2FinishPos = { Game::kScreenCenterX + 200.0f, Game::kScreenCenterY + 30.0f };
	const Vector2 kGear3FinishPos = { Game::kScreenCenterX - 150.0f, Game::kScreenCenterY + 60.0f };
	const Vector2 kGear4FinishPos = { Game::kScreenCenterX + 150.0f, Game::kScreenCenterY - 60.0f };
	//歯車大きさ
	constexpr double kGear1Scale = 0.4;
	constexpr double kGear2Scale = 0.4;
	constexpr double kGear3Scale = 0.3;
	constexpr double kGear4Scale = 0.3;
	

	//やや上にタイトルをずらす
	constexpr float kTitlePosYAdjust = -120.0f;

	//歯車回転速度
	constexpr double kGearRotateSpeed = 1.0;
	constexpr double kGearRotateFastSpeed = 10.0;

	//フェード速度
	constexpr int kFadeOutSpeed = 10;
}

TitleUI::TitleUI() :
	UIBase(),
	m_titleNameHandle(-1),
	m_titleNameShadowHandle(-1),
	m_titleBackHandle(-1),
	m_gearHandle(-1),
	m_clockHandle(-1),
	m_shakeCountFrame(0),
	m_titleCompleteCountFrame(0),
	m_clockPos(kClockStartPos),
	m_gear1Pos(kGear1StartPos),
	m_gear2Pos(kGear2StartPos),
	m_gear3Pos(kGear3StartPos),
	m_gear4Pos(kGear4StartPos),
	m_gearAngle(0.0),
	m_backAlpha(255)
{
	//ハンドル
	auto& assetManager = AssetManager::GetInstance();
	m_titleNameHandle = assetManager.GetImageHandle(kTitleNameHandlePath);
	m_titleNameShadowHandle = assetManager.GetImageHandle(kTitleNameShadowHandlePath);
	m_titleBackHandle = assetManager.GetImageHandle(kTitleBackHandlePath);
	m_gearHandle = assetManager.GetImageHandle(kGearHandlePath);
	m_clockHandle = assetManager.GetImageHandle(kClockHandlePath);
	//前描画
	m_isFrontDraw = true;


	auto& loadingManager = LoadingManager::GetInstance();
	bool isLoading = loadingManager.IsLoading();
	//ロード中なら
	if (isLoading)
	{
		//一度終了
		loadingManager.EndLoading();
	}
	//シェーダ(ここが非同期で止まる原因)
	//非同期読み込み中に定数バッファを作成するとまれにエラーが起きるようなので同期に変更
	m_shader = std::make_shared<ShaderPostProcess>();
	m_shader->Init();
	m_shader->SetPostEffectState(ShaderPostProcess::PostEffectState::NoColorGlitch);
	if (isLoading)
	{
		//再度非同期スタート
		loadingManager.StartLoading();
	}
}

TitleUI::~TitleUI()
{
}

void TitleUI::Update()
{
	m_shader->Update();

	if (m_shakeCountFrame >= kShakeStartFrame + kShakeFrame)
	{
		m_shakeCountFrame = 0;
	}
	else if (m_shakeCountFrame >= kShakeStartFrame)
	{
		//揺れる
		m_shader->SetBlockScele(kBlockScale);
		m_shader->SetNoiseSpeed(kNoiseSpeed);
		m_shader->SetShakeStrength(kShakeStrength);
	}
	else
	{
		//揺れない
		m_shader->SetBlockScele(0.0f);
		m_shader->SetNoiseSpeed(0.0f);
		m_shader->SetShakeStrength(0.0f);
	}
	++m_shakeCountFrame;

	++m_titleCompleteCountFrame;
	//歯車回転
	if(m_titleCompleteCountFrame > kGearStartFrame)
	{
		if (Fader::GetInstance().IsFadeNow())
		{
			m_gearAngle += kGearRotateFastSpeed;
		}
		else
		{
			m_gearAngle += kGearRotateSpeed;
		}
	}

	//タイトル完成後の処理
	if(m_titleCompleteCountFrame > kTitleCompleteFrame)
	{
		//入力開始
		Input::GetInstance().StartUpdate();
	}
	if(m_titleCompleteCountFrame > kTitleMoveStartFrame)
	{
		//時計移動
		m_clockPos = Vector2::Lerp(m_clockPos, kClockFinishPos, kTitleCompleteLerpRate);
		//歯車移動
		m_gear1Pos = Vector2::Lerp(m_gear1Pos, kGear1FinishPos, kTitleCompleteLerpRate);
		m_gear2Pos = Vector2::Lerp(m_gear2Pos, kGear2FinishPos, kTitleCompleteLerpRate);
		m_gear3Pos = Vector2::Lerp(m_gear3Pos, kGear3FinishPos, kTitleCompleteLerpRate);
		m_gear4Pos = Vector2::Lerp(m_gear4Pos, kGear4FinishPos, kTitleCompleteLerpRate);
		//タイトル位置
		m_titlePos = Vector2(Game::kScreenCenterX, Game::kScreenCenterY);
		//背景フェードアウト
		m_backAlpha = MathSub::ClampInt(m_backAlpha - kFadeOutSpeed, 0, 255);
	}
	else
	{
		//時計ランダム位置
		m_titlePos = Vector2(MyMath::GetRandF(0, Game::kScreenWidth), MyMath::GetRandF(0, Game::kScreenHeight));
		//入力停止
		Input::GetInstance().StopUpdate();
	}
}

void TitleUI::Draw() const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_backAlpha);
	//背景
	DrawRotaGraphF(Game::kScreenCenterX, Game::kScreenCenterY, 1.0, 0.0, m_titleBackHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	double angle = m_gearAngle * MyMath::DEG_2_RAD;
	//歯車1
	DrawRotaGraphF(m_gear1Pos.x, m_gear1Pos.y + kTitlePosYAdjust, kGear1Scale, angle, m_gearHandle, true);
	//歯車2
	DrawRotaGraphF(m_gear2Pos.x, m_gear2Pos.y + kTitlePosYAdjust, kGear2Scale, -angle, m_gearHandle, true);
	//歯車3
	DrawRotaGraphF(m_gear3Pos.x, m_gear3Pos.y + kTitlePosYAdjust, kGear3Scale, -angle, m_gearHandle, true);
	//歯車4
	DrawRotaGraphF(m_gear4Pos.x, m_gear4Pos.y + kTitlePosYAdjust, kGear4Scale, -angle, m_gearHandle, true);
	//時計
	DrawRotaGraphF(m_clockPos.x, m_clockPos.y + kTitlePosYAdjust, kClockScale, angle, m_clockHandle, true);

	//タイトルロゴ
	DrawRotaGraphF(m_titlePos.x, m_titlePos.y + kTitlePosYAdjust, 1.0, 0.0, m_titleNameShadowHandle, true);
	m_shader->Draw(m_titlePos.x + kTitleNameOffsetX, m_titlePos.y + kTitlePosYAdjust, m_titleNameHandle);
}
