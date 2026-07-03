#pragma once
#include "../Actor.h"
#include <memory>
#include <map>
#include <string>
#include "../../../General/CSV/AnimData.h"
#include "../../../General/CharaStatus.h"
class CharacterStateBase;
class ActorManager;
class ActorData;
class CharaStatusData;
class AttackBase;
class CSVDataLoader;
class AttackData;
class EffectData;
class SoundData;
class CharacterBase abstract:
    public Actor
{
public:
	CharacterBase(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData,Shape shape, std::weak_ptr<ActorManager> pActorManager);
	virtual ~CharacterBase() {};

    //初期化処理
    void Init(std::wstring animPath, std::wstring attackPath, std::wstring effectPath, std::wstring sePath, std::wstring voicePath);
	//更新処理
	virtual void Update() override;
    //半径
    virtual float GetRadius()const abstract;

    //ステートにアクセスさせる関数
    //リジッドボディ
    std::shared_ptr<Rigidbody> GetRb() const { return m_rb; }
    //コリジョン
    std::shared_ptr<ColliderBase> GetColl() const { return m_collisionData; }
    //コリジョンの状態を設定
    void SetCollState(CollisionState collState) { m_collState = collState; }
    CollisionState GetCollState()const { return m_collState; };
    //アクターマネージャー
    std::weak_ptr<ActorManager> GetActorManager()const { return m_pActorManager; };

    //ステータス
    std::shared_ptr<CharaStatus> GetCharaStatus()const;

    //攻撃の登録
    void SetAttack(std::shared_ptr<AttackBase> attack);
    //攻撃データの検索
    std::shared_ptr<AttackData> GetAttackData(std::wstring attackName)const;

    //アニメーションの検索
    virtual std::string GetAnim(std::wstring state, std::string path = "", AnimData::WeaponType type = AnimData::WeaponType::None) const;

    //エフェクトの検索
    std::wstring GetEffectPath(std::wstring effectName)const;

    //SEの検索
    std::wstring GetSEPath(std::wstring seName)const;

    //Voiceの検索
    std::wstring GetVoicePath(std::wstring voiceName)const;

    //アーマーを変える
    void ChangeArmor(CharaStatus::Armor armor);
    //アーマーを元に戻す
    void InitArmor();

    //中心座標
    Vector3 GetCenterPos()const;

    //イベントとかで使う
    //待機状態にする
    void Wait();
    //自由に動く
    void Operate();

protected:
    //攻撃データを読み込む
    void InitAttackData(CSVDataLoader& csvLoader, std::wstring path);
    //アニメーションの読み込み
    void InitAnimData(CSVDataLoader& csvLoader, std::wstring path);
    //エフェクトの読み込み
    void InitEffectData(CSVDataLoader& csvLoader, std::wstring effectPath);
    //SEの読み込み
    void InitSE(CSVDataLoader& csvLoader, std::wstring sePath);
    //Voiceの読み込み
    void InitVoice(CSVDataLoader& csvLoader, std::wstring voicePath);
protected:
    //キャラクターの状態
    std::shared_ptr<CharacterStateBase> m_state;
    //ステータス
    std::shared_ptr<CharaStatus> m_charaStatus;
    //攻撃データ
    std::vector<std::shared_ptr<AttackData>> m_attackDatas;
    //アニメーションデータ
    std::vector<std::shared_ptr<AnimData>> m_animDatas;
    //エフェクトのデータ
    std::vector<std::shared_ptr<EffectData>> m_effectDatas;
    //SEのデータ
    std::vector<std::shared_ptr<SoundData>> m_seDatas;
    //VCのデータ
    std::vector<std::shared_ptr<SoundData>> m_voiceDatas;

};

