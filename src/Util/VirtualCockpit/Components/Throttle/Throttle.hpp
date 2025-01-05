
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

static bool Throttle(const char *label, float *throttle, int *reverse_thrust, float width = 50.0f,
                     float height = 200.0f, bool disabled = false) {
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
    bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    ImDrawList *draw_list = window->DrawList;

    draw_list->AddRectFilled(bb.Min, bb.Max, IM_COL32(50, 50, 50, 255), 5.0f);

    const float idle_y = bb.Min.y + (height * 0.5f);
    const float reverse_max_y = bb.Max.y;
    const float normal_max_y = bb.Min.y;

    const float snap_threshold = 0.05f;
    const float snap_zone_height = (height * snap_threshold);
    draw_list->AddRectFilled(
            ImVec2(bb.Min.x, idle_y - snap_zone_height / 2),
            ImVec2(bb.Max.x, idle_y + snap_zone_height / 2),
            IM_COL32(70, 70, 70, 255), 5.0f);


    draw_list->AddLine(ImVec2(bb.Min.x, idle_y), ImVec2(bb.Max.x, idle_y), IM_COL32(200, 200, 200, 255), 2.0f);

    ImGui::RenderText(ImVec2(bb.Min.x, bb.Min.y - 20), "MAX");
    ImGui::RenderText(ImVec2(bb.Min.x, idle_y - 10), "IDLE");
    ImGui::RenderText(ImVec2(bb.Min.x, bb.Max.y + 5), "MAX\n REV");

    float handle_y;
    if (*reverse_thrust == 1) {
        handle_y = idle_y + (*throttle) * (reverse_max_y - idle_y);
    } else {
        handle_y = idle_y - (*throttle) * (idle_y - normal_max_y);
    }

    const float handle_radius = width * 0.4f;
    ImVec2 handle_center(bb.Min.x + width * 0.5f, handle_y);

    if (held) {
        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        float relative_y = ImClamp(mouse_pos.y, bb.Min.y, bb.Max.y);

        float relative_from_idle = (relative_y - idle_y) / height;

        if (fabsf(relative_from_idle) <= snap_threshold / 2) {
            relative_y = idle_y;
        }

        if (relative_y > idle_y) {
            *reverse_thrust = 1;
            *throttle = (relative_y - idle_y) / (reverse_max_y - idle_y);
        } else {
            *reverse_thrust = 0;
            *throttle = (idle_y - relative_y) / (idle_y - normal_max_y);
        }

        value_changed = true;
    }

    draw_list->AddCircleFilled(handle_center, handle_radius,
                               disabled ? IM_COL32(100, 100, 100, 255) : IM_COL32(200, 200, 200, 255));

    return value_changed;
}

static void DualThrottle(const char *label, float *throttle_a, int *reverse_a, float *throttle_b, int *reverse_b,
                         bool split_throttles, float width = 50.0f, float height = 200.0f, float spacing = 10.0f) {
    ImGui::BeginGroup();


    ImGui::PushID(label);
    Throttle("Engine 1", throttle_a, reverse_a, width, height);
    ImGui::PopID();

    if (!split_throttles) {
        *throttle_b = *throttle_a;
        *reverse_b = *reverse_a;
    }

    ImGui::SameLine(0, spacing);

    ImGui::PushID(label + 1);
    Throttle("Engine 2", throttle_b, reverse_b, width, height, !split_throttles);
    ImGui::PopID();


    ImGui::EndGroup();
}
