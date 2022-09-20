#include <conv.hpp>
#include <logger.hpp>
#include <structs.hpp>
#include <binarystream.hpp>
#include <structs.hpp>
#include <argtools.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <rendering/progress.hpp>
#include <vector>
#include <half.hpp>
uint32_t LBD_GetFlags(BinaryReader* Stream, int index)
{
	//Logger::Info("flagsPos: %d %d\n", index);
	Stream->seek(index);
	uint32_t flags; Stream->Read(&flags);
	return flags;

}

uint32_t LBD_GetParents(BinaryReader* Stream, int index)
{
	//Logger::Info("parentsPos: %d %d\n", index);
	Stream->seek(index);
	uint32_t parents; Stream->Read(&parents);
	return parents;
}

Vector3 LBD_GetBonePos(BinaryReader* Stream, int index)
{
	//Logger::Info("bonePos: %d %d\n", index);
	Stream->seek(index);
	Vector3 bonePos; Stream->Read(&bonePos);
	return bonePos;
}

Quaternion LBD_GetBoneQuat(BinaryReader* Stream, int index)
{
	//Logger::Info("boneQuatPos: %d %d\n", index);
	Stream->seek(index);
	Quaternion boneQuat; Stream->Read(&boneQuat);
	return boneQuat;
}

RadianEuler LBD_GetBoneRot(BinaryReader* Stream, int index)
{
	//Logger::Info("boneRotPos: %d %d\n", index);
	Stream->seek(index);
	RadianEuler boneRot; Stream->Read(&boneRot);
	return boneRot;
}

matrix3x4_t LBD_GetPoseToBone(BinaryReader* Stream, int index)
{
	//Logger::Info("poseToBonePos: %d %d\n", index);
	Stream->seek(index);
	matrix3x4_t poseToBone; Stream->Read(&poseToBone);
	return poseToBone;
}

Vector3 LBD_GetPosScale(BinaryReader* Stream, int index)
{
	//Logger::Info("posScalePos: %d %d\n", index);
	Stream->seek(index);
	Vector3 posScale; Stream->Read(&posScale);
	return posScale;
}

Vector3 LBD_GetRotScale(BinaryReader* Stream, int index)
{
	//Logger::Info("rotScalePos: %d %d\n", index);
	Stream->seek(index);
	Vector3 rotScale; Stream->Read(&rotScale);
	return rotScale;
}

Quaternion LBD_GetBoneAlignment(BinaryReader* Stream, int index)
{
	//Logger::Info("boneAlignmentPos: %d %d\n", index);
	Stream->seek(index);
	Quaternion boneAlignment; Stream->Read(&boneAlignment);
	return boneAlignment;
}


LinearBoneData* GetLinearBoneData(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int bone)
{
	LinearBoneData* boneData = new LinearBoneData();
	int pos = Stream->Position();
	if (bone < Initial_Header->numbones)
	{
		int filler = 4 * 6; 
		Stream->seek(Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex);//Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex);
		int idx = Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex;//Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex;
		int numOfBones; Stream->Read(&numOfBones);
		int flagIdx; Stream->Read(&flagIdx);
		int parentIdx; Stream->Read(&parentIdx);
		int bonePosIdx; Stream->Read(&bonePosIdx);
		int boneQuatIdx; Stream->Read(&boneQuatIdx);
		int boneRotIdx; Stream->Read(&boneRotIdx);
		int poseToBoneIdx; Stream->Read(&poseToBoneIdx);
		int posScaleIdx; Stream->Read(&posScaleIdx);
		int rotScaleIdx; Stream->Read(&rotScaleIdx);
		int boneAlignmentIdx; Stream->Read(&boneAlignmentIdx);
		//Logger::Info("flagIdx: %d %d\n", flagIdx);
		//Logger::Info("parentIdx: %d %d\n", parentIdx);
		//Logger::Info("BonePosIdx: %d %d\n", bonePosIdx);
		//Logger::Info("boneQuatIdx: %d %d\n", boneQuatIdx);
		//Logger::Info("boneRotIdx: %d %d\n", boneRotIdx);
		//Logger::Info("poseToBoneIdx: %d %d\n", poseToBoneIdx);
		//Logger::Info("posScaleIdx: %d %d\n", posScaleIdx);
		//Logger::Info("rotScaleIdx: %d %d\n", rotScaleIdx);
		//Logger::Info("boneAlignmentIdx: %d %d\n", boneAlignmentIdx);
		//Logger::Info("Bone: %d %d\n", bone);

		boneData->flags = LBD_GetFlags(Stream, idx + flagIdx + (sizeof(uint32_t) * bone));
		//Logger::Info("flags_check\n");
		boneData->parents = LBD_GetParents(Stream, idx + parentIdx + (sizeof(uint32_t) * bone));
		//Logger::Info("parents_check\n");
		boneData->bonePos = LBD_GetBonePos(Stream, idx + bonePosIdx + (sizeof(Vector3) * bone));
		//Logger::Info("bonePos_check\n");
		boneData->boneQuat = LBD_GetBoneQuat(Stream, idx + boneQuatIdx + (sizeof(Quaternion) * bone));
		//Logger::Info("boneQuat_check\n");
		boneData->boneRot = LBD_GetBoneRot(Stream, idx + boneRotIdx + (sizeof(RadianEuler) * bone));
		//Logger::Info("boneRot_check\n");
		boneData->poseToBone = LBD_GetPoseToBone(Stream, idx + poseToBoneIdx + (sizeof(matrix3x4_t) * bone));
		//Logger::Info("poseToBone_check\n");
		boneData->posScale = LBD_GetPosScale(Stream, idx + posScaleIdx + (sizeof(Vector3) * bone));
		//Logger::Info("posScale_check\n");
		boneData->rotScale = LBD_GetRotScale(Stream, idx + rotScaleIdx + (sizeof(Vector3) * bone));
		//Logger::Info("rotScale_check\n");
		boneData->boneAlignment = LBD_GetBoneAlignment(Stream, idx + boneAlignmentIdx + (sizeof(Quaternion) * bone));
		//Logger::Info("boneAlignment_check\n");
		Stream->seek(pos);
	}
	Stream->seek(pos);
	return boneData;
}

int GetAnimHeaderSize(int flag)
{
	switch (flag)
	{
	case 33: return 18;
	case 32: return 12;
	case 8: return 10;
	case 1: return 10;
	case 12: return 16;

	default: return 0;
	}
}
uint32_t as_uint(const float x) {
	return *(uint32_t*)&x;
}
float as_float(const uint32_t x) {
	return *(float*)&x;
}

float half_to_float(const uint16_t x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint32_t e = (x & 0x7C00) >> 10; // exponent
	const uint32_t m = (x & 0x03FF) << 13; // mantissa
	const uint32_t v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
	return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}

uint16_t float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const uint32_t e = (b & 0x7F800000) >> 23; // exponent
	const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

int* GetAnimSectionCount(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int secNum = 0;
	int* secPerAnim = new int[Initial_Header->numlocalanim];
	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int startPos = Initial_Header->localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		Stream->seek(framePos);
		int frames; Stream->Read(&frames);
		Stream->seek(secFramPos);
		int secFrames; Stream->Read(&secFrames);

		Stream->seek(animIndexPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(secIndexPos);
		int sectionIndex; Stream->Read(&sectionIndex);

		if (sectionIndex > 0)
		{
			int secNumber = (frames / secFrames) + 2;
			if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, secNumber);
			secPerAnim[i] = secNumber;
		}
		else secPerAnim[i] = 0;



	}
	return secPerAnim;
}


