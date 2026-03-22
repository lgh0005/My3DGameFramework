#pragma once

namespace MGF3D
{
	struct FramebufferLayout
	{
		uint32 width			   { 0 };
		uint32 height			   { 0 };
		uint32 samples			   { 1 };
		uint32 depthStencilFormat  { 0 };
		SVector<uint32> colorFormats;

		bool IsMultiSampled() const;
		bool HasDepthStencil() const;
	};
}