#include "stl_parser.h"

int main() {
    std::vector<STLFaceInfo> vertices;

#ifdef _WIN32
    const std::string input_file_name = "../../resource/input.stl";
    const std::string output_file_name = "../../resource/output.stl";
#else
    const std::string input_file_name = "../resource/test.stl";
    const std::string output_file_name = "../resource/output.stl";
#endif
    STLParser::read_from(input_file_name, vertices);
    STLParser::write_to(output_file_name, vertices);
    return 0;
}