#pragma once

#include <vr/Scene.h>

namespace vr
{
  /// Load a given file and add content to the scene
  std::shared_ptr<Node> load3DModelFile(const std::string& filename);

  // Load contents of an xml file into the scene
  bool loadSceneFile(const std::string& xmlFile, std::shared_ptr<Scene>& scene);
}