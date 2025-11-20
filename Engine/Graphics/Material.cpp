#include "EnginePch.h"
#include "Material.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"
#include "Graphics/Image.h"

void Material::SetToProgram(const Program* program) const 
{
    int textureCount = 0;
    if (diffuse) 
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.diffuse", textureCount);
        diffuse->Bind();
        textureCount++;
    }
    if (specular) 
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.specular", textureCount);
        specular->Bind();
        textureCount++;
    }
    if (emission)
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.emission", textureCount);
        emission->Bind();
        textureCount++;
    }
    if (normal)
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.normal", textureCount);
        normal->Bind();
        textureCount++;
    }
    glActiveTexture(GL_TEXTURE0);

    program->SetUniform("material.shininess", shininess);
    program->SetUniform("material.emissionStrength", emissionStrength);
}