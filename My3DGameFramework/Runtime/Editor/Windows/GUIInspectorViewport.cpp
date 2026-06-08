#include "RuntimePch.h"
#include "GUIInspectorViewport.h"
#include "Managers/EntityManager.h"
#include "Managers/RenderManager.h"
#include "Entities/GameObject.h"
#include "Components/Transform.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/SpotLight.h"
#include "Components/Animation/Animator.h"
#include "Pipelines/MGFRenderPipeline.h"
#include "Pipelines/RenderPasses/MGFPostProcessingPass.h"
#include "PostProcessing/Effects/LDREffects.h"
#include "PostProcessing/Effects/WorldSpaceEffects.h"

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
#pragma region Directional Light Tweaking

		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			auto DrawTransform = [](ObjectIDHash objectID)
				{
					auto* transform = MGF_ENTITY.GetComponent<Transform>(objectID);

					if (!transform)
					{
						ImGui::Text("Transform component is null.");
						return;
					}

					if (ImGui::TreeNodeEx("Transform"))
					{
						vec3 pos = transform->GetLocalPosition();
						quat rot = transform->GetLocalRotation();
						vec3 euler = glm::degrees(glm::eulerAngles(rot));
						vec3 scale = transform->GetLocalScale();

						if (ImGui::DragFloat3("Position", &pos.x, 0.05f))
						{
							transform->SetLocalPosition(pos);
						}

						if (ImGui::DragFloat3("Rotation", &euler.x, 0.5f))
						{
							quat newRot = quat(glm::radians(euler));
							transform->SetLocalRotation(newRot);
						}

						if (ImGui::DragFloat3("Scale", &scale.x, 0.05f, 0.001f, 1000.0f))
						{
							transform->SetLocalScale(scale);
						}

						ImGui::TreePop();
					}
				};

			auto DrawDirectionalLight = [](ObjectIDHash objectID)
				{
					auto* dirLight = MGF_ENTITY.GetComponent<DirectionalLight>(objectID);

					if (!dirLight)
					{
						return;
					}

					if (ImGui::TreeNodeEx("Directional Light"))
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

						bool castShadow = dirLight->IsCastShadow();
						if (ImGui::Checkbox("Cast Shadow", &castShadow))
						{
							dirLight->SetCastShadow(castShadow);
						}

						ImGui::TreePop();
					}
				};

			auto DrawPointLight = [](ObjectIDHash objectID)
				{
					auto* pointLight = MGF_ENTITY.GetComponent<PointLight>(objectID);

					if (!pointLight)
					{
						return;
					}

					if (ImGui::TreeNodeEx("Point Light"))
					{
						vec3 color = pointLight->GetColor();
						float intensity = pointLight->GetIntensity();
						float range = pointLight->GetRange();

						if (ImGui::ColorEdit3("Light Color", &color.r))
						{
							pointLight->SetColor(color);
						}

						if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 50.0f))
						{
							pointLight->SetIntensity(intensity);
						}

						if (ImGui::DragFloat("Range", &range, 0.1f, 0.0f, 500.0f))
						{
							pointLight->SetRange(range);
						}

						bool castShadow = pointLight->IsCastShadow();
						if (ImGui::Checkbox("Cast Shadow", &castShadow))
						{
							pointLight->SetCastShadow(castShadow);
						}
						

						ImGui::TreePop();
					}
				};

			auto DrawSpotLight = [](ObjectIDHash objectID)
				{
					auto* spotLight = MGF_ENTITY.GetComponent<SpotLight>(objectID);

					if (!spotLight)
					{
						return;
					}

					if (ImGui::TreeNodeEx("Spot Light"))
					{
						vec3 color = spotLight->GetColor();
						float intensity = spotLight->GetIntensity();
						float range = spotLight->GetRange();

						if (ImGui::ColorEdit3("Light Color", &color.r))
						{
							spotLight->SetColor(color);
						}

						if (ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 50.0f))
						{
							spotLight->SetIntensity(intensity);
						}

						if (ImGui::DragFloat("Range", &range, 0.1f, 0.0f, 500.0f))
						{
							spotLight->SetRange(range);
						}

						ImGui::TreePop();
					}
				};

			auto DrawAnimator = [](ObjectIDHash objectID)
				{
					auto* animator = MGF_ENTITY.GetComponent<Animator>(objectID);

					if (!animator)
					{
						return;
					}

					if (ImGui::TreeNodeEx("Animator"))
					{
						ImGui::Text("Animator component exists.");

						// Animator에 재생 속도, 현재 애니메이션, 상태 Getter/Setter가 있으면
						// 여기에 추가하면 됩니다.

						ImGui::TreePop();
					}
				};

			auto DrawGameObjectInspector = [&](const char* objectName)
				{
					GameObject* obj = MGF_ENTITY.GetGameObject(objectName);

					if (!obj)
					{
						ImGui::Text("Cannot find '%s' entity.", objectName);
						return;
					}

					ObjectIDHash objectID = obj->GetID();

					// Object 단위로 ImGui ID Stack 분리
					ImGui::PushID(static_cast<int>(objectID));

					if (ImGui::CollapsingHeader(objectName))
					{
						DrawTransform(objectID);
						DrawDirectionalLight(objectID);
						DrawPointLight(objectID);
						DrawSpotLight(objectID);
						DrawAnimator(objectID);
					}

					ImGui::PopID();
				};

			// Directional Light
			DrawGameObjectInspector("dirLight");

			// Sphere
			DrawGameObjectInspector("Sphere");

			// Cube
			DrawGameObjectInspector("Cube");

			// Cone
			DrawGameObjectInspector("Cone");

			// Spot Light
			DrawGameObjectInspector("spotLight");

			// Point Light
			DrawGameObjectInspector("PointLight");
		}
		ImGui::End();
