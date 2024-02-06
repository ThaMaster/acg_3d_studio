// Important: Import glad as first gl header
#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> 

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vr/FileSystem.h>
#include <vr/Loader.h>
#include <iostream>
#include <stack>
#include <vr/Scene.h>
#include <iostream>

#include <fstream>

#include "lab1/visitors/RenderVisitor.h"
#include "lab1/callbacks/RotateCallback.h"

#include <vr/Light.h>

using namespace vr;

std::string findTexture(const std::string& texturePath, const std::string& modelPath)
{
  bool found = vr::FileSystem::exists(texturePath);
  if (found)
    return texturePath;

  std::string newPath;

  std::string vrpath = vr::FileSystem::getEnv("VR_PATH");

  // Search in the same path as the model:
  std::string modelDir = vr::FileSystem::getDirectory(modelPath);
  newPath = modelDir + "/" + texturePath;
  found = vr::FileSystem::exists(newPath);
  if (found)
    return newPath;

  // What if we only keep the last directory name+filename?

  auto directories = vr::FileSystem::splitPath(texturePath);
  auto count = directories.size();
  if (count > 1) {
    newPath = directories[count - 2] + "/" + directories[count - 1];
    found = vr::FileSystem::exists(newPath);
    if (found)
      return newPath;

    // Try add model path also:
    newPath = modelDir + "/" + newPath;
    found = vr::FileSystem::exists(newPath);
    if (found)
      return newPath;

    // Search using the VR_PATH environment variable then.
    newPath = vrpath + "/" + newPath;

    found = vr::FileSystem::exists(newPath);
    if (found)
      return newPath;
  }

  // Search using the VR_PATH environment variable then.
  newPath = vrpath + "/" + texturePath;

  found = vr::FileSystem::exists(newPath);
  if (found)
    return newPath;


  return ""; // Unable to find
}

/**
 * @brief 
 * 
 * @param scene 
 * @param materials 
 * @param modelPath 
 * @return size_t 
 */
size_t ExtractMaterials(const aiScene* scene, MaterialVector& materials, const std::string modelPath)
{
  uint32_t num_materials = scene->mNumMaterials;
  aiMaterial* ai_material;
  aiColor4D color(0.0f, 0.0f, 0.0f, 1.0f);
  GLfloat shiniess;
  aiString path;

  for (uint32_t i = 0; i < num_materials; i++)
  {
    std::shared_ptr<Material> material(new Material);

    ai_material = scene->mMaterials[i];

    ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material->setAmbient(glm::vec4(color.r, color.g, color.b, color.a));

    ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material->setDiffuse(glm::vec4(color.r, color.g, color.b, color.a));

    ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material->setSpecular(glm::vec4(color.r, color.g, color.b, color.a));

    //ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
    //material->setAmbient(glm::vec4(color.r, color.g, color.b, color.a));

    ai_material->Get(AI_MATKEY_SHININESS, shiniess);
    material->setShininess(shiniess);

    if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
      aiString res("res\\");
      ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

      std::string texturePath = findTexture(path.C_Str(), modelPath);
      if (texturePath.empty())
      {
        std::cerr << "Unable to find texture: " << path.C_Str() << std::endl;
      }
      else {
        std::shared_ptr<vr::Texture> texture = std::make_shared<vr::Texture>();
        if (!texture->create(texturePath.c_str(), 0))
          std::cerr << "Error creating texture: " << texturePath << std::endl;
        else
          material->setTexture(texture, 0);
      }
    }

    if (ai_material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
      aiString res("res\\");
      path.Clear();
      ai_material->GetTexture(aiTextureType_SPECULAR, 0, &path);
    }

    if (ai_material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    {
      aiString res("res\\");
      path.Clear();
      ai_material->GetTexture(aiTextureType_HEIGHT, 0, &path);
    }

    if (ai_material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
    {
      aiString res("res\\");
      path.Clear();
      ai_material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path);
    }
    materials.push_back(material);
  }

  return materials.size();
}

/**
 * @brief 
 * 
 * @param ai_matrix 
 * @return glm::mat4 
 */
glm::mat4 assimpToGlmMatrix(const aiMatrix4x4& ai_matrix)
{
  glm::mat4 glm_matrix;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      glm_matrix[i][j] = ai_matrix[j][i];
    }
  }

  return glm_matrix;
}

/**
 * @brief 
 * 
 * @param root_node 
 * @param materials 
 * @param transformStack 
 * @param objNode 
 * @param aiScene 
 */
