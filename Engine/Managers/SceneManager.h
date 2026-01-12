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
	// TODO : 복잡하게 상태 기반으로 부등식 if문을 쓰지 말고 
	// 여기에 있는 API를 이용할 것.

private:
	std::unordered_map<std::string, SceneFactory> m_scenes;
	SceneUPtr m_activeScene;
};

#include "Managers/SceneManager.inl"