#include "IVGXT.h"
#include "uint32_hex.h"
#include "utf8cpp/utf8.h"
#include "binary_file.hpp"

#include <windows.h>

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <filesystem>
#include <array>
#include <regex>
#include <algorithm>
#include <iterator>
#include <memory>
#include <exception>

using namespace std;
using namespace std::experimental::filesystem::v1;

void IVGXT::load_txt(const path &text_path)
{
	ifstream text_stream(text_path);

	regex table_format(R"-(\[([0-9A-Za-z_]{1,7})\])-");
	regex entry_format(R"-((0[xX][0-9a-fA-F]{8})=(.+))-");

	smatch match_result;

	string table_name;

	uint32_t hash;
	array<char, 11> hash_string;

	string normal_text;
	vector<uint16_t> widen_text;

	map<string, vector<pair<uint32_t, pair<string, vector<uint16_t>>>>, TABLE_SORT>::iterator table_iter;

	string line_buffer;

	size_t line_no = 0;

	if (!text_stream)
	{
		throw exception("打开输入文件失败。");
	}

	table_iter = m_data.end();

	skip_utf8_signature(text_stream);

	while (getline(text_stream, line_buffer))
	{
		if (line_buffer.empty() || line_buffer.front() == ';')
		{
			continue;
		}

		if (regex_match(line_buffer, match_result, entry_format))
		{
			if (table_iter == m_data.end())
			{
				throw exception("文本没有以表名开头。");
			}

			hash_string.fill(0);
			copy(match_result[1].first, match_result[1].second, hash_string.begin());

			uint32_hex::from_hex(hash_string, hash);

			normal_text.assign(match_result[2].first, match_result[2].second);

			widen_text = to_wide(normal_text);

			collect_wide_chars(widen_text);

			transform_characters(widen_text);

			table_iter->second.push_back(make_pair(hash, make_pair(normal_text, widen_text)));
		}
		else if (regex_match(line_buffer, match_result, table_format))
		{
			table_name.assign(match_result[1].first, match_result[1].second);

			table_iter = m_data.insert(make_pair(table_name, vector<pair<uint32_t, pair<string, vector<uint16_t>>>>())).first;
		}
		else
		{
			cout << "第" << line_no << "行不能识别。" << endl;
		}

		++line_no;
	}
}

void IVGXT::load_txts(const std::experimental::filesystem::v1::path &input_texts)
{
	recursive_directory_iterator dirit{ input_texts };

	m_data.clear();
	m_collection.clear();

	while (dirit != recursive_directory_iterator{})
	{
		path temp = dirit->path();

		if (temp.extension() == ".txt")
		{
			load_txt(temp);
		}

		++dirit;
	}
}

void IVGXT::load_gxt(const path &binary_path)
{
	GXT_HEADER gxtheader;
	TABL_HEADER tablheader;
	TKEY_HEADER_MAIN tkeyheadermain;
	TKEY_HEADER tkeyheader;
	TDAT_HEADER tdatheader;

	vector<TABL_ENTRY> tabl_block;
	vector<TKEY_ENTRY>tkey_block;
	vector<std::uint16_t> tdat_block;

	vector<pair<uint32_t, pair<string, vector<uint16_t>>>> temp_table_data;

	vector<uint16_t> raw_text;
	string plain_text;

	binary_file input_file(binary_path, ios::in);

	if (!input_file)
	{
		throw exception("打开输入文件失败。");
	}

	m_data.clear();

	input_file.read(gxtheader);

	if (!gxtheader.valid())
	{
		throw exception("GXT文件损坏。");
	}

	input_file.read(tablheader);

	if (!tablheader.valid())
	{
		throw exception("GXT文件损坏。");
	}

	tabl_block.resize(tablheader.count());

	input_file.read((char *)tabl_block.data(), tablheader.size());

	for (auto &tablentry : tabl_block)
	{
		input_file.seekg(tablentry.m_offset);

		if (strcmp(tablentry.m_name, "MAIN") == 0)
		{
			input_file.read(tkeyheadermain);

			if (!tkeyheadermain.valid())
			{
				throw exception("GXT文件损坏。");
			}

			tkey_block.resize(tkeyheadermain.count());

			input_file.read((char *)tkey_block.data(), tkeyheadermain.size());

		}
		else
		{
			input_file.read(tkeyheader);

			if (!tkeyheader.valid(tablentry.m_name))
			{
				throw exception("GXT文件损坏。");
			}

			tkey_block.resize(tkeyheader.count());

			input_file.read((char *)tkey_block.data(), tkeyheader.size());
		}

		input_file.read(tdatheader);

		if (!tdatheader.valid())
		{
			throw exception("GXT文件损坏。");
		}

		tdat_block.resize(tdatheader.size() / 2);

		input_file.read(tdat_block.data(), tdatheader.size());

		temp_table_data.clear();

		for (auto &tkeyentry : tkey_block)
		{
			uint16_t *pStart = reinterpret_cast<uint16_t *>((&tdat_block[tkeyentry.m_offset / 2]));
			uint16_t *pEnd = find(pStart, &tdat_block.back() + 1, 0);

			raw_text.assign(pStart, pEnd);

			transform_characters(raw_text);

			plain_text = to_narrow(raw_text);

			temp_table_data.push_back(make_pair(tkeyentry.m_hash, make_pair(plain_text, raw_text)));
		}

		m_data[tablentry.m_name] = temp_table_data;
	}
}

