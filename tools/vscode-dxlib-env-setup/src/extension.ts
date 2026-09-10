import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';
import { execFileSync } from 'child_process';

const REQUIRED_SUBDIR = 'プロジェクトに追加すべきファイル_VC用';
const DRIVES = ['C:\\', 'D:\\'];

interface LibInfo {
    name: string;
    root: string;
    includeDir: string;
    found: boolean;
}

let statusBarItem: vscode.StatusBarItem;
let output: vscode.OutputChannel;

export function activate(context: vscode.ExtensionContext) {
    output = vscode.window.createOutputChannel('DxLib環境セットアップ');

    statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
    statusBarItem.text = '$(tools) DxLib環境セットアップ';
    statusBarItem.tooltip = 'DxLib/Effekseerのパス確認・IntelliSense設定・vcxproj修正をまとめて実行';
    statusBarItem.command = 'dxlibEnvSetup.run';
    statusBarItem.show();

    context.subscriptions.push(
        statusBarItem,
        output,
        vscode.commands.registerCommand('dxlibEnvSetup.run', runSetup)
    );
}

export function deactivate() {
    // no-op
}

async function runSetup(): Promise<void> {
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
    output.appendLine(
        compilerPath
            ? `✅ コンパイラ検出: ${compilerPath}`
            : '⚠️ cl.exeの自動検出に失敗しました(Visual Studio C++ワークロードが未インストールの可能性)'
    );

    updateCppProperties(wsRoot, dxlib.includeDir, effekseer.includeDir, compilerPath);

    const fixed = fixVcxprojPaths(vcxprojPath);
    output.appendLine(fixed ? '✅ vcxprojのパス連結バグを修正しました' : '✅ vcxprojに連結バグは見つかりませんでした');

    output.appendLine('=== セットアップ完了 ===');
    vscode.window.showInformationMessage('DxLib開発環境のセットアップが完了しました。詳細は出力パネルを確認してください。');
}

function findLib(namePattern: RegExp): LibInfo[] {
    const results: LibInfo[] = [];
    for (const drive of DRIVES) {
        let entries: string[];
        try {
            entries = fs.readdirSync(drive);
        } catch {
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

function pickBest(hits: LibInfo[]): LibInfo | undefined {
    const found = hits.filter(h => h.found).sort((a, b) => b.name.localeCompare(a.name));
    return found[0] ?? hits[0];
}

function logLibStatus(label: string, info?: LibInfo): void {
    if (!info) {
        output.appendLine(`❌ ${label}: 見つかりませんでした`);
        return;
    }
    output.appendLine(`${info.found ? '✅' : '❌'} ${label}: ${info.includeDir}`);
}

function detectCompilerPath(): string | undefined {
    const vswhere = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe';
    if (!fs.existsSync(vswhere)) {
        return undefined;
    }
    try {
        const installPath = execFileSync(vswhere, [
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
    } catch {
        return undefined;
    }
}

function updateCppProperties(
    wsRoot: string,
    dxlibInclude: string,
    effekseerInclude: string,
    compilerPath: string | undefined
): void {
    const vscodeDir = path.join(wsRoot, '.vscode');
    if (!fs.existsSync(vscodeDir)) {
        fs.mkdirSync(vscodeDir, { recursive: true });
    }
    const filePath = path.join(vscodeDir, 'c_cpp_properties.json');

    let json: any = {
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
        } catch {
            // 壊れたJSONは既定内容で上書き生成する
        }
    }

    if (compilerPath && json.configurations?.[0]) {
        json.configurations[0].compilerPath = compilerPath;
    }

    fs.writeFileSync(filePath, JSON.stringify(json, null, 4) + '\n', 'utf8');
    output.appendLine(`✅ c_cpp_properties.json を更新しました: ${filePath}`);
}

function fixVcxprojPaths(vcxprojPath: string): boolean {
    const original = fs.readFileSync(vcxprojPath, 'utf8');
    const fixedContent = original.replace(/(VC用)(?=[Cc]:\\)/g, '$1;');
    if (fixedContent !== original) {
        fs.writeFileSync(vcxprojPath, fixedContent, 'utf8');
        return true;
    }
    return false;
}
