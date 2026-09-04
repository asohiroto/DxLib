# 2026_summer — Claude向け引き継ぎメモ

このファイルはClaude Code(AI)が作業を素早く引き継ぐためのメモ。
家/学校のPCを行き来してGitHub経由で同期する運用のため、環境依存の注意点を優先的に書いている。

## プロジェクト概要

DxLib + EffekseerForDXLib(v1.80.7) を使ったC++20/DirectX11製の1対1対戦アクションゲーム。
ウィンドウタイトルは「魔術師流自分の殺し方」。プレイヤー vs ボス魔術師の魔法撃ち合いで、
タイトル→操作説明→本編→リザルトの一連の流れが実装済み。学校の夏季制作課題(授業)と思われる。

このフォルダ単体は独立したgitリポジトリではなく、親フォルダ `DxLib/` 直下の.gitが管理する
リポジトリのサブフォルダ(`3D_summer`, `Simulation` 等、他の授業/自主制作物と同居)。

## ⚠️ 環境依存・要注意事項(最優先で確認)

### 1. DxLib / Effekseerのインクルード・ライブラリパスがハードコード
`2026_summer.vcxproj` 内で以下のローカル絶対パスを直接指定している:
- `c:\DxLib_VC\プロジェクトに追加すべきファイル_VC用`
- `C:\EffekseerForDXLib_1.80.7_324f\プロジェクトに追加すべきファイル_VC用`

**家と学校のPCで両方とも同じパスにDxLib/Effekseerをインストールしておく必要がある。**
片方の環境にしかインストールされていない場合、そのPCではビルドが失敗する。
新しいPCでビルドできない相談を受けたら、まずこのパスの実在を疑うこと。

### 2. vcxprojの潜在バグ(x64設定)
`AdditionalLibraryDirectories`(x64 Debug/Release)に、無関係な旧パス
`C:\EffekseerForDXLib\プロジェクトに追加すべきファイル_VC用`(バージョン番号なし)が
セミコロン区切りなしで連結されて残っている箇所がある。実害は出ていない模様だが、
リンクエラーが出た場合はここを疑う。整理してよいか聞かれたら整理して良い。

### 3. .gitignore(2026-09-04 訂正: 存在することを確認済み)
親フォルダ `DxLib/` 直下に `.gitignore` が存在する(GitHub公式VisualStudio.gitignoreベース)。
`x64/`, `x86/`, `[Ww]in32/`, `.vs/` 等のビルド生成物・IDEキャッシュは正しく無視対象になっている。
`.vscode/` も `.vscode/*` + `!settings.json`/`!tasks.json`/`!launch.json`/`!extensions.json` の
allowlist方式で、設定ファイルだけ共有しビルド生成物やユーザー個別設定は除外する形になっている。
このフォルダには `.vscode/tasks.json`(MSBuildビルド)・`launch.json`(cppvsdbgデバッグ実行)・
`extensions.json`(推奨拡張機能)を追加済み。`c_cpp_properties.json`はallowlist対象外(マシン固有の
IntelliSense設定として意図的に共有しない設計と思われる)なので、新しいPCでは初回に再生成が必要。

### 4. コミット運用
過去のコミットメッセージはほぼ全て `"up"` のみ(reflogで確認)。内容の要約は無い。
差分の経緯を知りたい場合はコミットメッセージに頼らず、コード自体を読むこと。

## ディレクトリ構成

```
2026_summer/
├── main.cpp            … WinMain。DxLib/Effekseer初期化、60FPS固定ループ
├── GameDefine.h         … WIDTH/HEIGHT等グローバル定数
├── Bases/               … SceneBase, Character, MagicBase(基底クラス)
├── Scenes/              … SceneManager(ステートマシン)+ Load/Start/Explain/Main/Result各シーン, SkyDome
├── Players/             … Player, PlayerMove, PlayerDodge
├── Enemys/              … Enemy, EnemyManager, EnemyMove, EnemyRoutine(行動パターン定義)
├── Magics/              … MagicManager + Shot/Missile/Beam/Fury各魔法, MagicMove
├── Colls/               … Collision(キャラ同士), MagicCollision(魔法とキャラ)
├── Cameras/             … Camera(通常追従/ロックオンの補間切替)
├── UIs/                 … UIManager + 各種HP/MP/ゲージバー
├── Effects/             … EffectManager(Effekseerラッパー), EffectHandles
├── Anims/               … AnimManager(MV1アニメ再生管理), AnimInfo
├── Inputs/              … Input(ジョイパッド入力ラッパー)
├── Sounds/              … SoundInfo(BGM/SEハンドル定義)
└── data/                … 3Dモデル(.mv1)・エフェクト(.efkefc)・画像・音声
```

