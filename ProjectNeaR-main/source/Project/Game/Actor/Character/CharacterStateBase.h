#pragma once
#include <memory>
#include "../../../General/Math/MyMath.h"
#include "../Actor.h"
namespace
{
	//レイキャストの許容値(自分とターゲット)
	constexpr int kRayCastBeforeMoveToleranceNum = 2;
	//移動後のレイキャストの許容値(ターゲットのみ)
	constexpr int kRayCastAfterMoveToleranceNum = 1;
}

class Input;
class GameCamera;
class ActorManager;

class CharacterStateBase abstract
{
public:
	CharacterStateBase(std::weak_ptr<Actor> owner,bool isWait);
	virtual ~CharacterStateBase();
	//初期化処理
	virtual void Init()abstract;
	//状態に応じた更新処理
	virtual void Update() abstract;
	//次の状態を取得
	std::shared_ptr<CharacterStateBase> GetNextState() { return m_nextState; };
	//状態変化
	virtual void ChangeState(std::shared_ptr<CharacterStateBase> nextState);

	//フレームのカウント
	void CountFrame();

	//待機するか動くか
	void Wait() { m_isWait = true; };
	void Operate() { m_isWait = false; };
	//待機しているか
	bool IsWait()const { return m_isWait; };
private:
	//次の状態
	std::shared_ptr<CharacterStateBase> m_nextState;
protected:
	//自分のポインタ
	std::weak_ptr<Actor> m_pOwner;
	//フレームカウント
	float m_frame;
	//強制待機状態
	bool m_isWait;

};

