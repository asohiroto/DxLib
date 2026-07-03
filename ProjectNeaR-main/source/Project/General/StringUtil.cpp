#include "StringUtil.h"
#include <cassert>

TCHAR* StringUtil::ToTCHAR(const char* c)
{
	// char* → TCHAR* に変換
	TCHAR cT[256];
	MultiByteToWideChar(CP_ACP, 0, c, -1, cT, 256);
	return cT;
}

std::wstring StringUtil::InsertNewLines(const std::wstring& str, int maxLength)
{
	std::wstring newStr;
	//あらかじめ必要なメモリを確保(メモリ最適化)
	newStr.reserve(str.size() + (str.size() / maxLength));
	//文字を数えていく
	size_t count = 0;
	for (auto c : str)
	{
		//文字を入れていく
		newStr += c;
		//改行が挟まった時カウントを0に
		if (c == '\n')
		{
			count = 0;
			continue;
		}
		//半角
		if (c < 0x80)
		{
			//カウント
			++count;
		}
		else
		{
			//カウント
			count += 2;
		}
		//一行の最大文字数を超えたら
		if (count > maxLength)
		{
			//改行
			newStr += '\n';
			//リセット
			count = 0;
		}
	}
	return newStr;
}

int StringUtil::WStringLineNum(const std::wstring& str)
{
	size_t count = 0;
	for (char c : str) {
		if (c == '\n') {
			count++;
		}
	}
	return static_cast<int>(count);
}

std::string StringUtil::WstringToString(const std::wstring& wstr)
{
	std::string ret;
	//一度目の呼び出しは文字列数を知るため
	auto result = WideCharToMultiByte(
		CP_ACP,
		0,
		wstr.c_str(),//入力文字列
		wstr.length(),
		nullptr,
		0,
		nullptr,
		nullptr);
	assert(result >= 0);
	ret.resize(result);//確保する
	//二度目の呼び出しは変換
	result = WideCharToMultiByte(
		CP_ACP,
		0,
		wstr.c_str(),//入力文字列
		wstr.length(),
		ret.data(),
		ret.size(),
		nullptr,
		nullptr);
	return ret;
}

std::wstring StringUtil::StringToWstring(const std::string& str)
{
	std::wstring ret;
	//一度目の呼び出しは文字列数を知るため
	auto result = MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),//入力文字列
		str.length(),
		nullptr,
		0);
	assert(result >= 0);
	ret.resize(result);//確保する
	//二度目の呼び出しは変換
	result = MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),//入力文字列
		str.length(),
		ret.data(),
		ret.size());
	return ret;
}

float StringUtil::GetTextWidth(const std::wstring text, int handle)
{
	return static_cast<float>(GetDrawNStringWidthToHandle(text.c_str(), wcslen(text.c_str()), handle));
}
