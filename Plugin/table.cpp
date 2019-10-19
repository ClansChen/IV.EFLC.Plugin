#include "table.h"

CCharacterTable globalTable;

void CCharacterTable::LoadTable(const std::filesystem::path& filename)
{
    std::vector<CharacterDataForIO> buffer;

    BinaryFile file(filename, BinaryFile::OpenMode::ReadOnly);

    file.Seek(0, BinaryFile::SeekMode::End);
    auto size = file.Tell();
    file.Seek(0, BinaryFile::SeekMode::Begin);
    file.ReadArray(size / sizeof(CharacterDataForIO), buffer);

    m_Table.reserve(buffer.size() * 2);
    for (auto& entry : buffer)
    {
        m_Table.insert_or_assign(entry.character, entry.pos);
    }
}

CharacterPos CCharacterTable::GetCharPos(std::uint16_t chr) const
{
    auto it = m_Table.find(chr);

    if (it == m_Table.end())
    {
        return CharacterPos{ 50,63 }; //字库中此位置绘制白色方块
    }
    else
    {
        return it->second;
    }
}
