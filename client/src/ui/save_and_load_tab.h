#pragma once

#include <codecvt>
#include "ui.h"

static ImGuiTableFlags table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
        | ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
        | ImGuiTableFlags_SizingFixedFit;

struct HousingItemTableItem {
    std::string name;
    std::string color;
    float x, y, z, r;
};

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
                log("{}", file_path);
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
    if (ImGui::BeginTable("furniture_list", 7, table_flags)) {

        // Declare columns
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Color");
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableSetupColumn("Z");
        ImGui::TableSetupColumn("Rotation");
        ImGui::TableSetupColumn("Action");

        ImGui::TableSetupScrollFreeze(1, 1);

        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Name0");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Color0");
        ImGui::TableSetColumnIndex(2);
        if (ImGui::Button("X")) {
            log("Click X");
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Name0");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Color0");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("X1");

        ImGui::EndTable();
    }
}