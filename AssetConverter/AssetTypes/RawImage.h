#pragma once

struct RawImage
{
    int32_t width = 0;
    int32_t height = 0;
    int32_t channels = 0; // 3(RGB) or 4(RGBA)
    std::vector<uint8> pixels; // 실제 픽셀 데이터 (0~255)

    // 데이터가 비어있는지 확인
    bool IsValid() const; 
};