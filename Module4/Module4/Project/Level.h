#pragma once
#include <string>
#include <vector>
#include <mutex>

class PlacableActor;

class Level
{
	char* m_pLevelData;
	int m_height;
	int m_width;

	std::vector<PlacableActor*> m_pActors;

public:
	Level();
	~Level();

	bool Load(std::string levelName, int* playerX, int* playerY);
	void Draw();
	void UpdateActors();
	PlacableActor* CheckForCollision(int x, int y);

	bool IsSpace(int x, int y);
	bool IsWall(int x, int y);

	int GetHeight() { return m_height; }
	int GetWidth() { return m_width;  }

	static constexpr char WAL = (char)219;

private:
	bool ConvertLevel(int* playerX, int* playerY);
	int GetIndexFromCoordinates(int x, int y);

	std::mutex m_mutex;

};