#include "stl_parser.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

namespace {

std::string get_base_name(const std::string &full_name) { return ""; }

int write_binary(const std::string &dst_file_path, const std::vector<STLFaceInfo> &info) {
    std::ofstream out(dst_file_path, std::ios_base::binary);
    if (!out.is_open()) {
        return -1;
    }

    const std::string base_name = get_base_name(dst_file_path);
    char header[80];
    memset(header, 0, 80);
    memcpy(header, base_name.c_str(), std::min(80, int(base_name.size())));
    out.write(header, 80);

    unsigned int size = info.size();
    out.write((char *)(&size), 4);

    for (int i = 0; i < info.size(); i++) {
        const STLFaceInfo &current_info = info[i];
        out.write((char *)current_info.normal.data(), 12);
        out.write((char *)current_info.vertex1.data(), 12);
        out.write((char *)current_info.vertex2.data(), 12);
        out.write((char *)current_info.vertex3.data(), 12);
        out.write((char *)(&current_info.attribute), 2);
    }
    out.close();
    return 0;
}

int write_ascii(const std::string &dst_file_path, const std::vector<STLFaceInfo> &info) {
    std::ofstream out(dst_file_path);
    if (!out.is_open()) {
        return -1;
    }
    const std::string base_name = get_base_name(dst_file_path);
    out << "solid " << base_name << std::endl;
    for (int i = 0; i < info.size(); i++) {
        const STLFaceInfo &current_info = info[i];
        out << "\tfacet normal " << current_info.normal[0] << " " << current_info.normal[1] << " " << current_info.normal[2] << std::endl;
        out << "\t\tout loop" << std::endl;
        out << "\t\t\tvertex " << current_info.vertex1[0] << " " << current_info.vertex1[1] << " " << current_info.vertex1[2] << std::endl;
        out << "\t\t\tvertex " << current_info.vertex2[0] << " " << current_info.vertex2[1] << " " << current_info.vertex2[2] << std::endl;
        out << "\t\t\tvertex " << current_info.vertex3[0] << " " << current_info.vertex3[1] << " " << current_info.vertex3[2] << std::endl;
        out << "\t\tendloop" << std::endl;
        out << "\tendfacet" << std::endl;
    }
    out.close();
    return 0;
}
} // namespace

int STLParser::read_from(const std::string &src_file_path, std::vector<STLFaceInfo> &info) { return 0; }

int STLParser::write_to(const std::string &dst_file_path, const std::vector<STLFaceInfo> &info, STLFormat type) {
    if (type == STLFormat::ASCII) {
        return write_ascii(dst_file_path, info);
    } else {
        return write_binary(dst_file_path, info);
    }
}