#include "Scene.hpp"

#include <iostream>

Scene::Scene()
{

}

Scene::~Scene()
{
    for (unsigned int i = 0; i < _meshes.size(); ++i)
        delete _meshes[i];
    _meshes.clear();

    for (unsigned int i = 0; i < _textures.size(); ++i)
        delete _textures[i];
    _textures.clear();

    for (unsigned int i = 0; i < _models.size(); ++i)
        delete _models[i];
    _models.clear();
}

void Scene::AddObject(const std::string& mesh, const std::string& texOut, mat4x4 transform)
{
    _meshes.push_back(new Mesh());
    Mesh* m = _meshes[_meshes.size()-1];
    if (!m->Load(mesh)) std::cout << "MESH ERROR" << std::endl;

    _textures.push_back(new Texture());
    Texture* tO = _textures[_textures.size()-1];
    if (!tO->Load(texOut)) std::cout << "TEXTURE ERROR" << std::endl;

    _models.push_back(new Model(m, tO, tO));
    _models[_models.size()-1]->ApplyTransform(transform);
    _models[_models.size()-1]->Freeze();
}

void Scene::AddCuttableObject(const std::string& mesh, const std::string& texOut, const std::string& texIn, mat4x4 transform)
{
    _meshes.push_back(new Mesh());
    Mesh* m = _meshes[_meshes.size()-1];
    if (!m->Load(mesh)) std::cout << "MESH ERROR" << std::endl;

    _textures.push_back(new Texture());
    Texture* tO = _textures[_textures.size()-1];
    if (!tO->Load(texOut)) std::cout << "TEXTURE ERROR" << std::endl;

    _textures.push_back(new Texture());
    Texture* tI = _textures[_textures.size()-1];
    if (!tI->Load(texIn)) std::cout << "TEXTURE ERROR" << std::endl;

    _models.push_back(new Model(m, tO, tI));
    _models[_models.size()-1]->ApplyTransform(transform);
}

void Scene::SliceAll(vec3 point, vec3 normal, float separation)
{
    for (unsigned int i = 0; i < _models.size(); ++i)
        _models[i]->Slice(point, normal, separation);
}

void Scene::RenderAll(GLint program)
{
    for (unsigned int i = 0; i < _models.size(); ++i)
        _models[i]->Render(program);
}

void Scene::RenderAllWireframe(GLint program)
{
    for (unsigned int i = 0; i < _models.size(); ++i)
    {
        if (!_models[i]->IsFrozen())
        _models[i]->RenderWireframe(program);
    }
}

void Scene::UpdateAllPhysics(double dt)
{
    for (unsigned int i = 0; i < _models.size(); ++i)
        _models[i]->UpdatePhysics(dt);
}

void Scene::ResetAll()
{
    for (unsigned int i = 0; i < _models.size(); ++i)
        _models[i]->Reset();

    for (unsigned int i = 0; i < _meshes.size(); ++i)
        _meshes[i]->Reset();
}