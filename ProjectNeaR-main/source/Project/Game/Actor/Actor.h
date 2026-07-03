#pragma once
#include <memory>
#include <string>
#include "../../General/Math/MyMath.h"
#include "../../General/Collision/Collidable.h"
#include "../../General/CSV/ActorData.h"

class ActorManager;
class Collidable;
class Model;
/// <summary>
/// ゲーム中に配置可能な物体の基底クラス
/// </summary>
/// //newできなくなる
class Actor abstract :
	public Collidable
{
protected:
	//データ
	std::shared_ptr<ActorData> m_actorData;
	//消滅
	bool m_isDelete;
	//アクターの識別番号
	int m_id;
	//IDがセットされたことを記録するフラグ
	bool m_isSetId;
	//モデル
	std::shared_ptr<Model> m_model;
	//アクターマネージャーの参照
	std::weak_ptr<ActorManager> m_pActorManager;
public:
	Actor(std::shared_ptr<ActorData> actorData,Shape shape, std::weak_ptr<ActorManager> pActorManager);
	virtual ~Actor() {};
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init()abstract;
	/// <summary>
	/// 位置などの更新を行う(引数なし)
	/// </summary>
	virtual void Update() {};
	/// <summary>
	/// 対象のアクターを表示
	/// </summary>
	virtual void Draw()const abstract;
	/// <summary>
	/// 対象のアクターの影を表示
	/// </summary>
	virtual void DrawShadow()const abstract;
	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void End()abstract;
	/// <summary>
	/// 削除フラグ
	/// </summary>
	/// <returns></returns>
	bool IsDelete() { return m_isDelete; };
	/// <summary>
	/// 削除予約
	/// </summary>
	/// <returns></returns>
	void Delete() { m_isDelete = true; };
	/// <summary>
	/// IDを取得
	/// </summary>
	/// <returns></returns>
	int GetID() const { return m_id; };
	/// <summary>
	/// IDをセット
	/// </summary>
	/// <returns></returns>
	void SetID(int id);
	/// <summary>
	/// 座標
	/// </summary>
	Vector3 GetPos() const;
	/// <summary>
	/// 次の座標
	/// </summary>
	Vector3 GetNextPos() const;
	//モデルクラス
	std::shared_ptr<Model> GetModel() const { return m_model; };
	//タイムスケール
	void SetTimeScale(float scale)override;
	float GetTimeScale()const;
	//自分のタイムスケールを使う
	void EnableIsMyScale();
	//使わない
	void DisableIsMyScale();
};

