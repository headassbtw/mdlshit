#include <MLUtil.h>
#include <structs.hpp>
#include <vector>

namespace Utility
{
	namespace debug
	{
		int GetAnimHeaderSize(int flag)
		{
			switch (flag)
			{
			case 33: return 18;
			case 32: return 12;
			case 8: return 10;
			case 1: return 10;
			case 12: return 16;
			case 49: return 18;
			case 48: return 12;
			case 24: return 10;
			case 17: return 10;
			case 28: return 16;
			case 0: return 4;

			default: return 0;
			}
		}

		bool IsWhole(double n) {
			if (n == (int)n)
				return true;
			else
				return false;
		}

		//bool contains(std::vector<int> arry, int trgt)
		//{
		//	for (int i = 0; i < arry.size(); i++)
		//	{
		//		if (trgt == arry[i]) return true;
		//	}
		//	return false;
		//}
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

		//std::string ReadBoneFlags()
		//{
		//	return "";
		//}

		std::string ReadProcFlags(unsigned int flags)
		{
			std::string value = "";

			if (flags == 0)
				return "NONE";

			switch (flags)
			{
			case 0:
				break;
			case STUDIO_PROC_AXISINTERP:
				value += "STUDIO_PROC_AXISINTERP";
				break;
			case STUDIO_PROC_QUATINTERP:
				value += "STUDIO_PROC_QUATINTERP";
				break;
			case STUDIO_PROC_AIMATBONE:
				value += "STUDIO_PROC_AIMATBONE";
				break;
			case STUDIO_PROC_AIMATATTACH:
				value += "STUDIO_PROC_AIMATATTACH";
				break;
			case STUDIO_PROC_JIGGLE:
				value += "STUDIO_PROC_JIGGLE";
				break;
			case STUDIO_PROC_TWIST_MASTER:
				value += "STUDIO_PROC_TWIST_MASTER";
				break;
			case STUDIO_PROC_TWIST_SLAVE:
				value += "STUDIO_PROC_TWIST_SLAVE";
				break;
			default:
				value += std::string("UNK_%d", flags);
				break;
			}

			return value;
		}

