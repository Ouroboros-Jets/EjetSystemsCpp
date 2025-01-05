
#pragma once
#include "imgui.h"
#include "imgui_internal.h"

static bool Slider(const char *label, float *value, float width = 50.0f, float height = 200.0f, bool invert = false, bool disabled = false) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiID id = window->GetID(label);
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size(width, height);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool value_changed = false;
    bool hovered, held;
    ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList *draw_list = window->DrawList;

    draw_list->AddRectFilled(bb.Min, bb.Max, IM_COL32(50, 50, 50, 255), 5.0f);

    draw_list->AddLine(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Max.x, bb.Min.y), IM_COL32(200, 200, 200, 255), 2.0f);
    draw_list->AddLine(ImVec2(bb.Min.x, bb.Max.y), ImVec2(bb.Max.x, bb.Max.y), IM_COL32(200, 200, 200, 255), 2.0f);

    if (invert) {
        ImGui::RenderText(ImVec2(bb.Min.x, bb.Min.y - 20), "MIN");
        ImGui::RenderText(ImVec2(bb.Min.x, bb.Max.y + 5), "MAX");
    } else {
        ImGui::RenderText(ImVec2(bb.Min.x, bb.Min.y - 20), "MAX");
        ImGui::RenderText(ImVec2(bb.Min.x, bb.Max.y + 5), "MIN");
    }

    float handle_y = invert ? bb.Min.y + (*value) * height : bb.Max.y - (*value) * height;

    const float handle_radius = width * 0.4f;
    ImVec2 handle_center(bb.Min.x + width * 0.5f, handle_y);

    if (held) {
        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        float relative_y = ImClamp(mouse_pos.y, bb.Min.y, bb.Max.y);

        if (invert) {
            *value = ImClamp((relative_y - bb.Min.y) / height, 0.0f, 1.0f);
        } else {
            *value = ImClamp((bb.Max.y - relative_y) / height, 0.0f, 1.0f);
        }

        value_changed = true;
    }

    draw_list->AddCircleFilled(handle_center, handle_radius, disabled ? IM_COL32(100, 100, 100, 255) : IM_COL32(200, 200, 200, 255));

    return value_changed;
}
