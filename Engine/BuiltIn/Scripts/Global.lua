function class(base)

    -- 클래스 생성 및 상속 체인 구축
    -- 새로운 클래스 역할을 할 테이블을 만들고, '나에게 없는 것은 나(class)에서 찾아라'라고 설정
    local new_class = {}
    new_class.__index = new_class
    new_class.base = base
    if base then
        setmetatable(new_class, base)
    end

    -- Override 헬퍼 함수
    -- 부모에게 해당 함수가 있는지 검사하여 오버라이드의 안전성을 보장
    function new_class:override(name, func)
        if not base or not base[name] then
            return
        end
        self[name] = func
    end

    -- Super 헬퍼 함수
    -- 부모 클래스의 원본 함수를 호출할 때 사용
    function new_class:super(name, ...)
        if base and base[name] then
            return base[name](self, ...)
        end
    end
    
    return new_class
end