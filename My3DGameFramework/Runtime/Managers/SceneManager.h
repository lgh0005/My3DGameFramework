#pragma once
#include "Scene/Scene.h"

class SceneManager
{
	DECLARE_SINGLE(SceneManager)
	DECLARE_NONINSTANTIABLE(SceneManager)
	using SceneFactory = std::function<SceneUPtr()>;

/*===================================//
//   default scene manager methods   //
//===================================*/
public:
	Scene* GetActiveScene() const;
	template<typename T> void RegisterScene(const std::string& name);
	void LoadScene(const std::string& scene, const std::string& pipeline);
	void OnScreenResize(int32 width, int32 height);
	void Clear();

/*=========================================//
//   active scene state checking methods   //
//=========================================*/
public:
	void SetSceneState(SceneState state);
	bool HasActiveScene() const;
	bool IsUninitialized() const;
	bool IsSceneAwake() const;
	bool IsSceneRunning() const;
	bool IsSceneLoading() const;

private:
	std::unordered_map<std::string, SceneFactory> m_scenes;
	SceneUPtr m_activeScene;
};

#include "Managers/SceneManager.inl"