void ConvertBoneHeader(BinaryReader* Stream, int num, BoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2) //hacky way of converting the headers
{
	//Logger::Info("ConvBoneHdrPos: %d %d\n", Stream->Position());
	int pos2 = Stream->Position();
	std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
	std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
	short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
	short origOffset = nextOffset;
	int strEndPos = Stream->Position();
	nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;

	switch ((int)flag)
	{
	case 12:
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
		//boneHeaders_v53[num].rawStrPos = Stream->Position();
		Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
		strEndPos = Stream->Position();
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.k = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.one = (short)0;
		boneHeaders_v53[num].rawPos = posPtr;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 32:
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.i = (short)rawRot.one; boneHeaders_v53[num].packedRotation.j = (short)rawRot.i; boneHeaders_v53[num].packedRotation.k = (short)rawRot.j; boneHeaders_v53[num].packedRotation.one = (short)rawRot.k;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[num].rawPos.y = (short)boneData->bonePos.y; boneHeaders_v53[num].rawPos.z = (short)boneData->bonePos.z;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 33:
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		Vector3Short rawPos; Stream->Read(&rawPos);
		strEndPos = Stream->Position();
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.i = (short)rawRot.one; boneHeaders_v53[num].packedRotation.j = (short)rawRot.i; boneHeaders_v53[num].packedRotation.k = (short)rawRot.j; boneHeaders_v53[num].packedRotation.one = (short)rawRot.k;
		boneHeaders_v53[num].rawPos = rawPos;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 8:
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.k = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.one = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[num].rawPos.y = (short)boneData->bonePos.y; boneHeaders_v53[num].rawPos.z = (short)boneData->bonePos.z;// = boneData->bonePos;  //boneHeaders_v53[j].rawPos.x = 0; boneHeaders_v53[j].rawPos.y = 0; boneHeaders_v53[j].rawPos.z = 0; //= boneData->bonePos;  //boneHeaders_v53[j].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[j].rawPos.y = (short)boneData->bonePos.x; boneHeaders_v53[j].rawPos.z = (short)boneData->bonePos.z;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 1:
	{
		Vector3Short rawPos; Stream->Read(&rawPos);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0; boneHeaders_v53[num].packedRotation.one = (short)0;
		boneHeaders_v53[num].rawPos = rawPos;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}
	}
	Stream->seek(pos2 + origOffset);//(GetAnimHeaderSize((int)boneHeaders_v53[num].r2Flag)));
	//Logger::Info("Bone: %d, Flag:  %d, nextOffset:  %d, \n", boneHeaders_v53[num].bone, boneHeaders_v53[num].r2Flag, boneHeaders_v53[num].nextOffset);
}

bool Contains(int* arry, int trgt, int count)
{
	for (int i = 0; i < count; i++)
	{
		if (trgt == arry[i]) return true;
	}
	return false;
}

int* GetAnimsWithSections(BinaryReader* Stream, v49_Header* Initial_Header)
{
	int pos = Stream->Position();                                   //Mark Pos
	int* animsWithSections = new int[Initial_Header->numlocalanim]; //Create array for anims
	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int strPos = Initial_Header->localanimindex + (100 * i);    //Calculate start of animDesc
		Stream->seek(strPos + 80);                                  //Go to sectionIndex
		int strSecIdx; Stream->Read(&strSecIdx);                    //Read sectionIndex
		if (strSecIdx > 0) animsWithSections[i] = i;                //Add anim number to array if secIdx is greater than zero else add -1
		else animsWithSections[i] = -1;
	}
	Stream->seek(pos);                                              //Return to pos
	return animsWithSections;                                       //Return array
}

int* GetAnimSecs(BinaryReader* Stream, v49_Header* Initial_Header, bool secNumber)
{
	int pos = Stream->Position();                                   //Mark Pos
	int num = 0;
	int* sectionsPerAnim = new int[Initial_Header->numlocalanim];


	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int strPos = Initial_Header->localanimindex + (100 * i);        //Calculate start of animDesc
		Stream->seek(strPos + 56);                                      //Go to anim index
		int animIdx; Stream->Read(&animIdx);                            //Read anim index
		Stream->seek(strPos + 80);                                      //Go to section index
		int strSecIdx; Stream->Read(&strSecIdx);                        //Read section index
		int strSectionNum; Stream->Read(&strSectionNum);                //Read number of section frames
		if (strSecIdx > 0)                                              //If anim has sections or not
		{
			int secStrPos = strPos + strSecIdx;                         //Get animSec stream start pos
			int secNum = ( (strPos + animIdx - 8) - secStrPos) / 8;   //Calculate number of sections via offset division minus divider bytes
			num += secNum;                                              //Increase number of sections by secNum calculation
			//Logger::Info("secNum_2: %d %d\n", secNum);
			sectionsPerAnim[i] = secNum;
		}
		else if(strSecIdx == 0) sectionsPerAnim[i] = 0;
	}
	int* animSections = new int[num];                                   //Create array for animSecs
	Stream->seek(pos);                                                  //Return to pos
	if (secNumber) return sectionsPerAnim;                               //Return section number per anim array
	else
		return animSections;                                                //Return anim section array
}

