#include "EnginePch.h"
#include "Material.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"

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
    glActiveTexture(GL_TEXTURE0);

    program->SetUniform("material.shininess", shininess);
}