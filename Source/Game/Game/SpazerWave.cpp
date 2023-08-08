#include "SpazerWave.h"

#include "Player.h"
#include "Enemy.h"

#include "Framework/Emitter.h"
#include "Renderer/Renderer.h"
#include "Renderer/ParticleSystem.h"

#include "Framework/Components/EnginePhysicsComponent.h"

#include "Framework/Resource/ResourceManager.h"
#include "Framework/Scene.h"
#include "Framework/Components/SpriteComponent.h"

#include "Audio/AudioSystem.h"
#include "Input/InputSystem.h"
#include "Renderer/Renderer.h"
#include "Renderer/Text.h"
#include "Renderer/ModelManager.h"

bool SpazerWave::Initialize()
{
	// create font / text objects
	//m_font = kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24);
	m_scoreText = std::make_unique<kiko::Text>(kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24));
	m_scoreText->Create(kiko::g_renderer, "SCORE 0000", kiko::Color{1, 0, 1, 1});

	m_explainText = std::make_unique<kiko::Text>(kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24));
	m_explainText->Create(kiko::g_renderer, "WATCH OUT: DIFFICULTY INCREASES OVER TIME!", kiko::Color{1, 0, 0, 1});

	m_timerText = std::make_unique<kiko::Text>(kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24));
	m_timerText->Create(kiko::g_renderer, "TIME: ", kiko::Color{1, 0, 1, 1});

	m_livesText = std::make_unique<kiko::Text>(kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24));
	m_livesText->Create(kiko::g_renderer, "  {}  ", kiko::Color{0, 1, 0, 1});

	m_titleText = std::make_unique<kiko::Text>(kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24));
	m_titleText->Create(kiko::g_renderer, "SPAZERWAVE", kiko::Color{1, 1, 0, 1});

	m_gameOverText = std::make_unique<kiko::Text>(kiko::g_resources.Get<kiko::Font>("ALBAS___.TTF", 24));
	m_gameOverText->Create(kiko::g_renderer, "GAME OVER", kiko::Color{1, 1, 1, 1});
	// load audio
	kiko::g_audioSystem.AddAudio("shoot", "shoot.wav");
	kiko::g_audioSystem.AddAudio("laser", "laser.wav");
	kiko::g_audioSystem.AddAudio("music", "music.wav");
	kiko::g_audioSystem.AddAudio("destroyed", "destroyed.wav");
	kiko::g_audioSystem.AddAudio("enemyDestroyed", "enemy_destroyed.wav");
	kiko::g_audioSystem.AddAudio("boost", "boost.wav");

	m_scene = std::make_unique<kiko::Scene>();

	kiko::g_audioSystem.PlayOneShot("music", true);

	return true;
}

bool SpazerWave::Shutdown()
{
	return false;
}

