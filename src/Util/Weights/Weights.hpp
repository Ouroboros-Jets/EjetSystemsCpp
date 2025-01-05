
#pragma once

// weights are simply modifiers to system calculations used to obtain the desired output without needing perfect input data.

#include <string>

#include "Json/json.hpp"

class Weights {
public:
    static Weights &GetInstance();

    struct WeightData {
        float weight1 = 0.0f;
        float weight2 = 0.0f;
    } m_Weights;

#ifdef _DEBUG
    void RenderImGui();
#endif
    void SaveToFile(const std::string &file_name);
    void LoadFromFile(const std::string &file_name);

    Weights(const Weights &) = delete;
    Weights &operator=(const Weights &) = delete;

private:
    Weights() = default;
};
