#pragma once

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

    template<typename T>
    void Create(const vector<T>& vertices)
    {
        _stride = sizeof(T);
        _count = static_cast<GLuint>(vertices.size());
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, _stride * _count, vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

public:
    void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, _vbo); }
    void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    GLuint GetCount() const { return _count; }
    GLuint GetStride() const { return _stride; }
    GLuint GetVBO() const { return _vbo; }

private:
	GLuint _vbo = 0;
	GLuint _stride = 0;
	GLuint _count = 0;
};

