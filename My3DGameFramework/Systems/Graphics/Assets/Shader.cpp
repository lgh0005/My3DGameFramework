#include "GraphicsPch.h"
#include "Shader.h"
#include "Shader/GLShader.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Shader::Shader(const String& path, GLenum shaderType, EShaderFileType fileType, const String& entryPoint)
		: Super(path), m_shaderType(shaderType), m_fileType(fileType), m_entryPoint(entryPoint) { }
	Shader::~Shader() = default;

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

		auto shader = MakeShared<GLShader>();
		if (!shader) return false;

		bool loadSuccess = false;
		switch (m_fileType)
		{
			case EShaderFileType::GLSL: loadSuccess = LoadGLSL(shader); break;
			case EShaderFileType::SPIRV: loadSuccess = LoadSpirV(shader); break;
		}
		if (!loadSuccess) return false;

		shader->SetState(EResourceState::Loaded);
		AddResource(shader);
		m_state = EAssetState::Loaded;

		return true;
	}

	bool Shader::LoadGLSL(const GLShaderPtr& shader)
	{
		InputFileStream fin(m_path.c_str());
		if (!fin.is_open())
		{
			MGF_LOG_ERROR("Failed to open GLSL file: {}", m_path.c_str());
			m_state = EAssetState::Failed;
			return false;
		}

		StringStream text;
		text << fin.rdbuf();
		String code = text.str();
		if (code.empty())
		{
			MGF_LOG_ERROR("GLSL file is empty: {}", m_path.c_str());
			m_state = EAssetState::Failed;
			return false;
		}

		shader->SetSourceGLSL(m_shaderType, code);
		return true;
	}

	bool Shader::LoadSpirV(const GLShaderPtr& shader)
	{
		InputFileStream file(m_path.c_str(), std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			MGF_LOG_ERROR("Failed to open SPIR-V file: {}", m_path.c_str());
			m_state = EAssetState::Failed;
			return false;
		}

		usize fileSize = static_cast<usize>(file.tellg());
		if (fileSize == 0)
		{
			m_state = EAssetState::Failed;
			return false;
		}

		Vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		shader->SetSourceSpirv(m_shaderType, std::move(buffer), m_entryPoint);
		return true;
	}
}