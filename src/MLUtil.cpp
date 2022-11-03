#include <MLUtil.hpp>
#include <vector>

namespace Utility
{
	namespace debug
	{

	}

	namespace check
	{
		template <typename T>  bool contains(T arry, T trgt)
		{
			for (int i = 0; i < sizeof(arry); i++)
			{
				if (trgt == arry[i]) return true;
			}
			return false;
		}

		bool IsWhole(double n) {
			if (n == (int)n)
				return true;
			else
				return false;
		}
	}

	namespace func
	{
		template <typename T> std::vector<T> ArrayToVector(T* arry, int size)
		{
			std::vector<T> _vec;
			for (int i = 0; i < size; i++)
			{
				_vec.push_back(arry[i]);
			}
			return _vec;
		}

		float GetSecondLargest(float n1, float n2, float n3)
		{
			float n = 3, num[]{ n1,n2,n3 }, largest, second;

			/* Here we are comparing first two elements of the
			 * array, and storing the largest one in the variable
			 * "largest" and the other one to "second" variable.
			 */
			if (num[0] < num[1]) {
				largest = num[1];
				second = num[0];
			}
			else {
				largest = num[0];
				second = num[1];
			}
			for (int i = 2; i < n; i++) {
				/* If the current array element is greater than largest
				 * then the largest is copied to "second" and the element
				 * is copied to the "largest" variable.
				 */
				if (num[i] > largest) {
					second = largest;
					largest = num[i];
				}
				/* If current array element is less than largest but greater
				 * then second largest ("second" variable) then copy the
				 * element to "second"
				 */
				else if (num[i] > second && num[i] != largest) {
					second = num[i];
				}
			}
			return second;
			return 0;
		}

		float GetLargestNumber(float n1, float n2, float n3)
		{
			//if (n1 > n2 && n1 == n2 || n1 > n2 && n1 > n3 && n2 == n3 || n1 > n2 && n1 < n3 || n1 > n2 && n3 < n2)
			//	return n1;
			//
			//if (n2 > n1 && n2 == n3 || n2 > n1 && n2 > n3 && n1 == n3 || n2 > n1 && n2 < n3 || n2 > n1 && n3 < n1)
			//	return n2;
			//
			//if (n3 > n1 && n3 == n2 || n3 > n1 && n3 > n2 && n1 == n2 || n3 > n1 && n3 < n2 || n3 > n2 && n3 < n1)
			//	return n3;

			if (n1 >= n2 && n1 >= n3)
				return n1;

			if (n2 >= n1 && n2 >= n3)
				return n2;

			if (n3 >= n1 && n3 >= n2)
				return n3;

			return n1;
		}
	}

	namespace math
	{
		uint32_t as_uint(const float x) 
		{
			return *(uint32_t*)&x;
		}
		float as_float(const uint32_t x) 
		{
			return *(float*)&x;
		}

		float half_to_float(const uint16_t x) 
		{ // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
			const uint32_t e = (x & 0x7C00) >> 10; // exponent
			const uint32_t m = (x & 0x03FF) << 13; // mantissa
			const uint32_t v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
			return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
		}

		short float_to_half(const float x) 
		{ // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
			const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
			const uint32_t e = (b & 0x7F800000) >> 23; // exponent
			const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
			return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
		}
	}
}