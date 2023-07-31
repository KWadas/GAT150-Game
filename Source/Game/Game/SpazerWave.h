#pragma once
#include "Framework/Game.h"
#include "Renderer/Text.h"

class SpazerWave : public kiko::Game
{
public:
	
	enum class eState
	{
		Title,
		StartGame,
		StartLevel,
		Game,
		PlayerDeadStart,
		PlayerDead,
		GameOverStart,
		GameOver
	};

public:


	virtual bool Initialize() override;
	virtual bool Shutdown() override;

	virtual void Update(float dt) override;
	virtual void Draw(kiko::Renderer& renderer) override;

	void SetState(eState state) { m_state = state; }

private:
	eState m_state = eState::Title;
	float m_spawnTimer = 0;
	float m_spawnTime = 2.0f;
	float m_difficulty = 0.15f;

	float m_stateTimer = 0;
	float m_gameTimer = 0;

	std::shared_ptr<kiko::Font> m_font;
	std::unique_ptr<kiko::Text> m_scoreText;
	std::unique_ptr<kiko::Text> m_livesText;
	std::unique_ptr<kiko::Text> m_titleText;
	std::unique_ptr<kiko::Text> m_explainText;
	std::unique_ptr<kiko::Text> m_timerText;
	std::unique_ptr<kiko::Text> m_gameOverText;

};