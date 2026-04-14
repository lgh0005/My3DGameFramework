#pragma once
#include "Programs/Program.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Shader)

	MGF_CLASS_PTR(GraphicsProgram)
	class GraphicsProgram : public Program
	{
	/*========================//
	// GraphicsProgram Type   //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		GraphicsProgram();
		virtual ~GraphicsProgram() override;
	};
}