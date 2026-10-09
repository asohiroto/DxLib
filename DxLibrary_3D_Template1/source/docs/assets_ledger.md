# 素材台帳

| # | 素材 | 作者 | 取得元URL | ライセンス | 商用 | 改変 | クレジット | 取得日 | 用途 | AI生成 |
|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 16x16 DungeonTileset II v1.7 | 0x72 (Robert)、recolor GrafxKid | https://0x72.itch.io/dungeontileset-ii | CC0（ページ本文 "You can use this tileset for whatever you like (CC-0). Credit is not necessary"） | 可 | 可 | 不要（任意でクレジット記載） | 2026-10-03 | プレイヤー・敵・ボス・床・壁・柱・アイテム | なし |
| 2 | Probly12 Font（Probly12-CJK.ttf） | Problyme（一部グリフ：Ark Pixel / TakWolf、Galmuri / Lee Minseo） | https://problyme.itch.io/probly12-font | SIL OFL 1.1（ページ本文とフォント内メタデータで確認） | 可（同梱可、フォント単体の販売は不可） | 可（改変版は予約名不可） | ライセンス文の同梱が必要 → data/font/Probly12_LICENSE.txt | 2026-10-03 | ゲーム内の全テキスト | なし |
| 3 | blip8 sounds: 181 CC0 chiptune SFX | sindriax (Sandra) | https://sindriax.itch.io/blip8-sounds | CC0 1.0（ページ本文と同梱 LICENSE.txt） | 可 | 可 | 不要 | 2026-10-03 | 効果音26種（data/sfx、対応表は下記） | なし（プログラム生成音） |
| 4 | The Game Creator's Pack（Audio Pack - MP3） | Jonathan So | https://jonathan-so.itch.io/creatorpack | CC0（ページ本文 "This Creative-Commons-Zero [CC-0] asset package"） | 可 | 可 | 不要（任意で "Jonathan So" と記載） | 2026-10-03 | BGM 9曲（data/bgm） | なし |
| - | FREE Pixel Combat SFX（Helton Yan, CC BY 4.0） | — | https://heltonyan.itch.io/pixelcombat | CC BY 4.0 | — | — | — | — | **未使用**（2.1GBのため取得を見送り） | — |

### 効果音の対応（data/sfx ← blip8）
shot←laser_thin_short, hit←kick_soft, kill←explosion_small_crunched, card←spell_cast, fuse←powerup_minor_fast, evolve←level_up, hurt←hurt_quick, dash←dash_1, explode←explosion_mid, freeze←gem_2, shatter←crash_short, nomana←error_1, clear←win_short, select←blip_mid_short, confirm←confirm_soft, cancel←cancel_soft, coin←coin_classic, bosshit←boss_hit, death←game_over, fanfare←fanfare, warn←notify_1, shield←shield_up, heal←powerup_sus_fast, open←open_1, bossphase←alarm_motif, spawn←teleport_in

### BGMの対応（data/bgm ← Jonathan So）
title←Title Theme, hub←Waves in Flight, map←Fields of Ice, battle1←Crimson Drive, battle2←Sunstrider, battle3←Zero Respect, boss←The Monarch's Rule, victory←Victory! All Clear, defeat←Fallen in Battle

## ランタイム・ライブラリ
| 名称 | ライセンス表記 |
|---|---|
| DXライブラリ (DxLib) 3.24f | 作者：山田 巧。DxLib は配布時の表記条件に従い、クレジットに使用ライブラリとして記載（libpng / zlib / libjpeg / Ogg Vorbis 等の内包ライブラリ表記を含む） |
| Effekseer / EffekseerForDXLib | MIT License（Copyright (c) Effekseer Project）。クレジットに記載 |

## AI生成物
（生成ごとに追記：対象、ツール、日付、プロンプト雛形、後処理）

| 対象 | ツール | 日付 | 内容 | 後処理 | 保存先 |
|---|---|---|---|---|---|
| カードアイコン シート1（9枚：火5・氷4） | Google Gemini（Pro、画像生成） | 2026-10-03 | 3x3グリッド、レトロ16bitドット絵、黒背景、文字なし（プロンプト雛形は下記） | tools/iconsheet.ps1：セル切り出し→余白除去→32x32最近傍縮小→素材#1のパレット66色に量子化→黒背景透過→暗色1px縁取り | data/gfx/cards/*.png（原本 assets_src/gemini/icons_fire_ice.jpg） |
| カードアイコン シート2（16枚：雷4・土4・無5・追加3） | 同上 | 2026-10-03 | 4x4グリッド、シート1と同一スタイル指定 | 同上 | data/gfx/cards/*.png（原本 assets_src/gemini/icons_sheet2.jpg） |
| アイコン シート3（25枚：カード3・レリック20・UI2） | 同上 | 2026-10-03 | 5x5グリッド、同一スタイル指定 | 同上（クリップボード経由で取得） | data/gfx/cards, data/gfx/relics, data/gfx/ui（原本 assets_src/gemini/icons_sheet3.png） |

プロンプト雛形：「a NxN grid sprite sheet of fantasy spell icons in retro 16-bit pixel art style. Each icon square and centered in its own cell on a solid pure black background, wide black gaps, chunky visible pixels (~32x32 upscaled nearest), small limited palette, 1-pixel dark outline, light from the top-left, high contrast, no text/letters/numbers/frames/borders. Icons in reading order: …」（作品名・作家名は入れない）

AI開示文（ストア用・草案）：「本作のカードアイコンの一部は画像生成AI（Google Gemini）で下絵を生成し、開発者がドット絵パレットへの変換・縮小・縁取りの加工を行っています。キャラクター・背景のドット絵は CC0 素材（0x72 DungeonTileset II）を使用しています。」
