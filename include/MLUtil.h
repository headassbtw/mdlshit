#pragma once
#include <vector>
namespace Utility
{
	namespace debug
	{

	}

	namespace check
	{
		template <typename T>  bool contains(T* arry, T trgt);
	}

	namespace func
	{
		template <typename T> std::vector<T> ArrayToVector(T* arry, int size);

		float GetSecondLargest(float n1, float n2, float n3);

		float GetLargestNumber(float n1, float n2, float n3);
	}

	namespace math
	{
		uint32_t as_uint(const float x);

		float as_float(const uint32_t x);

		float half_to_float(const uint16_t x);

		short float_to_half(const float x);
	}
}