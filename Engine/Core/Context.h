#pragma once

// TODO : 이후에 Core로 옮겨야 함.
// Graphics와는 거리가 먼 클래스임.

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Update();
    void Render();
    ~Context();

private:
    Context() = default;
    bool Init();
};
