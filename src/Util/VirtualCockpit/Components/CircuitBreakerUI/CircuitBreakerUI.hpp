
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include <string>


const auto PULLED_COLOR = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
const auto PUSHED_COLOR = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);

static bool CircuitBreakerUI(const char *label, bool *state, float width = 40.0f, float height = 40.0f) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size = ImVec2(width, height);
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    if (clicked)
        *state = !*state;

    ImDrawList *draw_list = window->DrawList;

    ImVec4 color = *state ? PUSHED_COLOR : PULLED_COLOR;
    if (hovered) {
        color.x = ImMin(color.x * 1.2f, 1.0f);
        color.y = ImMin(color.y * 1.2f, 1.0f);
        color.z = ImMin(color.z * 1.2f, 1.0f);
    }

    draw_list->AddRectFilled(ImVec2(bb.Min.x + 5, bb.Min.y + 5),
                             ImVec2(bb.Max.x - 5, bb.Max.y - 5),
                             ImGui::ColorConvertFloat4ToU32(color),
                             5.0f);

    float switch_y = *state ? (bb.Min.y + height * 0.5f) : (bb.Min.y + height * 0.75f);

    draw_list->AddCircleFilled(
            ImVec2(bb.Min.x + width * 0.5f, switch_y),
            width * 0.3f,
            IM_COL32(240, 240, 240, 255)
            );

    if (label[0] != '\0') {
        ImGui::RenderText(
                ImVec2(bb.Min.x + width + 10, bb.Min.y + height * 0.5f - ImGui::GetTextLineHeight() * 0.5f),
                label
                );
    }

    if (hovered) {
        ImGui::BeginTooltip();
        ImGui::Text("%s: %s", label, *state ? "PUSHED" : "PULLED");
        ImGui::EndTooltip();
    }

    return clicked;
}
