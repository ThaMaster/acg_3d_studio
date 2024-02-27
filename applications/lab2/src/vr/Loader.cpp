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

#include "lab2/callbacks/Callbacks.h"

#include <vr/Light.h>

using namespace vr;

std::string findTexture(const std::string& diffuseTexPath, const std::string& modelPath)
{
  bool found = vr::FileSystem::exists(diffuseTexPath);
  if (found)
    return diffuseTexPath;

  std::string newPath;

  std::string vrpath = vr::FileSystem::getEnv("VR_PATH");

  // Search in the same path as the model:
  std::string modelDir = vr::FileSystem::getDirectory(modelPath);
  newPath = modelDir + "/" + diffuseTexPath;
  found = vr::FileSystem::exists(newPath);
  if (found)
    return newPath;

  // What if we only keep the last directory name+filename?

  auto directories = vr::FileSystem::splitPath(diffuseTexPath);
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
  newPath = vrpath + "/" + diffuseTexPath;

  found = vr::FileSystem::exists(newPath);
  if (found)
    return newPath;


  return ""; // Unable to find
}

/**
 * @brief Function for extracting all the materials that is provided from
 *        the loaded models file. If the model does not have any stated
 *        materials, ignore the default materials from the Assimp loader.
 * 
 * @param scene     The aiScene object provided from the Assimp loader.
 * @param materials The material vector that will be filled with the extracted
 *                  materials.                 
 * @param modelPath The path to the model where the materials should be stated.
 * 
 * @return size_t
 */
size_t ExtractMaterials(const aiScene* scene, MaterialVector& materials, const std::string modelPath)
{
  uint32_t num_materials = scene->mNumMaterials;
  aiMaterial* ai_material;
  aiColor4D color(0.0f, 0.0f, 0.0f, 1.0f);
  GLfloat shiniess;
  GLfloat opacity = 1.0f;
  aiString path;
  for (uint32_t i = 0; i < num_materials; i++)
  {
    int texUnit = 1;
    std::shared_ptr<Material> material(new Material);
    
    ai_material = scene->mMaterials[i];
    std::string mat_name = ai_material->GetName().C_Str();
    if(mat_name == "DefaultMaterial" || mat_name == "(null)") {
      materials.push_back(nullptr);
      continue; 
    }

    ai_material->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material->setAmbient(glm::vec4(color.r, color.g, color.b, color.a));

    ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material->setDiffuse(glm::vec4(color.r, color.g, color.b, color.a));

    ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material->setSpecular(glm::vec4(color.r, color.g, color.b, color.a));

    //ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
    //material->setAmbient(glm::vec4(color.r, color.g, color.b, color.a));

    if(ai_material->Get(AI_MATKEY_SHININESS, shiniess) == AI_SUCCESS)
        material->setShininess(shiniess);
  

    if(ai_material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
      material->setOpacity(opacity);
    } else {
      material->setOpacity(1.0f);
    }

    if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
      aiString res("res\\");
      ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

      std::string diffuseTexPath = findTexture(path.C_Str(), modelPath);
      if (diffuseTexPath.empty())
      {
        std::cerr << "Unable to find diffuse texture: " << path.C_Str() << std::endl;
      }
      else {
        std::shared_ptr<vr::Texture> texture = std::make_shared<vr::Texture>();
        if (!texture->create(diffuseTexPath.c_str(), texUnit))
          std::cerr << "Error creating texture: " << diffuseTexPath << std::endl;
        else {
          material->setTexture(texture, texUnit);
          texUnit++;
        }
      }
    }

    if (ai_material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
      aiString res("res\\");
      path.Clear();
      ai_material->GetTexture(aiTextureType_SPECULAR, 0, &path);
      std::string specularTexPath = findTexture(path.C_Str(), modelPath);
      if (specularTexPath.empty())
      {
        std::cerr << "Unable to find specular texture: " << path.C_Str() << std::endl;
      }
      else {
        std::shared_ptr<vr::Texture> texture = std::make_shared<vr::Texture>();
        if (!texture->create(specularTexPath.c_str(), 2))
          std::cerr << "Error creating texture: " << specularTexPath << std::endl;
        else {
          material->setTexture(texture, 2);
        }
      }
    }

    if (ai_material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
      aiString res("res\\");
      path.Clear();
      ai_material->GetTexture(aiTextureType_NORMALS, 0, &path);
      std::string heightPath = findTexture(path.C_Str(), modelPath);
      if (heightPath.empty())
      {
        std::cerr << "Unable to find specular texture: " << path.C_Str() << std::endl;
      }
      else {
        std::shared_ptr<vr::Texture> texture = std::make_shared<vr::Texture>();
        if (!texture->create(heightPath.c_str(), 0))
          std::cerr << "Error creating texture: " << heightPath << std::endl;
        else {
          material->setTexture(texture, 0);
        }
      }
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
 * @brief Function for parsing the nodes provided when loading
 *        a model from a file using the Assimp Loader. It will
 *        create a group node where all the sub-meshes of the
 *        object are stored as geomerty nodes.
 * 
 * @param root_node       The root node of the model.
 * @param materials       The material vector where all the materials will be stored.
 * @param transformStack  A stack of transformatoin matrices.
 * @param objNode         The group node that represents the whole object.
 * @param aiScene         The aiScene object provieded form the Assimp loader.
 */
void parseNodes(aiNode* root_node, MaterialVector& materials, std::stack<glm::mat4>& transformStack, Group& objNode, const aiScene* aiScene, std::shared_ptr<Scene>& scene)
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
    if(mesh->HasTangentsAndBitangents()) {
      loadedGeo->tangents.resize(num_vertices);
    }

    for (uint32_t j = 0; j < num_vertices; j++)
    {
      loadedGeo->vertices[j] = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1);
      loadedGeo->normals[j] = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);

      if (mesh->HasTangentsAndBitangents())
      {
        loadedGeo->tangents[j] = glm::vec3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z);
      }

      if (mesh->mTextureCoords[0])
        loadedGeo->texCoords[j] = glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
      

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

    loadedGeo->initShaders(scene->getDefaultShader());
    loadedGeo->upload();
    objNode.addChild(loadedGeo);
  }

  for (uint32_t i = 0; i < root_node->mNumChildren; i++)
  {
    parseNodes(root_node->mChildren[i], materials, transformStack, objNode, aiScene, scene);
  }
  transformStack.pop();
}

