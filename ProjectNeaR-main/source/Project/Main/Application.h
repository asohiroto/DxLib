#pragma once
#include<memory>
class ShaderPostProcess;
class Input;
class SoundManager;
/// <summary>
/// シングルトン
/// </summary>
class Application final
{
private:
	//シングルトンの準備
	Application() = default;
	~Application() = default;
	//コピー禁止
	Application(const Application&) = delete;
	Application& operator = (const Application&) = delete;
	//ムーブ禁止
	Application(Application&&) = delete;
	Application& operator = (Application&&) = delete;
public:
	/// <summary>
	/// シングルトンオブジェクトを返す関数
	/// 参照を返すことで、複製を防ぐ
	/// 間違えて値を返した場合、コピーコンストラクタが走るので
	/// それをprivateにしておくことで弾くことができる
	/// </summary>
	/// <returns></returns>
	static Application& GetInstance();

	/// <summary>
	/// アプリケーションの初期化
	/// </summary>
	/// <returns>ture:初期化成功, false:失敗</returns>
	bool Init();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Run();
	/// <summary>
	/// 終了処理
	/// </summary>
	void Terminate();

	//アプリケーションを終了させる
	void FinishApplication() { m_isFinishApplication = true; };
	//Window切り替え
	void SetWindowMode(bool isWindow);
	bool IsWindowMode()const { return m_isWindow; };
	//タイムスケールを設定
	void SetTimeScale(float timeScale) { m_timeScale = timeScale; };
	//タイムスケールを取得
	float GetTimeScale() const { return m_timeScale; };

	//ポストエフェクト
	std::unique_ptr<ShaderPostProcess>& GetPostProcess() { return m_postProcess; };
	
private:
	//アプリケーションの終了
	bool m_isFinishApplication = false;
	//ウィンドウモードにする
	bool m_isWindow = true;
	//タイムスケール
	float m_timeScale = 1.0f;
	//ポストエフェクト
	std::unique_ptr<ShaderPostProcess> m_postProcess;

	//デバッグ用
	enum class DebugState : int
	{
		SceneInfo	= 0,
		FPS			= 1,
		DebugMax	= 2
	};
	DebugState m_debugState = DebugState::SceneInfo;


	//デバッグ用
	bool m_isUpdateStop = false;
private:
	//スクリーンモードを切り替える際の処理
	void ChangeScreenMode();
	//ターゲットFPS
	void DebugDrawFPS()const;
};
