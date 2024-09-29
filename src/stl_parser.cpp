#include "stl_parser.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace {

std::string get_base_name(const std::string &full_name) {
    std::string base_name = full_name;
    size_t pos = base_name.find("\\");
    while (pos != std::string::npos) {
        base_name.replace(pos, pos + 2, "/");
        pos = base_name.find("\\");
    }
    pos = base_name.find_last_of('/');
    base_name = base_name.substr(pos);
    pos = base_name.find_last_of('.');
    base_name = base_name.substr(0, pos);
    return base_name;
}

STLFormat check_stl_format(const std::string &src_file_path) {
    std::ifstream in(src_file_path, std::ios_base::binary);
    if (!in.is_open()) {
        return STLFormat::UNKNOWN;
    }
    char c;
    while (in.get(c)) {
        if (c < 0 || c > 127) {
            in.close();
            return STLFormat::BINARY;
        }
    }
    in.close();
    return STLFormat::ASCII;
}

int read_ascii(const std::string &src_file_path, std::vector<STLFaceInfo> &info) {
    std::ifstream in(src_file_path);
    if (!in.is_open()) {
        return -1;
    }

    std::string line;
    std::string tmp_str;
    std::getline(in, line);
    std::vector<STLFaceInfo> tmp_info;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        ss >> tmp_str; //facet or endsolid
        if (tmp_str == "endsolid") {
            break;
        }

        STLFaceInfo current_info;
        ss >> tmp_str; //normal
        ss >> tmp_str; //normal.x
        current_info.normal[0] = std::stof(tmp_str);
        ss >> tmp_str; //normal.y
        current_info.normal[1] = std::stof(tmp_str);
        ss >> tmp_str; //normal.z
        current_info.normal[2] = std::stof(tmp_str);
        std::getline(in, line); //out loop
        ss = std::stringstream(line);

        std::getline(in, line);
        ss = std::stringstream(line);
        ss >> tmp_str; //vertex
        ss >> tmp_str; //vertex1.x
        current_info.vertex1[0] = std::stof(tmp_str);
        ss >> tmp_str; //vertex1.y
        current_info.vertex1[1] = std::stof(tmp_str);
        ss >> tmp_str; //vertex1.z
        current_info.vertex1[2] = std::stof(tmp_str);

        std::getline(in, line);
        ss = std::stringstream(line);
        ss >> tmp_str; //vertex
        ss >> tmp_str; //vertex2.x
        current_info.vertex2[0] = std::stof(tmp_str);
        ss >> tmp_str; //vertex2.y
        current_info.vertex2[1] = std::stof(tmp_str);
        ss >> tmp_str; //vertex2.z
        current_info.vertex2[2] = std::stof(tmp_str);

        std::getline(in, line);
        ss = std::stringstream(line);
        ss >> tmp_str; //vertex
        ss >> tmp_str; //vertex3.x
        current_info.vertex3[0] = std::stof(tmp_str);
        ss >> tmp_str; //vertex3.y
        current_info.vertex3[1] = std::stof(tmp_str);
        ss >> tmp_str; //vertex3.z
        current_info.vertex3[2] = std::stof(tmp_str);

        std::getline(in, line); //endloop
        std::getline(in, line); //endfacet
        tmp_info.emplace_back(current_info);
    }
    tmp_info.swap(info);
    return 0;

INVALID_TYPE:
    in.close();
    return -1;
}

int read_binary(const std::string &src_file_path, std::vector<STLFaceInfo> &info) {
    std::ifstream in(src_file_path, std::ios_base::binary);
    if (!in.is_open()) {
        return -1;
    }
    in.seekg(80);
    unsigned int face_num;
    in.read((char*)&face_num, 4);
    std::vector<STLFaceInfo> tmp_info;
    try {
        for (int i = 0; i < face_num; i++) {
            STLFaceInfo current_info;
            in.read((char*)current_info.normal.data(), 12);
            in.read((char*)current_info.vertex1.data(), 12);
            in.read((char*)current_info.vertex2.data(), 12);
            in.read((char*)current_info.vertex3.data(), 12);
            in.read((char*)&current_info.attribute, 2);
            tmp_info.emplace_back(current_info);
        }
    } catch (...) {
        in.close();
        return -1;
    }
    in.close();
    info.swap(tmp_info);
    return 0;
}

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
    out << "endsolid" << std::endl;
    out.close();
    return 0;
}
} // namespace

int STLParser::read_from(const std::string &src_file_path, std::vector<STLFaceInfo> &info) {
    const STLFormat type = check_stl_format(src_file_path);
    switch (type) {
        case STLFormat::ASCII:
            return read_ascii(src_file_path, info);
        case STLFormat::BINARY:
            return read_binary(src_file_path, info);
        default:
            return -1;
    }
}

int STLParser::write_to(const std::string &dst_file_path, const std::vector<STLFaceInfo> &info, STLFormat type) {
    switch (type) {
        case STLFormat::ASCII:
            return write_ascii(dst_file_path, info);
        case STLFormat::BINARY:
            return write_binary(dst_file_path, info);
        default:
            return -1;
    }
}