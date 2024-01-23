#include <vr/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vr/FileSystem.h>
#include <vr/glErrorUtil.h>
using namespace vr;


bool Texture::create(const char* image, unsigned int slot, GLenum texType, GLenum pixelType) 
{
	if (m_valid)
		cleanup();

	std::string filepath = image;

	bool exist = vr::FileSystem::exists(filepath);

	std::string vrPath = vr::FileSystem::getEnv("VR_PATH");
	if (vrPath.empty())
		std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;

	if (!exist && !vrPath.empty())
	{
		filepath = std::string(vrPath) + "/" + filepath;
		exist = vr::FileSystem::exists(filepath);
	}
	if (!exist)
	{
		std::cerr << "Unable to locate image: " << image << std::endl;
		return false;
	}

	m_textureSlot = slot;

	// Assigns the type of the texture of the texture object
	m_type = texType;

	GLenum texFormat = GL_RGBA;

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(filepath.c_str(), &widthImg, &heightImg, &numColCh, 0);
	if (!bytes) {
		std::cerr << "Error reading image: " << image << std::endl;
		return false;
	}

	if (numColCh == 3)
		texFormat = GL_RGB;

	// Generates an OpenGL texture object
	glGenTextures(1, &m_id);

	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(texType, m_id);

	CHECK_GL_ERROR_LINE_FILE();

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Assigns the image to the OpenGL Texture object
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, texFormat, pixelType, bytes);
	// Generates MipMaps
	glGenerateMipmap(texType);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);

	m_valid = true;
	return true;
}

Texture::Texture() : m_id(0), m_type(0), m_valid(false), m_textureSlot(0)
{
}

Texture::~Texture()
{
	cleanup();
}

bool Texture::isValid()
{
	return m_valid;
}

void Texture::texUnit(GLuint program, const char* uniform, unsigned int unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(program, uniform);

	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + m_textureSlot);

	if (m_valid)
		glBindTexture(m_type, m_id);
}

void Texture::unbind()
{
	if (m_valid)
		glBindTexture(m_type, m_id);
}

void Texture::cleanup()
{

	if (m_valid)
		glDeleteTextures(1, &m_id);

	m_valid = false;
}

