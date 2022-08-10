#pragma once
#include "GameState.h"
#include "Player.h"

#include <windows.h>
#include <vector>
#include <string>

namespace std
{
	class thread;
}

class Level;
class StateMachineExampleGame;

class GameplayState : public GameState
{
	StateMachineExampleGame* m_pOwner;
	
	Player m_player;
	Level* m_pLevel;

	bool m_didBeatLevel;
	int m_skipFrameCount;
	static constexpr int kFramesToSkip = 2;

	int m_currentLevel;

	std::vector<std::string> m_LevelNames;

public:
	GameplayState(StateMachineExampleGame* pOwner);
	~GameplayState();

	virtual void Enter() override;
	virtual bool Update(bool processInput = true) override;
	virtual void Draw() override;

protected:
	void ProcessInput() override;
	void CheckBeatLevel();

	bool m_shouldProcessInput = false;
	std::thread* m_inputThread = nullptr;
private:
	void HandleCollision(int newPlayerX, int newPlayerY);
	bool Load();
	void DrawHUD(const HANDLE& console);
};
