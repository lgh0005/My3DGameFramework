#version 460 core

// 하드웨어 최적화를 위해 출력을 명시하지 않습니다.
// GPU는 gl_Position의 Z값을 자동으로 Depth 버퍼에 기록합니다.

void main() 
{
    // 아무것도 하지 않음 (Depth-only pass 최적화)
}