#pragma once
#include "Assets/RawKeyPosition.h"
#include "Assets/RawKeyRotation.h"
#include "Assets/RawKeyScale.h"

namespace MGF3D
{
    struct RawAnimChannel
    {
        String nodeName; // 움직일 노드의 이름 (RawNode의 name과 매칭)
        Vector<RawKeyPosition> positions;
        Vector<RawKeyRotation> rotations;
        Vector<RawKeyScale>    scales;
    };
}