/**
 * @brief Function for handling all loading of 3D model files. Since
 *        the program utilizes the Assimp loader, multiple different
 *        model file formats are supported.
 * 
 * @param filename  The name of the model file.
 * @param objName   The name of the geometry node.
 * @param scene     The scene object.
 * 
 * @return Group*
 */
Group* vr::load3DModelFile(const std::string& filename, const std::string& objName, std::shared_ptr<Scene>& scene)
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
  parseNodes(root_node, materials, transformStack, *objNode, aiScene, scene);
  transformStack.pop();

  if (objNode->getChildren().empty())
    std::cerr << " File " << filepath << " did not contain any mesh data" << std::endl;
  return objNode;
}

template<class T>
T readValue(const std::string& string) {
  std::stringstream ss;
  ss << string;
  T result;
  ss >> result;
  return result;
}

std::string pathToString(std::vector<std::string>& path)
{
  std::string result;
  for (auto s : path)
    result = result + "/" + s;

  return result;
}

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
 * @brief Function for parsing a state node containing
 *        light nodes. It will go through all the values
 *        and parse them accordingly. The function will
 *        only add ambient and/or attentuatoin factors if
 *        they are found, otherwise it will use the default
 *        values.
 * 
 * @param node The xml node representing the light.
 * 
 * @return std::shared_ptr<Light> 
 */
