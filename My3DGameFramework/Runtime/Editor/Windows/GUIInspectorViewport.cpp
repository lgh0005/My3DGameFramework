#include "RuntimePch.h"
#include "GUIInspectorViewport.h"
#include "Managers/EntityManager.h"
#include "Entities/GameObject.h"
#include "Components/Transform.h"
#include "Components/Lights/DirectionalLight.h"

namespace MGF3D
{
	InspectorViewport::InspectorViewport() = default;
	InspectorViewport::~InspectorViewport() = default;

	InspectorViewportUPtr InspectorViewport::Create(const String& name)
	{
		auto window = InspectorViewportUPtr(new InspectorViewport());
		if (!window->Init(name)) return nullptr;
		return window;
	}

	bool InspectorViewport::Init(const String& name)
	{
		m_name = name;
		m_isOpen = true;
		return true;
	}

	void InspectorViewport::OnDraw()
	{
		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			// [DEBUG] Directional Light 트위킹
			{
				GameObject* lightObj = MGF_ENTITY.GetGameObject("dirLight");
				if (lightObj)
				{
					ObjectIDHash lightID = lightObj->GetID();

					ImGui::Text("Selected[Debug]: dirLight");
					ImGui::Separator();

					// Transform 컴포넌트 제어부
					auto* transform = MGF_ENTITY.GetComponent<Transform>(lightID);
					if (transform)
					{
						if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
						{
							vec3 pos = transform->GetLocalPosition();
							quat rot = transform->GetLocalRotation();
							vec3 euler = glm::degrees(glm::eulerAngles(rot));

							bool isPosChanged = ImGui::DragFloat3("Position", &pos.x, 0.05f);
							bool isRotChanged = ImGui::DragFloat3("Rotation", &euler.x, 0.5f);

							if (isPosChanged) transform->SetLocalPosition(pos);

							if (isRotChanged)
							{
								// 2. 변경된 Degree 수치를 다시 Radian으로 변환한 뒤 쿼터니언으로 조립
								quat newRot = quat(glm::radians(euler));
								transform->SetLocalRotation(newRot);
							}
						}
					}

					ImGui::Spacing();

					// Directional Light 컴포넌트 제어부
					auto* dirLight = MGF_ENTITY.GetComponent<DirectionalLight>(lightID);
					if (dirLight)
					{
						if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
						{
							vec3 color = dirLight->GetColor();
							float intensity = dirLight->GetIntensity();

							if (ImGui::ColorEdit3("Light Color", &color.r))
							{
								dirLight->SetColor(color);
							}

							if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 50.0f))
							{
								dirLight->SetIntensity(intensity);
							}
						}
					}
				}
				else
				{
					ImGui::Text("Cannot find 'dirLight' entity.");
				}
			}
		}
		ImGui::End();
	}
}