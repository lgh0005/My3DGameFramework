#pragma once
#include "IResource.h"
#include "Geometry.h"
#include "VertexData.h"

class VertexArray;
class VertexBuffer;
class IndexBuffer;

class Mesh : public IResource
{
	using Super = IResource;

public:
	Mesh(const string& name);
	virtual ~Mesh() override = default;

#pragma region PRIMITIVE_SHAPES
public:
	void CreateTriangle();
	void CreateQuad();
	void CreatePyramid();
	void CreateCube();
	void CreateSphere();
#pragma endregion

public:
	shared_ptr<VertexArray> GetVertexArray() { return _vao; }
	shared_ptr<VertexBuffer> GetVertexBuffer() { return _vertexBuffer; }
	shared_ptr<IndexBuffer> GetIndexBuffer() { return _indexBuffer; }

private:
	void CreateBuffers();

private:
	shared_ptr<VertexArray> _vao;
	shared_ptr<Geometry<VertexColorData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
};


