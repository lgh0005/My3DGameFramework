#pragma once
#include "Programs/Program.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Shader)

	MGF_CLASS_PTR(GraphicsProgram)
	class GraphicsProgram : public Program
	{
	public:
		virtual ~GraphicsProgram() override;
		static GraphicsProgramPtr Create(const Vector<ShaderPtr>& shaders);

	private:
		GraphicsProgram();
	};
}