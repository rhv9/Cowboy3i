#include "pch.h"
#include "FreeCameraController.h"

#include "Graphics/Camera.h"

#include "Input/Input.h"
#include "Game.h"

FreeCameraController::FreeCameraController(const float aspectRatio, const float zoomLevel)
	: aspectRatio(aspectRatio), zoomLevel(zoomLevel), bounds({ -aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel }), camera(bounds.Left, bounds.Right, bounds.Bottom, bounds.Top)
{
	Game::Instance().GetWindow()->MouseScrolledEventHandler += EVENT_BIND_MEMBER_FUNCTION(FreeCameraController::OnMouseScrollCallback);
	Game::Instance().GetWindow()->WindowResizeEventHandler += EVENT_BIND_MEMBER_FUNCTION(FreeCameraController::OnWindowResizeCallback);
	
	// Camera dragging
	Game::Instance().GetWindow()->MouseButtonPressedEventHandler += EVENT_BIND_MEMBER_FUNCTION(FreeCameraController::OnMousePressedCallback);
	Game::Instance().GetWindow()->MouseButtonReleasedEventHandler += EVENT_BIND_MEMBER_FUNCTION(FreeCameraController::OnMouseReleasedCallback);
	Game::Instance().GetWindow()->MouseMoveEventHandler += EVENT_BIND_MEMBER_FUNCTION(FreeCameraController::OnMouseMoveCallback);

	SetAspectRatio((float)Game::Instance().GetWindow()->GetWidth() / (float)Game::Instance().GetWindow()->GetHeight());
}

void FreeCameraController::SetPosition(const glm::vec2& pos)
{
	m_Position = pos;
	camera.SetPosition({ m_Position , 0.0f });
}

void FreeCameraController::OnResize(uint32_t width, uint32_t height)
{
	SetAspectRatio((float)width / (float)height);
}

void FreeCameraController::OnUpdate(Timestep ts)
{
	glm::vec2 move{ 0.0f };

	if (Input::IsKeyPressed(Input::KEY_W))
		move.y += zoomLevel * ts;
	if (Input::IsKeyPressed(Input::KEY_S))
		move.y -= zoomLevel * ts;
	if (Input::IsKeyPressed(Input::KEY_D))
		move.x += zoomLevel * ts;
	if (Input::IsKeyPressed(Input::KEY_A))
		move.x -= zoomLevel * ts;

	if (Input::IsKeyPressed(Input::KEY_LEFT))
		rotation -= rotationSpeed * ts;
	if (Input::IsKeyPressed(Input::KEY_RIGHT))
		rotation += rotationSpeed * ts;

	this->SetPosition(glm::vec3{ m_Position + move, 0.0f });
}


void FreeCameraController::CalculateView()
{
	bounds = { -aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel };
	camera.SetProjectionMatrix(bounds.Left, bounds.Right, bounds.Bottom, bounds.Top);
}

static glm::vec2 mousePressedPoint;
static glm::vec2 initialCameraPos;
static bool mouseHeld = false;

void FreeCameraController::OnMousePressedCallback(MouseButtonPressedEventArg& e)
{
	if (e.Button == Input::MOUSE_BUTTON_LEFT)
	{
		mousePressedPoint = Input::GetMousePosition();
		initialCameraPos = GetPosition();
		mouseHeld = true;
	}
}

void FreeCameraController::OnMouseReleasedCallback(MouseButtonReleasedEventArg& e)
{
	mouseHeld = false;
}

void FreeCameraController::OnMouseMoveCallback(MouseMoveEventArg& e)
{
	uint32_t windowWidth = Game::Instance().GetWindow()->GetWidth();
	uint32_t windowHeight = Game::Instance().GetWindow()->GetHeight();

	//LOG_CORE_TRACE("Window width, height: ({},{})", windowWidth, windowHeight);
	//LOG_CORE_TRACE("Bounds: ({},{})", bounds.GetWidth(), bounds.GetHeight());
	//LOG_CORE_TRACE("MousePos: ({},{})", Input::GetMousePosition().x, Input::GetMousePosition().y);


	if (mouseHeld)
	{
		glm::vec2 mousePos = Input::GetMousePosition();

		glm::vec2 offset = mousePos - mousePressedPoint;
		
		glm::vec2 offsetPercentage;
		offsetPercentage.x = offset.x / windowWidth;
		offsetPercentage.y = offset.y / windowHeight;

		//offset.x = -offset.x;
		glm::vec2 result;
		// how does multiplying this by half make it work????
		result.x = offsetPercentage.x * bounds.GetWidth() * 0.5f;
		result.y = offsetPercentage.y * bounds.GetHeight() * 0.5f;

		result.x = -result.x;
		SetPosition(initialCameraPos + result);	
	}
}

void FreeCameraController::OnMouseScrollCallback(MouseScrolledEventArg& e)
{
	SetZoomLevel(zoomLevel + -e.YOffset * 0.1f * zoomLevel);
	LOG_CORE_INFO("Zoom Level: {}", zoomLevel);
}

void FreeCameraController::OnWindowResizeCallback(WindowResizeEventArg& e)
{
	SetAspectRatio((float)e.Width / (float)e.Height);
}

