#include "lab1/nodes/Geometry.h"

void Geometry::accept(NodeVisitor& v)
{
    std::cerr << "Accept: " << getName() << std::endl;
    v.visit(*this);
}

void Geometry::render(std::shared_ptr<vr::Shader> shader, glm::mat4 transformMat)
{
    std::cerr << "Rendering" << std::endl;
    for(auto m : m_meshes)
    {
        m->initShaders(shader);
        m->upload();
        m->render(shader, (this->object2world * transformMat));
    }
}

void Geometry::add(std::shared_ptr<vr::Mesh>& mesh)
{
    m_meshes.push_back(mesh);
}

vr::MeshVector& Geometry::getMeshes()
{
    return m_meshes;
}

void Geometry::setInitialTransform(const glm::mat4& m)
{
  object2world = m_initialTransform = m;
}

void Geometry::resetTransform()
{
  object2world = m_initialTransform;
}

glm::mat4& Geometry::getObject2WorldMat()
{
    return object2world;
}

void Geometry::setObject2WorldMat(glm::mat4& m)
{
    object2world = m;
}