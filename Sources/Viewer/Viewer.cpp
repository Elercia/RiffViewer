#include "Viewer/Viewer.hpp"

#include "Riff/Riff.hpp"
#include "imgui.h"

bool LoadRiff(const std::string& path, Riff& out)
{
    return out.LoadFrom(path) == Error::None;
}

void ShowChunk(const Riff& riff, std::size_t index, std::size_t& selectedIndex)
{
    const RiffChunk& chunk = riff.chunks[index];
    if (chunk.childrenIndices.empty())
    {
        auto str = chunk.id.ToStr();
        str += " - " + std::to_string(chunk.length) + " bytes";
        if (ImGui::Selectable(str.c_str(), selectedIndex == index))
        {
            selectedIndex = index;
        }
    }
    else
    {
        auto str = chunk.formType.ToStr();
        str += " - " + std::to_string(chunk.length) + " bytes";
        if (ImGui::TreeNode(str.c_str()))
        {
            for (auto childIndex : chunk.childrenIndices)
            {
                ShowChunk(riff, childIndex, selectedIndex);
            }
            ImGui::TreePop();
        }
    }
}

const char* ToHex(u8 v)
{
    static const char hexTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    static char tmp[3] = {0, 0, 0};

    tmp[0] = hexTable[v % 16];
    tmp[1] = hexTable[(v / 16) % 16];

    return tmp;
}

char ToChar(u8 v)
{
    if (v < 33 || v >= 126)
    {
        return '.';
    }

    return (char) v;
}

u32 Min(u32 l, u32 r)
{
    return l < r ? l : r;
}

void ShowData(u8* data, u32 length)
{
    constexpr int flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit;
    constexpr u32 charPerRow = 15;

    if (ImGui::BeginTable("table2", 2, flags))
    {
        for (u32 row = 0; row < (length / charPerRow) + 1; row++)
        {
            const u32 start = row * charPerRow;
            const u32 end = Min(start + charPerRow, length);
            if (end <= start)
                break;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            std::string raw;
            std::string ascii;
            for (u32 i = start; i < end; i++)
            {
                raw += ToHex(data[i]);
                raw += " ";

                ascii += ToChar(data[i]);
            }

            ImGui::Text("%s", raw.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", ascii.c_str());
        }
        ImGui::EndTable();
    }
}

void ShowChunkData(const RiffChunk& chunk)
{
    auto str = chunk.id.ToStr();
    str += " - " + std::to_string(chunk.length) + " bytes";
    ImGui::TextUnformatted(str.c_str());
    ShowData(chunk.data, chunk.length);
}

#define invalid_selection ((std::size_t) -1)

void ShowRiff(const Riff& riff)
{
    if (riff.chunks.empty())
        return;

    static std::size_t selectedIndex = invalid_selection;

    constexpr int flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;

    if (ImGui::BeginTable("#riff global table", 2, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ShowChunk(riff, 0, selectedIndex);

        ImGui::TableNextColumn();

        if (selectedIndex != invalid_selection)
        {
            const auto& chunk = riff.chunks[selectedIndex];
            ShowChunkData(chunk);
        }

        ImGui::EndTable();
    }
}