int* GetBytesAddedToBoneHdrs(BinaryReader* Stream, v49_Header* Initial_Header, int returnIdx, bool debug )
{
	int bytesAdded = 0;
	int numOfHeaders = 0;
	int* animHdrBytesAdd = new int[Initial_Header->numlocalanim];
	int* animSecBytesAdd = new int[Initial_Header->numlocalanim];
	int* secHdrBytesAdd = GetAnimSecs(Stream, Initial_Header, false);
	int* animsWithSections = GetAnimsWithSections(Stream, Initial_Header);
	int* secHdrPerAnim = GetAnimSecs(Stream, Initial_Header, true);
	int secNum = 0;
	int tempNum = 0;
	int secBytesAdded = 0;
	int secHdrNum = 0;



	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int PISS2 = -((8 * (Initial_Header->numlocalanim - i)));
		int pos = Initial_Header->localanimindex + 100 * i;
		Stream->seek(pos + 56);
		int	 animindex; Stream->Read(&animindex);
		Stream->seek(pos + animindex);
		int lastHdrPos = 0;
		int lastSecHdr = 0;
		for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++) //It is time. -Liberty
		{
				int pos2 = Stream->Position();
				std::byte bone; Stream->Read(&bone);
				std::byte flag; Stream->Read(&flag);
				short nextOffset; Stream->Read(&nextOffset);
				bytesAdded += 32 - GetAnimHeaderSize((int)flag);

				numOfHeaders++;
				Stream->seek(pos2 + nextOffset);

				if (nextOffset == 0)
				{
					if(i + 1 < Initial_Header->numlocalanim) animHdrBytesAdd[i + 1] = bytesAdded;
					if (debug) Logger::Info("animHdrBytesAdded: %d, animNum %d\n", bytesAdded, i);
					//secHdrPerAnim[i] = numOfHeaders;

					if (Contains(animsWithSections, i, Initial_Header->numlocalanim))
					{
						int animPos = Initial_Header->localanimindex + 100 * (i);

						Stream->seek(animPos + 80);
						int strSectionIndex; Stream->Read(&strSectionIndex);
						int strSectionNum; Stream->Read(&strSectionNum);
						int secStrPos = animPos + strSectionIndex;
						int curSec = 1;
						int num = ( (animPos + animindex - 8) - secStrPos) / 8;
						int* secStrIndexes = new int[num];
						int maxSec = secNum + num;


						for (int k = 1; k < num; k++)
						{
							Stream->seek(secStrPos + 8 * k);
							int animBlock; Stream->Read(&animBlock);
							int animOffset; Stream->Read(&animOffset);
							secStrIndexes[k] = animOffset;
						}

						for (int k = secNum + 1; k < maxSec; k++)
						{
							Stream->seek(animPos + secStrIndexes[curSec]);

							for (int l = 0; l < Initial_Header->numbones * Initial_Header->numlocalanim; l++)
							{
								int secHdrPos = Stream->Position();
								std::byte secBone; Stream->Read(&secBone);
								std::byte secFlag; Stream->Read(&secFlag);
								short secNextOffset; Stream->Read(&secNextOffset);
								secBytesAdded += 32 - GetAnimHeaderSize((int)secFlag);

								secHdrNum++;
								Stream->seek(secHdrPos + secNextOffset);

								if (secNextOffset == 0)
								{
									if(debug) Logger::Info("secBytesAdded: %d, secHdrNum %d\n", secBytesAdded, k);
									lastHdrPos = Stream->Position();
									animSecBytesAdd[i] = secBytesAdded;
									secHdrBytesAdd[k] = secBytesAdded;
									secHdrPerAnim[i] = secHdrNum;
									curSec += 1;
									lastSecHdr = i;
									tempNum = num;
									//secBytesAdded = 0;
									break;
								}
							}
						}
					}
					break;
				}
		}
		secNum += tempNum;
	}
	switch (returnIdx)
	{
		case 0: return animHdrBytesAdd; //total bytes added to an animDesc

		case 1: return animSecBytesAdd; //total bytes added to an animDesc from the sections

		case 2: return secHdrBytesAdd; //bytes added to each anim section

		default: return animHdrBytesAdd; 
	}
}


int GetBoneHeaderNum(BinaryReader* Stream, v49_Header* Initial_Header, int returnIdx, bool debug)
{
	int* animsWithSections = GetAnimsWithSections(Stream, Initial_Header);
	int* numOfHdrsPerAnim = new int[Initial_Header->numlocalanim];
	int numOfHeaders = 0;
	int secNum = 0;
	int tempNum = 0;
	int secHdrNum = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int animPos = Initial_Header->localanimindex + 100 * i;
		int basePtr = -Stream->Position();
		int animIdxPos = animPos + 56;
		Stream->seek(animIdxPos);
		int animIdx; Stream->Read(&animIdx);
		Stream->seek(animPos + 80);
		int sectionIdx; Stream->Read(&sectionIdx);
		Stream->seek(animPos + animIdx);
		//Logger::Info("_check\n");

		for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++) //It is time. -Liberty
		{
			int pos2 = Stream->Position();
			std::byte bone; Stream->Read(&bone);
			std::byte flag; Stream->Read(&flag);
			short nextOffset; Stream->Read(&nextOffset);

			numOfHeaders++;
			Stream->seek(pos2 + nextOffset);
			//Logger::Info("_check2\n");
			if (nextOffset == 0)
			{
				numOfHdrsPerAnim[i] = j;
				if (Contains(animsWithSections, i, Initial_Header->numlocalanim))
				{
					Stream->seek(sectionIdx - basePtr);
					//Logger::Info("_check3\n");
					int secStrPos = animPos + sectionIdx;
					int curSec = 1;
					int num = ((animPos + animIdx - 8) - secStrPos) / 8;
					int* secStrIndexes = new int[num];
					int maxSec = secNum + num;


					for (int k = 0; k < num; k++)
					{
						if (debug) Logger::Info("secNum1: %d, maxSecNum1 %d\n", k, num);
						Stream->seek(secStrPos + 8 * k);
						//Logger::Info("_check4\n");
						int animBlock; Stream->Read(&animBlock);
						int animOffset; Stream->Read(&animOffset);

						if (k > 0)
						{
							for (int l = secNum + 1; l < maxSec; l++)
							{
								Stream->seek((animPos + animOffset));
								//Logger::Info("_check5\n");

								for (int m = 0; m < Initial_Header->numbones * Initial_Header->numlocalanim; m++)
								{
									int secHdrPos = Stream->Position();
									std::byte secBone; Stream->Read(&secBone);
									std::byte secFlag; Stream->Read(&secFlag);
									short secNextOffset; Stream->Read(&secNextOffset);

									secHdrNum++;

									if (secNextOffset == 0)
									{
										curSec += 1;
										tempNum = num;
										break;
									}
									Stream->seek(secHdrPos + (int)secNextOffset);
									//Logger::Info("_check6\n");
								}
							}
						}
						secNum++;
					}
				}
				break;
			}
		}
	}

	switch (returnIdx)
	{
	case 0: return numOfHeaders;

	case 1: return secHdrNum;

	case 2: return secNum;

	default: return 0;
	}
}

int* GetBoneHeaderNumList(BinaryReader* Stream, v49_Header* Initial_Header, int returnIdx, bool debug)
{
	int* animsWithSections = GetAnimsWithSections(Stream, Initial_Header);
	int* numOfHdrsPerAnim = new int[Initial_Header->numlocalanim];
	int* numOfSecHdrsPerAnim = new int[Initial_Header->numlocalanim];
	int* numOfSecsPerAnim = new int[Initial_Header->numlocalanim];

	int numOfSecs = GetBoneHeaderNum(Stream, Initial_Header, 2, false);

	int* numOfSecHdrsPerSec = new int[numOfSecs];
	int numOfHeaders = 0;
	int secNum = 0;
	int tempNum = 0;
	int secHdrNum = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int animPos = Initial_Header->localanimindex + 100 * i;
		int basePtr = -Stream->Position();
		int animIdxPos = animPos + 56;
		Stream->seek(animIdxPos);
		int animIdx; Stream->Read(&animIdx);
		Stream->seek(animPos + 80);
		int sectionIdx; Stream->Read(&sectionIdx);
		Stream->seek(animPos + animIdx);
		//Logger::Info("_check\n");

		for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++) //It is time. -Liberty
		{
			int pos2 = Stream->Position();
			std::byte bone; Stream->Read(&bone);
			std::byte flag; Stream->Read(&flag);
			short nextOffset; Stream->Read(&nextOffset);

			numOfHeaders++;
			Stream->seek(pos2 + nextOffset);
			//Logger::Info("_check2\n");
			if (nextOffset == 0)
			{
				numOfHdrsPerAnim[i] = j;
				if (Contains(animsWithSections, i, Initial_Header->numlocalanim))
				{
					Stream->seek(sectionIdx - basePtr);
					//Logger::Info("_check3\n");
					int secStrPos = animPos + sectionIdx;
					int curSec = 1;
					int num = ((animPos + animIdx - 8) - secStrPos) / 8;
					int* secStrIndexes = new int[num];
					int maxSec = secNum + num;
					numOfSecsPerAnim[i] = num;

					for (int k = 0; k < num; k++)
					{
						Stream->seek(secStrPos + 8 * k);
						//Logger::Info("_check4\n");
						int animBlock; Stream->Read(&animBlock);
						int animOffset; Stream->Read(&animOffset);

						if (k > 0)
						{
							for (int l = secNum + 1; l < maxSec; l++)
							{
								Stream->seek((animPos + animOffset));
								//Logger::Info("_check5\n");

								for (int m = 0; m < Initial_Header->numbones * Initial_Header->numlocalanim; m++)
								{
									int secHdrPos = Stream->Position();
									std::byte secBone; Stream->Read(&secBone);
									std::byte secFlag; Stream->Read(&secFlag);
									short secNextOffset; Stream->Read(&secNextOffset);

									secHdrNum++;

									if (secNextOffset == 0)
									{
										numOfSecHdrsPerSec[secNum] = secHdrNum;
										curSec += 1;
										break;
									}
									Stream->seek(secHdrPos + (int)secNextOffset);
									//Logger::Info("_check6\n");
								}
							}
						}
						numOfSecHdrsPerAnim[i] = secHdrNum;
						secNum++;
					}
				}
				break;
			}
		}
	}

	switch (returnIdx)
	{
	case 0: return numOfHdrsPerAnim;

	case 1: return numOfSecHdrsPerAnim;

	case 2: return numOfSecsPerAnim;

	case 3: return numOfSecHdrsPerAnim; //number of boneHdrs per sec;

	default: return 0;
	}
}

