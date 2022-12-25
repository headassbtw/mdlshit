#pragma once
#include <vector>
#include <string>
namespace Utility
{
	namespace debug
	{

	}

	namespace check
	{
		template <typename T, typename T2>  bool contains(T arry, T2 trgt);
	}

	namespace func
	{
		template <typename T> std::vector<T> ArrayToVector(T* arry, int size);

		float GetSecondLargest(float n1, float n2, float n3);

		float GetLargestNumber(float n1, float n2, float n3);

		int GetAttachedJiggleBone(int version, int boneIdx, int bone, int jBoneIdx, int maxBones);

		std::string ReadJiggleFlags(int flags);
	}

	namespace math
	{
		uint32_t as_uint(const float x);

		float as_float(const uint32_t x);

		float half_to_float(const uint16_t x);

		short float_to_half(const float x);
	}
}