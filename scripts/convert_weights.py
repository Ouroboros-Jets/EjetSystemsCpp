import json


def generate_header(json_file="system_weights.json"):
    with open(json_file) as f:
        weights = json.load(f)
        header = """#pragma once
namespace Systems::Weights {
    struct SystemWeights {
"""

        for key, value in weights.items():
            if isinstance(value, dict):
                header += f"    struct {key} {{\n"
                for subkey, subvalue in value.items():
                    header += f"        static constexpr float {subkey} = {subvalue}f;\n"
            else:
                header += f"        static constexpr float {key} = {value}f;\n"

            header += """    };
}
"""

        with open("GeneratedWeights.hpp", "w") as file:
            file.write(header)


if __name__ == "__main__":
    generate_header()
