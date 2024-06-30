#include "pch.h"
#include "Game.h"


//#define EMSCRIPTEN

#ifdef EMSCRIPTEN
#include <GLES3/gl3.h>
#include <GLFW/emscripten_glfw3.h>
#include <emscripten/emscripten.h>

#else
#include <glad/glad.h>
#endif

#include <Platform/Windows/WindowsWindow.h>
#include <Events/Events.h>
#include "Input/Input.h"

#include "Graphics/Renderer.h"

#include "Game/GameLayer.h"

Game& Game::Instance()
{
	static Game instance;
	return instance;
}

Game::Game() 
{
}


void Game::Init()
{
    Log::Init();
    
    window = std::make_unique<WindowsWindow>(WindowProps { 800 , 600, "Cowboy3i" });

    window->KeyPressedEventHandler += [](KeyPressedEventArg& arg)
        {
            if (arg.Key == Input::KEY_ESCAPE)
                Game::Instance().Shutdown();
            if (arg.Key == Input::KEY_V)
                Game::Instance().GetWindow()->SetVsync(!Game::Instance().GetWindow()->GetVsync());
        };
    window->WindowCloseEventHandler += std::bind(&Game::OnWindowClose, this, std::placeholders::_1);

    Renderer::Init();

    layerStack.PushLayer(new GameLayer());

    running = true;
}

void Game::Start()
{
    for (Layer* layer : layerStack)
    {
        layer->OnBegin();
    }

    //emscripten_set_main_loop(this->Loop, 60, GLFW_FALSE);
    // This is the render loop
    while (running)
    {
        Iterate();
    }
}

void Game::Loop()
{
#ifdef EMSCRIPTEN
    if (!iter())
    {
        m_Window->Shutdown();
        emscripten_cancel_main_loop();
    }
#endif
}

bool Game::Iterate()
{
    if (!running)
        return false;

    // Calculate FPS logic
    {
        auto timeNow = std::chrono::system_clock::now();
        delta = ((std::chrono::duration<double>)(timeNow - previousTime)).count();
        deltaCummulative += timeNow - previousTime;
        previousTime = timeNow;

        if (deltaCummulative.count() >= 1.0f)
        {
            deltaCummulative--;
            LOG_CORE_INFO("FPS: {}", fps);
            fps = 0;
        }
        fps++;
    }

    for (Layer* layer : layerStack)
    {
        layer->OnUpdate(delta);
    }

    window->OnUpdate();

    return true;
}

void Game::Shutdown()
{
    running = false;
}

void Game::OnWindowClose(WindowCloseEventArg arg)
{
    LOG_CORE_INFO("Window Closing...");
    Shutdown();
}
