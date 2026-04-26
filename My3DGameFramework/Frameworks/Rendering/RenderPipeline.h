#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(RenderContext)

    class RenderPipeline
    {
    public:
        virtual ~RenderPipeline();

    /*=====================================//
    //   MGF3D RenderPipeline Custom Type  //
    //=====================================*/
    public:
        static int16 s_typeIndex;
        virtual const MGFType* GetType() const;

    public:
        virtual void Render(RenderContext* context) = 0;
        virtual void Resize() = 0;

    protected:
        RenderPipeline();
    };
}