#include "pch.h"
#include "Scene.h"

Scene::Scene(const string& name) : _name(name)
{

}

void Scene::Init()
{
	_window = GRAPHICS.GetWindow();
}

void Scene::Update()
{

}