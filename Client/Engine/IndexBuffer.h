#pragma once

class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();

public:
	void Create(vector<uint32>& indices);
	void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo); }
	void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	GLuint GetCount() const { return _count; }
	GLuint GetIBO() const { return _ibo; }

private:
	GLuint _ibo = 0;
	GLuint _count = 0;
};

