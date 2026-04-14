#pragma once
#include "Sources/Resource.h"

namespace MGF3D
{
    MGF_CLASS_PTR(GLShader)

    MGF_CLASS_PTR(Program)
    class Program : public Resource
    {
    public:
        virtual ~Program();
        void AddShader(const GLShaderPtr& shader);
        usize GetHandle() const { return m_handle; }
        void Use() const;

    /*========================//
    //      Program Type      //
    //========================*/
    public:
        static int16 s_typeIndex;
        virtual const MGFType* GetType() const override;
        virtual bool OnSyncCreate() override;

    /*==============================================//
    //   default arithmetic value uniform setters   //
    //==============================================*/
    public:
        void SetUniform(const String& name, bool value);
        void SetUniform(const String& name, int32 value);
        void SetUniform(const String& name, float value);
        void SetUniform(const String& name, const vec2& value);
        void SetUniform(const String& name, const vec3& value);
        void SetUniform(const String& name, const vec4& value);
        void SetUniform(const String& name, const mat4& value);

    /*===================================//
    //   default array uniform setters   //
    //===================================*/
    public:
        void SetUniform(const String& name, const Vector<int32>& value);
        void SetUniform(const String& name, const Vector<mat4>& value);
        void SetUniform(const String& name, const Vector<vec3>& value);

    protected:
        Program();
        int32 GetUniformLocation(const String& name);

        usize m_handle  { 0 };
        HashMap<StringHash, int32> m_location;
        Vector<GLShaderPtr> m_pendingShaders;
    };
}