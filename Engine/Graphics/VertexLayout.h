#pragma once

CLASS_PTR(VertexLayout)
class VertexLayout
{
public:
    static VertexLayoutUPtr Create();
    ~VertexLayout();

    uint32_t Get() const { return m_vertexArrayObject; }
    void Bind() const;
    void SetAttrib(uint32_t attribIndex, int count, uint32 type, bool normalized, usize stride, uint64 offset) const;
    void DisableAttrib(int attribIndex) const;

private:
    VertexLayout() = default;
    void Init();
    uint32_t m_vertexArrayObject    { 0 };
};