std::shared_ptr<Light> parseStateLight(rapidxml::xml_node<>* node)
{
  std::string light_name = node->name();
  std::shared_ptr<Light> newLight(new Light);

  std::string ambient = getAttribute(node, "ambient");
  if(!ambient.empty()) {
    glm::vec4 am_vec;
    if (!getVec<glm::vec4>(am_vec, ambient))
      throw std::runtime_error("Light(" + light_name + "): Invalid ambient.\n");
    newLight->setAmbient(am_vec);
  }

  std::string diffuse = getAttribute(node, "diffuse");
  glm::vec4 d_vec;
  if (!getVec<glm::vec4>(d_vec, diffuse))
    throw std::runtime_error("Light(" + light_name + "): Invalid diffuse value.\n");

  std::string specular = getAttribute(node, "specular");
  glm::vec4 s_vec;
  if (!getVec<glm::vec4>(s_vec, specular))
    throw std::runtime_error("Light(" + light_name  + "): Invalid specular value.\n");

  std::string position = getAttribute(node, "position");
  glm::vec4 p_vec;
  if (!getVec<glm::vec4>(p_vec, position))
    throw std::runtime_error("Light(" + light_name  + "). Invalid position.\n ");

  std::string attenuation = getAttribute(node, "attenuation");
  if(!attenuation.empty()) {
    glm::vec3 a_vec;
    if (!getVec<glm::vec3>(a_vec, attenuation))
      throw std::runtime_error("Light(" + light_name + "): Invalid attenuation.\n");
    newLight->setAttenuation(a_vec.x, a_vec.y, a_vec.z);
  }
  newLight->setDiffuse(d_vec);
  newLight->setSpecular(s_vec);
  newLight->setPosition(p_vec);

  return newLight;
}

/**
 * @brief Function for parsing a xml node that represents
 *        a UpdateCallback node. Since there can be multiple
 *        different functions for a single update node,
 *        iterate through all the siblings and parse them accordingly.
 * 
 * @param node The node representing the update node.
 * 
 * @return std::vector<UpdateCallback*> 
 */
std::vector<UpdateCallback*> parseNodeCallbacks(rapidxml::xml_node<>* node)
{
  std::vector<UpdateCallback*> callbacks;
  for(node = node->first_node(); node; node = node->next_sibling())
  {
    if (!node || node->type() == rapidxml::node_comment || node->type() == rapidxml::node_doctype)
        continue;

    std::string node_type = node->name();
    if(node_type == "rotate") {
      
      float speed = atof(getAttribute(node, "speed").c_str());

      std::string rotate = getAttribute(node, "axis");
      glm::vec3 axis;
      if (!getVec<glm::vec3>(axis, rotate))
        throw std::runtime_error("Update(" + node_type + "): Invalid rotation axis.\n");
      
      callbacks.push_back(new RotateCallback(speed, axis));
    } else if(node_type == "translate") {
      float speed = atof(getAttribute(node, "speed").c_str());

      std::string translate = getAttribute(node, "axis");
      glm::vec3 axis;
      if (!getVec<glm::vec3>(axis, translate))
        throw std::runtime_error("Update(" + node_type + "): Invalid rotation axis.\n");
      
      callbacks.push_back(new TranslateCallback(speed, axis));
    } else if(node_type == "lightColor") {

      std::string specular = getAttribute(node, "specular");
      glm::vec3 s_vec;
      if (!getVec<glm::vec3>(s_vec, specular))
        throw std::runtime_error("Update(" + node_type + "): Invalid specular value.\n");

      std::string diffuse = getAttribute(node, "diffuse");
      glm::vec3 d_vec;
      if (!getVec<glm::vec3>(d_vec, diffuse))
        throw std::runtime_error("Update(" + node_type + "): Invalid diffuse value.\n");

      callbacks.push_back(new LightColorCallback(s_vec, d_vec));
    } else {
      std::cout << "Unknown node type: \'" << node_type << "\'" << std::endl;
    }
  }
  return callbacks;
}

/**
 * @brief Function for parsing a xml node that represents
 *        a shader node in the state. This function must
 *        contain both a path to the Vertex Shader and
 *        the Fragment shader so if only one of the shaders
 *        wants to be changed, simply supply the same path
 *        to the other one. Otherwise it will cause an error
 *        to occur.
 * 
 * @param node  The xml node representing a shader node.
 * @param scene The scene object.
 * 
 * @return std::shared_ptr<Shader> 
 */
