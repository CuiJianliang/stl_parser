#pragma once

#ifdef DLL_EXPORT
#ifdef _WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif // _WIN32
#else
#ifdef _WIN32
#define DLL_API __declspec(dllimport)
#else
#define DLL_API
#endif // _WIN32
#endif // DLL_EXPORT

#include <array>
#include <string>
#include <vector>

/**
 *  STL BINARY FORMAT
 *  UINT8[80] ----------------------------Header
 *  UINT32 -------------------------------Number of triangles
 *  foreach triangle
 *      FLOAT32[3]------------------------Normal vector
 *      FLOAT32[3]------------------------Vertex 1
 *      FLOAT32[3]------------------------Vertex 2
 *      FLOAT32[3]------------------------Vertex 3
 *      UINT16----------------------------Attribute byte count
 *  end
 */

/**
 *  STL ASCII FORMAT
 *  solid [NAME]
 *  foreach triangle
 *      facet normal [normal.x normal.y normal.z]
 *          out loop
 *              vertex [vertex1.x, vertex1.y, vertex1.z]
 *              vertex [vertex2.x, vertex2.y, vertex2.z]
 *              vertex [vertex3.x, vertex3.y, vertex3.z]
 *          endloop
 *      endfacet
 *  end
 *  endsolid
 */

struct STLFaceInfo {
public:
    std::array<float, 3> normal;
    std::array<float, 3> vertex1;
    std::array<float, 3> vertex2;
    std::array<float, 3> vertex3;
    unsigned short attribute;

public:
    STLFaceInfo(): attribute(0) {}
};

enum STLFormat { UNKNOWN = -1, BINARY = 0, ASCII = 1 };

class DLL_API STLParser {
  public:
    static int read_from(const std::string &src_file_path, std::vector<STLFaceInfo> &info);

    static int write_to(const std::string &dst_file_path, const std::vector<STLFaceInfo> &info, STLFormat type = BINARY);
};