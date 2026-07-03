#include <DxLib.h>
#include "Application.h"
// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
	AllocConsole();                                        // コンソール
	FILE* out = 0; freopen_s(&out, "CON", "w", stdout); // stdout
	FILE* in = 0; freopen_s(&in, "CON", "r", stdin);   // stdin
#endif
	//C++のstaticメンバの呼び出しは　::　なので注意
	Application& app = Application::GetInstance();
	if (!app.Init())
	{
		return -1;
	}
	app.Run();
	app.Terminate();
#if _DEBUG
	fclose(out); fclose(in); FreeConsole();//コンソール解放
#endif
	return 0;				// ソフトの終了 
}