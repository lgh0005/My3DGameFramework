#pragma once
#include "Engine\Scene.h"

class VertexArray;
class VertexBuffer;
class IndexBuffer;
class Shader;
struct Pass;

class Rectangle : public Scene
{
	using Super = Scene;

public:
	Rectangle(const string& name);
	virtual ~Rectangle() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

private:
	void CreateRectangle();

private:
	shared_ptr<VertexArray> _vao;
	shared_ptr<VertexBuffer> _vbo;
	shared_ptr<IndexBuffer> _ibo;
	shared_ptr<Shader> _shader;
	shared_ptr<Pass> _pass;
};

