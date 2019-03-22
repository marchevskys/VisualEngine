# VisualEngine
Realtime ender engine

# VisualEngine
Realtime render engine

What it can do?
Realtime render of open world.

Features implemented:
- Index-based LOD;
- Parallel split cascaded shadow maps;

Features to do:
- Lod for shadow maps;
- Occlustion and view frustum culling;
- Texturing;
- Ambient Occlusion

How to?

Create mesh data. MeshData contains arrays for shader.
vi::MeshDataPrimitive::sphere sphereData(40); //generated mesh
auto meshData = vi::MeshLoader::load("torus2.obj"); // from file

Create mesh. Mesh is a wrapper over Vertex Array Object in OpenGL.
Mesh creates by MeshData or array of meshDatas (each of them is LOD level);
std::vector<vi::MeshData> sphereData;
sphereData.emplace_back(vi::MeshDataPrimitive::sphere(40));
sphereData.emplace_back(vi::MeshDataPrimitive::sphere(20));
sphereData.emplace_back(vi::MeshDataPrimitive::sphere(10));
sphereData.emplace_back(vi::MeshDataPrimitive::sphere(5));
sphereData.emplace_back(vi::MeshDataPrimitive::sphere(2));
auto sphereMesh = std::make_shared<vi::Mesh>(sphereData);

Create PBR Material:
vi::MaterialPBR> material(vi::Color{0.5, 0.02, 0.1});

Scene - model container and observer.
Nothing specific in constructor yet.

Model - visual model. Consists of mesh, material and transform.
// create material and mesh shared
auto torusMesh = std::make_shared<vi::Mesh>(vi::MeshLoader::load("torus2.obj"));
auto torusMaterial = std::make_shared<vi::MaterialPBR>(vi::Color{0.5, 0.02, 0.1});

//create transform for this model
glm::dmat4 torusTransform = glm::translate(glm::dmat4(1), glm::dvec3(0, -1.5, 0.7));
vi::Model torusModel(&scene, torusMesh, torusMaterial, glm::value_ptr(torusTransform));

RenderData - subclass of Scene, contains shadow cascade (will contain pipeline as well).

ShadowCascade - cascade shadow map, pased on Nvidia GPU gems code.  https://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf

Render
Render::draw(Scene&, Camera&, IFrameBuffer&);
draw(what scene, by which camera, where? screen or other frameBuffer)

How it looks like:
https://www.instagram.com/p/BtrihCJIDC-/
