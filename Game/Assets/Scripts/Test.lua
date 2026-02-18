local TestScript = class()

function TestScript:Awake()
    print("[Lua] TestScript:Awake(), Object Name: " .. self.owner:GetName())
end

function TestScript:Start()
    print("[Lua] TestScript:Start()")
end

return TestScript