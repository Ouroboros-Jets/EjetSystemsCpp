
#pragma once

#include "imgui.h"

#include <cmath>
#include <string>
#include <vector>

#define PI 3.1415926535897932384 // from memory :)

static bool MultiPositionKnob(const char *label, int *position, int max_positions, const std::vector<std::string> &labels, bool show_label = true) {
    assert(max_positions > 1);
    assert(labels.size() == max_positions);

    ImGui::BeginGroup();
    if (show_label) {
        ImGui::Text("%s", label);
    }


    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(50, 80);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(pos.x + size.x / 2, pos.y + size.y / 2);
    float radius = size.x / 2;

    draw_list->AddCircleFilled(center, radius, IM_COL32(50, 50, 50, 255));

    float angle = (*position) * (2.0f * PI / max_positions) - PI / 2.0f;
    ImVec2 pointer = ImVec2(center.x + radius * 0.6f * cosf(angle), center.y + radius * 0.6f * sinf(angle));
    draw_list->AddLine(center, pointer, IM_COL32(255, 255, 255, 255), 2.0f);

    for (int i = 0; i < max_positions; ++i) {
        float label_angle = i * (2.0f * PI / max_positions) - PI / 2.0f;
        ImVec2 label_pos = ImVec2(center.x + radius * 1.45f * cosf(label_angle), center.y + radius * 1.45f * sinf(label_angle));

        ImVec2 text_size = ImGui::CalcTextSize(labels[i].c_str());
        draw_list->AddText(ImVec2(label_pos.x - text_size.x / 2, label_pos.y - text_size.y / 2), IM_COL32(255, 255, 255, 255), labels[i].c_str());
    }

    ImGui::InvisibleButton(label, size);
    bool is_hovered = ImGui::IsItemHovered();
    if (is_hovered && ImGui::IsMouseClicked(0)) {
        *position = (*position + 1) % max_positions;
    } else if (is_hovered && ImGui::IsMouseClicked(1)) {
        *position = (*position - 1 + max_positions) % max_positions;
    }

    if (is_hovered) {
        ImGui::SetTooltip("Position: %d", *position);
    }

    ImGui::Dummy(size);
    ImGui::EndGroup();

    return is_hovered;
}