float GetSecondLargest(float n1, float n2, float n3)
{
	float n = 3, num[]{n1,n2,n3}, largest, second;

	//for (int i = 0; i < n; i++) {
	//	cin >> num[i];
	//}
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
	//cout << "Enter three numbers: ";
	//cin >> n1 >> n2 >> n3;

	if (n1 > n2 && n1 < n3 || n1 < n2 && n1 > n3 )
		return n1;

	if (n2 > n1 && n2 < n3 || n2 < n1 && n2 > n3 )
		return n2;

	if (n3 > n1 && n3 < n2 || n3 < n1 && n3 > n2 )
		return n3;

	//if (n1 >= n2 && n1 >= n3)
	//	return n1;
	//
	//if (n2 >= n1 && n2 >= n3)
	//	return n2;
	//
	//if (n3 >= n1 && n3 >= n2)
		//return n3;

	return n1;
}

void ReadBoneHeader(BinaryReader* Stream, BoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int num, int dataSize, bool debug)
{
	int pos2 = Stream->Position();
	std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
	std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
	short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
	int strEndPos = Stream->Position();
	nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;

	switch ((int)flag)
	{
		case 12:
		{
			Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
			//boneHeaders_v53[num].rawStrPos = Stream->Position();
			Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);//GetSecondLargest(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);//(int)bone == 61 ? (float)0.002806369 : (float)0;//GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
			boneHeaders_v53[num].rawPos = posPtr;
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;

			break;
		}

		case 32:
		{
			QuaternionShort rawRot; Stream->Read(&rawRot);
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;// GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
			boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);//boneHeaders_v53[num].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[num].rawPos.y = (short)boneData->bonePos.y; boneHeaders_v53[num].rawPos.z = (short)boneData->bonePos.z;
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}

		case 33:
		{
			QuaternionShort rawRot; Stream->Read(&rawRot);
			Vector3Short rawPos; Stream->Read(&rawPos);
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;//GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
			boneHeaders_v53[num].rawPos = rawPos;
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}

		case 8:
		{
			Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;//GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
			boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);//boneHeaders_v53[num].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[num].rawPos.y = (short)boneData->bonePos.y; boneHeaders_v53[num].rawPos.z = (short)boneData->bonePos.z;// = boneData->bonePos;  //boneHeaders_v53[j].rawPos.x = 0; boneHeaders_v53[j].rawPos.y = 0; boneHeaders_v53[j].rawPos.z = 0; //= boneData->bonePos;  //boneHeaders_v53[j].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[j].rawPos.y = (short)boneData->bonePos.x; boneHeaders_v53[j].rawPos.z = (short)boneData->bonePos.z;
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}

		case 1:
		{
			Vector3Short rawPos; Stream->Read(&rawPos);
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0; //boneHeaders_v53[num].packedRotation.one = (short)boneData->boneRot.x; boneHeaders_v53[num].packedRotation.i = (short)boneData->boneRot.y; boneHeaders_v53[num].packedRotation.j = (short)boneData->boneRot.z; boneHeaders_v53[num].packedRotation.k = (short)0;
			boneHeaders_v53[num].rawPos = rawPos;
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}
	}
	//Logger::Info("_check\n");
	//Stream->seek(pos2 + nextOffset - (32 - GetAnimHeaderSize((int)flag)));
}

void ReadSecBoneHeader(BinaryReader* Stream, SectionBoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int num, int dataSize, bool debug)
{
	int pos2 = Stream->Position();
	std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
	std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
	short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
	int strEndPos = Stream->Position();
	nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;
	//Logger::Info("_check01\n");
	switch ((int)flag)
	{
	case 12:
	{
		
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
		//boneHeaders_v53[num].rawStrPos = Stream->Position();
		Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos = posPtr;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;

		break;
	}

	case 32:
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z); //boneHeaders_v53[num].rawPos.x = (short)0; boneHeaders_v53[num].rawPos.y = (short)0; boneHeaders_v53[num].rawPos.z = (short)0; //boneHeaders_v53[num].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[num].rawPos.y = (short)boneData->bonePos.y; boneHeaders_v53[num].rawPos.z = (short)boneData->bonePos.z;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 33:
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		Vector3Short rawPos; Stream->Read(&rawPos);
		strEndPos = Stream->Position();
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
		boneHeaders_v53[num].rawPos = rawPos;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 8:
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z); //boneHeaders_v53[num].rawPos.x = (short)0; boneHeaders_v53[num].rawPos.y = (short)0; boneHeaders_v53[num].rawPos.z = (short)0; //boneHeaders_v53[num].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[num].rawPos.y = (short)boneData->bonePos.y; boneHeaders_v53[num].rawPos.z = (short)boneData->bonePos.z;// = boneData->bonePos;  //boneHeaders_v53[j].rawPos.x = 0; boneHeaders_v53[j].rawPos.y = 0; boneHeaders_v53[j].rawPos.z = 0; //= boneData->bonePos;  //boneHeaders_v53[j].rawPos.x = (short)boneData->bonePos.x; boneHeaders_v53[j].rawPos.y = (short)boneData->bonePos.x; boneHeaders_v53[j].rawPos.z = (short)boneData->bonePos.z;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 1:
	{
		Vector3Short rawPos; Stream->Read(&rawPos);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos = rawPos;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	}

	//Logger::Info("BoneHeader: %d\n", num);
	//Logger::Info("Bone: %d\n", boneHeaders_v53[num].bone);
	//Logger::Info("Flag: %d\n", boneHeaders_v53[num].r2Flag);
	//Logger::Info("unk: %d\n", boneHeaders_v53[num].flags);
	//Logger::Info("PackedRot:, one: %d, i: %d, j: %d, k: %d\n", boneHeaders_v53[num].packedRotation.one, boneHeaders_v53[num].packedRotation.i, boneHeaders_v53[num].packedRotation.j, boneHeaders_v53[num].packedRotation.k);
	//Logger::Info("RawPos:, x: %d, y: %d, z: %d\n", boneHeaders_v53[num].rawPos.x, boneHeaders_v53[num].rawPos.y, boneHeaders_v53[num].rawPos.z);
	//Logger::Info("RawScale:, x: %d, y: %d, z: %d\n", (float)boneHeaders_v53[num].rawScale.x, (float)boneHeaders_v53[num].rawScale.y, (float)boneHeaders_v53[num].rawScale.z);
	//Logger::Info("NextOffset: %d\n", boneHeaders_v53[num].nextOffset);
	//Logger::Info("_check01\n");
	//Stream->seek(pos2 + nextOffset - (32 - GetAnimHeaderSize((int)flag)));
}