## アーキテクチャ概要

### シーン管理
`SceneManager` が `SceneState`(Load→Start→Explain→Main→Result→Startの循環)を管理。
`Update()`でプレイヤーHP0→Result遷移、敵HP0→スコア加算しMain再遷移を判定。
`ChangeScene()`で各シーンのInit呼び出しとBGM切替、`Fade()`でアルファブレンドのフェード演出。

### キャラクター(Character基底)
`CharacterState`(Approach/MoveAway/MoveLeft/MoveRight/DodgeLeft/DodgeRight/Shot/Missile/
Fury/Beam/HitStun/Dead/Wait)を持つ。`TranslateState()`でステート→アニメ番号+ループ有無への
静的マッピングを保持。

### プレイヤー
`Player`(HP1000/MP500、MP自動回復0.25/frame)。右スティックで向き変更、RBトリガーの
押下時間で短押し=ショット/長押し(30f以上)=ミサイルを切替、長押し中はロックオンカメラへ。
必殺技チャージ最大でフューリー、半分以上でビーム発動。`PlayerDodge`でジャスト回避判定あり
(成功でHP/MP回復+チャージ増加)。

### 敵AI
`EnemyManager`が`EnemyRoutine.h`の固定行動列(`NORM/NEAR/AWAY/HARD_ROUTINE`)を順に消化する
簡易ステートマシン。プレイヤーとの距離・体力割合(30%以下かつスコア>10でHARD)・ロックオン
状態に応じてルーチンを切替。

### 魔法
`MagicManager`がプレイヤー/敵それぞれの発射済み魔法リストを保持。Shot(直進/MP10/dmg50)、
Missile(誘導/MP30/dmg40)、Beam(狙い→発射の2段階/dmg75、プレイヤー使用時300)、
Fury(範囲落下/dmg200、必殺技専用)の4種。

### 当たり判定
`Collision`=キャラ同士のカプセル衝突+押し戻し。`MagicCollision`=魔法とキャラのセグメント/点
距離判定、回避中は無効化、ジャスト回避判定あり。

## 実装済み / 未完成の状態(2026-09-04時点で確認)

**実装済み**: シーン一巡(Title→説明→本編→Result)、プレイヤーの移動/回避/2種攻撃/2種必殺技、
敵の4パターン行動ルーチン、当たり判定一式、カメラ2モード、HPバー/必殺ゲージUI、
アニメ管理、BGM/SE。デバッグ時(`_DEBUG`)にHP/MP数値・当たり判定カプセル等を画面表示。

**未完成・コメントアウトされたまま**:
- `PlayerMpBar`: 実装済みだが`UIManager::Draw()`内で描画呼び出しがコメントアウト → **MPバーは画面に出ない**
- `PlayerDodge::DodgeCoolBar()`: 回避クールダウンバー描画は実装済みだが呼び出し側がコメントアウトで未使用
- `EffectManager::Load()`: 空実装。実際のエフェクトロードは`LoadScene`側に直書きされており責務が中途半端
- `Enemy::Update()`内にアニメ切替処理のコメントアウトが残存
- スコア/セーブ機能なし。単一ボス戦の繰り返し構造(倒すとSceneMain再初期化でリポップ)
- ポーズ/設定画面なし(ESCで即終了)

TODOコメントやFIXME等の明示的なマーカーは無い。未実装箇所はコメントアウトされたコードや
空実装関数から判断すること。

## ドキュメント状況

このCLAUDE.md以外に設計ドキュメントは存在しない(READMEもなし、`data/Suzuki01/`配下の
README.mdはEffekseer素材配布元のサンプル説明書で無関係)。コード内コメントは日本語で
丁寧に付与されており、実質的な仕様書はソースコード自体。

## 作業開始時のチェックリスト

1. 新しいPCで初めて開く場合、DxLib(`c:\DxLib_VC\`)とEffekseerForDXLib 1.80.7
   (`C:\EffekseerForDXLib_1.80.7_324f\`)が同じパスにインストールされているか確認。
2. ビルドが通らない場合、上記パス不一致か、x64設定の連結パスバグを疑う。
3. 大きな変更をする前に、ビルド生成物(`x64/`, `.vs/`)がgit管理下にないか一度確認し、
   あれば`.gitignore`整備を提案する。
4. 実装状況は上記「未完成」セクションを鵜呑みにせず、変更に着手する前に該当ファイルを
   Readで確認する(コード変更で解消済みの可能性があるため)。
