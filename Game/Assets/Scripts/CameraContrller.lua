-- CameraController.lua
local CameraController = class("CameraController")

function CameraController:Awake()
    -- 설정값
    self.moveSpeed = 10.0
    self.rotSpeed = 0.15

    -- 회전 상태 저장
    self.pitch = 0.0
    self.yaw = 0.0
    
    -- 마우스 제어 플래그
    self.isControl = false
    self.prevMousePos = Vector2(0, 0)
end

function CameraController:Start()
    local tr = self.owner:GetComponent(Transform)

    -- 초기 각도 동기화
    local currentRot = tr.eulerAngles 
    self.pitch = currentRot.x
    self.yaw = currentRot.y

    -- [입력 매핑]
    -- 모든 입력은 'Action 이름'으로 정의해야 합니다.
    
    -- 1. 이동 키 매핑
    Input.MapAction("Forward",  Input.Key.W)
    Input.MapAction("Back",     Input.Key.S)
    Input.MapAction("Left",     Input.Key.A)
    Input.MapAction("Right",    Input.Key.D)
    Input.MapAction("Up",       Input.Key.E)
    Input.MapAction("Down",     Input.Key.Q)
    
    -- 2. 기능 키 매핑 (Raw Key 체크 불가 -> 액션 매핑 필수)
    Input.MapAction("Sprint",   Input.Key.LeftShift) -- 달리기
    
    -- 3. 마우스 버튼 매핑 (GetMouseButtonDown 불가 -> 액션 매핑 필수)
    Input.MapMouseAction("ControlCam", Input.Mouse.Right) -- 카메라 회전
end

function CameraController:Update(dt)
    self:HandleMovement(dt)
    self:HandleRotation(dt)
end

-- [이동 처리]
function CameraController:HandleMovement(dt)
    local tr = self.owner:GetComponent(Transform)
    local speed = self.moveSpeed * dt

    -- [수정됨] Input.GetKey(...) 대신 매핑된 "Sprint" 액션 확인
    if Input.GetButton("Sprint") then
        speed = speed * 2.0
    end

    if Input.GetButton("Forward") then
        tr:Translate(tr.forward * speed)
    end
    if Input.GetButton("Back") then
        tr:Translate(tr.forward * -speed)
    end
    if Input.GetButton("Right") then
        tr:Translate(tr.right * speed)
    end
    if Input.GetButton("Left") then
        tr:Translate(tr.right * -speed)
    end
    if Input.GetButton("Up") then
        tr:Translate(tr.up * speed)
    end
    if Input.GetButton("Down") then
        tr:Translate(tr.up * -speed)
    end
end

-- [회전 처리]
function CameraController:HandleRotation(dt)
    -- [수정됨] Input.GetMouseButtonDown(...) 대신 매핑된 "ControlCam" 액션 확인
    if Input.GetButtonDown("ControlCam") then
        self.isControl = true
        self.prevMousePos = Input.GetMousePosition() -- 마우스 좌표는 액션 아님
    end

    -- [수정됨]
    if Input.GetButtonUp("ControlCam") then
        self.isControl = false
    end

    -- 회전 로직
    if self.isControl then
        local currentPos = Input.GetMousePosition()
        local delta = currentPos - self.prevMousePos
        self.prevMousePos = currentPos

        self.yaw = self.yaw - (delta.x * self.rotSpeed)
        self.pitch = self.pitch - (delta.y * self.rotSpeed)

        -- 짐벌락 방지
        if self.pitch > 89 then self.pitch = 89 end
        if self.pitch < -89 then self.pitch = -89 end

        local tr = self.owner:GetComponent(Transform)
        tr.eulerAngles = Vector3(self.pitch, self.yaw, 0)
    end
end

return CameraController