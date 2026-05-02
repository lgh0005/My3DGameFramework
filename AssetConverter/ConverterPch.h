#pragma once

// C++
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <memory>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// stb & ktx
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>
#include <ktx.h>

// Defines
#include "Defines/DefineDisables.h"
#include "Defines/DefineSingleton.h"
#include "Defines/DefineConverters.h"
#include "Defines/DefineManagers.h"
#include "Defines/DefineClassPointers.h"
#include "Defines/DefineUtilities.h"
#include "Defines/DefineLoggings.h"

// Types
#include "Types/Types.h"
#include "Types/Containers.h"
#include "Types/Strings.h"
#include "Types/FileStream.h"
#include "Types/Threading.h"
#include "Types/Math.h"
#include "Types/SmartPtrs.h"
#include "Types/Delegates.h"

// Debug
#include "Debug/Logger.h"

// Constants
#include "Constants/ConstantAsset.h"
#include "Constants/ConstantMath.h"

// Assets
#include "Assets/CubeFaceElement.h"
#include "Assets/RawAnimation.h"
#include "Assets/RawAnimChannel.h"
#include "Assets/RawBone.h"
#include "Assets/RawBoneInfo.h"
#include "Assets/RawKeyPosition.h"
#include "Assets/RawKeyRotation.h"
#include "Assets/RawKeyScale.h"
#include "Assets/RawMaterial.h"
#include "Assets/RawMesh.h"
#include "Assets/RawModel.h"
#include "Assets/RawNode.h"
#include "Assets/RawStaticVertex.h"
#include "Assets/RawTexture.h"
#include "Assets/RawSkinnedVertex.h"
#include "Assets/RawImage.h"

// Animation
#include "Animation/Pose.h"
#include "Animation/AnimChannel.h"

// Core
#include "Core/AssetUtils.h"
#include "Core/MathUtils.h"
