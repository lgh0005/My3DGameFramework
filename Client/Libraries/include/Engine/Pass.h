#pragma once
#include "Shader.h"

struct Pass
{
	shared_ptr<Shader> _shader;
    void Draw(const GLuint vao, const GLuint indexCount)
    {
        _shader->Bind();
        _shader->ApplyUniforms();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        _shader->Unbind();
    }
};