void parseNodes(aiNode* root_node, MaterialVector& materials, std::stack<glm::mat4>& transformStack, Group& objNode, const aiScene* aiScene)
{

  glm::mat4 transform = assimpToGlmMatrix(root_node->mTransformation);

  glm::mat4 m = transformStack.top() * transform;
  transformStack.push(m);

  uint32_t num_meshes = root_node->mNumMeshes;

  std::vector<glm::vec3> tangents;
  std::vector<glm::vec2> tex_coords;

  for (uint32_t i = 0; i < num_meshes; i++)
  {
    auto loadedGeo = new Geometry(objNode.getName() + "_" + std::to_string(i));

    aiMesh* mesh = aiScene->mMeshes[root_node->mMeshes[i]];
    uint32_t num_vertices = mesh->mNumVertices;

    loadedGeo->vertices.resize(num_vertices);
    loadedGeo->normals.resize(num_vertices);
    loadedGeo->texCoords.resize(num_vertices);

    //tangents.resize(num_vertices);

    for (uint32_t j = 0; j < num_vertices; j++)
    {
      loadedGeo->vertices[j] = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1);
      loadedGeo->normals[j] = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);

      glm::vec3 tangent;
      if (mesh->HasTangentsAndBitangents())
      {
        tangent.x = mesh->mTangents[j].x;
        tangent.y = mesh->mTangents[j].y;
        tangent.z = mesh->mTangents[j].z;
      }

      glm::vec2 tex_coord;
      if (mesh->mTextureCoords[0])
      {
        tex_coord.x = mesh->mTextureCoords[0][j].x;
        tex_coord.y = mesh->mTextureCoords[0][j].y;

        loadedGeo->texCoords[j] = tex_coord;
      }

      // Ignore color
      //       if (mesh->HasVertexColors(j))
      //       {
      //         vertex.color.x = mesh->mColors[j]->r;
      //         vertex.color.y = mesh->mColors[j]->g;
      //         vertex.color.z = mesh->mColors[j]->b;
      //       }

    }

    uint32_t num_faces = mesh->mNumFaces;
    loadedGeo->elements.resize(0);
    for (uint32_t j = 0; j < num_faces; j++)
    {
      aiFace face = mesh->mFaces[j];
      uint32_t num_indices = face.mNumIndices;
      for (uint32_t k = 0; k < num_indices; k++)
      {
        loadedGeo->elements.push_back(face.mIndices[k]);
      }
    }

    loadedGeo->setObject2WorldMat(transformStack.top());

    if (!materials.empty()) {
      std::shared_ptr<State> materialState(new State(loadedGeo->getName() + "_mat_state"));
      materialState->setMaterial(materials[mesh->mMaterialIndex]);
      loadedGeo->setState(materialState);
    }

    objNode.addChild(loadedGeo);
  }

  for (uint32_t i = 0; i < root_node->mNumChildren; i++)
  {
    parseNodes(root_node->mChildren[i], materials, transformStack, objNode, aiScene);
  }
  transformStack.pop();
}

/**
 * @brief 
 * 
 * @param filename 
 * @param objName 
 * @return Group* 
 */
Group* vr::load3DModelFile(const std::string& filename, const std::string& objName)
{
  std::string filepath = vr::FileSystem::findFile(filename);
  if (filepath.empty())
  {
    std::cerr << "The file " << filename << " does not exist" << std::endl;
    return nullptr;
  }

  MaterialVector materials;

  Assimp::Importer importer;
  const aiScene* aiScene = importer.ReadFile(filepath,
    aiProcess_CalcTangentSpace |
    aiProcess_GenSmoothNormals |
    aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType);

  if (!aiScene)
  {
    std::cerr << "Couldn't load model: " << filepath << " Error Importing Asset: " << importer.GetErrorString() << std::endl;
    return nullptr;
  }

  aiNode* root_node = aiScene->mRootNode;
  ExtractMaterials(aiScene, materials, filename);
  std::cout << "Found " << materials.size() << " materials" << std::endl;

  std::stack<glm::mat4> transformStack;
  transformStack.push(glm::mat4());

  auto objNode = new Group(objName);
  parseNodes(root_node, materials, transformStack, *objNode, aiScene);
  transformStack.pop();

  if (objNode->getChildren().empty())
    std::cerr << " File " << filepath << " did not contain any mesh data" << std::endl;

  return objNode;
}

/**
 * @brief 
 * 
 * @tparam T 
 * @param string 
 * @return T 
 */
