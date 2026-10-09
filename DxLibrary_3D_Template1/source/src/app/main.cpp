#include "DxLib.h"
#include "EffekseerForDXLib.h"
#include <cstdio>
#include <string>
#include <vector>
#include "app/App.h"
#include "game/Content.h"
#include "game/RunState.h"
#include "game/Profile.h"
#include "game/Save.h"
#include "core/Paths.h"
#include <ctime>
#include <algorithm>
#include "core/Log.h"
#include <dbghelp.h>
#include <cwchar>
#pragma comment(lib, "dbghelp.lib")

namespace {
AppOptions parseArgs(const char* cmd) {
    AppOptions o;
    std::string s = cmd ? cmd : "";
    std::vector<std::string> a;
    std::string cur;
    for (char c : s) {
        if (c == ' ') { if (!cur.empty()) a.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) a.push_back(cur);
    for (size_t i = 0; i < a.size(); ++i) {
        auto next = [&]() { return i + 1 < a.size() ? a[++i] : std::string(); };
        if (a[i] == "--autoplay") o.autoplay = next();
        else if (a[i] == "--skill") o.skill = (float)atof(next().c_str());
        else if (a[i] == "--seed") o.seed = (uint32_t)atoi(next().c_str());
        else if (a[i] == "--shots") o.shotInterval = atoi(next().c_str());
        else if (a[i] == "--shotfrom") o.shotFrom = atoi(next().c_str());
        else if (a[i] == "--frames") o.maxFrames = atoi(next().c_str());
        else if (a[i] == "--runs") o.maxRuns = atoi(next().c_str());
        else if (a[i] == "--shotdir") o.shotDir = next();
        else if (a[i] == "--mute") o.mute = true;
        else if (a[i] == "--portable") o.portable = true;
        else if (a[i] == "--gallery") o.gallery = true;
        else if (a[i] == "--gallery-transitions") { o.gallery = true; o.galleryTransitionsOnly = true; }
        else if (a[i] == "--f11stress") { o.gallery = true; o.f11Stress = atoi(next().c_str()); }
        else if (a[i] == "--modestress") { o.gallery = true; o.modeStress = atoi(next().c_str()); }
        else if (a[i] == "--fsmode") o.fsMode = atoi(next().c_str());
        else if (a[i] == "--gallery-fullscreen") { o.gallery = true; o.galleryFullscreen = true; }
        else if (a[i] == "--fresh") o.fresh = true;
        else if (a[i] == "--screenshots") o.screenShots = true;
        else if (a[i] == "--screen") o.startScreen = next();
        else if (a[i] == "--speed") o.speed = std::max(1, atoi(next().c_str()));
    }
    return o;
}
}  // namespace

namespace {
bool g_quietCrash = false;   // automated runs: record the crash and exit, no dialog
// Last-resort crash report so players can send something useful.
LONG WINAPI crashHandler(EXCEPTION_POINTERS* ep) {
    if (FILE* f = Paths::open(Paths::save("crash.txt"), "a")) {
        HMODULE base = GetModuleHandleW(nullptr);
        void* addr = ep && ep->ExceptionRecord ? ep->ExceptionRecord->ExceptionAddress : nullptr;
        std::fprintf(f, "Arcana Forge %s crashed: code 0x%08lX at %p (exe+0x%llX) screen=%s seed=%u time=%lld\n", ARCANA_VERSION,
                     ep && ep->ExceptionRecord ? ep->ExceptionRecord->ExceptionCode : 0ul, addr,
                     (unsigned long long)((char*)addr - (char*)base), g_crashScreen, g_crashSeed, (long long)time(nullptr));
        if (ep && ep->ContextRecord) {   // call stack: module + offset (+ symbol when a PDB is beside the exe)
            HANDLE proc = GetCurrentProcess(), th = GetCurrentThread();
            SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
            SymInitialize(proc, nullptr, TRUE);
            CONTEXT ctx = *ep->ContextRecord;
            STACKFRAME64 sf{};
            sf.AddrPC.Offset = ctx.Rip; sf.AddrPC.Mode = AddrModeFlat;
            sf.AddrFrame.Offset = ctx.Rbp; sf.AddrFrame.Mode = AddrModeFlat;
            sf.AddrStack.Offset = ctx.Rsp; sf.AddrStack.Mode = AddrModeFlat;
            for (int i = 0; i < 24; ++i) {
                if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, proc, th, &sf, &ctx, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) break;
                DWORD64 pc = sf.AddrPC.Offset;
                if (!pc) break;
                HMODULE mod = nullptr;
                wchar_t mname[MAX_PATH] = L"?";
                if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)pc, &mod))
                    GetModuleFileNameW(mod, mname, MAX_PATH);
                const wchar_t* shortName = wcsrchr(mname, L'\\') ? wcsrchr(mname, L'\\') + 1 : mname;
                char symBuf[sizeof(SYMBOL_INFO) + 256] = {};
                SYMBOL_INFO* sym = (SYMBOL_INFO*)symBuf;
                sym->SizeOfStruct = sizeof(SYMBOL_INFO); sym->MaxNameLen = 255;
                DWORD64 disp = 0;
                bool named = SymFromAddr(proc, pc, &disp, sym) != FALSE;
                std::fprintf(f, "  #%d %ls+0x%llX %s%s\n", i, shortName, (unsigned long long)(pc - (DWORD64)mod), named ? sym->Name : "", named ? "" : "");
            }
            SymCleanup(proc);
        }
        std::fclose(f);
    }
    if (!g_quietCrash) MessageBoxW(nullptr, L"予期しないエラーで終了しました。\n%APPDATA%\\ArcanaForge\\save\\crash.txt に記録しました。\n冒険は最後にクリアした部屋から再開できます。",
                L"Arcana Forge", MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}

