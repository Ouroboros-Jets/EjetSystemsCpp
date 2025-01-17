import json


# TODO: implement types for weights so we can parse the struct in c++ and generate a consistent json file

def generate_header(json_file="system_weights.json"):
    with open(json_file) as f:
        weights = json.load(f)

        header = """#pragma once
        
// This file was generated by convert_weights.py using the current system_weights.json

// values cannot be const if we are going to change them in the gui
#ifdef _DEBUG
    #define WEIGHT_CONST 
#else
    #define WEIGHT_CONST constexpr
#endif

namespace Systems::Weights {
"""

        header += "    struct SystemWeights {\n"

        for key, value in weights.items():
            if isinstance(value, dict):
                header += f"        struct {key} {{\n"
                for subkey, subvalue in value.items():
                    header += f"            static WEIGHT_CONST float {subkey} = {subvalue}f;\n"
                header += "        };\n"
            else:
                header += f"        static WEIGHT_CONST float {key} = {value}f;\n"

        header += """    };
}

#undef WEIGHT_CONST
"""

        with open("GeneratedWeights.hpp", "w") as file:
            file.write(header)


if __name__ == "__main__":
    generate_header()