template<class T>
T readValue(const std::string& string) {
  std::stringstream ss;
  ss << string;
  T result;
  ss >> result;
  return result;
}

/**
 * @brief 
 * 
 * @param path 
 * @return std::string 
 */
std::string pathToString(std::vector<std::string>& path)
{
  std::string result;
  for (auto s : path)
    result = result + "/" + s;

  return result;
}

/**
 * @brief 
 * 
 * @tparam ContainerT 
 * @param str 
 * @param tokens 
 * @param delimiters 
 * @param trimEmpty 
 */
template < class ContainerT >
void tokenize(const std::string& str, ContainerT& tokens,
  const std::string& delimiters = " ", bool trimEmpty = false)
{
  std::string::size_type pos, lastPos = 0, length = str.length();

  using value_type = typename ContainerT::value_type;
  using size_type = typename ContainerT::size_type;

  while (lastPos < length + 1)
  {
    pos = str.find_first_of(delimiters, lastPos);
    if (pos == std::string::npos)
    {
      pos = length;
    }

    if (pos != lastPos || !trimEmpty)
      tokens.push_back(value_type(str.data() + lastPos,
        (size_type)pos - lastPos));

    lastPos = pos + 1;
  }
}

/**
 * @brief Get the Vec object
 * 
 * @tparam T 
 * @param vec 
 * @param string 
 * @param def 
 * @return true 
 * @return false 
 */
template<class T>
bool getVec(T& vec, const std::string& string, const T& def = T())
{
  vec = def;

  std::vector<std::string> tokens;
  tokenize(string, tokens, " ", true);

  if (tokens.size() > 0 && tokens.size() != T::length())
    return false;

  int i = 0;
  for (auto v : tokens)
  {
    float val = readValue<float>(v);
    vec[i++] = val;
  }

  return true;
}

/**
 * @brief Get the Attribute object
 * 
 * @param node 
 * @param attribute 
 * @return std::string 
 */
std::string getAttribute(rapidxml::xml_node<>* node, const std::string& attribute)
{
  if (!node)
    return "";

  rapidxml::xml_attribute<>* attrib = node->first_attribute(attribute.c_str());
  if (!attrib)
    return "";

  return attrib->value();
}

/**
 * @brief 
 * 
 * @param node 
 * @return Transform* 
 */
Transform* parseTransformNode(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string node_name = getAttribute(node, "name");
  auto transformNode = new Transform(node_name);

  std::string translate = getAttribute(node, "translate");
  glm::vec3 t_vec;
  if (!getVec<glm::vec3>(t_vec, translate))
    throw std::runtime_error("Transform(" + node_name + "): Invalid translate.\n");

  std::string rotate = getAttribute(node, "rotate");
  glm::vec3 r_vec;
  if (!getVec<glm::vec3>(r_vec, rotate))
    throw std::runtime_error("Transform(" + node_name + "): Invalid rotate.\n");

  std::string scale = getAttribute(node, "scale");
  glm::vec3 s_vec;
  if (!getVec<glm::vec3>(s_vec, scale, glm::vec3(1)))
    throw std::runtime_error("Transform(" + node_name + "). Invalid scale.\n ");

  glm::mat4 mt = glm::translate(glm::mat4(), t_vec);
  glm::mat4 ms = glm::scale(glm::mat4(), s_vec);
  glm::mat4 rx = glm::rotate(glm::mat4(), glm::radians(r_vec.x), glm::vec3(1, 0, 0));
  glm::mat4 ry = glm::rotate(glm::mat4(), glm::radians(r_vec.y), glm::vec3(0, 1, 0));
  glm::mat4 rz = glm::rotate(glm::mat4(), glm::radians(r_vec.z), glm::vec3(0, 0, 1));
  auto t = mt * rz * ry * rx;
  t = glm::scale(t, s_vec);
  transformNode->setTransformMat(t);

  loadSceneGraph(node->first_node(), transformNode, scene);

  return transformNode;
}

/**
 * @brief Function for parsing a single Group node from a given XML node.
 * 
 * @param node The XML node.
 * @return Group* 
 */
Group* parseGroupNode(rapidxml::xml_node<>* node,std::shared_ptr<Scene>& scene)
{
  std::string node_name = getAttribute(node, "name");
  auto groupNode = new Group(node_name);
  loadSceneGraph(node->first_node(), groupNode, scene);
  return groupNode;
}

/**
 * @brief 
 * 
 * @param node 
 * @param scene 
 * @return Group* 
 */
