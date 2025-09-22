#pragma once

class Scene;

class Launcher
{
public:
	Launcher() = default;
	~Launcher() = default;

public:
	void AddScenes(const vector<shared_ptr<Scene>>& scenes);
	void Run(const string& FirstSceneName);
};

extern unique_ptr<Launcher> LAUNCH;
