#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Context)
#pragma endregion

CLASS_PTR(Game)
class Game
{
public:
	static GameUPtr Create();

	bool Init();
	void Update();
	void Shutdown();
    ~Game();

private:
	Game() = default;
};