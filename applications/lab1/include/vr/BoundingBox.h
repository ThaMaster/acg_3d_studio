#pragma once

namespace vr
{
  /// Simple class to store and handle a AABB (Axis aligned bounding box)
  class BoundingBox
  {
  public:
    BoundingBox(const glm::vec3& min, const glm::vec3& max) : m_max(max), m_min(min)
    {
    }

    BoundingBox() : m_max(-1E30f), m_min(1E30f)
    {
    }

    BoundingBox operator*(const glm::mat4& mat) const
    {
      BoundingBox box;

      box.m_min = mat * glm::vec4(m_min, 1);
      box.m_max = mat * glm::vec4(m_max, 1);
      return box;
    }

    void expand(const glm::vec3& v)
    {
      m_min = glm::min(m_min, v);
      m_max = glm::max(m_max, v);
    }

    void expand(const BoundingBox& other)
    {
      m_min = glm::min(m_min, other.min());
      m_max = glm::max(m_max, other.max());
    }

    glm::vec3 getCenter()
    {
      return (m_max - m_min) * 0.5f + m_min;
    }

    float getRadius()
    {
      return glm::length((m_max - m_min)) * 0.5f;
    }

    const glm::vec3& min() const { return m_min; }
    const glm::vec3& max() const { return m_max; }

  private:
    glm::vec3 m_min, m_max, m_center;
  };

}
