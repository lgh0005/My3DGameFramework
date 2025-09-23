#pragma once
#include "Shader.h"

struct Pass
{
	shared_ptr<Shader> _shader;
	void Draw(const GLuint vao, const GLuint vertexCount)
	{
		_shader->Bind();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		glBindVertexArray(0);
		_shader->Unbind();
	}
};

