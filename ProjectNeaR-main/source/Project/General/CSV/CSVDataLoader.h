#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include <memory>
class CSVData;
class CSVDataLoader
{
private:
	//シングルトンの準備
	CSVDataLoader() = default;
	~CSVDataLoader() = default;
	//コピー禁止
	CSVDataLoader(const CSVDataLoader&) = delete;
	CSVDataLoader& operator = (const CSVDataLoader&) = delete;
	//ムーブ禁止
	CSVDataLoader(CSVDataLoader&&) = delete;
	CSVDataLoader& operator = (CSVDataLoader&&) = delete;
public:
	/// <summary>
	/// シングルトンオブジェクトを返す関数
	/// 参照を返すことで、複製を防ぐ
	/// 間違えて値を返した場合、コピーコンストラクタが走るので
	/// それをprivateにしておくことで弾くことができる
	/// </summary>
	/// <returns></returns>
	static CSVDataLoader& GetInstance();

    /// <summary>
    /// CSVファイルからオブジェクトのTranceform情報を取得
    /// </summary>
    /// <param name="fileName">csv</param>
    /// <returns></returns>
    std::vector<std::shared_ptr<CSVData>> LoadCSV(const wchar_t* path);
private:
    std::vector<std::vector<std::wstring>> GetWStringList(const wchar_t* path);
};