void startupError(const wchar_t* what) {
    std::wstring msg = std::wstring(what) +
        L"\n\n・DirectX 11 に対応したグラフィックドライバが必要です。ドライバを更新してください。"
        L"\n・zip を展開してから起動してください（zip の中から直接起動すると失敗します）。"
        L"\n・data フォルダが ArcanaForge.exe と同じ場所にあるか確認してください。";
    MessageBoxW(nullptr, msg.c_str(), L"Arcana Forge", MB_OK | MB_ICONERROR);
}

// Keep the window's client area at 16:9 while the player drags its edges (DxLib would stretch the picture).
LRESULT CALLBACK keepAspectHook(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg != WM_SIZING) return 0;
    RECT* r = (RECT*)lp;
    RECT frame{0, 0, 0, 0};
    AdjustWindowRectEx(&frame, (DWORD)GetWindowLongPtr(hw, GWL_STYLE), FALSE, (DWORD)GetWindowLongPtr(hw, GWL_EXSTYLE));
    int fw = frame.right - frame.left, fh = frame.bottom - frame.top;   // border sizes
    int cw = std::max(320, (int)(r->right - r->left) - fw), ch = std::max(180, (int)(r->bottom - r->top) - fh);
    bool horizontalEdge = wp == WMSZ_LEFT || wp == WMSZ_RIGHT;
    if (horizontalEdge || (wp != WMSZ_TOP && wp != WMSZ_BOTTOM && cw * 9 >= ch * 16)) ch = cw * 9 / 16;
    else cw = ch * 16 / 9;
    if (wp == WMSZ_LEFT || wp == WMSZ_TOPLEFT || wp == WMSZ_BOTTOMLEFT) r->left = r->right - (cw + fw); else r->right = r->left + cw + fw;
    if (wp == WMSZ_TOP || wp == WMSZ_TOPLEFT || wp == WMSZ_TOPRIGHT) r->top = r->bottom - (ch + fh); else r->bottom = r->top + ch + fh;
    SetUseHookWinProcReturnValue(TRUE);
    return TRUE;
}

