#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(Shader)

    MGF_CLASS_PTR(Program)
    class Program
    {
    public:
        virtual ~Program();
        usize GetHandle() const { return m_handle; }
        void Use() const;

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
        bool Link(const Vector<ShaderPtr>& shaders);
        int32 GetUniformLocation(const String& name);

        usize m_handle  { 0 };
        HashMap<StringHash, int32> m_location;
    };
}