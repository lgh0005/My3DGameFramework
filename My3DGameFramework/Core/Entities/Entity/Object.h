#pragma once

CLASS_PTR(Object)
class Object
{
public:
	virtual ~Object();
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
};