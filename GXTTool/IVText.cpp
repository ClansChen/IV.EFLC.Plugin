#include "IVText.h"

using namespace std;
using namespace std::filesystem;

void IVText::Process0Arg()
{
    wchar_t temp[512];

    HMODULE self = GetModuleHandleW(NULL);
    GetModuleFileNameW(self, temp, 512);

    tPath text_path = temp;
    text_path = text_path.parent_path();

    m_Data.clear();
    m_Collection.clear();

    LoadText(text_path / "GTA4.txt");

    if (!m_Data.empty())
    {
        GenerateBinary(text_path / "chinese.gxt");
        GenerateCollection(text_path / "characters.txt");
        GenerateTable(text_path / "table.dat");
    }
}

void IVText::Process2Args(const tPath& arg1, const tPath& arg2)
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

void IVText::SkipUTF8Signature(ifstream& stream)
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

void IVText::AddUTF8Signature(ofstream& stream)
{
    stream << "\xEF\xBB\xBF";
}

IVText::tWideString IVText::ConvertToWide(const string& in)
{
    tWideString result;
    utf8::utf8to16(in.begin(), in.end(), back_inserter(result));
    return result;
}

string IVText::ConvertToNarrow(const tWideString& in)
{
    string result;
    utf8::utf16to8(in.begin(), in.end(), back_inserter(result));
    return result;
}

bool IVText::IsNativeCharacter(uint16_t character)
{
    return (character < 0x100 || character == L'™');
}

void IVText::CollectCharacters(const string& text)
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

void IVText::LoadText(const tPath& input_text)
{
    regex table_regex(R"(\[([0-9a-zA-Z_]{1,7})\])");
    regex entry_regex(R"((0[xX][0-9a-fA-F]{8})=(.*))");
    regex origin_regex(R"(;(0[xX][0-9a-fA-F]{8})=(.*))");

    smatch matches;

    string line;

    auto table_iter = m_Data.end();
    std::string filename = input_text.filename().string();

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

        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);

        if (line.empty() || line.front() == ';')
        {

        }
        else if (line.front() == '0' && regex_match(line, matches, entry_regex))
        {
            if (table_iter != m_Data.end())
            {
                uint32_t hash = stoul(matches.str(1), nullptr, 16);
                string text = matches.str(2);

                CollectCharacters(text);

                table_iter->second.emplace_back(hash, text);
            }
            else
            {
                cout << filename << ": " << "第" << line_no << "行没有所属的表。" << endl;
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
            cout << filename << ": " << "第" << line_no << "行无法识别。" << endl;
        }
    }
}

void IVText::LoadTexts(const tPath& input_texts)
{
    directory_iterator dir_it{ input_texts };

    while (dir_it != directory_iterator{})
    {
        tPath filename = dir_it->path();

        LoadText(filename);

        ++dir_it;
    }
}

void IVText::GenerateBinary(const tPath& output_binary) const
{
    BinaryFile file(output_binary, BinaryFile::OpenMode::WriteOnly);

    std::vector<uint8_t> buffer;
    long long writePostion;

    GXTHeader gxtHeader;

    TableBlock tableBlock;
    KeyBlockOthers keyBlock;

    DataBlock dataBlock;

    TableEntry tableEntry;
    KeyEntry keyEntry;

    vector<TableEntry> tables;
    vector<KeyEntry> keys;
    vector<uint16_t> datas;

    tWideString wideText;

    if (!file)
    {
        cout << "创建输出文件 " << output_binary.string() << " 失败。" << endl;
        return;
    }

    gxtHeader.Version = 4;
    gxtHeader.CharBits = 16;
    file.Write(gxtHeader);

    tableBlock.TABL[0] = 'T';
    tableBlock.TABL[1] = 'A';
    tableBlock.TABL[2] = 'B';
    tableBlock.TABL[3] = 'L';

    keyBlock.Body.TKEY[0] = 'T';
    keyBlock.Body.TKEY[1] = 'K';
    keyBlock.Body.TKEY[2] = 'E';
    keyBlock.Body.TKEY[3] = 'Y';

    dataBlock.TDAT[0] = 'T';
    dataBlock.TDAT[1] = 'D';
    dataBlock.TDAT[2] = 'A';
    dataBlock.TDAT[3] = 'T';

    tableBlock.Size = m_Data.size() * sizeof(TableEntry);
    file.Write(tableBlock);

    writePostion = 4 + 8 + tableBlock.Size;

    tables.clear();

    for (auto& table : m_Data)
    {
        keys.clear();
        datas.clear();

        strcpy(tableEntry.Name, table.first.c_str());
        tableEntry.Offset = static_cast<int>(writePostion);
        tables.push_back(tableEntry);

        strcpy(keyBlock.Name, table.first.c_str());
        keyBlock.Body.Size = table.second.size() * sizeof(KeyEntry);

        for (auto& key : table.second)
        {
            keyEntry.Hash = key.first;
            keyEntry.Offset = datas.size() * 2;

            wideText = ConvertToWide(key.second);
            LiteralToGame(wideText);

            copy(wideText.begin(), wideText.end(), back_inserter(datas));
            datas.push_back(0);

            keys.push_back(keyEntry);
        }

        dataBlock.Size = datas.size() * 2;

        file.Seek(writePostion, BinaryFile::SeekMode::Begin);

        if (table.first == "MAIN")
        {
            file.Write(keyBlock.Body);
        }
        else
        {
            file.Write(keyBlock);
        }

        file.WriteArray(keys);
        file.Write(dataBlock);
        file.WriteArray(datas);

        writePostion = file.Tell();
    }

    file.Seek(4 + 8, BinaryFile::SeekMode::Begin);
    file.WriteArray(tables);
}

