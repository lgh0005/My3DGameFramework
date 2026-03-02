-- Global.lua

-- base: 부모 클래스 (코드 상속용)
-- typeId: 엔진 내장 컴포넌트 Enum (없으면 Script)
-- classId: 유저 스크립트 고유 ID (숫자)

function class(name, base)

    -- 클래스 생성 및 상속 체인 구축
    local new_class = {}
    new_class.__index = new_class

    -- 1. 엔진 내장 타입 ID 설정
    new_class._ClassHash = Utils.Hash(name)
    
    -- 2. 코드 재사용을 위한 연결고리 (C++은 몰라도 됨)
    new_class.base = base
    if base then
        setmetatable(new_class, base)
    end

    -- Override 헬퍼
    function new_class:override(name, func)
        if not base or not base[name] then
            return
        end
        self[name] = func
    end

    -- Super 헬퍼
    function new_class:super(name, ...)
        if base and base[name] then
            return base[name](self, ...)
        end
    end
    
    return new_class
end