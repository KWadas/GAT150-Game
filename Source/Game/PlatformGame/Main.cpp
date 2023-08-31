#include "PlatformGame.h"
#include "Core/Core.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Audio/AudioSystem.h"
#include "Framework/Framework.h"
#include "Physics/PhysicsSystem.h"

#include "Framework/Resource/ResourceManager.h"

#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <array>
#include <map>
#include <functional>

using namespace std;

using namespace kiko;

int main(int argc, char* argv[])
{

	INFO_LOG("Initialize Engine...");

	kiko::MemoryTracker::Initialize();
	kiko::seedRandom((unsigned int)time(nullptr));
	kiko::setFilePath("Assets/PlatformGame");

	kiko::g_renderer.Initialize();
	kiko::g_renderer.CreateWindow("GAT150", 1280, 720);

	kiko::g_inputSystem.Initialize();
	kiko::g_audioSystem.Initialize();
	kiko::PhysicsSystem::Instance().Initialize();

	// create game
	unique_ptr<PlatformGame> game = make_unique<PlatformGame>();
	game->Initialize();

	// main game loop
	bool quit = false;
	while (!quit)
	{
		// update engine
		kiko::g_time.Tick();
		kiko::g_audioSystem.Update();
		kiko::g_inputSystem.Update();

		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_ESCAPE))
		{
			quit = true;
		}

		kiko::g_particleSystem.Update(kiko::g_time.getDeltaTime());
		kiko::PhysicsSystem::Instance().Update(kiko::g_time.getDeltaTime());

		//update game
		game->Update(kiko::g_time.getDeltaTime());

		kiko::g_renderer.SetColor(0, 0, 0, 0);
		kiko::g_renderer.BeginFrame();
		game->Draw(kiko::g_renderer);

		// draw
		kiko::g_particleSystem.Draw(kiko::g_renderer);
		kiko::g_renderer.EndFrame();
	}

	return 0;
}