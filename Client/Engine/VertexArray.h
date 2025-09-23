#pragma once

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

public:
	void AddVertexBuffer(const shared_ptr<VertexBuffer>& vbo, const vector<GLuint>& attribSizes);
	void SetIndexBuffer(const shared_ptr<IndexBuffer>& ibo);
	void Bind() const { glBindVertexArray(_vao); }
	void Unbind() const { glBindVertexArray(0); }

private:
	GLuint _vao;
};