void SpazerWave::Update(float dt)
{
	switch (m_state)
	{
	case SpazerWave::eState::Title:
		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE)) {
			m_state = eState::StartGame;
		}
		break;
	case SpazerWave::eState::StartGame:
		m_score = 0;
		m_lives = 3;
		m_score = 0;
		m_gameTimer = 0;
		m_state = eState::StartLevel;
		break;
	case SpazerWave::eState::StartLevel:
		m_scene->RemoveAll();
	{
		// create player
		auto player = std::make_unique<Player>(20.0f, kiko::Pi, kiko::Transform{ {400, 300}, 0, 4 });
		player->m_tag = "Player";
		player->m_game = this;
		player->setLevelUpTime((int) m_gameTimer);

		//create components
		auto renderComponent = std::make_unique < kiko::SpriteComponent>();
		renderComponent->m_texture = kiko::g_resources.Get<kiko::Texture>("player_ship.png", kiko::g_renderer);
		player->AddComponent(std::move(renderComponent));

		auto physicsComponent = std::make_unique<kiko::EnginePhysicsComponent>();
		physicsComponent->m_damping = 0.9f;
		player->AddComponent(std::move(physicsComponent));

		m_scene->Add(std::move(player));

		m_state = eState::Game;
	}
		break;
	case SpazerWave::eState::Game:
	{
		
		m_gameTimer += dt;
		m_spawnTimer += dt;

		m_scene->GetActor<Player>()->LevelUp((int) m_gameTimer);
		
		std::unique_ptr<Enemy> enemy;
		if (m_spawnTimer >= m_spawnTime)
		{
			m_spawnTimer = 0;
			float n = kiko::randomf(0, 1.0f);
			if (n <= m_difficulty) {
				enemy = std::make_unique<Enemy>(kiko::randomf(175.0f, 250.0f), 1.0f, kiko::Pi / 3.0f, kiko::Transform{ {kiko::random(800), kiko::random(600)}, kiko::randomf(kiko::TwoPi), 3}, 200);
			}
			else {
				enemy = std::make_unique<Enemy>(kiko::randomf(75.0f, 150.0f), 2.0f, kiko::Pi, kiko::Transform{ {kiko::random(800), kiko::random(600)}, kiko::randomf(kiko::TwoPi), 5}, 100);
			}
			
			
			enemy->m_tag = "Enemy";
			enemy->m_game = this;

			auto renderComponent = std::make_unique < kiko::SpriteComponent>();
			renderComponent->m_texture = kiko::g_resources.Get<kiko::Texture>("player_ship.png", kiko::g_renderer);
			enemy->AddComponent(std::move(renderComponent));

			m_scene->Add(std::move(enemy));

			m_difficulty += 0.01f;
			m_spawnTime -= 0.01f;
		}

		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE) && !kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_SPACE))
		{
			kiko::g_audioSystem.PlayOneShot("shoot", false);
		}

		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_E) && kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_E))
		{
			kiko::g_audioSystem.PlayOneShot("laser", false);
		}

		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_LSHIFT) && kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_LSHIFT))
		{
			kiko::g_audioSystem.PlayOneShot("boost", false);
		}

	}
		
		break;

	case SpazerWave::eState::PlayerDeadStart:
		m_stateTimer = 3;
		if (m_lives == 0) m_state = eState::GameOver;
		else m_state = eState::PlayerDead;
		break;

	case SpazerWave::eState::PlayerDead:
		m_stateTimer -= dt;
		if (m_stateTimer <= 0) {
			m_state = eState::StartLevel;
		}
		break;

	case SpazerWave::eState::GameOver:
		m_stateTimer -= dt;
		if (m_stateTimer <= 0) {
			m_state = eState::Title;
		}
		break;
	default:
		break;
	}

	m_scoreText->Create(kiko::g_renderer, "SCORE: " + std::to_string(m_score), {1, 1, 1, 1});
	m_timerText->Create(kiko::g_renderer, "TIME: " + std::to_string((int) m_gameTimer), { 1, 1, 1, 1 });

	m_scene->Update(dt);
}

void SpazerWave::Draw(kiko::Renderer& renderer)
{
	if (m_state == eState::Title)
	{
		m_titleText->Draw(renderer, 180, 250);
		m_explainText->Draw(renderer, 140, 550);
	}
	if (m_state == eState::GameOver)
	{
		m_gameOverText->Draw(renderer, 250, 300);
	}
	if (m_state != eState::Title) {
		m_scoreText->Draw(renderer, 40, 40);
		m_timerText->Draw(renderer, 360, 40);
		for (int i = 0; i < kiko::Game::GetLives(); i++) {
			m_livesText->Draw(renderer, 640 + (40*i), 30);
		}
		
	}
	

	m_scene->Draw(renderer);
}


/*kiko::EmitterData data;
		data.burst = true;
		data.burstCount = 100;
		data.spawnRate = 200;
		data.angle = 25;
		data.angleRange = kiko::Pi;
		data.lifetimeMin = 0.5f;
		data.lifetimeMin = 1.5f;
		data.speedMin = 50;
		data.speedMax = 250;
		data.damping = 0.5f;
		data.color = kiko::Color{ 1, 1, 0, 1 };
		kiko::Transform transform{ { kiko::g_inputSystem.GetMousePosition() }, 0, 1 };
		auto emitter = std::make_unique<kiko::Emitter>(transform, data);
		emitter->m_lifespan = 1.0f;
		m_scene->Add(std::move(emitter));*/