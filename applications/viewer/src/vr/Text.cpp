#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <vr/Text.h>
#include <vector>
#include <vr/FileSystem.h>
#include <vr/Shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vr/glErrorUtil.h>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
}

#include <iostream>
#include <map>

using namespace vr;

class TextInitializer
{
private:
  struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
  };


  bool m_initialized;
  std::map<GLchar, Character> m_characters;

  std::shared_ptr<vr::Shader> m_shader;

  GLuint m_vao;
  GLuint m_vbo;
  
  typedef std::vector<std::string> StringVector;
  StringVector m_fontPaths;

public:

  TextInitializer() : m_initialized(false), m_vao(0), m_vbo(0)
  {
    initFontPath();
  }

  std::string findInPath(TextInitializer::StringVector& paths, const std::string& filename)
  {
    for (auto& it : paths)
    {
      std::string path = vr::FileSystem::convertToNativeFilePath(it + "/" + filename);
      bool exist = vr::FileSystem::exists(path);
      if (exist)
        return path;

    }
    return "";
  }


  std::string getFontPath(const std::string& fontName)
  {
    return findInPath(m_fontPaths, fontName);
  }


  std::shared_ptr<Shader> shader()
  {
    return m_shader;
  }

  void init()
  {
    if (m_initialized)
      return;

    initFreeType();

    m_shader = std::make_shared<vr::Shader>("shaders/text.vs", "shaders/text.fs");

    // -----------------------------------
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_initialized = true;
  }

  void render(int width, int height, const std::string& text, const glm::vec2& position, glm::vec4& color, float scale)
  {
    init();

    glm::vec2 pos = position * glm::vec2(width, height);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    shader()->use();
    glUniformMatrix4fv(glGetUniformLocation(shader()->program(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activate corresponding render state	
    glUniform4f(glGetUniformLocation(shader()->program(), "textColor"), color.x, color.y, color.z, color.w);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_vao);

    // iterate through all characters
    for (auto c : text)
    {
      Character ch = m_characters[c];

      float xpos = pos.x + ch.Bearing.x * scale;
      float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // update VBO for each character
      float vertices[6][4] = {
          { xpos,     ypos + h,   0.0f, 0.0f },
          { xpos,     ypos,       0.0f, 1.0f },
          { xpos + w, ypos,       1.0f, 1.0f },

          { xpos,     ypos + h,   0.0f, 0.0f },
          { xpos + w, ypos,       1.0f, 1.0f },
          { xpos + w, ypos + h,   1.0f, 0.0f }
      };

      // render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      // update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      pos.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    CHECK_GL_ERROR_LINE_FILE();

  }

  void initFreeType()
  {
    if (m_initialized)
      return;

    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
      std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
      return;
    }

    const std::string default_font = "arial.ttf";
    // find path to font
    std::string font_name = getFontPath(default_font);
    if (font_name.empty())
    {
      std::cerr << "ERROR::FREETYPE: Failed to load " << default_font << std::endl;
      return;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
      std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
      return;
    }
    else {
      // set size to load glyphs as
      FT_Set_Pixel_Sizes(face, 0, 48);

      // disable byte-alignment restriction
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      // load first 128 characters of ASCII set
      for (unsigned char c = 0; c < 128; c++)
      {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
          std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
          continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RED,
          face->glyph->bitmap.width,
          face->glyph->bitmap.rows,
          0,
          GL_RED,
          GL_UNSIGNED_BYTE,
          face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        m_characters.insert(std::pair<char, Character>(c, character));
      }
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
  }

  void initFontPath()
  {
#if defined(WIN32)
    m_fontPaths.push_back(".");
    m_fontPaths.push_back("c:/winnt/fonts");
    m_fontPaths.push_back("c:/windows/fonts");
    m_fontPaths.push_back(getenv("VR_PATH") + std::string("/fonts"));

    char* ptr = getenv("windir");
    if (ptr)
    {
      std::string winFontPath = ptr;
      winFontPath += "\\fonts";
      m_fontPaths.push_back(winFontPath);
    }
#elif defined(__APPLE__)
    m_fontPaths.push_back(".");
    m_fontPaths.push_back("/usr/share/fonts/ttf");
    m_fontPaths.push_back("/usr/share/fonts/ttf/western");
    m_fontPaths.push_back("/usr/share/fonts/ttf/decoratives");
    m_fontPaths.push_back("/Library/Fonts:/System/Library/Fonts");
    m_fontPaths.push_back(getenv("VR_PATH") + std::string("/fonts"));
#else
    m_fontPaths.push_back(".");
    m_fontPaths.push_back("/usr/share/fonts/ttf");
    m_fontPaths.push_back("/usr/share/fonts/ttf/western");
    m_fontPaths.push_back("/usr/share/fonts/ttf/decoratives");
    m_fontPaths.push_back("/usr/share/fonts/truetype/freefont");
    m_fontPaths.push_back(getenv("VR_PATH") + std::string("/fonts"));
#endif

  }

};

TextInitializer s_textInitializer;




Text::Text(const std::string& text, const glm::vec2& position, const glm::vec4& color, float scale) : 
  m_text(text), m_position(position), m_color(color), m_scale(scale)
{
}

Text::~Text()
{

}

void Text::setText(const std::string& text)
{
  m_text = text;
}

const std::string& Text::getText() const
{
  return m_text;
}

void Text::setColor(const glm::vec4& color)
{
  m_color = color;
}

void Text::setPosition(const glm::vec2& position)
{
  m_position = position;
}

void Text::setScale(float scale)
{
  m_scale = scale;
}

void Text::render(int width, int height)
{
  s_textInitializer.render(width, height, m_text, m_position, m_color, m_scale); 
}