std::shared_ptr<Shader> parseStateShader(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string node_type = node->name();
  rapidxml::xml_attribute<>* vShaderAttrib = node->first_attribute("vPath");
  rapidxml::xml_attribute<>* fShaderAttrib = node->first_attribute("fPath");
  if(!vShaderAttrib)
    throw std::runtime_error("State(" + node_type + "): Missing vertex shader attribute!.\n");
  std::string vShader_path = vShaderAttrib->value();
  if (vShader_path.empty())
    throw std::runtime_error("State(" + node_type + "): Empty path to vertex shader.\n");
  if(!fShaderAttrib)
    throw std::runtime_error("State(" + node_type + "): Missing fragment shader attribute!.\n");
  std::string fShader_path = fShaderAttrib->value();
  if (fShader_path.empty())
    throw std::runtime_error("State(" + node_type + "): Empty path to fragment shader.\n");

  auto shader = std::shared_ptr<vr::Shader>(new Shader(vShader_path, fShader_path));
  if(!shader->valid())
    throw std::runtime_error("ERROR: Invalid shader!.\n");

  return shader;
}

/**
 * @brief Function for parsing a xml node that represents
 *        a texture inside a state node. It will simply
 *        check for the path to the texture and create a 
 *        texture from the file. It will then return the
 *        created texture.
 * 
 * @param node  The xml node representing a texture.
 * @param scene The scene object.
 * 
 * @return std::shared_ptr<Texture> 
 */
std::shared_ptr<Texture> parseTexture(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene, int texUnit)
{
  std::shared_ptr<vr::Texture> texture;
  std::string diffuseTexPath = getAttribute(node, "path");
  if (diffuseTexPath.empty()) {
    std::cerr << "Unable to find texture: Path Empty!" << std::endl;
  } else {
    texture = std::make_shared<vr::Texture>();
    if (!texture->create(diffuseTexPath.c_str(), texUnit))
      std::cerr << "Error creating texture: " << diffuseTexPath << std::endl;
  }
  return texture;
}

/**
 * @brief Function for parsing a xml node that represents
 *        a material inside a state node. It will iterate
 *        through all the attributes and set them accordingly.
 *        If any of them are missing, use the default value
 *        instead.
 * 
 * @param node  The xml node representing a material.
 * @param scene The scene object.
 * 
 * @return std::shared_ptr<Material> 
 */
std::shared_ptr<Material> parseStateMaterial(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string node_type = node->name();
  std::shared_ptr<vr::Material> material(new Material());

  std::string ambient = getAttribute(node, "ambient");
  if(!ambient.empty()) {
    glm::vec3 amb_vec;
    if (getVec<glm::vec3>(amb_vec, ambient))
      material->setAmbient(glm::vec4(amb_vec, 1));
  }

  std::string diffuse = getAttribute(node, "diffuse");
  if(!diffuse.empty()) {
    glm::vec3 diff_vec;
    if (getVec<glm::vec3>(diff_vec, diffuse))
      material->setDiffuse(glm::vec4(diff_vec, 1));
  }

  std::string specular = getAttribute(node, "specular");
  if(!specular.empty()) {
    glm::vec3 spec_vec;
    if (getVec<glm::vec3>(spec_vec, specular))
      material->setSpecular(glm::vec4(spec_vec, 1));
  }
  
  std::string shininess = getAttribute(node, "shininess");
  if(!shininess.empty()) 
    material->setShininess(atof(shininess.c_str()));
  
  
  rapidxml::xml_node<>* texNode;
  int textUnit = 5;
  for(texNode = node->first_node("texture"); texNode; texNode = texNode->next_sibling("texture"))
  {
    std::string map_type = getAttribute(texNode, "mapType");
    if(!map_type.empty()) {
      if(map_type == "diffuse")
        material->setTexture(parseTexture(texNode, scene, 1), 1);
      else if(map_type == "specular")
        material->setTexture(parseTexture(texNode, scene, 2), 2);
      else if(map_type == "normal")
        material->setTexture(parseTexture(texNode, scene, 0), 0);
    } else {
      material->setTexture(parseTexture(texNode, scene, textUnit), textUnit);
      textUnit++;
    }
  }
  
  return material;
}

