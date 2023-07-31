#include "Core/Core.h"
#include "Renderer/Renderer.h"
#include "Renderer/Font.h"
#include "Renderer/Text.h"
#include "Renderer/ModelManager.h"
#include "Input/InputSystem.h"
#include "Audio/AudioSystem.h"
#include "Framework/Scene.h"
#include "Player.h"
#include "Enemy.h"

#include "Framework/Emitter.h"
#include "Renderer/Particle.h"
#include "Renderer/ParticleSystem.h"

#include "SpazerWave.h"

#include <iostream>
#include <vector>
#include <thread>
#include <memory>

using namespace std;

using namespace kiko;//vec2 = kiko::Vector2;

class Star
{
public:
	Star(const vec2& pos, const vec2& vel) :
		m_pos{ pos }, 
		m_vel{ vel }
	{}

	void Update(int width, int height)
	{
		m_pos += m_vel * kiko::g_time.getDeltaTime();
		if (m_pos.x >= width) m_pos.x = 0;
		if (m_pos.y >= height) m_pos.y = 0;
	}

	void Draw(kiko::Renderer& renderer)
	{
		renderer.DrawPoint(m_pos.x, m_pos.y);
	}

public:
	vec2 m_pos;
	vec2 m_vel;
};



int main(int argc, char* argv[])
{
	
	kiko::MemoryTracker::Initialize();
	
	kiko::seedRandom((unsigned int)time(nullptr));
	kiko::setFilePath("assets");

	kiko::g_renderer.Initialize();
	kiko::g_renderer.CreateWindow("CSC196", 800, 600);

	//kiko::InputSystem inputSystem;
	kiko::g_inputSystem.Initialize();
	kiko::g_audioSystem.Initialize();

	unique_ptr<SpazerWave> game = make_unique<SpazerWave>();
	game->Initialize();

	vector<Star> stars;
	for (int i = 0; i < 10; i++) {
		vec2 pos(kiko::random(kiko::g_renderer.GetWidth()), kiko::random(kiko::g_renderer.GetHeight()));
		vec2 vel(kiko::randomf(100, 200), 0.0f);

		stars.push_back(Star(pos, vel));
	}

	// main game loop
	bool quit = false;
	while (!quit)
	{
		kiko::g_time.Tick();
		kiko::g_audioSystem.Update();
		kiko::g_inputSystem.Update();
		kiko::g_particleSystem.Update(kiko::g_time.getDeltaTime());

		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_ESCAPE))
		{
			quit = true;
		}

		//update game

		game->Update(kiko::g_time.getDeltaTime());

		kiko::g_renderer.SetColor(0, 0, 0, 0);
		kiko::g_renderer.BeginFrame();

		// draw
		kiko::g_particleSystem.Draw(kiko::g_renderer);

		for (auto& star : stars)
		{
			star.Update(kiko::g_renderer.GetWidth(), kiko::g_renderer.GetHeight());

			kiko::g_renderer.SetColor(255, 255, 255, 255);

			star.Draw(kiko::g_renderer);


		}

		game->Draw(kiko::g_renderer);
		kiko::g_renderer.EndFrame();

		
	}

	stars.clear();
	

	kiko::MemoryTracker::DisplayInfo();

	return 0;
}
