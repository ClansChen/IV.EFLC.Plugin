#pragma once

class binary_file;

#include <cstdint>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <array>
#include <set>
#include <utility>

struct TABL_ENTRY
{
	char m_name[8];
	std::int32_t m_offset;
};

struct TKEY_ENTRY
{
	std::int32_t m_offset;
	std::uint32_t m_hash;
};

struct GXT_HEADER
{
	std::int32_t m_magic;

	bool valid() const
	{
		return (m_magic == 0x100004);
	}
};

struct TABL_HEADER
{
	char m_tabl[4];
	std::int32_t m_size;

	bool valid() const
	{
		return (m_tabl[0] == 'T' && m_tabl[1] == 'A' && m_tabl[2] == 'B' && m_tabl[3] == 'L');
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(TABL_ENTRY));
	}
};

struct TKEY_HEADER
{
	char m_name[8];
	char m_tkey[4];
	std::int32_t m_size;

	bool valid(const char *name) const
	{
		return (m_tkey[0] == 'T' && m_tkey[1] == 'K' && m_tkey[2] == 'E' && m_tkey[3] == 'Y' && std::strncmp(m_name, name, 8) == 0);
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(TKEY_ENTRY));
	}
};

struct TKEY_HEADER_MAIN
{
	char m_tkey[4];
	std::int32_t m_size;

	bool valid() const
	{
		return (m_tkey[0] == 'T' && m_tkey[1] == 'K' && m_tkey[2] == 'E' && m_tkey[3] == 'Y');
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(TKEY_ENTRY));
	}
};

struct TDAT_HEADER
{
	char m_tdat[4];
	std::int32_t m_size;

	bool valid() const
	{
		return (m_tdat[0] == 'T' && m_tdat[1] == 'D' && m_tdat[2] == 'A' && m_tdat[3] == 'T');
	}

	std::int32_t size() const
	{
		return m_size;
	}

	std::int32_t count() const
	{
		return (m_size / sizeof(wchar_t));
	}
};

struct TABLE_SORT
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
			return (lhs < rhs);
		}
	}
};

class IVGXT
{
public:
	//处理命令行参数
	void process0arg();
	void process2args(const std::experimental::filesystem::v1::path &input_string, const std::experimental::filesystem::v1::path &output_string);

private:
	//加载单个文本
	void load_txt(const std::experimental::filesystem::v1::path &input_text);
	//加载多个文本
	void load_txts(const std::experimental::filesystem::v1::path &input_texts);

	//生成GXT
	void generate_gxt(const std::experimental::filesystem::v1::path &output_binary);

	//加载gxt
	void load_gxt(const std::experimental::filesystem::v1::path &input_binary);

	//生成多个文本
	void generate_txts(const std::experimental::filesystem::v1::path &output_texts);

	//GXT编码，标准编码互转
	static void transform_characters(std::vector<uint16_t> &input);
	
	//转码函数
	static std::vector<uint16_t> to_wide(const std::string &input);
	static std::string to_narrow(const std::vector<uint16_t> &input);

	//处理UTF8签名
	static void skip_utf8_signature(std::ifstream &text_stream);
	static void add_utf8_signature(std::ofstream &text_stream);

	//提取非原版字符
	void collect_wide_chars(const std::vector<uint16_t> &wide_text);
	
	//计算Data block字节数
	static std::size_t get_data_size(const std::vector<std::pair<std::uint32_t, std::pair<std::string, std::vector<uint16_t>>>> &table);

	//生成汉化补丁相关文件
	void generate_collection(const std::experimental::filesystem::v1::path &output_collection);
	void generate_table(const std::experimental::filesystem::v1::path &output_table);

//------------------------------------------------------------------------------------------------------------------------------------------------
	//GXT数据表
	std::map<std::string, std::vector<std::pair<std::uint32_t, std::pair<std::string, std::vector<std::uint16_t>>>>, TABLE_SORT> m_data;

	//非原版字符集合
	std::set<std::uint16_t> m_collection;
};
