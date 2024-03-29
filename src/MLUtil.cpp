#include <MLUtil.h>
#include <structs.hpp>
#include <vector>

namespace Utility
{
	namespace debug
	{

	}

	namespace check
	{
		template <typename T, typename T2 >  bool contains(T arry, T2 trgt)
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

		float GetLargestNumber(float n1, float n2, float n3)
		{
			if (n1 >= n2 && n1 >= n3)
				return n1;

			if (n2 >= n1 && n2 >= n3)
				return n2;

			if (n3 >= n1 && n3 >= n2)
				return n3;

			return n1;
		}

		int GetAttachedJiggleBone(int version, int boneIdx, int bone, int jBoneIdx, int maxBones)
		{
			int size = 120;

			switch(version)
			{
				case 49:
				{
					int boneSize = 216;
					int pos = boneIdx + (boneSize * bone) + jBoneIdx;
					int jiggleStart = boneIdx + (boneSize * maxBones);
					int dist = pos - jiggleStart;
					int curJBone = dist / size;

					return curJBone;

					break;
				}

				case 52:
				{
					int boneSize = 216;
					int pos = boneIdx + (boneSize * bone) + jBoneIdx;
					int jiggleStart = boneIdx + (boneSize * maxBones);
					int dist = pos - jiggleStart;
					int curJBone = dist / size;

					return curJBone;

					break;
				}

				case 53:
				{
					int boneSize = 244;
					int pos = boneIdx + (boneSize * bone) + jBoneIdx;
					int jiggleStart = boneIdx + (boneSize * maxBones);
					int dist = pos - jiggleStart;
					int curJBone = dist / size;

					return curJBone;

					break;
				}

				default:
				{
					return -1;

					break;
				}
			}
			return -1;
		}

		std::string ReadBoneFlags()
		{
			return "";
		}

		std::string ReadJiggleFlags(int flags)
		{
			std::string value = "";

			if (flags == 0) return "NONE";

			int flag = 0;

			int flagsFound = 0;

			for (int i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				if (flag != 0)
				{
					flagsFound++;
				}
			}

			int flagCounter = 0;
			for (int i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				switch (flag)
				{
				case 0:
				{
					break;
				}
				case JIGGLE_IS_FLEXIBLE:
				{
					value += "JIGGLE_IS_FLEXIBLE";
					break;
				}
				case JIGGLE_IS_RIGID:
				{
					value += "JIGGLE_IS_RIGID";
					break;
				}
				case JIGGLE_HAS_YAW_CONSTRAINT:
					value += "JIGGLE_HAS_YAW_CONSTRAINT";
					break;
				case JIGGLE_HAS_PITCH_CONSTRAINT:
					value += "JIGGLE_HAS_PITCH_CONSTRAINT";
					break;
				case JIGGLE_HAS_ANGLE_CONSTRAINT:
					value += "JIGGLE_HAS_ANGLE_CONSTRAINT";
					break;
				case JIGGLE_HAS_LENGTH_CONSTRAINT:
					value += "JIGGLE_HAS_LENGTH_CONSTRAINT";
					break;
				case JIGGLE_HAS_BASE_SPRING:
					value += "JIGGLE_HAS_BASE_SPRING";
					break;
				default:
					value += "UNK_"; value += std::to_string(flag).c_str();
					break;
				}

				if (flag != 0)
				{
					flagCounter++;
					if (flagCounter < flagsFound)
					{
						value += " | ";
					}
				}
			}

			return std::string(value);
		}

		std::string ReadAnimDescFlags()
		{
			return "";
		}

		std::string ReadSeqFlags()
		{
			return "";
		}

		std::string ReadMdlHdrFlags()
		{
			return "";
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
		{ 
			// IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
			const uint32_t e = (x & 0x7C00) >> 10; // exponent
			const uint32_t m = (x & 0x03FF) << 13; // mantissa
			const uint32_t v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
			return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
		}

		short float_to_half(const float x) 
		{ 
			// IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
			const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
			const uint32_t e = (b & 0x7F800000) >> 23; // exponent
			const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
			return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
		}
	}
}