/**
 * @brief Function for parsing a xml node that represents
 *        a state. A state can contain two attributes and
 *        multiple other information such as which shader
 *        to use, texture and material. These are all parsed 
 *        accordingly if they are present inside the state node.
 * 
 * @param node The node representing a state.
 * 
 * @return std::shared_ptr<State> 
 */
std::shared_ptr<State> parseNodeState(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string state_name = getAttribute(node, "name");
  std::shared_ptr<State> newState(new State(state_name));

  std::string enableCullFace = getAttribute(node, "enableCullface");
  if(!enableCullFace.empty())
    newState->setCullFace(std::shared_ptr<bool>(new bool(enableCullFace == "1" || enableCullFace == "true")));

  std::string enableLight = getAttribute(node, "enableLight");
  if(!enableLight.empty())
    newState->setEnableLight(std::shared_ptr<bool>(new bool(enableLight == "1" || enableLight == "true")));

  int texUnit = 5;
  for(node = node->first_node(); node; node = node->next_sibling())
  {
    std::string node_type = node->name();
    if (!node || node->type() == rapidxml::node_comment || node->type() == rapidxml::node_doctype)
        continue;

    if(node_type == "light") {
      auto newLight = parseStateLight(node);
      newState->addLight(newLight);
      scene->addLight(newLight);
      scene->addLightMatrix(newLight->getLightMatrix());
      scene->getRTT()->addRenderTarget();
    } else if(node_type == "shaders") {
      newState->setShader(parseStateShader(node, scene));
    } else if(node_type == "texture") {
      newState->addTexture(parseTexture(node, scene, texUnit));
      texUnit++;
    } else if(node_type == "material") {
      newState->setMaterial(parseStateMaterial(node, scene));
    } else {
      std::cout << "Unknow node: \'"<< node->name() << "\'" << std::endl;
    }
  }

  return newState;
}

/**
 * @brief Function for adding potential states and
 *        UpdateCallbacks to a given node. If the
 *        xml node does not state any of these types
 *        of objects, simply do nothing, otherwise add
 *        them to the node.
 * 
 * @param graph_node  The node to add potential state and UpdateCallbacks.
 * @param xml_node    The xml node representing the node to update.
 * @param scene       The scene object.
 */
void addStateAndUpdate(Node& graph_node, rapidxml::xml_node<>* xml_node, std::shared_ptr<Scene>& scene)
{
  rapidxml::xml_node<>* stateNode = xml_node->first_node("state");
  if(stateNode) {
    auto newState = parseNodeState(stateNode, scene);
    if(!graph_node.getState()) {
      graph_node.setState(newState);
    } else {
      auto mergedState = graph_node.getState()->merge(newState);
      graph_node.setState(mergedState);
    }
  }
  rapidxml::xml_node<>* updateNode = xml_node->first_node("update");
  if(updateNode) {
    auto callbackVector = parseNodeCallbacks(updateNode);
    for(auto callback : callbackVector)
      graph_node.addUpdateCallback(callback);
  }
}

/**
 * @brief Function for parsing a xml node that represents a
 *        transformation node. All values such as translation,
 *        rotation and scaling will be attempted to parsed but
 *        if they are not present it will use vectors where
 *        each element are equal to 1.
 * 
 * @param node  The xml node representing a transform node.
 * @param scene The scene object. 
 * 
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
  auto t = mt * ms * rz * ry * rx;
  transformNode->setTransformMat(t);

  loadSceneGraph(node->first_node(), transformNode, scene);

  addStateAndUpdate(*transformNode, node, scene);
  return transformNode;
}

/**
 * @brief Function for parsing a single Group node from a given XML node.
 *        if the group node is the entire scene, check whether it contains
 *        any stated lights, if not signal the scene to add a default light
 *        to use, otherwise use the stated light instead. The group can 
 *        have states and UpdateCallbacks.
 * 
 * @param node  The xml node representing a group/scene node.
 * @param scene The scene object. 
 * @return Group* 
 */
