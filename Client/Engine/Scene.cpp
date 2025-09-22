#include "pch.h"
#include "Scene.h"

Scene::Scene(const string& name) : _name(name)
{

}

void Scene::Init()
{
	_window = RENDER.GetWindow();
}

void Scene::Update()
{

}