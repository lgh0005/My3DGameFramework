local TestScript = class()

function TestScript:Awake()
    print("--------------------------------------------------")
    print("[Lua] Math System Integration Test Start")
    print("--------------------------------------------------")

    -- 1. Vector2 & Vector3 기초 테스트 (기존 코드 확장)
    local v2a = Vector2.new(3.0, 0.0)
    local v2b = Vector2.new(0.0, 4.0)
    print(string.format("[Vector2] Distance between (3,0) and (0,4): %.1f", v2a:Distance(v2b))) -- 5.0 예상
    
    local v3 = Vector3.new(1, 0, 0)
    local v3Up = Vector3.new(0, 1, 0)
    local vCross = v3:Cross(v3Up) -- (0, 0, 1) 예상
    print(string.format("[Vector3] Cross Product (Right x Up): (%.1f, %.1f, %.1f)", vCross.x, vCross.y, vCross.z))

    -- 2. Quaternion & Rotation 테스트 (핵심!)
    -- Y축으로 90도 회전하는 쿼터니언 생성
    local rotY90 = Quaternion.FromEuler(0, 90, 0)
    
    -- 앞방향(0, 0, 1) 벡터를 90도 회전시키면 오른쪽(1, 0, 0)이 되어야 함
    local forward = Vector3.new(0, 0, 1)
    local rotatedDir = rotY90 * forward
    
    print(string.format("[Quaternion] Rotate Forward(0,0,1) by 90deg Y: (%.1f, %.1f, %.1f)", 
        rotatedDir.x, rotatedDir.y, rotatedDir.z))

    -- 3. ToEuler 역변환 테스트
    local euler = rotY90:ToEuler()
    print(string.format("[Quaternion] Back to Euler: (%.1f, %.1f, %.1f)", euler.x, euler.y, euler.z))

    -- 4. Identity & Inverse 테스트
    local identity = Quaternion.Identity()
    local invRot = rotY90:Inverse()
    local resultQ = rotY90 * invRot -- 회전과 그 역회전을 곱하면 Identity가 되어야 함
    
    print(string.format("[Quaternion] Inverse Multi Test (W should be 1.0): %.1f", resultQ.w))

    print("--------------------------------------------------")
    print("[Lua] Math System Integration Test Complete")
    print("--------------------------------------------------")
end

function TestScript:Start()
    print("[Lua] TestScript:Start()")
end

return TestScript