Group* parseGroupNode(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string node_name = getAttribute(node, "name");
  auto groupNode = new Group(node_name);
  loadSceneGraph(node->first_node(), groupNode, scene);

  addStateAndUpdate(*groupNode, node, scene);

  std::string node_type = node->name();  
  return groupNode;
}

/**
 * @brief Function for parsing the xml node when a object is to be
 *        added to the scene graph. Since a object can contain multiple
 *        submeshes, create a group node and add geometries to the group 
 *        node for each submesh. Object node can have states and 
 *        UpdateCallbacks.
 * 
 * @param node    The xml node representing a Geometry node.
 * @param scene   The scene object.
 * 
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
    objNode = vr::load3DModelFile(geo_path, node_name, scene);
    scene->addObject(geo_path, objNode);
  } else {
    objNode = scene->getObject(geo_path);
  }
  if (!objNode)
    std::cerr << "Unable to load object \'" << node_name << "\' path: " << geo_path << std::endl;
  
  addStateAndUpdate(*objNode, node, scene);
  
  return objNode;
}

/**
 * @brief Function for parsing an xml node when a Level of Detail (LOD) 
 *        node should be created. It will create a LOD object and
 *        iterate through all the geometries that are inside the node
 *        and parse them accordingly. The LOD node can have states and
 *        UpdateCallbacks.
 * 
 * @param node  The xml node representing a LOD node.
 * @param scene The scene object.
 * 
 * @return LOD* 
 */
LOD* parseLodNode(rapidxml::xml_node<>* node, std::shared_ptr<Scene>& scene)
{
  std::string node_name = getAttribute(node, "name");
  auto lodNode = new LOD(node_name);
  float maxDistance = atof(getAttribute(node, "maxDistance").c_str());
  lodNode->setMaxRenderDistance(maxDistance);
  std::string lodChildType;
  for(rapidxml::xml_node<>* geo_node = node->first_node(); geo_node; geo_node = geo_node->next_sibling())
  {
    lodChildType = geo_node->name();
    if (!geo_node || geo_node->type() == rapidxml::node_comment || geo_node->type() == rapidxml::node_doctype || lodChildType != "geometry")
        continue;

    lodNode->addObject(parseObjNode(geo_node, scene));
  }
  addStateAndUpdate(*lodNode, node, scene);
  lodNode->addObject(new Group("empty"));
  return lodNode;
}

/**
 * @brief Function for parsing the scene graph and connecting
 *        all the nodes togheter. It will iterate through the 
 *        file recursively and add potential states and
 *        UpdateCallbacks functions accordingly.
 * 
 * @param parent_node The xml node representing the parent node.
 * @param root        The scene graph node that is currently being parsed.
 * @param scene       The scene object.
 */
void vr::loadSceneGraph(rapidxml::xml_node<>* parent_node, Group* root, std::shared_ptr<Scene>& scene) {

  if (!parent_node || parent_node->type() == rapidxml::node_comment || parent_node->type() == rapidxml::node_doctype)
        return;
  
  std::string root_name = root->getName();
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
      root->addChild(transformNode);
      
    } else if(node_type == "lod") {

      auto lodNode = parseLodNode(curr_node, scene);
      root->addChild(lodNode);
    }
  }
}

/**
 * @brief Function for loading a scene from a provided XML file. If
 *        any errors are included in the scene, it will exit and
 *        provided an error message. Also, if the scene does not
 *        include a light, it will signal the scene to create a
 *        default light to use.
 * 
 * @param sceneFile The path to the scene file to load.
 * @param scene     The scene object.
 * 
 * @return true, if the scene was loaded successfully.
 * @return false, otherwise.
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
    if (root_node) {
      scene->setRootGroup(new Group("scene_root"));
      std::string useGround = getAttribute(root_node, "useGround");
      if(!useGround.empty())
        scene->setUseGroundPlane(useGround == "1" || useGround == "true");
      scene->setDefaultRootState(*scene->getRootGroup());
      addStateAndUpdate(*scene->getRootGroup(), root_node, scene);

    } else {
      throw std::runtime_error("File missing scene/");
    }

    xmlpath.push_back("scene");
    loadSceneGraph(root_node->first_node(), scene->getRootGroup(), scene);
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