#pragma once
#include "../CharacterBase.h"
#include <memory>
class ActorData;
class CharaStatusData;
class ActorManager;
class Collidable;
class TitlePlayer :
	public CharacterBase
{
public:
	TitlePlayer(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager);
	virtual ~TitlePlayer();
	//初期化処理
	void Init()override;
	//更新処理
	void Update() override;
	//衝突イベント関数
	void OnCollide(const std::shared_ptr<Collidable> other)override;
	//描画
	void Draw()const override;
	void DrawShadow()const override;
	//更新処理による結果の確定
	void Complete() override;
	//終了処理
	void End()override;
	//半径
	float GetRadius()const override;
};

