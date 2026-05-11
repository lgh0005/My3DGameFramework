#pragma once

namespace MGF3D
{
	class ModelConverter
	{
		MGF_DECLARE_SINGLE(ModelConverter)

	private:
		ModelConverter();
		~ModelConverter();

	public:
		bool Convert
		(
			const String& inputPath,
			const String& outputPath,
			bool extractORM, bool flipY = true
		);

	private:
		bool m_extractORM{ false };
		bool m_flipY{ true };

		/*============================//
		//   main coversion methods   //
		//============================*/
	private:
		bool RunConversion();
		bool WriteCustomModelFile();
		void CreateORMTextureFromAssimp(aiMaterial* material, RawMaterial& rawMat, int32 index);

		// 모델 데이터 구조체
		RawModel m_rawModel;

		// 경로 문자열 멤버
		String m_inputPath;
		String m_outputPath;
		String m_modelDirectory;
		String m_modelName;

		/*====================================//
		//   default assimp process methods   //
		//====================================*/
	private:
		void ParseNodeHierarchy(const aiScene* scene);
		void ProcessMesh(aiMesh* mesh);
		void ProcessSkinnedMesh(aiMesh* mesh, RawMesh& rawMesh);
		void ProcessStaticMesh(aiMesh* mesh, RawMesh& rawMesh);
		RawMaterial ProcessMaterial(aiMaterial* material, int32 index);

		/*=========================================================//
		//   texture helper methods for material and ORM texture   //
		//=========================================================*/
	private:
		String ResolveTexturePath(const String& relativePath);
		void AddTextureToMaterial(RawMaterial& rawMat, aiMaterial* aiMat, aiTextureType aiType, RawTextureType rawType);
		String GetTexturePath(aiMaterial* material, aiTextureType type);
		void LogFinalMappedTextures(aiMaterial* material, const RawMaterial& rawMat);
		void ProcessTextureToKTX(RawMaterial& rawMat, const String& srcFileName, RawTextureType type);

		HashSet<String> m_convertedTextures;

		/*==========================================//
		//   skeleton and rigging process methods   //
		//==========================================*/
	private:
		void ExtractBoneWeights(Vector<RawSkinnedVertex>& vertices, aiMesh* mesh);
		void CollectBoneOffsets(const aiScene* scene);
		void FinalizeBoneIDs();

		int32 m_boneCounter{ 0 };
		HashMap<String, mat4> m_boneOffsets;
		HashMap<String, int32> m_boneNameToIdMap;

		/*=============================//
		//   multi-threading members   //
		//=============================*/
	private:
		Atomic<int32> m_activeTasks{ 0 };
		Mutex m_taskMutex;
		ConditionVariable m_taskCv;
		Mutex m_textureSetMutex;

		void AddTaskCount();
		void CompleteTaskCount();
		void WaitAllTasks();
	};
}