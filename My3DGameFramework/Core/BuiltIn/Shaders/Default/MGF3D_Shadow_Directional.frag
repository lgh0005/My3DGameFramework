#version 460 core

void main() 
{
    // 아무것도 적지 않아도 됩니다!
    // 프래그먼트 셰이더가 비어있어도, OpenGL 파이프라인은 
    // 지오메트리 셰이더에서 넘어온 gl_Position의 Z값을 사용하여 
    // 자동으로 바인딩된 텍스처(m_dirShadowMapArray)에 깊이(Depth)를 기록합니다.

    /* 참고: 만약 나뭇잎이나 철조망 같이 구멍이 뚫린 메쉬(Alpha Cutout)의 
    그림자를 정확히 처리해야 한다면, 여기서 Albedo 텍스처를 샘플링한 뒤 
    알파 값이 일정 이하일 때 discard; 를 호출하는 로직이 추가되어야 합니다.
    */
}