Group* parseObjNode(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string node_name = getAttribute(node, "name");
  
  std::string geo_path = node->first_attribute("path")->value();
  if (geo_path.empty())
    throw std::runtime_error("Geometry(" + node_name + "): Empty path.\n");

  Group* objNode;
  if(!scene->objectExists(geo_path)) {
    objNode = vr::load3DModelFile(geo_path, node_name);
    scene->addObject(geo_path, objNode);
  } else {
    objNode = scene->getObject(geo_path);
  }
  if (!objNode)
    std::cerr << "Unable to load object \'" << node_name << "\' path: " << geo_path << std::endl;
    
  return objNode;
}

std::shared_ptr<State> parseNodeState(rapidxml::xml_node<>* node)
{
}

std::vector<UpdateCallback*> parseNodeUpdate(rapidxml::xml_node<>* node)
{
}

/**
 * @brief 
 * 
 * @param parent_node 
 * @param root 
 * @param scene 
 */
void vr::loadSceneGraph(rapidxml::xml_node<>* parent_node, Group* root, std::shared_ptr<Scene>& scene) {
  if (!parent_node || parent_node->type() == rapidxml::node_comment || parent_node->type() == rapidxml::node_doctype)
        return;
  
  std::string node_name;
  for(rapidxml::xml_node<>* curr_node = parent_node; curr_node; curr_node = curr_node->next_sibling()) {
    std::string node_type = curr_node->name();

    if(node_type == "group") {
      auto groupNode = parseGroupNode(curr_node, scene);
      root->addChild(groupNode);

    } else if(node_type == "geometry") {

      auto objNode = parseObjNode(curr_node, scene);
      root->addChild(objNode);

    } else if(node_type == "transform") {

      auto transformNode = parseTransformNode(curr_node, scene);
      transformNode->addUpdateCallback(new RotateCallback(1, glm::vec3(0,1,0)));

      // --- Example that state works! --- //
      std::shared_ptr<State> coolState(new State("green_lamp_state"));
      std::shared_ptr<Light> light(new Light);
      light->diffuse = glm::vec4(0.2, 1, 0.3, 1);
      light->specular = glm::vec4(1, 0.1, 0.65, 0.4);
      light->position = glm::vec4(0.0, -2.0, 2.0, 0.0);
      coolState->addLight(light);
      transformNode->setState(coolState);

      root->addChild(transformNode);
    } else if(node_type == "state") {

      auto newState = parseNodeState(curr_node);
      root->setState(newState);

    } else if(node_type == "update") {

      auto updateCallbacks = parseNodeUpdate(curr_node);

      for(auto c : updateCallbacks)
        root->addUpdateCallback(c);

    } else {
      std::cerr << "Unknown node found: \'" << node_type << "\'" << std::endl;
    }
  }
}

/**
 * @brief 
 * 
 * @param sceneFile 
 * @param scene 
 * @return true 
 * @return false 
 */
bool vr::loadSceneFile(const std::string& sceneFile, std::shared_ptr<Scene>& scene)
{
  std::string filepath = sceneFile;
  bool exist = vr::FileSystem::exists(filepath);

  std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
  if (vrPath.empty())
    std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

  if (!exist && !vrPath.empty())
  {
    filepath = std::string(vrPath) + "/" + sceneFile;
    exist = vr::FileSystem::exists(filepath);
  }

  if (!exist)
  {
    std::cerr << "The file " << sceneFile << " does not exist" << std::endl;
    return false;
  }

  rapidxml::xml_node<>* root_node = nullptr;
  std::vector<std::string> xmlpath;

  try
  { 
    rapidxml::file<> xmlFile(filepath.c_str()); // Default template is char
    rapidxml::xml_document<> doc;

    doc.parse<rapidxml::parse_trim_whitespace | rapidxml::parse_normalize_whitespace | rapidxml::parse_full>(xmlFile.data());

    root_node = doc.first_node("scene");
    if (!root_node)
      throw std::runtime_error("File missing scene/");

    xmlpath.push_back("scene");
    if(scene->getRootGroup()->getChildren().size() == 0){
      loadSceneGraph(root_node->first_node(), scene->getRootGroup(), scene);
    }
  }
  catch (rapidxml::parse_error& error)
  {
    std::cerr << "XML parse error: " << error.what() << std::endl;
    return false;
  }
  catch (std::runtime_error& error)
  {
    std::cerr << "XML parse error: " << error.what() << std::endl;
    return false;
  }

  return true;
}