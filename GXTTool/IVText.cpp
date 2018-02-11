#include <windows.h>
#include "IVText.h"
#include "BinaryFile.hpp"

using namespace std;
using namespace std::experimental::filesystem::v1;

void IVText::Process0Arg()
{
	wchar_t temp[512];

	HMODULE	self = GetModuleHandleW(NULL);
	GetModuleFileNameW(self, temp, 512);

	path text_path = temp;
	text_path = text_path.parent_path();

	LoadText(text_path / "GTA4.txt");
	GenerateBinary(text_path / "chinese.gxt");
	GenerateCollection(text_path / "characters.txt");
	GenerateTable(text_path / "table.dat");
}

void IVText::Process2Args(const std::experimental::filesystem::v1::path &arg1, const std::experimental::filesystem::v1::path &arg2)
{
	if (is_directory(arg1))
	{
		create_directories(arg2);
		LoadTexts(arg1);
		GenerateBinary(arg2 / "chinese.gxt");
		GenerateCollection(arg2 / "characters.txt");
		GenerateTable(arg2 / "table.dat");
	}
	else if (is_regular_file(arg1))
	{
		create_directories(arg2);
		LoadBinary(arg1);
		GenerateTexts(arg2);
	}
}

void IVText::SkipUTF8Signature(std::ifstream &stream)
{
	char bom[3];

	if (stream.get(bom[0]) && stream.get(bom[1]) && stream.get(bom[2]))
	{
		if (bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')
		{
			return;
		}
	}

	stream.seekg(0);
}

void IVText::AddUTF8Signature(std::ofstream &stream)
{
	stream << "\xEF\xBB\xBF";
}

IVText::tWideString IVText::ConvertToWideString(const std::string &in)
{
	tWideString result;
	utf8::utf8to16(in.begin(), in.end(), back_inserter(result));
	return result;
}

std::string IVText::ConvertToNarrow(const tWideString &in)
{
	std::string result;
	utf8::utf16to8(in.begin(), in.end(), back_inserter(result));
	return result;
}

bool IVText::IsNativeCharacter(uint16_t character)
{
	return (character < 0x80 ||
		character == 0xA9 ||
		character == 0xAC ||
		character == 0xAE ||
		character == 0xC1 ||
		character == 0xC9 ||
		character == 0xD6 ||
		character == 0xDC ||
		character == 0xE0 ||
		character == 0xE1 ||
		character == 0xE7 ||
		character == 0xE8 ||
		character == 0xE9 ||
		character == 0xEA ||
		character == 0xED ||
		character == 0xEF ||
		character == 0xF1 ||
		character == 0xF3 ||
		character == 0xF5 ||
		character == 0xFA ||
		character == 0xFC ||
		character == 0x2122);
}

void IVText::CollectCharacters(const std::string & text)
{
	utf8::iterator<std::string::const_iterator> u8it(text.begin(), text.begin(), text.end());

	while (u8it.base() != text.end())
	{
		uint16_t character = *u8it;

		if (!IsNativeCharacter(character))
		{
			m_Collection.insert(character);
		}

		++u8it;
	}
}

void IVText::LoadText(const std::experimental::filesystem::v1::path &input_text)
{
	regex table_regex(R"(\[([0-9a-zA-Z_]{1,7})\])");
	regex entry_regex(R"((0[xX][0-9a-fA-F]{8})=(.+))");

	smatch matches;

	string line;

	map<string, vector<tEntry>, IVTextTableSorting>::iterator table_iter = m_Data.end();

	size_t line_no = 0;

	std::ifstream stream(input_text);

	if (!stream)
	{
		cout << "打开文件 " << input_text.string() << " 失败。" << endl;
		return;
	}

	SkipUTF8Signature(stream);

	while (getline(stream, line))
	{
		++line_no;

		if (line.empty() || line.front() == ';')
		{

		}
		else if (line.front() == '0' && regex_match(line, matches, entry_regex))
		{
			if (table_iter != m_Data.end())
			{
				uint32_t hash = stoull(matches.str(1), nullptr, 16);
				string text = matches.str(2);

				CollectCharacters(text);

				table_iter->second.push_back(make_pair(hash, text));
			}
			else
			{
				cout << "第" << line_no << "行没有所属的表。" << endl;
			}

		}
		else if (line.front() == '[' && regex_match(line, matches, table_regex))
		{
			string table_name = matches.str(1);

			tTable temp;
			temp.first = table_name;
			table_iter = m_Data.insert(temp).first;
		}
		else
		{
			cout << "第" << line_no << "行无法识别。" << endl;
		}
	}
}

void IVText::LoadTexts(const std::experimental::filesystem::v1::path &input_texts)
{
	directory_iterator dir_it{ input_texts };

	m_Data.clear();
	m_Collection.clear();

	while (dir_it != directory_iterator{})
	{
		path filename = dir_it->path();

		LoadText(filename);

		++dir_it;
	}
}

void IVText::GenerateBinary(const std::experimental::filesystem::v1::path & output_binary) const
{
	BinaryFile file(output_binary, BinaryFile::OpenMode::WriteOnly);

	TABLBlockHeader TablHeader;

	if (!file)
	{
		cout << "创建输出文件 " << output_binary.string() << " 失败。" << endl;
		return;
	}

	file.Write(0x100004i32);

	TablHeader.Name[0] = 'T';
	TablHeader.Name[1] = 'A';
	TablHeader.Name[2] = 'B';
	TablHeader.Name[3] = 'L';
	TablHeader.Size = m_Data.size() * sizeof(TABLEntry);
	file.Write(TablHeader);

	for (auto &table : m_Data)
	{

	}
}

void IVText::GenerateCollection(const std::experimental::filesystem::v1::path & output_text) const
{
	vector<char> sequence;

	size_t count = 0;

	for (auto char_it = m_Collection.begin(); char_it != m_Collection.end(); ++char_it)
	{
		if (count == MaxColumns)
		{
			sequence.push_back('\n');
			count = 0;
		}

		utf8::append(*char_it, back_inserter(sequence));
		++count;
	}

	ofstream stream(output_text, ios::trunc);

	AddUTF8Signature(stream);

	copy(sequence.begin(), sequence.end(), ostreambuf_iterator<char>(stream));
}

void IVText::GenerateTable(const std::experimental::filesystem::v1::path & output_binary) const
{
	vector<pair<uint8_t, uint8_t>> data;

	uint8_t row = 0, colunm = 0;

	data.resize(0x10000, { MaxRows - 1, MaxColumns - 1 });

	for (auto character : m_Collection)
	{
		if (colunm == MaxColumns)
		{
			++row;
			colunm = 0;
		}

		data[character] = { row,colunm };

		++colunm;
	}

	BinaryFile stream(output_binary, BinaryFile::OpenMode::WriteOnly);

	stream.Write(data.data(), 0x20000);
}

void IVText::FixCharacters(tWideString &wtext)
{
	//bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
	//bad character in EFLC stock text: 0x93

	for (auto &character : wtext)
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

		default:
			break;
		}
	}
}

