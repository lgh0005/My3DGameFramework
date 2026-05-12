#include "FrameworkPch.h"
#include "MGFSSAOPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Framebuffers/GLFramebuffer2D.h"
#include "Textures/GLTexture2D.h"
#include "Buffers/GLUniformBuffer.h"
#include "Rendering/RenderContext.h"
#include "Meshes/ScreenMesh.h"

namespace MGF3D
{
	MGFSSAOPass::MGFSSAOPass() = default;
	MGFSSAOPass::~MGFSSAOPass() = default;

	MGFSSAOPassUPtr MGFSSAOPass::Create(int32 width, int32 height)
	{
		auto renderPass = MGFSSAOPassUPtr(new MGFSSAOPass());
		if (!renderPass->Init(width, height)) return nullptr;
		return renderPass;
	}

	bool MGFSSAOPass::Init(int32 width, int32 height)
	{
		auto vs1 = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_SSAO.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs1 = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_SSAO_Pass.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_ssaoProgram = MGF_RESOURCE.Create<GraphicsProgram>("SSAOProgram", Vector<ShaderPtr>{ vs1, fs1 });
		
		auto vs2 = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_SSAO.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs2 = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_SSAO_Blur.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_ssaoBlurProgram = MGF_RESOURCE.Create<GraphicsProgram>("SSAOBlurProgram", Vector<ShaderPtr>{ vs2, fs2 });
		
		// 2. SSAO FBO 텍스처 생성
		auto ssaoTex = MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, 90, 1);
		ssaoTex->SetFilter(GL_NEAREST, GL_NEAREST);
		m_ssaoFBO = GLFramebuffer2D::Create(Vector<GLTexture2DPtr>{ ssaoTex });

		// 3. Blur FBO 텍스처 생성
		auto ssaoBlurTex = MGF_RESOURCE.CreateImmediate<GLTexture2D>(width, height, 90, 1);
		ssaoBlurTex->SetFilter(GL_NEAREST, GL_NEAREST);
		m_ssaoBlurFBO = GLFramebuffer2D::Create(Vector<GLTexture2DPtr>{ ssaoBlurTex });

		if (!m_ssaoFBO || !m_ssaoBlurFBO || !m_ssaoProgram || !m_ssaoBlurProgram) 
			return false;

		GenerateKernel();
		GenerateNoiseTexture();

		m_ssaoKernalUBO = GLUniformBuffer::Create(&m_kernelData, sizeof(SSAOKernelData));

		return true;
	}

	void MGFSSAOPass::GenerateKernel()
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;

		for (uint32 i = 0; i < SSAO_KERNEL_SIZE; ++i)
		{
			vec3 sample
			(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator)
			);
			sample = Math::Normalize(sample);
			sample *= randomFloats(generator);

			float scale = (float)i / (float)SSAO_KERNEL_SIZE;
			scale = Math::Lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;

			m_kernelData.samples[i] = vec4(sample, 0.0f);
		}
	}

	void MGFSSAOPass::GenerateNoiseTexture()
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;
		Vector<vec3> ssaoNoise;

		const uint32 noiseCount = SSAO_NOISE_DIM * SSAO_NOISE_DIM;
		ssaoNoise.reserve(noiseCount);
		for (uint32 i = 0; i < noiseCount; ++i)
		{
			ssaoNoise.emplace_back
			(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				0.0f
			);
		}

		m_noiseTexture = MGF_RESOURCE.CreateImmediate<GLTexture2D>(SSAO_NOISE_DIM, SSAO_NOISE_DIM, 95, 1);
		m_noiseTexture->SetFilter(GL_NEAREST, GL_NEAREST);
		m_noiseTexture->SetWrap(GL_REPEAT, GL_REPEAT);

		// 텍스처 데이터 업로드 
		m_noiseTexture->UpdateSubData
		(
			0, 0,
			SSAO_NOISE_DIM, SSAO_NOISE_DIM,
			GL_RGB, GL_FLOAT,
			ssaoNoise.data()
		);
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFSSAOPass::s_typeIndex = -1;
	const MGFType* MGFSSAOPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFSSAOPass::Execute(RenderContext* context)
	{
		if (!context || !context->GetScreenMesh()) return;
		if (m_ssaoProgram->GetState() != EResourceState::Ready ||
			m_ssaoBlurProgram->GetState() != EResourceState::Ready) return;

		auto* screenMesh = context->GetScreenMesh();
		auto posAOTex = context->GetGeometryBufferTexture(EGBufferSlot::PositionAO);
		auto normalRoughTex = context->GetGeometryBufferTexture(EGBufferSlot::NormalRoughness);
		if (!posAOTex || !normalRoughTex) return;

		// 1. SSAO 계산
		m_ssaoFBO->Bind();
		glViewport(0, 0, m_ssaoFBO->GetWidth(), m_ssaoFBO->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT);

		m_ssaoProgram->Use();

		m_ssaoKernalUBO->Bind(2);

		posAOTex->Bind(0);
		normalRoughTex->Bind(1);
		m_noiseTexture->Bind(2);

		screenMesh->Bind();
		glDrawElements(GL_TRIANGLES, screenMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

		// 2. 노이즈 제거
		m_ssaoBlurFBO->Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		m_ssaoBlurProgram->Use();

		// Raw SSAO 결과를 0번 슬롯에 바인딩
		m_ssaoFBO->GetColorAttachment(0)->Bind(0);
		glDrawElements(GL_TRIANGLES, screenMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

		// 3. Context에 SSAO 결과 캐싱
		context->SetCachedTexture(ETextureCache::SSAO, m_ssaoBlurFBO->GetColorAttachment(0));

		// 4. 복귀
		GLFramebufferHandle::Unbind();
	}
}