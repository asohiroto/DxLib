#pragma once
#include <DxLib.h>
#include <string>

namespace StringUtil
{

	/// <summary>
	/// char -> TCHA
	/// </summary>
	/// <param name="c"></param>
	/// <returns></returns>
	TCHAR* ToTCHAR(const char* c);

	/// <summary>
	/// •¶š”‚ª‚ ‚é”‚ğ’´‚¦‚é‚½‚Ñ‚É‰üs‚ğ‚Í‚³‚İA‚»‚Ì•¶š—ñ‚ğ•Ô‚·
	/// </summary>
	/// <param name="str"></param>
	/// <param name="maxLength"></param>
	/// <returns></returns>
	std::wstring InsertNewLines(const std::wstring& str, int maxLength);

	/// <summary>
	/// s‚Ì”
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	int WStringLineNum(const std::wstring& str);

	/// <summary>
	/// wstring‚©‚çstring‚Ö
	/// </summary>
	std::string WstringToString(const std::wstring& wstr);

	/// <summary>
	/// string‚©‚çwstring‚Ö
	/// </summary>
	std::wstring StringToWstring(const std::string& str);

	/// <summary>
	/// •¶š—ñ‚Ì’·‚³
	/// </summary>
	/// <param name="text"></param>
	/// <param name="handle"></param>
	/// <returns></returns>
	float GetTextWidth(const std::wstring text, int handle);
};