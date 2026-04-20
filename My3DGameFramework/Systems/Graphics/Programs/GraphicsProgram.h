#pragma once
#include "Programs/Program.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Shader)

	MGF_CLASS_PTR(GraphicsProgram)
	class GraphicsProgram : public Program
	{
		using Super = Program;

	public:
		virtual ~GraphicsProgram() override;
		static GraphicsProgramPtr Create(StringView name);

	/*========================//
	// GraphicsProgram Type   //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	private:
		GraphicsProgram(StringView name);
	};
}