		std::string ReadContents(int flags)
		{
			std::string value = "";

			if (flags == CONTENTS_EMPTY)
				return "CONTENTS_EMPTY";

			int i = 0;
			int flag = 0;

			int flagsFound = 0;

			for (i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				if (flag != 0)
				{
					flagsFound++;
				}
			}

			int flagCounter = 0;
			for (i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				switch (flag)
				{
				case CONTENTS_EMPTY:
					break;
				case CONTENTS_SOLID:
					value += "CONTENTS_SOLID";
					break;
				case CONTENTS_GRATE:
					value += "CONTENTS_GRATE";
					break;
				case CONTENTS_MONSTER:
					value += "CONTENTS_MONSTER";
					break;
				case CONTENTS_LADDER:
					value += "CONTENTS_LADDER";
					break;
				default:
					value += std::string("UNK_%d", flag);
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

			return value;
		}

		std::string ReadBoneFlags(int flags) //Borrowed from Rika's structs and modified a bit. Will be modified more for debug purposes. -Liberty
		{
			string value = "";

			if (flags == 0)
				return "NONE";

			int i = 0;
			int flag = 0;

			int flagsFound = 0;

			for (i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				if (flag != 0)
				{
					flagsFound++;
				}
			}

			int flagCounter = 0;
			for (i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				switch (flag)
				{
				case 0:
					break;
				case BONE_CALCULATE_MASK:
					value += "BONE_CALCULATE_MASK";
					break;
				case BONE_PHYSICALLY_SIMULATED:
					value += "BONE_PHYSICALLY_SIMULATED";
					break;
				case BONE_PHYSICS_PROCEDURAL:
					value += "BONE_PHYSICS_PROCEDURAL";
					break;
				case BONE_ALWAYS_PROCEDURAL:
					value += "BONE_ALWAYS_PROCEDURAL";
					break;
				case BONE_SCREEN_ALIGN_SPHERE:
					value += "BONE_SCREEN_ALIGN_SPHERE";
					break;
				case BONE_SCREEN_ALIGN_CYLINDER:
					value += "BONE_SCREEN_ALIGN_CYLINDER";
					break;
				case BONE_USED_BY_IKCHAIN:
					value += "BONE_USED_BY_IKCHAIN";
					break;
				case BONE_USED_BY_ANYTHING:
					value += "BONE_USED_BY_ANYTHING";
					break;
				case BONE_USED_BY_HITBOX:
					value += "BONE_USED_BY_HITBOX";
					break;
				case BONE_USED_BY_ATTACHMENT:
					value += "BONE_USED_BY_ATTACHMENT";
					break;
				case BONE_USED_BY_VERTEX_MASK:
					value += "BONE_USED_BY_VERTEX_MASK";
					break;
				case BONE_USED_BY_VERTEX_LOD0:
					value += "BONE_USED_BY_VERTEX_LOD0";
					break;
				case BONE_USED_BY_VERTEX_LOD1:
					value += "BONE_USED_BY_VERTEX_LOD1";
					break;
				case BONE_USED_BY_VERTEX_LOD2:
					value += "BONE_USED_BY_VERTEX_LOD2";
					break;
				case BONE_USED_BY_VERTEX_LOD3:
					value += "BONE_USED_BY_VERTEX_LOD3";
					break;
				case BONE_USED_BY_VERTEX_LOD4:
					value += "BONE_USED_BY_VERTEX_LOD4";
					break;
				case BONE_USED_BY_VERTEX_LOD5:
					value += "BONE_USED_BY_VERTEX_LOD5";
					break;
				case BONE_USED_BY_VERTEX_LOD6:
					value += "BONE_USED_BY_VERTEX_LOD6";
					break;
				case BONE_USED_BY_VERTEX_LOD7:
					value += "BONE_USED_BY_VERTEX_LOD7";
					break;
				case BONE_USED_BY_BONE_MERGE:
					value += "BONE_USED_BY_BONE_MERGE";
					break;
				case BONE_FLAG_UNK_53:
					value += "BONE_FLAG_UNK_53";
					break;
				case BONE_TYPE_MASK:
					value += "BONE_TYPE_MASK";
					break;
				case BONE_FIXED_ALIGNMENT:
					value += "BONE_FIXED_ALIGNMENT";
					break;
				case BONE_HAS_SAVEFRAME_POS:
					value += "BONE_HAS_SAVEFRAME_POS";
					break;
				case BONE_HAS_SAVEFRAME_ROT64:
					value += "BONE_HAS_SAVEFRAME_ROT64";
					break;
				case BONE_HAS_SAVEFRAME_ROT32:
					value += "BONE_HAS_SAVEFRAME_ROT32";
					break;
				default:
					value += string("UNK_%d", flag);
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

			return value;
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

		std::string ReadAnimDescFlags(int flags)
		{
			string value = "";

			if (flags == 0)
				return "NONE";

			int i = 0;
			int flag = 0;

			int flagsFound = 0;

			for (i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				if (flag != 0)
				{
					flagsFound++;
				}
			}

			int flagCounter = 0;
			for (i = 0; i < 32; i++)
			{
				flag = flags & (1 << i);

				switch (flag)
				{
				case 0:
					break;
				case 0x1:
					value += "STUDIO_LOOPING";
					break;
				case 0x2:
					value += "STUDIO_SNAP";
					break;
				case 0x4:
					value += "STUDIO_DELTA";
					break;
				case 0x8:
					value += "STUDIO_AUTOPLAY";
					break;
				case 0x10:
					value += "STUDIO_POST";
					break;
				case 0x20:
					value += "STUDIO_ALLZEROS";
					break;
				case 0x40:
					value += "STUDIO_FRAMEANIM";
					break;
				case 0x80:
					value += "STUDIO_CYCLEPOSE";
					break;
				case 0x100:
					value += "STUDIO_REALTIME";
					break;
				case 0x200:
					value += "STUDIO_LOCAL";
					break;
				case 0x400:
					value += "STUDIO_HIDDEN";
					break;
				case 0x800:
					value += "STUDIO_OVERRIDE";
					break;
				case 0x1000:
					value += "STUDIO_ACTIVITY";
					break;
				case 0x2000:
					value += "STUDIO_EVENT";
					break;
				case 0x4000:
					value += "STUDIO_WORLD";
					break;
				case 0x8000:
					value += "STUDIO_NOFORCELOOP";
					break;
				case 0x10000:
					value += "STUDIO_EVENT_CLIENT";
					break;
				case 0x20000:
					value += "STUDIO_ANIM_UNK";
					break;
				case STUDIO_FRAMEMOVEMENT:
					value += "STUDIO_FRAMEMOVEMENT";
					break;
				case STUDIO_ANIMDESC_53_UNK1:
					value += "STUDIO_ANIMDESC_53_UNK1";
					break;
				default:
					value += std::string("UNK_%x", flag);
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

			return value;
		}

		std::string ReadSeqFlags()
		{
			return "";
		}

		std::string ReadMdlHdrFlags()
		{
			return "";
		}

		template <typename T> int GetTrgtPosInArry(std::vector<T> arry, T trgt)
		{
			for (int i = 0; i < arry.size(); i++)
			{
				if (trgt == arry[i]) return i;
			}
			return -1;
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