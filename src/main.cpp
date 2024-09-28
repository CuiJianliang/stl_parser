#include "stl_parser.h"

int main() {
    std::vector<STLFaceInfo> a;
    STLParser::write_to("1.stl", a);
}