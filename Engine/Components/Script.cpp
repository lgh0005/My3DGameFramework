#include "EnginePch.h"
#include "Script.h"

DECLARE_DEFAULTS_IMPL(Script)

/*=======================================//
//  additional event methods for script  //
//=======================================*/
void Script::OnEnable() {}
void Script::OnDisable() {}

/*=======================================//
//  event methods for script : physics   //
//=======================================*/
void Script::OnTriggerEnter() {}
void Script::OnTriggerStay() {}
void Script::OnTriggerExit() {}

void Script::OnCollisionEnter() {}
void Script::OnCollisionStay() {}
void Script::OnCollisionExit() {}