//int* GetSecBytesAddedPerSec(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, bool debug)
//{
//	int secBytesAdded = 0;
//	int secNum = 0;
//	int secIdxNum = 0;
//	int secHdrNum = 0;
//	int maxSec = 0;
//
//
//	for (int i = 0; i < Initial_Header->numlocalanim; i++)
//	{
//		int startPos = Initial_Header->localanimindex + 100 * i;
//		int animIndexPos = startPos + 56;
//		int sectionIndexPos = startPos + 80;
//
//		Stream->seek(animIndexPos);		//Go to animIndex pos;
//		int animIndex; Stream->Read(&animIndex);		//Read animIndex;
//		Stream->seek(sectionIndexPos);	//Go to sectionIndex pos;
//		int sectionIndex; Stream->Read(&sectionIndex);	//Read sectionIndex;
//		if (sectionIndex > 0)
//		{
//
//			int num = ((startPos + animIndex - 8) - (startPos + sectionIndex)) / 8;
//			maxSec += num;
//		}
//	}
//	int* secHdrBytesAddedPerSec = new int[maxSec];
//
//
//	for (int i = 0; i < Initial_Header->numlocalanim; i++)
//	{
//		int startPos = Initial_Header->localanimindex + 100 * i;
//		int animIndexPos = startPos + 56;
//		int sectionIndexPos = startPos + 80;
//
//		Stream->seek(animIndexPos);		//Go to animIndex pos;
//		int animIndex; Stream->Read(&animIndex);		//Read animIndex;
//		Stream->seek(sectionIndexPos);	//Go to sectionIndex pos;
//		int sectionIndex; Stream->Read(&sectionIndex);	//Read sectionIndex;
//		if (sectionIndex > 0)
//		{
//			int num = ( ( startPos + animIndex - 8 ) - ( startPos + sectionIndex ) ) / 8;
//
//			for (int j = 0; j < num; j++)
//			{
//				Stream->seek(startPos + sectionIndex + 8 * j);
//				int animBlock; Stream->Read(&animBlock);
//				int animOffset; Stream->Read(&animOffset);
//				Logger::Info("SecNum: %d,SecMaxNum: %d\n", j, num);
//
//				Stream->seek(startPos + animOffset);
//				secHdrBytesAddedPerSec[secNum] = secBytesAdded;
//				for (int k = 0; k < Initial_Header->numlocalanim * Initial_Header->numbones; k++)
//				{
//					int hdrStartPos = Stream->Position();
//					std::byte bone; Stream->Read(&bone);
//					std::byte flag; Stream->Read(&flag);
//					short nextOffset; Stream->Read(&nextOffset);
//					int headerSize = GetAnimHeaderSize((int)flag);
//					secBytesAdded += 32 - headerSize;
//					int dataSize = nextOffset > 0 ? nextOffset - headerSize : 0;
//
//					//Stream->seek(hdrStartPos);
//
//					secHdrNum++;
//
//
//					if (nextOffset == 0)
//					{
//						//secHdrBytesAddedPerSec[secNum] = secBytesAdded;
//						Logger::Info("secHdrNum1: %d,SecNum: %d\n", secHdrNum, secNum);
//						Logger::Info("secHdrBytesAddedToSec1: %d,SecNum: %d\n", secBytesAdded, secNum);
//						//Stream->seek(hdrStartPos + headerSize);
//						break;
//					}
//
//					Stream->seek(hdrStartPos + nextOffset);
//				}
//				secNum++;
//
//			}
//		}
//	}
//	return secHdrBytesAddedPerSec;
//}