void IVText::GenerateCollection(const tPath& output_text) const
{
    vector<char> sequence;

    size_t count = 0;

    for (auto char_it = m_Collection.begin(); char_it != m_Collection.end(); ++char_it)
    {
        if (count == 64)
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

void IVText::GenerateTable(const tPath& output_binary) const
{
    vector<CharacterDataForIO> datas;

    CharacterDataForIO data;

    data.pos.row = 0;
    data.pos.column = 0;

    for (auto chr : m_Collection)
    {
        data.character = chr;
        if (data.pos.column == 64)
        {
            ++data.pos.row;
            data.pos.column = 0;
        }

        datas.push_back(data);

        ++data.pos.column;
    }

    BinaryFile stream(output_binary, BinaryFile::OpenMode::WriteOnly);
    stream.WriteArray(datas);
}

void IVText::FixCharacters(tWideString& wtext)
{
    //bad character in IV stock text: 0x85 0x92 0x94 0x96 0x97 0xA0
    //bad character in EFLC stock text: 0x93

    for (auto& character : wtext)
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

void IVText::LiteralToGame(tWideString& wtext)
{
    for (auto& character : wtext)
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

void IVText::GameToLiteral(tWideString& wtext)
{
    for (auto& character : wtext)
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

void IVText::LoadBinary(const tPath& input_binary)
{
    GXTHeader gxtHeader;
    TableBlock tableBlock;
    KeyBlockOthers keyBlock;
    DataBlock tdatHeader;

    vector<TableEntry> tables;
    vector<KeyEntry> keys;
    vector<uint16_t> datas;

    m_Data.clear();

    auto tableIter = m_Data.end();

    BinaryFile file(input_binary, BinaryFile::OpenMode::ReadOnly);

    if (!file)
    {
        cout << "打开输入文件 " << input_binary.string() << " 失败。" << endl;
        return;
    }

    file.Read(gxtHeader);

    file.Read(tableBlock);

    file.ReadArray(tableBlock.Size / sizeof(TableEntry), tables);

    for (TableEntry& table : tables)
    {
        tableIter = m_Data.insert(tTable(table.Name, vector<tEntry>())).first;

        file.Seek(table.Offset, BinaryFile::SeekMode::Begin);

        if (strcmp(table.Name, "MAIN") != 0)
        {
            file.Read(keyBlock);
        }
        else
        {
            file.Read(keyBlock.Body);
        }

        file.ReadArray(keyBlock.Body.Size / sizeof(KeyEntry), keys);

        file.Read(tdatHeader);

        file.ReadArray(tdatHeader.Size / 2, datas);

        for (auto& key : keys)
        {
            tWideString wtext = &datas[key.Offset];

            FixCharacters(wtext);
            GameToLiteral(wtext);

            tableIter->second.emplace_back(key.Hash, ConvertToNarrow(wtext));
        }
    }
}

void IVText::GenerateTexts(const tPath& output_texts) const
{
    ofstream stream;
    std::string line;

    for (auto& table : m_Data)
    {
        stream.open(output_texts / (table.first + ".txt"), ios::trunc);

        if (!stream)
        {
            cout << "创建输出文件失败" << endl;
        }

        AddUTF8Signature(stream);

        stream << fmt::sprintf("[%s]\n", table.first);

        for (auto& entry : table.second)
        {
            line = fmt::sprintf("0x%08X=%s\n", entry.first, entry.second);
            stream << ';' << line << line;
        }

        stream.close();
    }
}
