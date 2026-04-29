#include "FrameworkPch.h"
#include "Script.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Script::Script(ObjectIDHash id, ObjectIDHash ownerID) : Super(id, ownerID) { }
	Script::~Script() = default;
	Script::Script(Script&& other) noexcept = default;
	Script& Script::operator=(Script&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 Script::s_typeIndex = -1;
	const MGFType* Script::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	/*======================================//
	//       Script virtual methods         //
	//======================================*/
	void Script::Awake() {}
	void Script::Start() {}
	void Script::Update() {}
	void Script::FixedUpdate() {}
	void Script::LateUpdate() {}

	void Script::OnEnable() {}
	void Script::OnDisable() {}

	void Script::OnCollisionEnter() {}
	void Script::OnCollisionStay() {}
	void Script::OnCollisionExit() {}
}