BoneHeaderData* GetBoneHeaderData(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, bool debug)
{
	int numOfBoneHeaders = GetBoneHeaderNum(Stream, Initial_Header, 0, false);
	int numOfSecBoneHeaders = GetBoneHeaderNum(Stream, Initial_Header, 1, false);
	int numOfSecs = GetBoneHeaderNum(Stream, Initial_Header, 2, false);

	//int* numOfBoneHdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, true);
	//int* numOfSecBoneHdrsPerAnim = GetBoneHeaderNumList(Stream, Initial_Header, 1, false);
	//int* numOfSecsPerAnim = GetBoneHeaderNumList(Stream, Initial_Header, 2, false);
	//int* numOfSecBoneHdrsPerSec = GetBoneHeaderNumList(Stream, Initial_Header, 3, false);

	//int* hdrBytesAddedToAnimDesc = GetAnimBoneHeaderBytesAdded(Stream, Initial_Header, true);
	//int* //hdrBytesAnimDescAdd = GetAnimBoneHeaderBytesAdded(Stream, Initial_Header, true);
	//int* //secHdrBytesAnimDescAdd = GetAnimSectionBoneHeaderBytesAdded(Stream, Initial_Header, true);
	//int* //secHdrBytesSecAdd = GetAnimSectionBoneHeaderBytesAddedPerSec(Stream, Initial_Header, true);
	//int* //numOfBoneHdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, true);
	//int* //numOfSecBoneHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, true);
	//int* //numOfSecBoneHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(Stream, Initial_Header, true);

	int* secHdrBytesAddedToAnimDesc = new int[Initial_Header->numlocalanim];

	int* secHdrBytesAddedToSec = new int[numOfSecs];

	int* sectionOffsets = new int[numOfSecs];
	int* secStartStrPos = new int[numOfSecs];


	BoneHeaderData* boneHeaderData = new BoneHeaderData();
	BoneHeaderv53* boneHeaders = new BoneHeaderv53[numOfBoneHeaders];
	SectionBoneHeaderv53* secBoneHeaders = new SectionBoneHeaderv53[numOfSecBoneHeaders];

	int hdrNum = 0;
	int secHdrNum = 0;
	int secNum = 0;

	int secIdxNum = 0;

	int bytesAdded = 0;
	int secBytesAdded = 0;

	//sectionOffsets[0] = 0;
	bool passedFirstSec = false;


	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int startPos = Initial_Header->localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		Stream->seek(framePos);
		int frames; Stream->Read(&frames);
		Stream->seek(secFramPos);
		int secFrames; Stream->Read(&secFrames);

		Stream->seek(animIndexPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(secIndexPos);
		int sectionIndex; Stream->Read(&sectionIndex);
		//hdrBytesAddedToAnimDesc[i] = bytesAdded;
		//Logger::Info("AnimNum: %d,BytesAdded: %d\n", i, bytesAdded);
		if (sectionIndex == 0)
		{
			Stream->seek(startPos + animIndex);

			for (int j = 0; j < 10000; j++)
			{
				int hdrStartPos = Stream->Position();
				std::byte bone; Stream->Read(&bone);
				std::byte flag; Stream->Read(&flag);
				short nextOffset; Stream->Read(&nextOffset);
				int headerSize = GetAnimHeaderSize((int)flag);
				bytesAdded += 32 - headerSize;
				int dataSize = nextOffset > 0 ? nextOffset - headerSize : 0;

				Stream->seek(hdrStartPos);
				ReadBoneHeader(Stream, boneHeaders, Initial_Header, Initial_Header_Part2, hdrNum, dataSize, false);
				hdrNum++;


				if (nextOffset == 0)
				{
					break;
				}

				Stream->seek(hdrStartPos + nextOffset);
			}
		}
		//secHdrBytesAddedToAnimDesc[i] = secBytesAdded;
		//Logger::Info("AnimNum: %d,SecBytesAdded: %d\n", i, secBytesAdded);
		if (sectionIndex > 0)
		{
			int num = (frames / secFrames) + 2;
			//Logger::Info("NUM: %d\n", num);
			for (int j = 0; j < num; j++)
			{
				Stream->seek(startPos + sectionIndex + 8 * j);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				//sectionOffsets[secNum] = animOffset;
				Stream->seek(startPos + animOffset);
				//Logger::Info("strPos: %d\n", Stream->Position());
				//Logger::Info("secHdrBytesAddedToSec1: %d,SecNum: %d\n", secBytesAdded, secNum);
				//secHdrBytesAddedToSec[secNum] = secBytesAdded;
				for (int k = 0; k < 10000; k++)
				{
					int hdrStartPos = Stream->Position();
					std::byte bone; Stream->Read(&bone);
					std::byte flag; Stream->Read(&flag);
					short nextOffset; Stream->Read(&nextOffset);
					int headerSize = GetAnimHeaderSize((int)flag);
					secBytesAdded += 32 - headerSize;
					int dataSize = nextOffset > 0 ? nextOffset - headerSize : 0;

					Stream->seek(hdrStartPos);
					ReadSecBoneHeader(Stream, secBoneHeaders, Initial_Header, Initial_Header_Part2, secHdrNum, dataSize, false);
					secHdrNum++;


					if (nextOffset == 0)
					{
						//secHdrBytesAddedToSec[secNum] = secBytesAdded;
						//Logger::Info("secHdrBytesAddedToSec1: %d,SecNum: %d\n", secBytesAdded, secNum);
						Stream->seek(hdrStartPos + headerSize);
						break;
					}

					Stream->seek(hdrStartPos + nextOffset);
				}
				secNum++;
			}

		}
		//Logger::Info("AnimNum: %d,SecBytesAdded: %d\n", i, secBytesAdded);
		//Logger::Info("AnimNum: %d,BytesAdded: %d\n", i, bytesAdded);
	}
	//hdrBytesAddedToAnimDesc[0] = 0;
	//secHdrBytesAddedToAnimDesc[0] = 0;
	//secHdrBytesAddedToSec[0] = 0;

	boneHeaderData->secOffsets = sectionOffsets;
//	boneHeaderData->hdrBytesAnimDescAdd = GetAnimBoneHeaderBytesAdded(Stream, Initial_Header, true);;
//	boneHeaderData->secHdrBytesAnimDescAdd = GetAnimSectionBoneHeaderBytesAdded(Stream, Initial_Header, 0, true);
//	boneHeaderData->secHdrBytesSecAdd = GetAnimSectionBoneHeaderBytesAdded(Stream, Initial_Header, 1, true);
	boneHeaderData->numOfBoneHeaders = numOfBoneHeaders;
	boneHeaderData->numOfSecBoneHeaders = numOfSecBoneHeaders;
//	boneHeaderData->numOfBoneHdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, true);
//	boneHeaderData->numOfSecBoneHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, 0, true);
//	boneHeaderData->numOfSecBoneHdrsPerSec = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, 1, true);//secHdrBytesAddedToSec;//GetSecBytesAddedPerSec(Stream,Initial_Header, Initial_Header_Part2, false);
	boneHeaderData->boneHeaders = boneHeaders;
	boneHeaderData->secBoneheaders = secBoneHeaders;
	boneHeaderData->secBytesAdded = secBytesAdded;
	boneHeaderData->animBytesAdded = bytesAdded;
	boneHeaderData->strStartPos = secStartStrPos;
	return boneHeaderData;
}

AnimData* GetAnimData(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, bool debug)
{
	int startPos = Stream->Position();
	AnimData* animData = new AnimData();
	int animIdx = Initial_Header->localanimindex;
	animData->numOfAnims = Initial_Header->localanimindex;
	animData->secPerAnim = GetAnimSectionCount(Stream, Initial_Header, true);
	animData->bytesAddedToAnimDescs = -8 * Initial_Header->numlocalanim;
	animData->bytesAddedToSec = GetBytesAddedToBoneHdrs(Stream, Initial_Header, 2, true);
	mstudioanimdescv53_t* animDescs = new mstudioanimdescv53_t[Initial_Header->numlocalanim]();
	BoneHeaderData* boneHeaderData = GetBoneHeaderData(Stream, Initial_Header, Initial_Header_Part2, false);
	int hdrNum = 0;
	int secHdrNum = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		BoneHeaderv53* boneHeaders = new BoneHeaderv53[boneHeaderData->numOfBoneHeaders];
		SectionBoneHeaderv53* secBoneHeaders = new SectionBoneHeaderv53[boneHeaderData->numOfSecBoneHeaders];
		int animStr = animIdx + 100 * i;
		Stream->seek(animStr);
		uint32_t basePtr; Stream->Read(&basePtr);
		int sznameindex; Stream->Read(&sznameindex);
		float fps; Stream->Read(&fps);
		int flags; Stream->Read(&flags);
		int numframes; Stream->Read(&numframes);
		int nummovements; Stream->Read(&nummovements);
		int movementindex; Stream->Read(&movementindex);
		Stream->seek(Stream->Position() + 24);
		int animblock; Stream->Read(&animblock);
		int animindex; Stream->Read(&animindex);
		int numikrules; Stream->Read(&numikrules);
		int ikruleindex; Stream->Read(&ikruleindex);
		int animblockikruleindex; Stream->Read(&animblockikruleindex);
		int numlocalhierarchy; Stream->Read(&numlocalhierarchy);
		int localhierarchyindex; Stream->Read(&localhierarchyindex);
		int sectionindex; Stream->Read(&sectionindex);
		int sectionframes; Stream->Read(&sectionframes);
		short zeroframespan; Stream->Read(&zeroframespan);
		short zeroframecount; Stream->Read(&zeroframecount);
		int zeroframeindex; Stream->Read(&zeroframeindex);
		float zeroframestalltime; Stream->Read(&zeroframestalltime);
		int* bytesAddedPerSec = new int[animData->secPerAnim[i]];

		mstudioanimdescv53_t animDesc = { -8 * (Initial_Header->numlocalanim - i), -4 * (animData->secPerAnim[i]), bytesAddedPerSec, basePtr, sznameindex, (float)fps, flags, numframes, nummovements, movementindex, animblock, animindex, numikrules, ikruleindex, animblockikruleindex, numlocalhierarchy, localhierarchyindex, sectionindex, sectionframes, zeroframespan, zeroframecount, zeroframeindex, zeroframestalltime, 0, 0, 0, 0 };
		animDescs[i] = animDesc;

		if (debug)
		{
			Logger::Info("animDesc: %d, bytesAddedToDesc %d, bytesAddedToSec: %d, secNum: %d\n", i, animDesc.bytesAddedToDesc, animDesc.bytesAddedToSec, animData->secPerAnim[i]);
			Logger::Info("basePtr: %d %d\n", animDesc.baseptr);
			Logger::Info("nameIdx: %d %d\n", animDesc.sznameindex);
			Logger::Info("fps: %d %d\n", animDesc.fps);
			Logger::Info("flags: %d %d\n", animDesc.flags);
			Logger::Info("numFrames: %d %d\n", animDesc.numframes);
			Logger::Info("numMovements: %d %d\n", animDesc.nummovements);
			Logger::Info("movementIdx: %d %d\n", animDesc.movementindex);
			Logger::Info("animBlock: %d %d\n", animDesc.animblock);
			Logger::Info("animIdx: %d %d\n", animDesc.animindex);
			Logger::Info("numIkRules: %d %d\n", animDesc.numikrules);
			Logger::Info("ikRuleIdx: %d %d\n", animDesc.ikruleindex);
			Logger::Info("animBlockIkRuleIdx: %d %d\n", animDesc.animblockikruleindex);
			Logger::Info("numLocalHierarchy: %d %d\n", animDesc.numlocalhierarchy);
			Logger::Info("localHierachyIdx: %d %d\n", animDesc.localhierarchyindex);
			Logger::Info("sectionIdx: %d %d\n", animDesc.sectionindex);
			Logger::Info("sectionFrames: %d %d\n", animDesc.sectionframes);
			Logger::Info("zeroFrameSpan: %d %d\n", animDesc.zeroframespan);
			Logger::Info("zeroFrameCount: %d %d\n", animDesc.zeroframecount);
			Logger::Info("zeroFrameIdx: %d %d\n", animDesc.zeroframeindex);
			Logger::Info("zeroFrameStallTime: %d %d\n", animDesc.zeroframestalltime);
		}
		Logger::Info("_check5.1\n");


	}
	Logger::Info("_check5\n");
	animData->boneHeaderData = boneHeaderData;
	animData->animDescs = animDescs;
	animData->boneHeaders = boneHeaderData->boneHeaders;
	return animData;
}

