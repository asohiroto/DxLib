"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.activate = activate;
exports.deactivate = deactivate;
const vscode = __importStar(require("vscode"));
const fs = __importStar(require("fs"));
const path = __importStar(require("path"));
const child_process_1 = require("child_process");
const REQUIRED_SUBDIR = 'プロジェクトに追加すべきファイル_VC用';
const DRIVES = ['C:\\', 'D:\\'];
let statusBarItem;
let output;
function activate(context) {
    output = vscode.window.createOutputChannel('DxLib環境セットアップ');
    statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
    statusBarItem.text = '$(tools) DxLib環境セットアップ';
    statusBarItem.tooltip = 'DxLib/Effekseerのパス確認・IntelliSense設定・vcxproj修正をまとめて実行';
    statusBarItem.command = 'dxlibEnvSetup.run';
    statusBarItem.show();
    context.subscriptions.push(statusBarItem, output, vscode.commands.registerCommand('dxlibEnvSetup.run', runSetup));
}
function deactivate() {
    // no-op
}
async function runSetup() {
    output.clear();
    output.show(true);
    output.appendLine('=== DxLib開発環境セットアップ開始 ===');
    const folders = vscode.workspace.workspaceFolders;
    if (!folders || folders.length === 0) {
        vscode.window.showErrorMessage('ワークスペースフォルダが開かれていません。');
        return;
    }
    const vcxprojUris = await vscode.workspace.findFiles('**/*.vcxproj', '**/node_modules/**', 5);
    if (vcxprojUris.length === 0) {
        const msg = 'このワークスペースに.vcxprojが見つかりませんでした。DxLib/C++プロジェクトのフォルダを開いてください。';
        output.appendLine(`❌ ${msg}`);
        vscode.window.showWarningMessage(msg);
        return;
    }
    const vcxprojPath = vcxprojUris[0].fsPath;
    const wsRoot = path.dirname(vcxprojPath);
    output.appendLine(`対象プロジェクト: ${vcxprojPath}`);
    const dxlib = pickBest(findLib(/^DxLib_VC$/i));
    const effekseer = pickBest(findLib(/^EffekseerForDXLib(_[\d.]+_\w+)?$/i));
    logLibStatus('DxLib', dxlib);
    logLibStatus('EffekseerForDXLib', effekseer);
    if (!dxlib?.found || !effekseer?.found) {
        const msg = 'DxLibまたはEffekseerForDXLibが既定の場所に見つかりません。CLAUDE.mdの手順に従い、C:\\DxLib_VC およびEffekseerForDXLibを導入してください(ライセンス同意が必要なため自動ダウンロードは行いません)。';
        output.appendLine(`❌ ${msg}`);
        vscode.window.showWarningMessage(msg);
        return;
    }
    const compilerPath = detectCompilerPath();
    output.appendLine(compilerPath
        ? `✅ コンパイラ検出: ${compilerPath}`
        : '⚠️ cl.exeの自動検出に失敗しました(Visual Studio C++ワークロードが未インストールの可能性)');
    updateCppProperties(wsRoot, dxlib.includeDir, effekseer.includeDir, compilerPath);
    const fixed = fixVcxprojPaths(vcxprojPath);
    output.appendLine(fixed ? '✅ vcxprojのパス連結バグを修正しました' : '✅ vcxprojに連結バグは見つかりませんでした');
    output.appendLine('=== セットアップ完了 ===');
    vscode.window.showInformationMessage('DxLib開発環境のセットアップが完了しました。詳細は出力パネルを確認してください。');
}
function findLib(namePattern) {
    const results = [];
    for (const drive of DRIVES) {
        let entries;
        try {
            entries = fs.readdirSync(drive);
        }
        catch {
            continue;
        }
        for (const entry of entries) {
            if (namePattern.test(entry)) {
                const root = path.join(drive, entry);
                const includeDir = path.join(root, REQUIRED_SUBDIR);
                results.push({ name: entry, root, includeDir, found: fs.existsSync(includeDir) });
            }
        }
    }
    return results;
}
function pickBest(hits) {
    const found = hits.filter(h => h.found).sort((a, b) => b.name.localeCompare(a.name));
    return found[0] ?? hits[0];
}
function logLibStatus(label, info) {
    if (!info) {
        output.appendLine(`❌ ${label}: 見つかりませんでした`);
        return;
    }
    output.appendLine(`${info.found ? '✅' : '❌'} ${label}: ${info.includeDir}`);
}
function detectCompilerPath() {
    const vswhere = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe';
    if (!fs.existsSync(vswhere)) {
        return undefined;
    }
    try {
        const installPath = (0, child_process_1.execFileSync)(vswhere, [
            '-latest',
            '-products', '*',
            '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
            '-property', 'installationPath'
        ]).toString().trim();
        if (!installPath) {
            return undefined;
        }
        const msvcDir = path.join(installPath, 'VC', 'Tools', 'MSVC');
        const versions = fs.readdirSync(msvcDir).sort();
        const latest = versions[versions.length - 1];
        if (!latest) {
            return undefined;
        }
        const clPath = path.join(msvcDir, latest, 'bin', 'Hostx64', 'x64', 'cl.exe');
        return fs.existsSync(clPath) ? clPath : undefined;
    }
    catch {
        return undefined;
    }
}
function updateCppProperties(wsRoot, dxlibInclude, effekseerInclude, compilerPath) {
    const vscodeDir = path.join(wsRoot, '.vscode');
    if (!fs.existsSync(vscodeDir)) {
        fs.mkdirSync(vscodeDir, { recursive: true });
    }
    const filePath = path.join(vscodeDir, 'c_cpp_properties.json');
    let json = {
        configurations: [
            {
                name: 'Win32',
                includePath: ['${workspaceFolder}/**', dxlibInclude, effekseerInclude],
                defines: ['_DEBUG', '_WINDOWS', '_MBCS', 'DX_NON_NAMESPACE'],
                cStandard: 'c17',
                cppStandard: 'c++20',
                intelliSenseMode: 'windows-msvc-x64'
            }
        ],
        version: 4
    };
    if (fs.existsSync(filePath)) {
        try {
            const existing = JSON.parse(fs.readFileSync(filePath, 'utf8'));
            if (existing?.configurations?.[0]) {
                json = existing;
                json.configurations[0].includePath = ['${workspaceFolder}/**', dxlibInclude, effekseerInclude];
            }
        }
        catch {
            // 壊れたJSONは既定内容で上書き生成する
        }
    }
    if (compilerPath && json.configurations?.[0]) {
        json.configurations[0].compilerPath = compilerPath;
    }
    fs.writeFileSync(filePath, JSON.stringify(json, null, 4) + '\n', 'utf8');
    output.appendLine(`✅ c_cpp_properties.json を更新しました: ${filePath}`);
}
function fixVcxprojPaths(vcxprojPath) {
    const original = fs.readFileSync(vcxprojPath, 'utf8');
    const fixedContent = original.replace(/(VC用)(?=[Cc]:\\)/g, '$1;');
    if (fixedContent !== original) {
        fs.writeFileSync(vcxprojPath, fixedContent, 'utf8');
        return true;
    }
    return false;
}
//# sourceMappingURL=extension.js.map