#ifndef MESHLOADER_H
#define MESHLOADER_H
#include <vector>
namespace Visual {
class Model;
class MeshData;
class MeshLoader
{
public:
    static std::vector<MeshData> load(const char* path);
};
}

#endif // MESHLOADER_H
