#include "EnginePch.h"
#include "StandardSSAOPass.h"

#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Components/Camera.h"
#include "Resources/Texture.h"
#include "Resources/ScreenMesh.h"
#include "Resources/StaticMesh.h"
#include "Graphics/Geometry.h"
#include "Resources/Program.h"
#include "Framebuffers/SSAOFramebuffer.h"

#include "Pipelines/SRP/StandardRenderContext.h"

DECLARE_DEFAULTS_IMPL(StandardSSAOPass)

StandardSSAOPassUPtr StandardSSAOPass::Create(int32 width, int32 height)
{
    auto pass = StandardSSAOPassUPtr(new StandardSSAOPass());
    if (!pass->Init(width, height)) return nullptr;
    return std::move(pass);
}

bool StandardSSAOPass::Init(int32 width, int32 height)
{
    m_ssaoProgram = RESOURCE.GetResource<Program>("standard_ssao");
    m_ssaoBlurProgram = RESOURCE.GetResource<Program>("standard_ssao_blur");
    if (!m_ssaoProgram || !m_ssaoBlurProgram) return false;

    // FBO 생성 (우리가 추가한 CreateSSAO 사용)
    m_ssaoFBO = SSAOFramebuffer::Create(width, height);
    m_ssaoBlurFBO = SSAOFramebuffer::Create(width, height);
    if (!m_ssaoFBO || !m_ssaoBlurFBO) return false;

    // 화면 전체를 덮는 Quad 생성
    m_screenQuad = RESOURCE.GetResource<ScreenMesh>("Screen");
    if (!m_screenQuad) return false;

    GenerateKernel();
    GenerateNoiseTexture();

    // 커널은 초기화할 때 한 번 만 전송
    m_ssaoProgram->Use();
    m_ssaoProgram->SetUniform("samples", m_ssaoKernel);

    return true;
}

void StandardSSAOPass::Resize(int32 width, int32 height)
{
    m_ssaoFBO = SSAOFramebuffer::Create(width, height);
    m_ssaoBlurFBO = SSAOFramebuffer::Create(width, height);
}

void StandardSSAOPass::Render(RenderContext* context)
{
    // 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
    auto stdCtx = (StandardRenderContext*)context;
    
    // 1. SSAO 계산 (G-Buffer -> Raw SSAO)
    ComputeSSAO(stdCtx);

    // 2. 노이즈 제거 (Raw SSAO -> Blurred SSAO)
    BlurSSAOResult(stdCtx);

    // 3. context에 ssao 결과 캐싱
    stdCtx->SetSSAOTexture(m_ssaoBlurFBO->GetColorAttachment(0).get());
    
    // 4. 복귀
    Framebuffer::BindToDefault();
}

/*==============================//
//   ssao pass helper methods   //
//==============================*/
void StandardSSAOPass::GenerateKernel()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (uint32 i = 0; i < SSAO_KERNEL_SIZE; ++i)
    {
        glm::vec3 sample
        (
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = float(i) / (float)SSAO_KERNEL_SIZE;
        scale = Utils::Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        m_ssaoKernel.push_back(sample);
    }
}

void StandardSSAOPass::GenerateNoiseTexture()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoNoise;

    const uint32 noiseCount = SSAO_NOISE_DIM * SSAO_NOISE_DIM;
    for (uint32 i = 0; i < noiseCount; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f
        );
        ssaoNoise.push_back(noise);
    }

    // Texture::Create를 사용하여 포맷 지정 (GL_RGB16F or GL_RGB32F)
    // 4x4 크기, 노이즈는 반복되어야 하므로 GL_REPEAT 필수
    m_noiseTexture = Texture::Create(SSAO_NOISE_DIM, SSAO_NOISE_DIM, GL_RGB16F, GL_RGB, GL_FLOAT);
    m_noiseTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    m_noiseTexture->SetWrap(GL_REPEAT, GL_REPEAT);

    // 데이터 업로드 (우리가 추가한 SetData 사용)
    m_noiseTexture->SetData(ssaoNoise.data());
}

void StandardSSAOPass::ComputeSSAO(StandardRenderContext* context)
{
    Camera* camera = context->GetCamera();
    Texture* gPos = context->GetGBufferPosition();
    Texture* gNormal = context->GetGBufferNormal();
    if (!camera || !gPos || !gNormal) return;

    m_ssaoFBO->Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_ssaoProgram->Use();

    // 텍스처 바인딩
    glActiveTexture(GL_TEXTURE0); gPos->Bind();
    glActiveTexture(GL_TEXTURE1); gNormal->Bind();
    glActiveTexture(GL_TEXTURE2); m_noiseTexture->Bind();

    m_ssaoProgram->SetUniform("gPosition", 0);
    m_ssaoProgram->SetUniform("gNormal", 1);
    m_ssaoProgram->SetUniform("texNoise", 2);

    // 카메라 행렬 전송 (Context에서 가져온 카메라 사용)
    m_ssaoProgram->SetUniform("projection", camera->GetProjectionMatrix());
    m_ssaoProgram->SetUniform("view", camera->GetViewMatrix());

    m_screenQuad->Draw();
}

void StandardSSAOPass::BlurSSAOResult(StandardRenderContext* context)
{
    // SSAO Blur
    m_ssaoBlurFBO->Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_ssaoBlurProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    m_ssaoFBO->GetColorAttachment(0)->Bind();
    m_ssaoBlurProgram->SetUniform("ssaoInput", 0);

    m_screenQuad->Draw();
}
