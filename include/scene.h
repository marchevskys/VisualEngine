#ifndef MODELCONTAINER_H
#define MODELCONTAINER_H
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace Visual {

template <class Inner, class Arg>
struct Group {
    std::vector<Inner> vec;
    const Arg *arg;
    Group(const Arg *ar) : arg(ar) {}
    Group() : arg(nullptr) {}
    ~Group(){};
    template <typename T>
    typename std::vector<Inner>::iterator find(const T *ar) {
        if (!vec.empty())
            return std::find_if(vec.begin(), vec.end(), [&ar](const Inner &v) { return ar == v.arg; });
        else
            return vec.end();
    }

    template <typename T>
    typename std::vector<Inner>::iterator insert(const T *ar) {
        auto it = find(ar); // find this shader in array
        if (it == vec.end()) {
            vec.emplace_back(Inner(ar));
            it = vec.end() - 1;
        }
        return it;
    }

    void triggerArg(std::function<void(Arg *)> func) { func(arg); }
    void forEvery(std::function<void(Inner &value)> func) {
        for (auto &v : vec)
            func(v);
    }
    void forEvery(std::function<void(const Inner &value)> func) const {
        for (const auto &v : vec)
            func(v);
    }
};

class IMaterial;
class Mesh;
class Shader3d;
class Model;

class Scene {
    typedef Group<Model *, IMaterial> MaterialGroup;
    typedef Group<MaterialGroup, Mesh> MeshGroup;
    typedef Group<MeshGroup, Shader3d> ShaderGroup;
    typedef Group<ShaderGroup, void> Container;

    std::unique_ptr<Container> m_data;

  public:
    friend class Renderer;

    Scene();
    Scene(Scene &&other);
    ~Scene() {}

    void addModel(Model *m);
    void removeModel(Model *m);
    void forEveryModel(std::function<void(Model *)> func);
    void forEveryModel(std::function<void(const Model *)> func) const;
};

} // namespace Visual
#endif // MODELCONTAINER_H