bool hasData() { DWORD a = GetFileAttributesW(L"data\\cards.tsv"); return a != INVALID_FILE_ATTRIBUTES; }
// Double-clicking the exe (or a shortcut with another working directory) must still find data/ next to it.
void locateDataDir() {
    if (hasData()) return;
    wchar_t path[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return;
    std::wstring dir(path, n);
    dir = dir.substr(0, dir.find_last_of(L"\\/"));
    SetCurrentDirectoryW(dir.c_str());
    if (hasData()) return;
    SetCurrentDirectoryW((dir + L"\\..").c_str());   // development layout: bin/ next to data/
}
}  // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdLine, int) {
    locateDataDir();
    AppOptions opt = parseArgs(cmdLine);
    Paths::init(!opt.autoplay.empty() || opt.portable || opt.gallery);   // autopilot/QA never touches the player's real saves
    if (opt.shotDir.empty()) opt.shotDir = Paths::save("screenshots");
    g_quietCrash = !opt.autoplay.empty() || opt.gallery;
    SetUnhandledExceptionFilter(crashHandler);
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
    SetOutApplicationLogValidFlag(FALSE);
    bool full = false;
    if (opt.autoplay.empty() && !opt.fresh && !opt.gallery) {
        std::string text;
        Profile p;
        if (Save::readFile(Paths::save("profile.txt"), text) && ProfileIO::deserialize(text, p)) full = p.settings.fullscreen;
    }
    ChangeWindowMode(full ? FALSE : TRUE);
    SetGraphMode(1280, 720, 32);
    // DxLib's default fullscreen already keeps the desktop resolution (verified: 1920x1080 stays 1920x1080).
    // Setting DX_FSRESOLUTIONMODE_DESKTOP explicitly made every run crash at exit (6/6) on 3.24f, so it stays off.
    // Nearest-neighbour scaling keeps the pixel art sharp.
    if (opt.fsMode & 1) SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_DESKTOP);
    if (opt.fsMode & 2) SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST);
    SetMainWindowText("Arcana Forge");
    SetWindowIconID(1);
    SetUseDirect3DVersion(DX_DIRECT3D_11);
    SetAlwaysRunFlag(TRUE);
    SetWaitVSyncFlag(FALSE);
    SetWindowSizeChangeEnableFlag(TRUE, TRUE);
    if (!opt.autoplay.empty() || opt.gallery) SetDoubleStartValidFlag(TRUE);   // parallel QA instances
    SetHookWinProc(keepAspectHook);
    if (DxLib_Init() == -1) { startupError(L"グラフィックの初期化に失敗しました。"); return -1; }
    if (Effekseer_Init(8000) == -1) { DxLib_End(); startupError(L"エフェクトの初期化に失敗しました。"); return -1; }
    SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
    Effekseer_Set2DSetting(1280, 720);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
    SetDrawScreen(DX_SCREEN_BACK);

    std::string err;
    if (!Content::load("data", &err) || !RunLogic::loadData("data", &err)) {
        MessageBoxW(nullptr, Paths::widen("ゲームデータが壊れているか不足しています。\n再度ダウンロードしてください。\n\n" + err).c_str(), L"Arcana Forge", MB_OK | MB_ICONERROR);
        DxLib_End();
        return -1;
    }
    App app;
    if (!app.init(opt)) { DxLib_End(); startupError(L"ゲームデータの読み込みに失敗しました。"); return -1; }

    if (opt.gallery) {
        app.runGallery();
        Effkseer_End();
        DxLib_End();
        return 0;
    }
    LONGLONG next = GetNowHiPerformanceCount();
    std::vector<float> frameMs;
    frameMs.reserve(60 * 60 * 30);
    while (ProcessMessage() == 0) {
        LONGLONG workStart = GetNowHiPerformanceCount();
        if (!app.frame()) break;
        ScreenFlip();
        {   // frame cost statistics (work only, not the wait), written to the log at exit
            double ms = (GetNowHiPerformanceCount() - workStart) / 1000.0;
            frameMs.push_back((float)ms);
        }
        next += 16667;
        LONGLONG now = GetNowHiPerformanceCount();
        if (next > now) { while (GetNowHiPerformanceCount() < next - 1500) WaitTimer(1); while (GetNowHiPerformanceCount() < next) {} }
        else if (now - next > 100000) next = now;
    }
    if (!frameMs.empty()) {
        std::vector<float> s = frameMs;
        std::sort(s.begin(), s.end());
        double sum = 0;
        int over = 0;
        for (float v : s) { sum += v; if (v > 16.7f) ++over; }
        Log::write("frame work ms: avg %.2f p50 %.2f p99 %.2f max %.2f, over 16.7ms: %d of %d", sum / s.size(), s[s.size() / 2],
                   s[(size_t)(s.size() * 0.99)], s.back(), over, (int)s.size());
    }
    app.shutdown();
    Effkseer_End();
    DxLib_End();
    return 0;
}
