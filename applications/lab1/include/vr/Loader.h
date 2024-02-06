#pragma once

#include <vr/Scene.h>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
class Group;
namespace vr
{
  /// Load a given file and add content to the scene
  Group* load3DModelFile(const std::string& filename, const std::string& objName);

  // Load contents of an xml file into the scene
  bool loadSceneFile(const std::string& xmlFile, std::shared_ptr<Scene>& scene);
  void loadSceneGraph(rapidxml::xml_node<>* parent_node, Group* root, std::shared_ptr<Scene>& scene);
}