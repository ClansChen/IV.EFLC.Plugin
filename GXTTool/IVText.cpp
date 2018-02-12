#include <windows.h>
#include "IVText.h"
#include "BinaryFile.hpp"

using namespace std;
using namespace experimental::filesystem::v1;

void IVText::Process0Arg()
{
	wchar_t temp[512];

	HMODULE	self = GetModuleHandleW(NULL);
	GetModuleFileNameW(self, temp, 512);

	path text_path = temp;
	text_path = text_path.parent_path();

	m_Data.clear();
	m_Collection.clear();

	text_path = "D:/";

	LoadText(text_path / "GTA4.txt");
	GenerateBinary(text_path / "chinese.gxt");
	GenerateCollection(text_path / "characters.txt");
	GenerateTable(text_path / "table.dat");
}

void IVText::Process2Args(const path &arg1, const path &arg2)
{
	m_Data.clear();
	m_Collection.clear();

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

void IVText::SkipUTF8Signature(ifstream &stream)
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

void IVText::AddUTF8Signature(ofstream &stream)
{
	stream << "\xEF\xBB\xBF";
}

IVText::tWideString IVText::ConvertToWide(const string &in)
{
	tWideString result;
	utf8::utf8to16(in.begin(), in.end(), back_inserter(result));
	return result;
}

string IVText::ConvertToNarrow(const tWideString &in)
{
	string result;
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

void IVText::CollectCharacters(const string & text)
{
	utf8::iterator<string::const_iterator> u8it(text.begin(), text.begin(), text.end());

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

void IVText::LoadText(const path &input_text)
{
	regex table_regex(R"(\[([0-9a-zA-Z_]{1,7})\])");
	regex entry_regex(R"((0[xX][0-9a-fA-F]{8})=(.+))");

	smatch matches;

	string line;

	map<string, vector<tEntry>, IVTextTableSorting>::iterator table_iter = m_Data.end();

	size_t line_no = 0;

	ifstream stream(input_text);

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

void IVText::LoadTexts(const path &input_texts)
{
	directory_iterator dir_it{ input_texts };

	while (dir_it != directory_iterator{})
	{
		path filename = dir_it->path();

		LoadText(filename);

		++dir_it;
	}
}

void IVText::GenerateBinary(const path & output_binary) const
{
	BinaryFile file(output_binary, BinaryFile::OpenMode::WriteOnly);

	long long writePostion;

	GXTHeader gxtHeader;

	TABLHeader tablHeader;
	TKEYHeader tkeyHeader;
	TDATHeader tdatHeader;

	TABLEntry tablEntry;
	TKEYEntry tkeyEntry;

	vector<TABLEntry> tablBlock;
	vector<TKEYEntry> tkeyBlock;
	vector<uint16_t> tdatBlock;

	tWideString wideText;

	if (!file)
	{
		cout << "创建输出文件 " << output_binary.string() << " 失败。" << endl;
		return;
	}

	gxtHeader.Signature = 0x100004;
	file.Write(gxtHeader);

	tablHeader.TABL[0] = 'T';
	tablHeader.TABL[1] = 'A';
	tablHeader.TABL[2] = 'B';
	tablHeader.TABL[3] = 'L';

	tkeyHeader.Header.TKEY[0] = 'T';
	tkeyHeader.Header.TKEY[1] = 'K';
	tkeyHeader.Header.TKEY[2] = 'E';
	tkeyHeader.Header.TKEY[3] = 'Y';

	tdatHeader.TDAT[0] = 'T';
	tdatHeader.TDAT[1] = 'D';
	tdatHeader.TDAT[2] = 'A';
	tdatHeader.TDAT[3] = 'T';

	tablHeader.Size = m_Data.size() * sizeof(TABLEntry);
	file.Write(tablHeader);

	writePostion = 4 + sizeof(TABLHeader) + tablHeader.Size;

	tablBlock.clear();

	for (auto &table : m_Data)
	{
		tkeyBlock.clear();
		tdatBlock.clear();

		strcpy(tablEntry.Name, table.first.c_str());
		tablEntry.Offset = writePostion;
		tablBlock.push_back(tablEntry);

		strcpy(tkeyHeader.Name, table.first.c_str());
		tkeyHeader.Header.Size = table.second.size() * sizeof(TKEYEntry);

		for (auto &key : table.second)
		{
			tkeyEntry.Hash = key.first;
			tkeyEntry.Offset = tdatBlock.size() * 2;

			wideText = ConvertToWide(key.second);
			LiteralToGame(wideText);

			copy(wideText.begin(), wideText.end(), back_inserter(tdatBlock));
			tdatBlock.push_back(0);

			tkeyBlock.push_back(tkeyEntry);
		}

		tdatHeader.Size = tdatBlock.size() * 2;

		file.Seek(writePostion, BinaryFile::SeekMode::Begin);

		if (table.first == "MAIN")
		{
			file.Write(tkeyHeader.Header);
		}
		else
		{
			file.Write(tkeyHeader);
		}

		file.WriteArray(tkeyBlock);
		file.Write(tdatHeader);
		file.WriteArray(tdatBlock);

		writePostion = file.Tell();
	}

	file.Seek(4 + sizeof(TABLHeader), BinaryFile::SeekMode::Begin);
	file.WriteArray(tablBlock);
}

void IVText::GenerateCollection(const path & output_text) const
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

void IVText::GenerateTable(const path & output_binary) const
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

		data[character] = { row, colunm };

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

void IVText::LoadBinary(const path & input_binary)
{
	GXTHeader gxtHeader;
	TABLHeader tablHeader;
	TKEYHeader tkeyHeader;
	TDATHeader tdatHeader;

	vector<TABLEntry> tablBlock;
	vector<TKEYEntry> tkeyBlock;
	vector<char> tdatBlock;

	m_Data.clear();
	m_Collection.clear();

	map<string, vector<tEntry>, IVTextTableSorting>::iterator tableIter = m_Data.end();

	BinaryFile file(input_binary, BinaryFile::OpenMode::ReadOnly);

	if (!file)
	{
		cout << "打开输入文件 " << input_binary.string() << " 失败。" << endl;
		return;
	}

	file.Read(gxtHeader);

	file.Read(tablHeader);

	file.ReadArray(tablHeader.Size / sizeof(TABLEntry), tablBlock);

	for (TABLEntry &table : tablBlock)
	{
		tableIter = m_Data.insert(tTable(table.Name, vector<tEntry>())).first;

		file.Seek(table.Offset, BinaryFile::SeekMode::Begin);

		if (strcmp(table.Name, "MAIN") != 0)
		{
			file.Read(tkeyHeader);
		}
		else
		{
			file.Read(tkeyHeader.Header);
		}

		file.ReadArray(tkeyHeader.Header.Size / sizeof(TKEYEntry), tkeyBlock);

		file.Read(tdatHeader);

		file.ReadArray(tdatHeader.Size / 2, tdatBlock);

		for (auto &key : tkeyBlock)
		{
			tWideString wtext = (uint16_t *)(&tdatBlock[key.Offset]);

			FixCharacters(wtext);
			GameToLiteral(wtext);

			tableIter->second.push_back(make_pair(key.Hash, ConvertToNarrow(wtext)));
		}
	}
}

void IVText::GenerateTexts(const path & output_texts) const
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