BoneData* GetBoneData(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	BoneData* boneData = new BoneData();
	boneData->numOfBones = Initial_Header->numbones;
	boneData->bytesAddedToBones = 28 * Initial_Header->numbones;
	mstudiobonev53_t* bones = new mstudiobonev53_t[Initial_Header->numbones];

	for (int i = 0; i < Initial_Header->numbones; i++) 
	{
		Stream->seek(Initial_Header->boneindex + 216 * i);
		int sznameindex; Stream->Read(&sznameindex);					//sznameindex
		int parent; Stream->Read(&parent);								//parent

		//int bonecontrollers[6]; Stream->Read(bonecontrollers);			//bonecontroller
		Stream->seek(Stream->Position() + 24);
		Vector3 pos; Stream->Read(&pos);								//pos

		Quaternion quat; Stream->Read(&quat);
		RadianEuler rot; Stream->Read(&rot);

		Vector3 unkvector {0,0,0}; //Stream->Read(&unkvector);

		// compression scale
		Vector3 posscale; Stream->Read(&posscale);
		Vector3 rotscale; Stream->Read(&rotscale);

		Vector3 unkvector1 { 0.0004882813 ,0.0004882813 ,0.0004882813 };// Stream->Read(&unkvector1);

		matrix3x4_t poseToBone;	Stream->Read(&poseToBone);
		Quaternion qAlignment; Stream->Read(&qAlignment);

		int flags;	Stream->Read(&flags);
		int proctype; Stream->Read(&proctype);
		int procindex; Stream->Read(&procindex);
		int physicsbone; Stream->Read(&physicsbone);

		int surfacepropidx; Stream->Read(&surfacepropidx);

		int contents; Stream->Read(&contents);

		int surfacepropLookup; Stream->Read(&surfacepropLookup);

		int unkindex = 0;

		posscale = { 1,1,1 };

		mstudiobonev53_t bone{ sznameindex, parent, -1,-1,-1,-1,-1,-1, pos, quat, rot, unkvector, posscale, rotscale, unkvector1, poseToBone, qAlignment, flags, proctype, procindex, physicsbone, surfacepropidx, contents, surfacepropidx, unkindex, 0, 0, 0, 0, 0, 0, 0};
		bones[i] = bone;
	}
	boneData->bones = bones;
	return boneData;
}

AttachmentData* GetAttachmentData(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	AttachmentData* attachmentData = new AttachmentData();
	mstudioattachment_t* attachments = new mstudioattachment_t[Initial_Header->numlocalattachments];
	attachmentData->numOfAttachments = Initial_Header->numlocalattachments;

	for (int i = 0; i < Initial_Header->numlocalattachments; i++)
	{
		Stream->seek(Initial_Header->localattachmentindex + 92 * i);
		int					sznameindex; Stream->Read(&sznameindex);
		int					flags; Stream->Read(&flags);

		int					localbone; Stream->Read(&localbone); // parent bone

		matrix3x4_t			localmatrix; Stream->Read(&localmatrix); // attachment point

		int					unused[8]{0,0,0,0,0,0,0,0};

		mstudioattachment_t attachment{sznameindex, flags, localbone, localmatrix, 0,0,0,0,0,0,0,0 };
		attachments[i] = attachment;
	}
	attachmentData->attachments = attachments;
	return attachmentData;
}

HitboxData* GetHitboxData(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	HitboxData* hitboxData = new HitboxData();
	v53_HitboxSet* hitboxsets = new v53_HitboxSet[Initial_Header->numhitboxsets];
	int* numOfHitboxesPerSet = new int[Initial_Header->numhitboxsets];
	int numOfHitboxes = 0;
	int hitboxNum = 0;
	for (int i = 0; i < Initial_Header->numhitboxsets; i++)
	{
		Stream->seek(Initial_Header->hitboxsetindex + 12 * i);
		int sznameindex; Stream->Read(&sznameindex);
		int numofhitboxes; Stream->Read(&numofhitboxes);
		int hitboxindex; Stream->Read(&hitboxindex);
		numOfHitboxes += numofhitboxes;
		v53_HitboxSet hitboxset{ sznameindex, numofhitboxes, hitboxindex };
		hitboxsets[i] = hitboxset;

		if (debug)
		{
			Logger::Info("HitboxsetNum: %d %d\n", i);
			Logger::Info("sznameindex: %d %d\n", hitboxsets[i].sznameindex);
			Logger::Info("numOfHitboxes: %d %d\n", hitboxsets[i].numOfHitboxes);
			Logger::Info("hitboxIdx: %d %d\n", hitboxsets[i].hitboxIdx);
		}
		numOfHitboxesPerSet[i] = hitboxset.numOfHitboxes;
	}
	hitboxData->numOfHitboxesPerSet = numOfHitboxesPerSet;

	if (numOfHitboxes > 0)
	{
		mstudiobboxv53_t* hitboxes = new mstudiobboxv53_t[numOfHitboxes];
		Stream->seek(Initial_Header->hitboxsetindex + 12 * 0 + hitboxsets[0].hitboxIdx);

		for (int j = 0; j < numOfHitboxes; j++)
		{
			int bone; Stream->Read(&bone);
			int group; Stream->Read(&group);// intersection group

			Vector3 bbmin; Stream->Read(&bbmin); // bounding box
			Vector3 bbmax; Stream->Read(&bbmax);

			int szhitboxnameindex; Stream->Read(&szhitboxnameindex); // offset to the name of the hitbox.
			Stream->seek(Stream->Position() + 32);
			int unk = 0;
			int keyvalueindex = 0; // used for KV names in string block, should be set to hitboxname if kv unneeded.

			int unused[6]{ 0,0,0,0,0,0 };

			mstudiobboxv53_t hitbox{ bone, group, bbmin, bbmax, szhitboxnameindex, unk, keyvalueindex, 0,0,0,0,0,0 };
			hitboxes[j] = hitbox;

			if (debug)
			{
				Logger::Info("HitboxNum: %d, HitboxsetNum %d\n", j);
				Logger::Info("bone: %d %d\n", hitboxes[j].bone);
				Logger::Info("group: %d %d\n", hitboxes[j].group);
				Logger::Info("bbmin: %d %d\n", hitboxes[j].bbmin);
				Logger::Info("bbmax: %d %d\n", hitboxes[j].bbmax);
				Logger::Info("szhitboxnameindex: %d %d\n", hitboxes[j].szhitboxnameindex);
				Logger::Info("unk: %d %d\n", hitboxes[j].unk);
				Logger::Info("keyvalueindex: %d %d\n", hitboxes[j].keyvalueindex);
			}
		}
		hitboxData->hitboxes = hitboxes;
	}
	hitboxData->hitboxSets = hitboxsets;
	hitboxData->numOfHitboxes = numOfHitboxes;
	return hitboxData;
}

