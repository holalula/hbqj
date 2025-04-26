#pragma once

#include <codecvt>
#include <vector>
#include <map>

#include "file/file_reader.h"
#include "ui.h"

namespace hbqj {
    static const std::string unknown = "Unknown";

    template<typename K, typename V>
    static V GetOrDefault(const std::map<K, V> &map, const K &key, const V &default_value) {
        auto it = map.find(key);
        return it != map.end() ? it->second : default_value;
    }

    static FileReader file_reader;

    static ImGuiTableFlags table_flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_SizingFixedFit;

    struct HousingItemTableItem {
        uint32_t name;
        uint8_t color;
        float x, y, z, r;
    };

    std::vector<HousingItemTableItem> table_items;

    std::map<uint32_t, std::string> item_type_name_dict;
    std::map<Byte, std::string> item_color_name_dict;

    static HousingItemTableItem HouingItemToTableItem(const HousingItem &housing_item) {
        return HousingItemTableItem{
                .name = housing_item.type,
                .color = housing_item.color,
                .x = housing_item.position.x,
                .y = housing_item.position.y,
                .z = housing_item.position.z,
                .r = housing_item.rotation,
        };
    }

    struct LoadingHousingLayoutTableItem {
        std::string name;
        std::string color;
        float x, y, z, r;
        float t_x, t_y, t_z, t_r;
        int item_type = 0;
    };

    std::optional<std::wstring> ShowFileDialog(bool isSave) {
        OPENFILENAMEW ofn;
        wchar_t szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;

        if (isSave) {
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            // ofn.lpstrDefExt = L"json";
            if (GetSaveFileNameW(&ofn)) {
                return std::wstring(szFile);
            }
        } else {
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (GetOpenFileNameW(&ofn)) {
                return std::wstring(szFile);
            }
        }
        return std::nullopt;
    }

    void SaveAndLoadTab() {
        if (ImGui::Button("Load Furniture List From Game")) {

        }

        if (ImGui::Button("Import Furniture List From File")) {
            if (auto file_path_result = ShowFileDialog(false)) {
                if (file_path_result) {
                    std::filesystem::path path(file_path_result.value());
                    // Don't use utf16_to_utf8 for file paths
                    // Chinese Windows uses CP_ACP (GBK encoding) for file paths, not UTF-8
                    // std::filesystem::path handles the system-specific encoding automatically
                    // const auto& file_path = hbqj::utf16_to_utf8(file_path_result.value());
                    const auto &file_path = path.string();

                    const auto &result = file_reader
                            .DeserializeFile(path)
                            .transform(FileReader::ToHousingLayout);

                    if (result) {
                        table_items = result.value().items
                                      | std::views::transform(HouingItemToTableItem)
                                      | std::ranges::to<std::vector<HousingItemTableItem>>();
                    }
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Export Furniture List To File")) {
            if (auto file_path_result = ShowFileDialog(true)) {
                if (file_path_result) {
                    std::filesystem::path path(file_path_result.value());
                    const auto &file_path = path.string();
                    log("{}", file_path);
                }
            }
        }

        if (ImGui::Button("Preview Housing Layout")) {

        }

        if (ImGui::Button("Load Current Housing Layout Into Game")) {

        }


        ImGuiStyle &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
        if (ImGui::BeginTable("furniture_list", 6, table_flags)) {

            // Declare columns
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 150.0f);
            ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_None, 100.0f);
            ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Rotation", ImGuiTableColumnFlags_None, 80.0f);

            ImGui::TableSetupScrollFreeze(1, 1);

            ImGui::TableHeadersRow();

            for (const auto &item: table_items) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", GetOrDefault(item_type_name_dict, item.name, unknown).c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetOrDefault(item_color_name_dict, item.color, unknown).c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.3f", item.x);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.3f", item.y);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.3f", item.z);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%.3f", item.r);
            }
            ImGui::EndTable();
        }
    }
}
