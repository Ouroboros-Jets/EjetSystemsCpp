#pragma once

#include "imgui.h"
#include "imgui_internal.h"

static bool IlluminatedButton(const char *label, int *state, float width = 40.0f, float height = 40.0f, ImU32 on_color = IM_COL32(0, 0, 0, 255),
                              ImU32 off_color = IM_COL32(0, 0, 0, 255)) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size(width, height);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    const ImVec2 label_size = ImGui::CalcTextSize(label);
    const ImVec2 label_pos = ImVec2(bb.Min.x + (width - label_size.x) * 0.5f, pos.y - label_size.y - 4.0f);

    ImGui::ItemSize(ImVec2(size.x, size.y + label_size.y + 4.0f));
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    if (clicked) {
        *state = (*state == 0) ? 1 : 0;
    }

    ImDrawList *draw_list = window->DrawList;

    draw_list->AddText(label_pos, IM_COL32(255, 255, 255, 255), label);

    draw_list->AddRectFilled(bb.Min, bb.Max, (*state == 1) ? on_color : off_color, height * 0.1f);

    const float indicator_width = width * 0.3f;
    const float indicator_height = height * 0.1f;
    const ImVec2 indicator_min = ImVec2(bb.Min.x + (width - indicator_width) * 0.5f, bb.Min.y + (height - indicator_height) * 0.5f);
    const ImVec2 indicator_max = ImVec2(indicator_min.x + indicator_width, indicator_min.y + indicator_height);

    draw_list->AddRectFilled(indicator_min, indicator_max, (*state == 1) ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255));

    draw_list->AddRect(bb.Min, bb.Max, IM_COL32(0, 0, 0, 255), height * 0.1f, 0, 2.0f);

    if (hovered) {
        ImGui::BeginTooltip();
        ImGui::Text("%s: %s", label, (*state == 1) ? "On" : "Off");
        ImGui::EndTooltip();
    }

    return clicked;
}