SequenceData* GetSequenceData(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int startPos = Stream->Position();
	SequenceData* sequenceData = new SequenceData();
	int* numOfEvntPerSeq = new int[Initial_Header->numlocalseq];
	int* evntIdxs = new int[Initial_Header->numlocalseq];
	int* numOfActModsPerSeq = new int[Initial_Header->numlocalseq];
	int* actModIdxs = new int[Initial_Header->numlocalseq];
	int numOfEvents = 0;
	int numOfActMods = 0;

	mstudioseqdesc_t_v53* sequences = new mstudioseqdesc_t_v53[Initial_Header->numlocalseq];

	int bytesAdded = 0;
	int* bytesAddedPerSeq = new int[Initial_Header->numlocalseq];

	for (int i = 0; i < Initial_Header->numlocalseq; i++)
	{
		Stream->seek(Initial_Header->localseqindex + 212 * i);
		int baseptr; Stream->Read(&baseptr);

		int	szlabelindex; Stream->Read(&szlabelindex);

		int szactivitynameindex; Stream->Read(&szactivitynameindex);

		int flags; Stream->Read(&flags);							// looping/non-looping flags

		int activity;	Stream->Read(&activity); 							// initialized at loadtime to game DLL values
		int actweight;	Stream->Read(&actweight);

		int numevents;	Stream->Read(&numevents);
		int eventindex; Stream->Read(&eventindex);

		Vector3 bbmin;	Stream->Read(&bbmin);											// per sequence bounding box
		Vector3 bbmax;	Stream->Read(&bbmax);

		int numblends;	Stream->Read(&numblends);

		// Index into array of shorts which is groupsize[0] x groupsize[1] in length
		int animindexindex; Stream->Read(&animindexindex);

		int movementindex; Stream->Read(&movementindex); // [blend] float array for blended movement
		int groupsize0;  Stream->Read(&groupsize0);
		int groupsize1;  Stream->Read(&groupsize1);

		int paramindex0; Stream->Read(&paramindex0);// X, Y, Z, XR, YR, ZR
		int paramindex1; Stream->Read(&paramindex1);

		float paramstart0; Stream->Read(&paramstart0);// local (0..1) starting value
		float paramstart1; Stream->Read(&paramstart1);

		float paramend0; Stream->Read(&paramend0);// local (0..1) ending value
		float paramend1; Stream->Read(&paramend1);

		int paramparent; Stream->Read(&paramparent);

		float fadeintime; Stream->Read(&fadeintime);// ideal cross fate in time (0.2 default)
		float fadeouttime; Stream->Read(&fadeouttime);// ideal cross fade out time (0.2 default)

		int localentrynode; Stream->Read(&localentrynode);// transition node at entry
		int localexitnode; Stream->Read(&localexitnode);// transition node at exit
		int nodeflags; Stream->Read(&nodeflags);// transition rules

		float entryphase; Stream->Read(&entryphase);// used to match entry gait
		float exitphase; Stream->Read(&exitphase);// used to match exit gait

		float lastframe; Stream->Read(&lastframe);// frame that should generation EndOfSequence

		int nextseq; Stream->Read(&nextseq);// auto advancing sequences
		int pose; Stream->Read(&pose);// index of delta animation between end and nextseq

		int numikrules; Stream->Read(&numikrules);

		int numautolayers; Stream->Read(&numautolayers);
		int autolayerindex; Stream->Read(&autolayerindex);

		int weightlistindex; Stream->Read(&weightlistindex);

		int posekeyindex; Stream->Read(&posekeyindex);

		int numiklocks; Stream->Read(&numiklocks);
		int iklockindex; Stream->Read(&iklockindex);

		// Key values
		int keyvalueindex; Stream->Read(&keyvalueindex);
		int keyvaluesize; Stream->Read(&keyvaluesize);

		int cycleposeindex; Stream->Read(&cycleposeindex);// index of pose parameter to use as cycle index

		int activitymodifierindex; Stream->Read(&activitymodifierindex);
		int numactivitymodifiers; Stream->Read(&numactivitymodifiers);

		int unused[10]; // some might be used or they might've just been reseting space like on bones.
		bytesAdded += 20;
		numOfEvents += numevents;
		numOfActMods += numactivitymodifiers;

		bytesAddedPerSeq[i] = bytesAdded;
		numOfEvntPerSeq[i] = numevents;
		evntIdxs[i] = eventindex;
		numOfActModsPerSeq[i] = numactivitymodifiers;
		actModIdxs[i] = activitymodifierindex;
		mstudioseqdesc_t_v53 sequence{ baseptr,szlabelindex,szactivitynameindex,flags,activity,actweight,numevents,eventindex, bbmin, bbmax,numblends,animindexindex,movementindex,groupsize0,groupsize1,paramindex0,paramindex1,paramstart0,paramstart1,paramend0,paramend1,paramparent,fadeintime,fadeouttime,localentrynode,localexitnode,nodeflags,entryphase,exitphase,lastframe,nextseq,pose,numikrules,numautolayers,autolayerindex,weightlistindex,posekeyindex,numiklocks,iklockindex, keyvalueindex, keyvaluesize,cycleposeindex,activitymodifierindex,numactivitymodifiers, 0,0,0,0,0,0,0,0,0,0 };
		sequences[i] = sequence;
	}
	sequenceData->numOfSeqs = Initial_Header->numlocalseq;
	sequenceData->numOfEvents = numOfEvents;
	sequenceData->numOfActMods = numOfActMods;
	sequenceData->bytesAdded = bytesAdded;
	sequenceData->bytesAddedPerSeq = bytesAddedPerSeq;
	sequenceData->numOfEvntPerSeq = numOfEvntPerSeq;
	sequenceData->evntIdxs = evntIdxs;
	sequenceData->numOfActModsPerSeq = numOfActModsPerSeq;
	sequenceData->actModIdxs = actModIdxs;
	sequenceData->sequences = sequences;

	return sequenceData;
}


