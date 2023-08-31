#include "SpazerWave.h"
#include "Player.h"
#include "Enemy.h"

#include "Renderer/Renderer.h"

#include "Framework/Framework.h"

#include "Audio/AudioSystem.h"
#include "Input/InputSystem.h"

bool SpazerWave::Initialize()
{
	// create font / text objects
	m_font = GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 24);
	m_scoreText = std::make_unique<kiko::Text>(GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 24));
	m_scoreText->Create(kiko::g_renderer, "SCORE 0000", kiko::Color{1, 0, 1, 1});

	m_timerText = std::make_unique<kiko::Text>(GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 24));
	m_timerText->Create(kiko::g_renderer, "TIME: ", kiko::Color{1, 0, 1, 1});

	m_livesText = std::make_unique<kiko::Text>(GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 24));
	m_livesText->Create(kiko::g_renderer, "  {}  ", kiko::Color{0, 1, 0, 1});

	m_gameOverText = std::make_unique<kiko::Text>(GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 24));
	m_gameOverText->Create(kiko::g_renderer, "GAME OVER", kiko::Color{1, 1, 1, 1});
	// load audio
	kiko::g_audioSystem.AddAudio("shoot", "shoot.wav");
	kiko::g_audioSystem.AddAudio("laser", "laser.wav");
	kiko::g_audioSystem.AddAudio("music", "music.wav");
	kiko::g_audioSystem.AddAudio("destroyed", "destroyed.wav");
	kiko::g_audioSystem.AddAudio("enemyDestroyed", "enemy_destroyed.wav");
	kiko::g_audioSystem.AddAudio("boost", "boost.wav");

	// create scene
	m_scene = std::make_unique<kiko::Scene>();
	m_scene->Load("scenes/spacescene.json");
	m_scene->Initialize();
	kiko::g_audioSystem.PlayOneShot("music", true);

	// add events
	EVENT_SUBSCRIBE("OnAddPoints", SpazerWave::OnAddPoints);
	EVENT_SUBSCRIBE("OnPlayerDead", SpazerWave::OnPlayerDead);

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
			//m_scene->GetActorByName("Background")->active;
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
		//auto player = std::make_unique<Player>(20.0f, kiko::Pi, kiko::Transform{ {400, 300}, 0, 1.0f });
		auto player = INSTANTIATE(Player, "Player");
		player->transform = kiko::Transform{ {400, 300}, 0, 1.0f };
		player->Initialize();
		m_scene->Add(std::move(player));

		m_state = eState::Game;
	}
		break;

	case SpazerWave::eState::Game:
	{
		
		m_gameTimer += dt;
		m_spawnTimer += dt;
		
		
		if (m_spawnTimer >= m_spawnTime)
		{
			m_spawnTimer = 0;
			auto enemy = INSTANTIATE(Enemy, "Enemy");
			enemy->transform = kiko::Transform{ { kiko::random(1200), kiko::random(100) }, kiko::randomf(kiko::TwoPi), 1 };
			enemy->Initialize();
			m_scene->Add(std::move(enemy));
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
	m_scene->Draw(renderer);

	if (m_state == eState::Title)
	{
		m_scene->GetActorByName("Title")->active = true;
	}
	if (m_state == eState::GameOver)
	{
		m_gameOverText->Draw(renderer, 250, 300);
	}
	if (m_state != eState::Title) {
		m_scene->GetActorByName("Title")->active = false;
		m_scoreText->Draw(renderer, 40, 40);
		m_timerText->Draw(renderer, 360, 40);
		for (int i = 0; i < kiko::Game::GetLives(); i++) {
			m_livesText->Draw(renderer, 640 + (40*i), 30);
		}
		
	}
}

void SpazerWave::OnAddPoints(const kiko::Event& event)
{
	m_score += std::get<int>(event.data);

}

void SpazerWave::OnPlayerDead(const kiko::Event& event)
{
	m_lives--;
	m_state = eState::PlayerDeadStart;
}
