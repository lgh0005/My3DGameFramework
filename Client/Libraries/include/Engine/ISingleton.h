#pragma once

template<typename T>
class ISingleton
{
protected:
	ISingleton() = default;
	virtual ~ISingleton() = default;
	ISingleton(const ISingleton&) = delete;
	ISingleton& operator=(const ISingleton&) = delete;

public:
	virtual void Init() abstract;
	virtual void Update() abstract;
	static T& GetInstance() { static T s_Instance; return s_Instance; }
};