void IVGXT::generate_txts(const path &output_text)
{
	array<char, 11> hex_string;
	
	ofstream text_stream;

	for (auto &table : m_data)
	{
		text_stream.open(output_text / (table.first + ".txt"), ios::trunc);

		if (!text_stream)
		{
			throw exception("创建输出文件失败");
		}

		add_utf8_signature(text_stream);

		text_stream << '[' << table.first << ']' << '\n';

		for (auto &entry : table.second)
		{
			uint32_hex::to_hex(entry.first, hex_string);

			text_stream << ';' << hex_string.data() << '=' << entry.second.first << '\n';
			text_stream << hex_string.data() << '=' << entry.second.first << '\n' << '\n';
		}

		text_stream.close();
	}
}

void IVGXT::generate_gxt(const path &output_binary)
{
	int32_t foTableBlock, foKeyBlock, foDataBlock;
	int32_t tableBlockSize, keyBlockOffset, keyBlockSize, TDATOffset, dataOffset, dataBlockSize;

	tableBlockSize = this->m_data.size() * 12;

	binary_file binary_stream(output_binary, ios::out | ios::trunc);

	if (!binary_stream)
	{
		throw exception("创建输出文件失败");
	}

	binary_stream.write("\x04\x00\x10\x00", 4);
	binary_stream.write("TABL", 4);
	binary_stream.write(tableBlockSize);

	foTableBlock = 12;
	foKeyBlock = tableBlockSize + 12;
	keyBlockOffset = foKeyBlock;

	for (auto &table : this->m_data)
	{
		keyBlockSize = table.second.size() * 8;
		dataBlockSize = get_data_size(table.second);
		binary_stream.seekp(foTableBlock);
		binary_stream.write_container(table.first);
		binary_stream.write(keyBlockOffset);
		foTableBlock += 12;
		binary_stream.seekp(foKeyBlock);

		if (table.first != "MAIN")
		{
			array<char, 8> temp;
			temp.fill(0);
			table.first.copy(temp.data(), 7, 0);
			binary_stream.write_container(temp);
		}

		binary_stream.write("TKEY", 4);
		binary_stream.write(keyBlockSize);
		foKeyBlock = binary_stream.tellp();
		binary_stream.seekp(keyBlockSize, ios::cur);
		TDATOffset = binary_stream.tellp();
		binary_stream.write("TDAT", 4);
		binary_stream.write(dataBlockSize);
		foDataBlock = binary_stream.tellp();

		for (auto &entry : table.second)
		{
			dataOffset = foDataBlock - TDATOffset - 8;
			binary_stream.seekp(foKeyBlock);
			binary_stream.write(dataOffset);
			binary_stream.write(entry.first);
			foKeyBlock += 8;
			binary_stream.seekp(foDataBlock);
			binary_stream.write_container(entry.second.second);
			binary_stream.write(0ui16);
			foDataBlock = binary_stream.tellp();
		}

		foKeyBlock = binary_stream.tellp();
		keyBlockOffset = foKeyBlock;
	}
}

vector<uint16_t> IVGXT::to_wide(const string &input)
{
	vector<uint16_t> result;
	utf8::utf8to16(input.begin(), input.end(), back_inserter(result));
	return result;
}

string IVGXT::to_narrow(const vector<uint16_t> &input)
{
	string result;
	utf8::utf16to8(input.begin(), input.end(), back_inserter(result));
	return result;
}

size_t IVGXT::get_data_size(const vector<pair<uint32_t, pair<string, vector<uint16_t>>>> &table)
{
	size_t result = 0;

	for (auto &key : table)
	{
		result += (key.second.second.size() * 2 + 2);
	}

	return result;
}