void IVText::LiteralToGame(tWideString & wtext)
{
	for (auto &character : wtext)
	{
		switch (character)
		{
		case L'™':
			character = 0x99;
			break;

		default:
			break;
		}
	}
}

void IVText::GameToLiteral(tWideString & wtext)
{
	for (auto &character : wtext)
	{
		switch (character)
		{
		case 0x99:
			character = L'™';
			break;

		default:
			break;
		}
	}
}

void IVText::LoadBinary(const std::experimental::filesystem::v1::path & input_binary)
{
	GXTHeader GxtHeader;
	TABLBlockHeader TablHeader;
	TKEYBlockHeader TkeyHeader;
	TDATBlockHeader TdatHeader;

	vector<TABLEntry> TablBlock;
	vector<TKEYEntry> TkeyBlock;
	vector<char> TdatBlock;

	m_Data.clear();
	m_Collection.clear();

	std::map<std::string, std::vector<tEntry>, IVTextTableSorting>::iterator current_table = m_Data.end();

	BinaryFile file(input_binary, BinaryFile::OpenMode::ReadOnly);

	if (!file)
	{
		cout << "打开输入文件 " << input_binary.string() << " 失败。" << endl;
		return;
	}

	file.Read(GxtHeader);

	file.Read(TablHeader);

	TablBlock.resize(TablHeader.Size / sizeof(TABLEntry));
	file.Read(TablBlock.data(), TablHeader.Size);

	for (TABLEntry &table : TablBlock)
	{
		current_table = m_Data.insert(tTable(table.Name, vector<tEntry>())).first;

		file.Seek(table.Offset, BinaryFile::SeekMode::Begin);

		if (strcmp(table.Name, "MAIN") != 0)
		{
			file.Seek(8, BinaryFile::SeekMode::Current);
		}

		file.Read(TkeyHeader);

		TkeyBlock.resize(TkeyHeader.Size / sizeof(TKEYEntry));
		file.Read(TkeyBlock.data(), TkeyHeader.Size);

		file.Read(TdatHeader);
		TdatBlock.resize(TdatHeader.Size);
		file.Read(TdatBlock.data(), TdatHeader.Size);

		for (auto &key : TkeyBlock)
		{
			tWideString wtext = (uint16_t *)(&TdatBlock[key.Offset]);

			FixCharacters(wtext);
			GameToLiteral(wtext);

			current_table->second.push_back(make_pair(key.Hash, ConvertToNarrow(wtext)));
		}
	}
}

void IVText::GenerateTexts(const std::experimental::filesystem::v1::path & output_texts) const
{
	ofstream stream;

	for (auto &table : m_Data)
	{
		stream.open(output_texts / (table.first + ".txt"), ios::trunc);

		if (!stream)
		{
			cout << "创建输出文件失败" << endl;
		}

		AddUTF8Signature(stream);

		stream << '[' << table.first << ']' << '\n';

		stream << hex << uppercase;
		stream.precision(8);

		for (auto &entry : table.second)
		{
			stream << ';' << "0x" << entry.first << '=' << entry.second << '\n';
			stream << "0x" << entry.first << '=' << entry.second << '\n' << '\n';
		}

		stream.close();
	}
}
