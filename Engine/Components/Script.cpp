#include "EnginePch.h"
#include "Script.h"
#include "Components/Collider.h"

DECLARE_DEFAULTS_IMPL(Script)

/*=======================================//
//  additional event methods for script  //
//=======================================*/
void Script::OnEnable() {}
void Script::OnDisable() {}

/*=======================================//
//  event methods for script : physics   //
//=======================================*/
void Script::OnTriggerEnter(Collider* other) {}
void Script::OnTriggerStay(Collider* other) {}
void Script::OnTriggerExit(Collider* other) {}

void Script::OnCollisionEnter(Collider* other) {}
void Script::OnCollisionStay(Collider* other) {}
void Script::OnCollisionExit(Collider* other) {}