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
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>


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

void parseNodes(aiNode* root_node, MaterialVector& materials, std::stack<glm::mat4>& transformStack, std::shared_ptr<Node>& node, const aiScene* aiScene)
{

  glm::mat4 transform = assimpToGlmMatrix(root_node->mTransformation);

  glm::mat4 m = transformStack.top() * transform;
  transformStack.push(m);

  uint32_t num_meshes = root_node->mNumMeshes;

  Mesh::vec3Vector tangents;
  Mesh::vec2Vector tex_coords;

  for (uint32_t i = 0; i < num_meshes; i++)
  {
    std::shared_ptr<Mesh> loadedMesh(new Mesh());

    aiMesh* mesh = aiScene->mMeshes[root_node->mMeshes[i]];
    uint32_t num_vertices = mesh->mNumVertices;

    loadedMesh->vertices.resize(num_vertices);
    loadedMesh->normals.resize(num_vertices);
    loadedMesh->texCoords.resize(num_vertices);

    //tangents.resize(num_vertices);

    for (uint32_t j = 0; j < num_vertices; j++)
    {
      loadedMesh->vertices[j] = glm::vec4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1);
      loadedMesh->normals[j] = glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);

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

        loadedMesh->texCoords[j] = tex_coord;
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
    loadedMesh->elements.resize(0);
    for (uint32_t j = 0; j < num_faces; j++)
    {
      aiFace face = mesh->mFaces[j];
      uint32_t num_indices = face.mNumIndices;
      for (uint32_t k = 0; k < num_indices; k++)
      {
        loadedMesh->elements.push_back(face.mIndices[k]);
      }
    }

    loadedMesh->object2world = transformStack.top();

    if (!materials.empty())
      loadedMesh->setMaterial(materials[mesh->mMaterialIndex]);

    node->add(loadedMesh);
  }

  for (uint32_t i = 0; i < root_node->mNumChildren; i++)
  {
    parseNodes(root_node->mChildren[i], materials, transformStack, node, aiScene);
  }
  transformStack.pop();
}

std::shared_ptr<Node> vr::load3DModelFile(const std::string& filename)
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

  std::shared_ptr<Node> node = std::shared_ptr<Node>(new Node);
  parseNodes(root_node, materials, transformStack, node, aiScene);
  transformStack.pop();

  if (node->getMeshes().empty())
    std::cerr << " File " << filepath << " did not contain any mesh data" << std::endl;

  return node;
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
    // Iterate over the nodes
    for (rapidxml::xml_node<>* node_node = root_node->first_node("node"); node_node; node_node = node_node->next_sibling())
    {
      xmlpath.push_back("node");

      if (node_node->type() == rapidxml::node_comment || node_node->type() == rapidxml::node_doctype)
        continue;


      std::string name = getAttribute(node_node, "name");

      rapidxml::xml_node<>* file = node_node->first_node("file");
      if (!file)
        throw std::runtime_error("Missing file: " + pathToString(xmlpath));

      xmlpath.push_back("file");

      std::string path = file->first_attribute("path")->value();
      if (path.empty())
        throw std::runtime_error("Empty path: " + pathToString(xmlpath));
      xmlpath.pop_back(); // file

      // Do we have a transform?
      rapidxml::xml_node<>* transform = node_node->first_node("transform");
      if (transform) {
        xmlpath.push_back("transform");


        std::string translate = getAttribute(transform, "translate");
        glm::vec3 t_vec;
        if (!getVec<glm::vec3>(t_vec, translate))
          throw std::runtime_error("Node (" + name + ") Invalid translate in: " + pathToString(xmlpath));

        std::string rotate = getAttribute(transform, "rotate");
        glm::vec3 r_vec;
        if (!getVec<glm::vec3>(r_vec, rotate))
          throw std::runtime_error("Node (" + name + ") Invalid rotate in: " + pathToString(xmlpath));

        std::string scale = getAttribute(transform, "scale");
        glm::vec3 s_vec;
        if (!getVec<glm::vec3>(s_vec, scale, glm::vec3(1)))
          throw std::runtime_error("Node (" + name + ") Invalid scale in: " + pathToString(xmlpath));

        // Now create the node
        std::shared_ptr<Node> loadedNode = vr::load3DModelFile(path);
        if (!loadedNode)
          std::cerr << "Unable to load node \'" << name << "\' path: " << path << std::endl;
        else
        {
          glm::mat4 mt = glm::translate(glm::mat4(), t_vec);
          glm::mat4 ms = glm::scale(glm::mat4(), s_vec);
          glm::mat4 rx = glm::rotate(glm::mat4(), glm::radians(r_vec.x), glm::vec3(1, 0, 0));
          glm::mat4 ry = glm::rotate(glm::mat4(), glm::radians(r_vec.y), glm::vec3(0, 1, 0));
          glm::mat4 rz = glm::rotate(glm::mat4(), glm::radians(r_vec.z), glm::vec3(0, 0, 1));

          auto t = mt * rz * ry * rx;
          t = glm::scale(t, s_vec);
          loadedNode->setInitialTransform(t);
          loadedNode->name = name;
          scene->add(loadedNode);
        }

        xmlpath.pop_back(); // transform
      }


      xmlpath.pop_back(); // node
    }
    xmlpath.pop_back(); // scene
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

