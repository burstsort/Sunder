#ifndef SCENE
#define SCENE

#include <vector>
#include <string>
#include "linmath.h"

#include "Model.hpp"

class Scene
{
    public:
        Scene();
        ~Scene();

        void AddObject(const std::string& mesh, const std::string& texOut, mat4x4 transform);
        void AddCuttableObject(const std::string& mesh, const std::string& texOut, const std::string& texIn, mat4x4 transform);

        void SliceAll(vec3 point, vec3 normal, float separation = 1.0f);
        void RenderAll(GLint program);
        void RenderAllWireframe(GLint program);
        void UpdateAllPhysics(double dt);

        void ResetAll();

    private:
        std::vector<Model*> _models;
        std::vector<Mesh*> _meshes;
        std::vector<Texture*> _textures;
};

#endif //SCENE