void IVGXT::skip_utf8_signature(ifstream &text_stream)
{
	char header[3];

	text_stream.seekg(0);

	if (text_stream.get(header[0]) && text_stream.get(header[1]) && text_stream.get(header[2]))
	{
		if (header[0] == '\xEF' && header[1] == '\xBB' && header[2] == '\xBF')
		{
			return;
		}
	}

	text_stream.seekg(0);
}

void IVGXT::add_utf8_signature(ofstream &text_stream)
{
	text_stream << "\xEF\xBB\xBF";
}

void IVGXT::collect_wide_chars(const vector<uint16_t> &wide_text)
{
	for (uint16_t wchar : wide_text)
	{
		if (wchar <  0x80 ||
			wchar == 0xA9 ||
			wchar == 0xAC ||
			wchar == 0xAE ||
			wchar == 0xC1 ||
			wchar == 0xC9 ||
			wchar == 0xD6 ||
			wchar == 0xDC ||
			wchar == 0xE0 ||
			wchar == 0xE1 ||
			wchar == 0xE7 ||
			wchar == 0xE8 ||
			wchar == 0xE9 ||
			wchar == 0xEA ||
			wchar == 0xED ||
			wchar == 0xEF ||
			wchar == 0xF1 ||
			wchar == 0xF3 ||
			wchar == 0xF5 ||
			wchar == 0xFA ||
			wchar == 0xFC ||
			wchar == 0x2122)
		{
			continue;
		}

		m_collection.insert(wchar);
	}
}

void IVGXT::transform_characters(vector<uint16_t> &input)
{
	//bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
	//bad character in EFLC stock text: 0x93

	for (uint16_t &character : input)
	{
		switch (character)
		{

		case 0x85:
			character = L' ';
			break;

		case 0x92:
		case 0x94:
			character = L'\'';
			break;

		case 0x93: //EFLC
			break;

		case 0x96:
			character = L'-';
			break;

		case 0x97:
		case 0xA0:
			character = L' ';
			break;

		case 0x99:
			character = L'™';
			break;

		case L'™':
			character = 0x99;
			break;

		default:
			break;
		}
	}
}

void IVGXT::generate_collection(const path &output_collection)
{
	binary_file output_stream(output_collection, ios::out);

	if (!output_stream)
	{
		throw std::exception("创建输出文件失败。");
	}

	unsigned char curcolumn = 0;

	output_stream.write(0xFEFFui16);

	for (uint16_t character : this->m_collection)
	{
		output_stream.write(character);

		curcolumn += 1;

		if (curcolumn > 63)
		{
			output_stream.write(L'\n');

			curcolumn = 0;
		}
	}
}

void IVGXT::generate_table(const path &table_path)
{
	struct CharPos
	{
		unsigned char row, column;
	};

	unsigned char currow = 0;
	unsigned char curcolumn = 0;

	binary_file output_stream(table_path, ios::out);

	if (!output_stream)
	{
		throw std::exception("创建输出文件失败。");
	}

	vector<CharPos> Table;
	Table.resize(0x10000, CharPos{ 50, 63 });

	for (uint16_t character : this->m_collection)
	{
		Table[character].row = currow;
		Table[character].column = curcolumn;
		
		if (curcolumn < 63)
		{
			++curcolumn;
		}
		else
		{
			++currow;

			curcolumn = 0;
		}
	}

	output_stream.write_container(Table);
}

void IVGXT::process0arg()
{
	wchar_t wide_path[512];
	path input_filename;

	HMODULE	self = GetModuleHandleA(NULL);
	GetModuleFileNameW(self, wide_path, 512);

	input_filename = wide_path;
	input_filename = input_filename.parent_path();

	load_txt(input_filename / "GTA4.txt");
	generate_gxt(input_filename / "chinese.gxt");
	generate_table(input_filename / "table.dat");
	generate_collection(input_filename / "characters.txt");
}

void IVGXT::process2args(const path & input_string, const path & output_string)
{
	if (is_directory(input_string))
	{
		load_txts(input_string);
		create_directories(output_string);
		generate_gxt(output_string / "chinese.gxt");
		generate_table(output_string / "table.dat");
		generate_collection(output_string / "characters.txt");
	}
	else if (is_regular_file(input_string))
	{
		load_gxt(input_string);
		create_directories(output_string);
		generate_txts(output_string);
	}
	else
	{
		throw std::exception("无法识别的参数。");
	}
}
