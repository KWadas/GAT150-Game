#include "PlatformGame.h"
#include "Player.h"
#include "Enemy.h"
#include "GroundEnemy.h"
#include "Framework/Framework.h"
#include "Renderer/Renderer.h"
#include "Audio/AudioSystem.h"
#include "Input/InputSystem.h"

bool PlatformGame::Initialize()
{
	// create font / text objects
	m_font = GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 32);
	m_scoreText = std::make_unique<kiko::Text>(GET_RESOURCE(kiko::Font, "Fonts/ALBAS___.TTF", 24));
	m_scoreText->Create(kiko::g_renderer, "000", kiko::Color{ 0, 0, 0, 1 });
	// load audio
	kiko::g_audioSystem.AddAudio("coin", "Audio/coin_collect.wav");
	kiko::g_audioSystem.AddAudio("music", "Audio/adventure_music.mp3");

	// create scene
	m_scene = std::make_unique<kiko::Scene>();
	m_scene->Load("Scenes/platformscene.json");
	m_scene->Load("Scenes/tilemap.json");
	m_scene->Initialize();

	kiko::g_audioSystem.PlayOneShot("music", true);

	// add events
	EVENT_SUBSCRIBE("OnAddPoints", PlatformGame::OnAddPoints);
	EVENT_SUBSCRIBE("OnPlayerDead", PlatformGame::OnPlayerDead);

	return true;
}

bool PlatformGame::Shutdown()
{
	return false;
}

void PlatformGame::Update(float dt)
{
	switch (m_state)
	{
	case PlatformGame::eState::Title:
		

		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_SPACE)) {
			m_state = eState::StartGame;
		}
		break;

	case PlatformGame::eState::StartGame:
		m_score = 0;
		m_lives = 3;
		m_state = eState::StartLevel;
		break;

	case PlatformGame::eState::StartLevel:
	{
		auto player = INSTANTIATE(Player, "Player");
		player->Initialize();
		m_scene->Add(std::move(player));

		auto companion = INSTANTIATE(Enemy, "Companion");
		companion->Initialize();
		m_scene->Add(std::move(companion));

		auto genemy = INSTANTIATE(GroundEnemy, "Skeleton");
		genemy->Initialize();
		m_scene->Add(std::move(genemy));
	}

		for (int i = 0; i < 5; i++)
		{
			auto coin = INSTANTIATE(Actor, "Coin");
			coin->transform = kiko::Transform{ { kiko::random(1200), 100 }, 0, 3};
			coin->Initialize();
			m_scene->Add(std::move(coin));
		}

		m_state = eState::Game;
		break;

	case PlatformGame::eState::Game:
		if (kiko::g_inputSystem.GetKeyDown(SDL_SCANCODE_C) &&
			!kiko::g_inputSystem.GetPreviousKeyDown(SDL_SCANCODE_C))
		{
			auto coin = INSTANTIATE(Actor, "Coin");
			coin->transform = kiko::Transform{ { kiko::random(1200), 100 }, 0, 3 };
			coin->Initialize();
			m_scene->Add(std::move(coin));
		}

		break;

	case PlatformGame::eState::PlayerDeadStart:
		break;

	case PlatformGame::eState::PlayerDead:
		break;

	case PlatformGame::eState::GameOver:
		break;

	default:
		break;
	}

	m_scoreText->Create(kiko::g_renderer, std::to_string(m_score), { 0, 0, 0, 1 });

	m_scene->Update(dt);
}

void PlatformGame::Draw(kiko::Renderer& renderer)
{
	m_scene->Draw(renderer);

	if (m_state == eState::Title)
	{
		m_scene->GetActorByName("Title")->active = true;
		m_scene->GetActorByName("TitleShadow")->active = true;
		m_scene->GetActorByName("Score")->active = false;
		m_scene->GetActorByName("Banner")->active = false;
		//m_scene->GetActorByName("Enemy")->active = false;
	}
	if (m_state == eState::GameOver)
	{
		
	}
	if (m_state != eState::Title && m_state != eState::GameOver) {
		m_scene->GetActorByName("Title")->active = false;
		m_scene->GetActorByName("TitleShadow")->active = false;
		m_scene->GetActorByName("Score")->active = true;
		m_scene->GetActorByName("Banner")->active = true;
		m_scoreText->Draw(renderer, 1050, 64);
		//m_scene->GetActorByName("Enemy")->active = true;
	}
}

void PlatformGame::OnAddPoints(const kiko::Event& event)
{
	m_score += std::get<int>(event.data);

}

void PlatformGame::OnPlayerDead(const kiko::Event& event)
{
	m_lives--;
	m_state = eState::PlayerDeadStart;
}
