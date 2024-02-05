#pragma once

#include <vr/Scene.h>

class Group;
class Geometry;
namespace vr
{
  /// Load a given file and add content to the scene
  Group* load3DModelFile(const std::string& filename, const std::string& objName);

  // Load contents of an xml file into the scene
  bool loadSceneFile(const std::string& xmlFile, std::shared_ptr<Scene>& scene);

  //Geometry* buildGeometry(std::string geo_name, vec4Vector vertices, GLushortVector indices, vec3Vector normals, vec2Vector texCoords);
}