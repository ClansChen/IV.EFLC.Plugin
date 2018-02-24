#pragma once

#include <iostream>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <cstring>
#include <string>
#include <set>
#include <vector>
#include <array>
#include <regex>
#include <utility>
#include <map>
#include <algorithm>
#include <regex>
#include <fstream>
#include <iterator>
#include "utf8cpp/utf8.h"

struct IVTextTableSorting 
{
	bool operator()(const std::string &lhs, const std::string &rhs) const
	{
		if (rhs == "MAIN")
		{
			return false;
		}
		else if (lhs == "MAIN")
		{
			return true;
		}
		else
		{
			return lhs < rhs;
		}
	}
};

struct GXTHeader
{
	std::int32_t Signature;
};

struct TABLHeader
{
	char TABL[4];
	std::int32_t Size;
};

struct TABLEntry
{
	char Name[8];
	std::int32_t Offset;
};

struct TKEYHeaderMAIN
{
	char TKEY[4];
	std::int32_t Size;
};

struct TKEYHeader
{
	char Name[8];
	TKEYHeaderMAIN Header;
};

struct TKEYEntry
{
	std::int32_t Offset;
	std::uint32_t Hash;
};

struct TDATHeader
{
	char TDAT[4];
	std::int32_t Size;
};

class IVText
{
public:
	typedef std::uint32_t tHash;
	typedef std::basic_string<std::uint16_t> tWideString;
	typedef std::pair<tHash, std::string> tEntry;
	typedef std::pair<std::string, std::vector<tEntry>> tTable;

	//不带参数，读取同一目录下GTA4.txt生成汉化补丁相关文件
	void Process0Arg();

	//文件, 目录，读取GXT文件并存放在指定目录下
	//目录, 目录，读取文件夹内所有txt生成汉化补丁相关文件
	void Process2Args(const std::experimental::filesystem::v1::path &arg1, const std::experimental::filesystem::v1::path &arg2);

private:
	static void SkipUTF8Signature(std::ifstream &stream);
	static void AddUTF8Signature(std::ofstream &stream);

	static tWideString ConvertToWide(const std::string &in);
	static std::string ConvertToNarrow(const tWideString &in);

	static bool IsNativeCharacter(uint16_t character);
	void CollectCharacters(const std::string &text);

	void LoadText(const std::experimental::filesystem::v1::path &input_text);
	void LoadTexts(const std::experimental::filesystem::v1::path &input_texts);

	void GenerateBinary(const std::experimental::filesystem::v1::path &output_binary) const;
	void GenerateCollection(const std::experimental::filesystem::v1::path &output_text) const;
	void GenerateTable(const std::experimental::filesystem::v1::path &output_binary) const;

	//修正原版GXT中的混乱字符
	static void FixCharacters(tWideString &wtext);

	//标准编码转到游戏编码
	static void LiteralToGame(tWideString &wtext);

	//游戏编码转到标准编码
	static void GameToLiteral(tWideString &wtext);

	void LoadBinary(const std::experimental::filesystem::v1::path &input_binary);

	void GenerateTexts(const std::experimental::filesystem::v1::path &output_texts) const;

	std::map<std::string, std::vector<tEntry>, IVTextTableSorting> m_Data;
	std::set<uint16_t> m_Collection;
};