#pragma endregion
	
#pragma region Post-Processing Tweaking
		auto* activePipeline = MGF_RENDER.GetActiveRenderPipeline();
		auto* mgfPipeline = static_cast<MGFRenderPipeline*>(activePipeline);
		auto* postProcessingPass = mgfPipeline->GetPostProcessingPass();
		if (!postProcessingPass) return;

		if (ImGui::Begin("LDR Effects", &m_isOpen))
		{
			// MGFRenderPipeline의 Post-Processing Pass에서 LDR 효과 트위킹
			auto* ldrEffects = postProcessingPass->GetLDREffects();
			if (ldrEffects)
			{
				ImGui::Text("LDR Effects");
				ImGui::Separator();

				// -----------------------------
				// Tone Mapping
				// -----------------------------
				if (ImGui::CollapsingHeader("Tone Mapping"))
				{
					const char* toneMappingItems[] =
					{
						"Exposure",
						"ACES",
						"Reihard",
						"None"
					};

					int toneMappingMode = static_cast<int>(ldrEffects->GetToneMappingMode());

					if (ImGui::Combo(
						"Tone Mapping Mode",
						&toneMappingMode,
						toneMappingItems,
						IM_ARRAYSIZE(toneMappingItems)))
					{
						ldrEffects->SetToneMappingMode(static_cast<EToneMappingMode>(toneMappingMode));
					}

					float exposure = ldrEffects->GetExposure();

					if (ImGui::DragFloat("Exposure", &exposure, 0.01f, 0.0f, 10.0f))
					{
						ldrEffects->SetExposure(exposure);
					}

					float gamma = ldrEffects->GetGamma();

					if (ImGui::DragFloat("Gamma", &gamma, 0.01f, 0.0f, 5.0f))
					{
						ldrEffects->SetGamma(gamma);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// Vignette
				// -----------------------------
				if (ImGui::CollapsingHeader("Vignette"))
				{
					float vignetteIntensity = ldrEffects->GetVignetteIntensity();

					if (ImGui::DragFloat(
						"Vignette Intensity",
						&vignetteIntensity,
						0.01f,
						0.0f,
						1.0f))
					{
						ldrEffects->SetVignetteIntensity(vignetteIntensity);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// Sharpen
				// -----------------------------
				if (ImGui::CollapsingHeader("Sharpen"))
				{
					float sharpenAmount = ldrEffects->GetSharpenAmount();

					if (ImGui::DragFloat(
						"Sharpen Amount",
						&sharpenAmount,
						0.01f,
						0.0f,
						5.0f))
					{
						ldrEffects->SetSharpenAmount(sharpenAmount);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// Film Grain
				// -----------------------------
				if (ImGui::CollapsingHeader("Film Grain"))
				{
					float grainAmount = ldrEffects->GetGrainAmount();

					if (ImGui::DragFloat(
						"Grain Amount",
						&grainAmount,
						0.001f,
						0.0f,
						1.0f))
					{
						ldrEffects->SetGrainAmount(grainAmount);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// CRT Screen
				// -----------------------------
				if (ImGui::CollapsingHeader("CRT Screen"))
				{
					vec2 distortionK = ldrEffects->GetDistortionK();

					if (ImGui::DragFloat2(
						"Distortion K",
						&distortionK.x,
						0.001f,
						-1.0f,
						1.0f))
					{
						ldrEffects->SetDistortionK(distortionK);
					}

					float scanlineIntensity = ldrEffects->GetScanlineIntensity();

					if (ImGui::DragFloat(
						"Scanline Intensity",
						&scanlineIntensity,
						0.01f,
						0.0f,
						1.0f))
					{
						ldrEffects->SetScanlineIntensity(scanlineIntensity);
					}

					float phosphorIntensity = ldrEffects->GetPhosphorIntensity();

					if (ImGui::DragFloat(
						"Phosphor Intensity",
						&phosphorIntensity,
						0.01f,
						0.0f,
						1.0f))
					{
						ldrEffects->SetPhosphorIntensity(phosphorIntensity);
					}

					float flickerIntensity = ldrEffects->GetFlickerIntensity();

					if (ImGui::DragFloat(
						"Flicker Intensity",
						&flickerIntensity,
						0.0001f,
						0.0f,
						0.01f))
					{
						ldrEffects->SetFlickerIntensity(flickerIntensity);
					}
				}
			}
			else ImGui::Text("LDREffects is null.");
		} ImGui::End();
		if (ImGui::Begin("World Space Effects", &m_isOpen))
		{
			// MGFRenderPipeline의 Post-Processing Pass에서 World-Space 효과 트위킹
			auto* worldSpaceEffects = postProcessingPass->GetWorldSpaceEffects();
			if (worldSpaceEffects)
			{
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Text("World Space Effects");
				ImGui::Separator();

				// -----------------------------
				// Fog
				// -----------------------------
				if (ImGui::CollapsingHeader("Fog"))
				{
					vec3 fogColor = worldSpaceEffects->GetFogColor();

					if (ImGui::ColorEdit3("Fog Color", &fogColor.r))
					{
						worldSpaceEffects->SetFogColor(fogColor);
					}

					float fogDensity = worldSpaceEffects->GetFogDensity();

					if (ImGui::DragFloat(
						"Fog Density",
						&fogDensity,
						0.0001f,
						0.0f,
						1.0f,
						"%.6f"))
					{
						worldSpaceEffects->SetFogDensity(fogDensity);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// Pixelization
				// -----------------------------
				if (ImGui::CollapsingHeader("Pixelization"))
				{
					float pixelSize = worldSpaceEffects->GetPixelSize();

					if (ImGui::DragFloat(
						"Pixel Size",
						&pixelSize,
						0.1f,
						0.0f,
						128.0f))
					{
						worldSpaceEffects->SetPixelSize(pixelSize);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// Posterization
				// -----------------------------
				if (ImGui::CollapsingHeader("Posterization"))
				{
					float postLevels = worldSpaceEffects->GetPostLevels();

					if (ImGui::DragFloat(
						"Posterization Levels",
						&postLevels,
						1.0f,
						0.0f,
						256.0f,
						"%.0f"))
					{
						worldSpaceEffects->SetPostLevels(postLevels);
					}
				}

				ImGui::Spacing();

				// -----------------------------
				// Dilation
				// -----------------------------
				if (ImGui::CollapsingHeader("Dilation"))
				{
					int dilationRadius = static_cast<int>(worldSpaceEffects->GetDilationRadius());

					if (ImGui::DragInt(
						"Dilation Radius",
						&dilationRadius,
						1.0f,
						0,
						32))
					{
						worldSpaceEffects->SetDilationRadius(static_cast<int32>(dilationRadius));
					}
				}
			}
			else ImGui::Text("WorldSpaceEffects is null.");
		} ImGui::End();
#pragma endregion
	}
}