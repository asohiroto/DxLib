#pragma once
#include <string>

enum class Sfx {
    Shot, Hit, Kill, Card, Fuse, Evolve, Hurt, Dash, Explode, Freeze, Shatter, NoMana, Clear, Select,
    Confirm, Cancel, Coin, BossHit, Death, Fanfare, Warn, Shield, Heal, Open, BossPhase, Spawn,
    Count
};

namespace Sound {
void init();            // loads data/sfx/*.wav (falls back to generated waveforms)
void play(Sfx s, float vol = 1.0f);
void setMasterVolume(float v);
void tick();            // once per frame (rate limiting)
}

// Streaming background music with cross-fades (data/bgm/<name>.mp3)
namespace Music {
void play(const std::string& name, bool loop = true);   // no-op if already playing
void stop();
void setVolume(float v);
void update();          // once per frame
}
