#pragma once
#include <string>
enum class StageIndex
{
	Stage1,
	Stage2,
	Stage3,
	Tutorial1,
	Tutorial2,
	Tutorial3,
};

inline std::wstring GetStageNameByIndex(StageIndex index)
{
	std::wstring name;
	switch (index)
	{
	case StageIndex::Stage1:
		name = L"Stage1";
		break;
	case StageIndex::Stage2:
		name = L"Stage2";
		break;
	case StageIndex::Stage3:
		name = L"Stage3";
		break;
	case StageIndex::Tutorial1:
		name = L"Tutorial1";
		break;
	case StageIndex::Tutorial2:
		name = L"Tutorial2";
		break;
	case StageIndex::Tutorial3:
		name = L"Tutorial3";
		break;
	default:
		name = L"Stage1";
		break;
	}
	return name;
}

inline StageIndex GetStageIndexByName(const std::wstring stageName)
{
	StageIndex index;
	if (stageName == L"Stage1")
	{
		index = StageIndex::Stage1;
	}
	else if (stageName == L"Stage2")
	{
		index = StageIndex::Stage2;
	}
	else if (stageName == L"Stage3")
	{
		index = StageIndex::Stage3;
	}
	else if (stageName == L"Tutorial1")
	{
		index = StageIndex::Tutorial1;
	}
	else if (stageName == L"Tutorial2")
	{
		index = StageIndex::Tutorial2;
	}
	else if (stageName == L"Tutorial3")
	{
		index = StageIndex::Tutorial3;
	}
	else
	{
		index = StageIndex::Stage1;
	}

	return index;
}