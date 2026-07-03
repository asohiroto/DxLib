#include <fstream>   // ファイル読み込み用
#include <sstream>   // 文字列分解用（stringstream）stringをファイルのように扱える
#include "CSVDataLoader.h"
#include "CSVData.h"
#include "ActorData.h"
#include "../StringUtil.h"
#include <codecvt>

namespace
{
	const std::wstring kPath = L"Data/CSV/";
	const std::wstring kCSV = L".csv";
}

CSVDataLoader& CSVDataLoader::GetInstance()
{
	//この時点でメモリが確保されてアプリ終了まで残る
	static CSVDataLoader csvDataLoader;
	return csvDataLoader;
}

std::vector<std::shared_ptr<CSVData>> CSVDataLoader::LoadCSV(const wchar_t* path)
{
	//データを格納する配列
	std::vector<std::shared_ptr<CSVData>> datas;

	//パス
	std::wstring pPath = kPath + path + kCSV;
	//データをすべて読み込む
	auto valuesDatas = GetWStringList(pPath.c_str());

	for (auto values : valuesDatas)
	{
		//データ
		std::shared_ptr<CSVData> data = std::make_shared<CSVData>();
		//データを入れる
		data->SetData(values);
		//配列に追加
		datas.emplace_back(data);
	}

	return datas;
}
std::vector<std::vector<std::wstring>> CSVDataLoader::GetWStringList(const wchar_t* path)
{
    std::vector<std::vector<std::wstring>> valuesDatas;

    //ファイルを開く
    std::ifstream file(path);

    // ファイルを開けなかった場合は空のリストを返す
    if (!file.is_open())
    {
        return valuesDatas;
    }

    //ファイル内容を一括で読み込み(UTF-8)
    std::string utf8Data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    //ファイルが空の場合は空のリストを返す
    if (utf8Data.empty()) {
        return valuesDatas;
    }

    //UTF-8からUTF-16 (wstring) に変換
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Data.c_str(), static_cast<int>(utf8Data.size()), nullptr, 0);
    std::wstring wdata(wideSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Data.c_str(), static_cast<int>(utf8Data.size()), &wdata[0], wideSize);

    //行ごとに処理
    std::wstringstream ws(wdata);
    std::wstring line;
    bool isHeader = false;
    std::vector<std::wstring> header;

    //CSVの終わりまで読み取る
    //getlineで1行ずつ読み取る
    while (std::getline(ws, line))
    {
        //行をカンマ区切りで1つずつ分解する準備
        std::wstringstream ss(line);
        std::wstring part;
        std::vector<std::wstring> values;

        //カンマ区切りで分解してリストに追加
        while (std::getline(ss, part, L','))
        {
            //改行処理(findで探してnposが返ってこなければ改行を挟む
            for (size_t i = 0; (i = part.find(L"\\n", i)) != std::wstring::npos; ++i)
            {
                part.replace(i, 2, L"\n");
            }

            values.emplace_back(part);
        }

        //ヘッダー
        if (!isHeader)
        {
            header = values;
            isHeader = true;
            continue;
        }

        //要素数チェック（ヘッダーより少ない場合は不正な行としてスキップ）
        if (values.size() < header.size()) continue;

        //データを配列に追加
        valuesDatas.emplace_back(values);
    }

    //暗黙ムーブが走るのでコピーコストは問題なし
    return valuesDatas;
}
