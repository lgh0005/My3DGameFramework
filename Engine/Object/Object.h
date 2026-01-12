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

protected:
	Object();
	std::string m_name;
};