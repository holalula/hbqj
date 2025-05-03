#pragma once

#include <codecvt>
#include <vector>
#include <map>

#include "file/csv/reader.h"
#include "file/file_reader.h"
#include "ui.h"
#include "file.h"
#include "layout_loader.h"

namespace hbqj {
    // TODO: Fix file path
    static std::map<int, std::string> item_key_to_name = CsvParser::FurnitureKeyToNameMapping(
            std::filesystem::current_path() / "HousingFurniture.csv"
    );

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

    enum TableItemStatus {
        IN_GAME,
        IN_FILE,
        MATCHED,
        UNMATCHED_CURRENT,
        UNMATCHED_TARGET,
    };

    static std::string GetTableItemStatus(TableItemStatus status) {
        switch (status) {
            case IN_GAME:
                return "InGame";
            case IN_FILE:
                return "InFile";
            case MATCHED:
                return "Matched";
            case UNMATCHED_CURRENT:
                return "UnmatchedInGame";
            case UNMATCHED_TARGET:
                return "UnmatchedInTarget";
            default:
                return unknown;
        }
    }

    struct HousingItemTableItem {
        uint32_t name;
        uint8_t color;
        float x, y, z, r;
        TableItemStatus status;
    };

    std::vector<HousingItemTableItem> table_items;

    std::map<Byte, std::string> item_color_name_dict;

    static HousingItemTableItem HousingItemToTableItem(const HousingItem &housing_item) {
        return HousingItemTableItem{
                .name = housing_item.type,
                .color = housing_item.color,
                .x = housing_item.position.x,
                .y = housing_item.position.y,
                .z = housing_item.position.z,
                .r = housing_item.rotation,
        };
    }

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
        const auto &memory = HeartBeatMonitor::GetInstance().GetMemoryOperation();

        if (ImGui::Button("Load Furniture List From Game")) {
            if (memory->initialized) {
                auto mode_result = memory->GetLayoutMode();
                if (mode_result && mode_result.value() == HousingLayoutMode::Rotate) {
                    table_items = memory->GetFurnitureList()
                            .transform([](const auto &items) {
                                return items
                                       | std::views::transform(&HousingItemToTableItem)
                                       | std::views::transform([](auto item) {
                                    item.status = TableItemStatus::IN_GAME;
                                    return item;
                                })
                                       | std::ranges::to<std::vector<HousingItemTableItem>>();
                            })
                            .value_or(std::vector<HousingItemTableItem>{});
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear")) {
            table_items.clear();
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
                                      | std::views::transform(&HousingItemToTableItem)
                                      | std::views::transform([](auto item) {
                            item.status = TableItemStatus::IN_FILE;
                            return item;
                        })
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

                    auto items = table_items
                                 | std::views::transform([](const auto &table_item) {
                        return HousingItem{
                                .type = table_item.name,
                                .position = {.x = table_item.x, .y = table_item.y, .z = table_item.z},
                                .rotation = table_item.r,
                                .color = table_item.color,
                        };
                    })
                                 | std::ranges::to<std::vector<HousingItem>>();

                    std::map<std::string, std::string> metadata{};

                    HousingLayout layout{
                            .metadata = metadata,
                            .items = items,
                    };

                    File::SaveToFile(path, layout);
                }
            }
        }

        if (ImGui::Button("Preview Housing Layout")) {
            auto sm = ProcessResources::GetInstance().GetSharedMemory();

            sm->preview_items_count = 0;
            for (auto const [i, table_item]: std::views::enumerate(table_items)) {
                if (TableItemStatus::IN_FILE == table_item.status || TableItemStatus::IN_GAME == table_item.status
                    || TableItemStatus::MATCHED == table_item.status) {
                    sm->preview_items_count++;

                    sm->preview_items[i].type = table_item.name;
                    sm->preview_items[i].position = {
                            .x = table_item.x,
                            .y = table_item.y,
                            .z = table_item.z,
                    };
                    sm->preview_items[i].rotation = table_item.r;
                    sm->preview_items[i].color = table_item.color;
                }
            }

            if (sm->preview_items_count > 0) {
                SetEvent(ProcessResources::GetInstance().events_.preview_housing_layout.get());
            }
        }

        if (ImGui::Button("Load Housing Layout From File Into Game")) {
            if (auto file_path_result = ShowFileDialog(false)) {
                if (file_path_result) {
                    std::filesystem::path path(file_path_result.value());

                    const auto &result = file_reader
                            .DeserializeFile(path)
                            .transform(FileReader::ToHousingLayout);

                    if (result) {
                        auto items_from_file = result.value().items;

                        if (memory->initialized) {
                            auto mode_result = memory->GetLayoutMode();
                            if (mode_result && mode_result.value() == HousingLayoutMode::Rotate) {
                                auto items_from_game_result = memory->GetFurnitureList();
                                if (items_from_game_result) {
                                    auto items_from_game = items_from_game_result.value();

                                    const auto &plan = LayoutLoader::GetLoadingPlan(items_from_game, items_from_file);

                                    table_items.clear();
                                    for (const auto &item: plan.matched_items) {
                                        auto table_item = HousingItemToTableItem(item);
                                        table_item.status = TableItemStatus::MATCHED;
                                        table_items.push_back(table_item);
                                    }

                                    for (const auto &item: plan.unmatched_current) {
                                        auto table_item = HousingItemToTableItem(item);
                                        table_item.status = TableItemStatus::UNMATCHED_CURRENT;
                                        table_items.push_back(table_item);
                                    }

                                    for (const auto &item: plan.unmatched_target) {
                                        auto table_item = HousingItemToTableItem(item);
                                        table_item.status = TableItemStatus::UNMATCHED_TARGET;
                                        table_items.push_back(table_item);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        ImGuiStyle &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
        if (ImGui::BeginTable("furniture_list", 7, table_flags)) {

            // Declare columns
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 150.0f);
            ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_None, 100.0f);
            ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Rotation", ImGuiTableColumnFlags_None, 80.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_None, 120.0f);

            ImGui::TableSetupScrollFreeze(1, 1);

            ImGui::TableHeadersRow();

            for (const auto &item: table_items) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", GetOrDefault(item_key_to_name, static_cast<int>(item.name),
                                               std::format("{}({})", unknown, item.name)).c_str());

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

                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%s", GetTableItemStatus(item.status).c_str());
            }
            ImGui::EndTable();
        }
    }
}
