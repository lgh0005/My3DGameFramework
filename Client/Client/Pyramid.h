#pragma once
#include "Engine\Scene.h"

class Mesh;
class Shader;
struct Pass;

class Pyramid : public Scene
{
	using Super = Scene;

public:
	Pyramid(const string& name);
	virtual ~Pyramid() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

private:
	glm::mat4 _model = glm::mat4(1.0f);
	float _rotationAngle = 0.0f;

private:
	shared_ptr<Mesh> _pyramid;
	shared_ptr<Shader> _shader;
	shared_ptr<Pass> _pass;
};

