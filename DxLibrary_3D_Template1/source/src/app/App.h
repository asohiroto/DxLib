#pragma once
#define ARCANA_VERSION "v1.0.1"
extern const char* g_crashScreen;   // updated every frame for crash reports
extern unsigned g_crashSeed;
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include "game/RunState.h"
#include "game/Profile.h"
#include "game/World.h"
#include "sim/Policy.h"
#include "sim/RunPolicy.h"
#include "app/InputDx.h"
#include "app/Render.h"
#include "app/Ui.h"

void changeDisplayMode(bool windowed);   // every window/fullscreen switch goes through this (see App.cpp)

struct AppOptions {
    std::string autoplay;      // combat policy name; non-empty = autopilot drives every screen
    float skill = 0.6f;
    uint32_t seed = 0;         // 0 = time based
    int shotInterval = 0;
    int shotFrom = 0;          // only capture interval shots from this frame on
    int maxFrames = 0;
    int maxRuns = 0;           // autopilot: quit after N finished runs
    std::string shotDir;       // empty = <save dir>\screenshots
    bool portable = false;     // keep saves in .\save next to the exe
    bool mute = false;
    bool fresh = false;        // ignore saved profile / run
    int speed = 1;             // autopilot: world updates per frame
    bool screenShots = false;  // save one PNG shortly after every screen change
    std::string startScreen;   // debug: options / keys / credits
    bool gallery = false;      // QA: capture every screen / state, then quit
    bool galleryFullscreen = false;
    bool galleryTransitionsOnly = false;   // QA: only the display-mode / mouse checks
    int f11Stress = 0;                     // QA: toggle the display mode N times during a battle, then exit
    int modeStress = 0;                    // QA: N x (new battle, pause, F11) - the sequence of the rare d3d11 crash
    int fsMode = 2;                        // bit0 explicit desktop-resolution fullscreen (crashes at exit on DxLib 3.24f: off), bit1 nearest scaling
};

enum class Screen { Title, Hub, Options, Run, Result, Tutorial, Keys, Credits };

// Deck picker used by shop removal, forge and rest.
enum class Pick { None, Remove, Anchor1, Anchor2, RankUp1, RankUp2, Train, RestTrain, Upgrade, View };

class App {
public:
    bool init(const AppOptions& o);
    bool frame();      // false = quit
    void shutdown();
    void runGallery();   // --gallery
    void toggleFullscreen();
    void updateCursor();

    struct GalleryStep {
        std::string name;
        std::function<void()> setup;
        int frames = 4;
        int focus = -1;
        bool expectCursor = true;
        bool pad = false;
        std::vector<int> keys;   // injected after the screen-change input lock, one every 2 frames
        std::function<std::string()> verify;   // returns "" if fine, else a finding
    };

private:
    // screens
    void doTitle();
    void doHub();
    void doOptions();
    void doKeys();
    void doCredits();
    float creditsY_ = 0;
    int rebind_ = -1;
    void doResult();
    void startTutorial();
    void doTutorial();
    void tutEnterStep(int s);
    int tutStep_ = 0;
    float tutT_ = 0;
    Vec2 tutStart_;
    int tutCount_ = 0;
    void doRun();
    void doMap();
    void doBattle();
    void doReward();
    void doShop();
    void doForge();
    void doRest();
    void doEvent();
    void doChapterClear();
    bool doPicker();           // returns true while the picker is open
    void drawTopBar();
    void drawBackground(int variant);

    // helpers
    void startRun();
    void continueRun();
    void applyAction(const Action& a);
    void saveRun();
    void saveProfile();
    void endRun(bool victory);
    void applySettings();
    void updateMusic();
    void hint(int bit, const char* text);   // show a one-time hint
    std::string hintText_;
    float hintT_ = 0;
    void button(int id, int x, int y, int w, int h, const char* label, bool enabled = true, unsigned textColor = 0);   // 0 = default
    void panel(int x, int y, int w, int h, unsigned border);
    void focusFrame(int x, int y, int w, int h);
    void wrapText(int x, int y, int w, const std::string& s, unsigned col, int font, int lineH);
    static std::vector<std::string> wrapLines(const std::string& s, int w, int font);
    std::string cardDesc(const CardInstance& c) const;
    void drawCardBig(int x, int y, const CardInstance& c, bool focus, const char* footer = nullptr, bool affordable = true, bool sold = false);
    static int cardTileH(bool footer);
    static constexpr int kTileW = 220;
    void beginBattle();
    // autopilot
    void autopilotMenus();
    int autoIdFor(const Action& a);

    AppOptions opt_;
    Profile profile_;
    std::unique_ptr<RunState> run_;
    std::unique_ptr<World> world_;
    std::unique_ptr<Policy> combatAI_;
    std::unique_ptr<RunPolicy> runAI_;
    Screen screen_ = Screen::Title;
    Screen optionsBack_ = Screen::Title;
    Renderer R;
    InputDx in;
    Ui ui;
    Pick pick_ = Pick::None;
    int pickFirst_ = -1;
    int deckScroll_ = 0;
    bool paused_ = false;
    int battleEnd_ = 0;
    bool lastVictory_ = false;
    RunState lastRun_;
    int lastShards_ = 0;
    int totalFrames_ = 0;
    int runsFinished_ = 0;
    int autoDelay_ = 0;
    bool debug_ = false;
    bool quit_ = false;
    bool hasSave_ = false;
    std::string toast_;
    float toastT_ = 0;
    uint32_t seedCounter_ = 1;
    int lastScreenKey_ = -1;
    int lastUiKey_ = -1;
    bool confirmNew_ = false;
    int screenShotT_ = -1;
    int screenShotN_ = 0;
    // gallery
    bool galleryFullscreen_ = false;
    int cursorShown_ = -1;
    bool fullscreenPending_ = false;
    bool galleryAutoPause_ = false;   // gallery normally ignores focus loss; one check turns it back on   // -1 = unknown, forces SetMouseDispFlag on the next frame
    void galleryReset();
    std::unique_ptr<RunState> galleryRun(int chapter, int deckSize, int relicCount);
    std::vector<GalleryStep> galleryBuild();
};
