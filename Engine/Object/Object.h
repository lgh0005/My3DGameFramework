#pragma once

CLASS_PTR(Object)
class Object
{
public:
	virtual ~Object();

/*========================//
//     Identity Methods   //
//========================*/
public:
	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

/*===============================//
//   common life-cycle methods   //
//===============================*/
public:
	virtual void Awake();
	virtual void Start();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void LateUpdate();
	virtual void OnDestroy();

/*=========================//
//   find object methods   //
//=========================*/
public:
	// TODO : 찾는 계열 메서드들을 적을 필요가 있다.

protected:
	Object();
	std::string m_name;
};