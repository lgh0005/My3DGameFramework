#include "EnginePch.h"
#include "Object.h"

DECLARE_DEFAULTS_IMPL(Object)

/*===============================//
//   common life-cycle methods   //
//===============================*/
void Object::Awake()	   {}
void Object::Start()	   {}
void Object::FixedUpdate() {}
void Object::Update()	   {}
void Object::LateUpdate()  {}
void Object::OnDestroy()   {}
