
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <vector>

struct SwitchPosition {
    const char *label{};
    ImVec4 color;
};

static bool MultiPositionSwitch(const char *label, int *current_position, const int max_positions, float width = 50.0f, float height = 50.0f,
                                const std::vector<SwitchPosition> &positions = {}, bool invert_direction = false) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size(width, height);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool value_changed = false;
    bool hovered, held;
    bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    if (clicked) {
        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        float relative_y;

        // Adjust the click handling based on invert_direction
        if (invert_direction) {
            relative_y = (mouse_pos.y - bb.Min.y) / height;
        } else {
            relative_y = (bb.Max.y - mouse_pos.y) / height;
        }

        int new_position = static_cast<int>(relative_y * max_positions);
        new_position = ImClamp(new_position, 0, max_positions - 1);

        if (new_position != *current_position) {
            *current_position = new_position;
            value_changed = true;
        }
    }

    ImDrawList *draw_list = window->DrawList;

    const float groove_width = width * 0.3f;
    draw_list->AddRectFilled(ImVec2(bb.Min.x + (width - groove_width) * 0.5f, bb.Min.y + 10), ImVec2(bb.Min.x + (width + groove_width) * 0.5f, bb.Max.y - 10),
                             IM_COL32(60, 60, 60, 255), groove_width * 0.5f);

    for (int i = 0; i < max_positions; i++) {
        float y_pos = invert_direction ? bb.Min.y + 10 + (height - 20) * i / (max_positions - 1) : bb.Max.y - 10 - (height - 20) * i / (max_positions - 1);
        float marker_width = width * 0.4f;

        draw_list->AddLine(ImVec2(bb.Min.x + (width - marker_width) * 0.5f, y_pos), ImVec2(bb.Min.x + (width + marker_width) * 0.5f, y_pos), IM_COL32(200, 200, 200, 255), 2.0f);

        if (i < positions.size() && positions[i].label) {
            ImGui::RenderText(ImVec2(bb.Max.x + 10, y_pos - ImGui::GetTextLineHeight() * 0.5f), positions[i].label);
        }
    }

    float handle_y = invert_direction ? bb.Min.y + 10 + (height - 20) * (*current_position) / (max_positions - 1)
                                      : bb.Max.y - 10 - (height - 20) * (*current_position) / (max_positions - 1);
    ImVec4 handle_color = (hovered || held) ? ImVec4(0.9f, 0.9f, 0.9f, 1.0f) : ImVec4(0.8f, 0.8f, 0.8f, 1.0f);

    if (*current_position < positions.size()) {
        handle_color = positions[*current_position].color;
        if (hovered || held) {
            handle_color.x = ImMin(handle_color.x * 1.2f, 1.0f);
            handle_color.y = ImMin(handle_color.y * 1.2f, 1.0f);
            handle_color.z = ImMin(handle_color.z * 1.2f, 1.0f);
        }
    }

    const float handle_width = width * 0.8f;
    const float handle_height = height * 0.15f;
    draw_list->AddRectFilled(ImVec2(bb.Min.x + (width - handle_width) * 0.5f, handle_y - handle_height * 0.5f),
                             ImVec2(bb.Min.x + (width + handle_width) * 0.5f, handle_y + handle_height * 0.5f), ImGui::ColorConvertFloat4ToU32(handle_color),
                             handle_height * 0.25f);

    if (label[0] != '\0') {
        ImGui::RenderText(ImVec2(bb.Min.x, bb.Min.y - ImGui::GetTextLineHeight() - 5), label);
    }

    if (hovered) {
        ImGui::BeginTooltip();
        if (*current_position < positions.size() && positions[*current_position].label) {
            ImGui::Text("%s: %s", label, positions[*current_position].label);
        } else {
            ImGui::Text("%s: Position %d", label, *current_position);
        }
        ImGui::EndTooltip();
    }

    return value_changed;
}
