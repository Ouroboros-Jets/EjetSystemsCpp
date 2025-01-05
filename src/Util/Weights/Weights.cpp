
#include "Weights.hpp"

#include <fstream>
#include <imgui.h>

#ifdef _DEBUG
void Weights::RenderImGui() {
    if (ImGui::Begin("Weights")) {
        ImGui::Text("Category1");
        ImGui::SliderFloat("Weight1", &m_Weights.weight1, 0.1f, 5.0f);
        ImGui::SliderFloat("Weight2", &m_Weights.weight2, 0.1f, 5.0f);

        if (ImGui::Button("Save")) {
            SaveToFile("system_weights.json");
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            LoadFromFile("system_weights.json");
        }
    }
    ImGui::End();
}
#endif

Weights &Weights::GetInstance() {
    static Weights instance;
    return instance;
}

void Weights::LoadFromFile(const std::string &file_name) {
    std::ifstream file(file_name);
    if (file.is_open()) {
        nlohmann::json json;
        file >> json;

        m_Weights.weight1 = json["Weight1"].get<float>();
        m_Weights.weight2 = json["Weight2"].get<float>();
    }
}

void Weights::SaveToFile(const std::string &file_name) {
    nlohmann::json j = {{"Weight1", m_Weights.weight1}, {"Weight2", m_Weights.weight2}};
    std::ofstream file(file_name);
    file << j.dump(4);
}
