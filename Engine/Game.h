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

    Context& GetContext();

private:
	Game() = default;
    ContextUPtr m_context  = nullptr;
};