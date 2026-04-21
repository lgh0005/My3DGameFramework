#include "GraphicsPch.h"
#include "Shader.h"
#include "Shader/GLShader.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Shader::Shader
	(
		const String& path, 
		GLenum shaderType, 
		EShaderFileType fileType, 
		const String& entryPoint
	)
		: Super(path), m_shaderType(shaderType), m_fileType(fileType), m_entryPoint(entryPoint) { }
	Shader::~Shader() = default;

	ShaderPtr Shader::Create(const String& path, GLenum shaderType, EShaderFileType fileType, const String& entryPoint)
	{
		return SharedPtr<Shader>(new Shader(path, shaderType, fileType, entryPoint));
	}

	/*========================//
	//       Shader Type      //
	//========================*/
	int16 Shader::s_typeIndex = -1;
	const MGFType* Shader::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Asset");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool Shader::Load()
	{
		m_state = EAssetState::Loading;

		GLShaderPtr resource = nullptr;
		switch (m_fileType)
		{
			case EShaderFileType::GLSL: resource = LoadGLSL(); break;
			case EShaderFileType::SPIRV: resource = LoadSpirV(); break;
		}
		if (!resource)
		{
			m_state = EAssetState::Failed;
			return false;
		}

		AddResource(resource);
		m_state = EAssetState::Loaded;
		return true;
	}

	GLShaderPtr Shader::LoadGLSL()
	{
		InputFileStream fin(m_path.c_str());
		if (!fin.is_open())
		{
			MGF_LOG_ERROR("Failed to open GLSL file: {}", m_path.c_str());
			return nullptr;
		}

		StringStream text;
		text << fin.rdbuf();
		String code = text.str();
		if (code.empty())
		{
			MGF_LOG_ERROR("GLSL file is empty: {}", m_path.c_str());
			return nullptr;
		}

		return GLShader::Create(m_shaderType, code);
	}

	GLShaderPtr Shader::LoadSpirV()
	{
		InputFileStream file(m_path.c_str(), std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			MGF_LOG_ERROR("Failed to open SPIR-V file: {}", m_path.c_str());
			return nullptr;
		}

		usize fileSize = static_cast<usize>(file.tellg());
		if (fileSize == 0)
		{
			MGF_LOG_ERROR("SPIR-V file is empty: {}", m_path.c_str());
			return nullptr;
		}

		Vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return GLShader::Create(m_shaderType, std::move(buffer), m_entryPoint);
	}
}