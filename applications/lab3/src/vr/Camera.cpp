// Important: Import glad as first gl header
#include <glad/glad.h>
#include <vr/Camera.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtx/io.hpp>
#include <glm/gtc/constants.hpp>
#include <sstream>

using namespace vr;

Camera::Camera() : 
m_firstClick(true),
m_speed(3.0f),
m_fov(50),
m_horizontalAngle(glm::pi<float>()),
m_verticalAngle(0.0f),
m_mouseSpeed(1),
m_lastTime(0),
m_speedup(1)
{
	m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	m_defDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_defPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	m_position = glm::vec3(0.0f, -1.0f, 0.0f);
	m_defPosition = glm::vec3(0.0f, -1.0f, 0.0f);

  m_screenSize[0] = 1280;
  m_screenSize[1] = 720;

  m_nearFar = glm::vec2(0.1, 100);
	
	// glfwGetTime is called only once, the first time this function is called
	m_lastTime = (float)glfwGetTime();
}

void Camera::set(glm::vec3 eye, glm::vec3 direction, glm::vec3 up)
{
	m_position = m_defPosition = eye;
	m_direction = m_defDirection = direction;
	m_up = m_defUp = up;
	m_verticalAngle = acos(sqrt(m_direction[2]));
	m_verticalAngle = asin(m_direction[1]);
}

void Camera::setTransform(glm::mat4 transform)
{
	glm::vec3 scale, translation, skew;
	glm::vec4 perspective;
	glm::quat direction;
	glm::decompose(transform, scale, direction, translation, skew, perspective);

	m_position = translation;
	m_up = glm::vec3(0, 1, 0) * direction;
	m_direction = glm::vec3(0.0f, 0.0f, -1.0f) * direction;

	m_verticalAngle = acos(sqrt(m_direction[2]));
	m_verticalAngle = asin(m_direction[1]);
}

void Camera::setPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 Camera::getPosition() const
{
	return m_position;
}

void Camera::setDefaultView(glm::vec3 pos, glm::vec3 dir, glm::vec3 up)
{
	set(pos, dir, up);
}

void Camera::resetView()
{
	m_position = m_defPosition;
	m_direction = m_defDirection;
	m_up = m_defUp;
}

void Camera::handleMouse(GLFWwindow *window, double xpos, double ypos)
{
	const float mouseSpeedMultiplier = 0.005f; // Just to be able to use 1 as a good default value.

	double xDiff = xpos-m_lastX + m_screenSize[0] / 2;
	double yDiff = ypos-m_lastY + m_screenSize[1] / 2;

	m_lastX = xpos;
	m_lastY = ypos;

	// Compute new orientation
	m_horizontalAngle += m_mouseSpeed * mouseSpeedMultiplier * float(m_screenSize[0] / 2 - xDiff);
	m_verticalAngle += m_mouseSpeed * mouseSpeedMultiplier * float(m_screenSize[1] / 2 - yDiff);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	m_direction = glm::vec3(
		cos(m_verticalAngle) * sin(m_horizontalAngle),
		sin(m_verticalAngle),
		cos(m_verticalAngle) * cos(m_horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(m_horizontalAngle - glm::pi<float>() / 2.0f),
		0,
		cos(m_horizontalAngle - glm::pi<float>() / 2.0f)
	);

	m_up = glm::cross(right, m_direction);
}	

void Camera::processInput(GLFWwindow* window)
{
	// Compute time difference between current and last frame
	float currentTime = (float)glfwGetTime();
	float deltaTime = float(currentTime - m_lastTime);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(m_horizontalAngle - glm::pi<float>() / 2.0f),
		0,
		cos(m_horizontalAngle - glm::pi<float>() / 2.0f)
	);

	// Speedup with left shift
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		m_speedup = 3;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		m_speedup = 1;
	}

	// Press down of the left mouse button
	if (m_firstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glfwGetCursorPos(window, &m_lastX, &m_lastY);

		m_firstClick = false;

		glfwSetWindowUserPointer(window, this);

		glfwSetCursorPosCallback( window, [](GLFWwindow *window, double x, double y)
		{
    	if (Camera *camera = static_cast<Camera*>(glfwGetWindowUserPointer(window)))
        camera->handleMouse(window, x, y);
		} );
	}

	// Released the left mouse button
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_firstClick = true;
		glfwSetCursorPosCallback( window, nullptr);

	}

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		m_position += m_direction * deltaTime * m_speed * m_speedup;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		m_position -= m_direction * deltaTime * m_speed * m_speedup;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		m_position += right * deltaTime * m_speed * m_speedup;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		m_position -= right * deltaTime * m_speed * m_speedup;
	}
	// Up
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		m_position += m_up * deltaTime * m_speed * m_speedup;
	}
	// Down
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		m_position -= m_up * deltaTime * m_speed * m_speedup;
	}

	// For the next frame, the "last time" will be "now"
	m_lastTime = currentTime;
}

glm::uvec2 Camera::getScreenSize()
{
  return m_screenSize;
}

void Camera::setScreenSize(const glm::uvec2& size)
{
  m_screenSize = size;
}


void Camera::setNearFar(const glm::vec2& nearFar)
{
  m_nearFar = nearFar;
}

glm::vec2 Camera::getNearFar(void)
{
	return m_nearFar;
}

void Camera::apply(std::shared_ptr<vr::Shader> shader)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(m_position, m_position + m_direction, m_up);
	
	float aspect = (float)m_screenSize[0] / (float)m_screenSize[1];

	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(m_fov), aspect, m_nearFar[0], m_nearFar[1]);

	shader->setMat4("v", view);
	shader->setMat4("p", projection);
	glm::mat4 v_inv = glm::inverse(view);
	//shader->setMat4("v_inv", v_inv);
}

void Camera::setFOV(float fov)
{
	m_fov = fov;
}

float Camera::getFOV() const
{
	return m_fov;
}

void Camera::setMouseSpeed(float speed)
{
	m_mouseSpeed = speed;
}

float Camera::getMouseSpeed() const
{
	return m_mouseSpeed;
}

void Camera::setSpeed(float speed)
{
	m_speed = speed;
}

float Camera::getSpeed() const
{
	return m_speed;
}
