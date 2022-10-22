#include <mdls.hpp>
#include <conv.hpp>
#include <rapidjson/istreamwrapper.h>
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
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <logger.hpp>

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

short float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const uint32_t e = (b & 0x7F800000) >> 23; // exponent
	const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

bool IsWhole(double n) {
	if (n == (int)n)
		return true;
	else
		return false;
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

bool contains(std::vector<int> arry, int trgt)
{
	for (int i = 0; i < arry.size(); i++)
	{
		if (trgt == arry[i]) return true;
	}
	return false;
}

template <typename T> std::vector<T> ArrayToVector(T* arry, int size)
{
	std::vector<T> _vec;
	for (int i = 0; i < size; i++)
	{
		_vec.push_back(arry[i]);
	}
	return _vec;
}



std::string ReadMDLString(BinaryReader* Stream, int pos)
{
	int startPos = Stream->Position();
//	int size = 0;

	int defSize = 100;

	std::vector<char> vec;
	Stream->seek(pos);
	if (defSize > Stream->size - pos) defSize = Stream->size - pos; //Prevents overflow - Liberty.

	for (int i = 0; i < defSize; i++)
	{
		char value; Stream->read(&value, 1);
		if (&value == ".")
		{
			break;
		}
		else
		{
			vec.push_back(value);
//			size++;
		}
	}
	std::string string(vec.begin(), vec.end());
	Stream->seek(startPos);
	return string;

}

template <typename T> int GetJiggleBoneCount(T mdl, bool debug)
{
	const char* type = typeid(mdl).name(); if (debug) Logger::Info("Type: %s\n", (std::string)type);
	int jiggleBoneCount = 0;

	if (type == typeid(MDL::v53Mdl).name() || type == typeid(MDL::v49Mdl).name() || type == typeid(MDL::v49Mdl).name())
	{
		int version = mdl.mdlhdr.version;
		switch (version)
		{
		case 53: for (int i = 0; i < mdl.mdlhdr.numbones; i++) { if (mdl.bones[i].proctype == 5) jiggleBoneCount++; } if (debug) Logger::Info("JiggleBone Count: %d\n", jiggleBoneCount); return jiggleBoneCount;

		case 52: for (int i = 0; i < mdl.mdlhdr.numbones; i++) { if (mdl.bones[i].proctype == 5) jiggleBoneCount++; } if (debug) Logger::Info("JiggleBone Count: %d\n", jiggleBoneCount); return jiggleBoneCount;

		case 49: for (int i = 0; i < mdl.mdlhdr.numbones; i++) { if (mdl.bones[i].proctype == 5) jiggleBoneCount++; } if (debug) Logger::Info("JiggleBone Count: %d\n", jiggleBoneCount); return jiggleBoneCount;

		default: return jiggleBoneCount;
		}
	}
	Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type);
	return 0;
}

template <typename T> int GetHitboxCount(T mdl, bool debug)
{
	const char* type = typeid(mdl).name();
	int hitboxCount = 0;

	if (type == typeid(MDL::v53Mdl).name() || type == typeid(MDL::v49Mdl).name() || type == typeid(MDL::v49Mdl).name())
	{
		int version = mdl.mdlhdr.version;
		switch (version)
		{
		case 53: for (int i = 0; i < mdl.mdlhdr.numhitboxsets; i++) { if (mdl.hitboxsets[i].numhitboxes > 0) hitboxCount += mdl.hitboxsets[i].numhitboxes; } if (debug) Logger::Info("Hitbox Count: %d\n", hitboxCount); return hitboxCount;

		case 52: for (int i = 0; i < mdl.mdlhdr.numhitboxsets; i++) { if (mdl.hitboxsets[i].numhitboxes > 0) hitboxCount += mdl.hitboxsets[i].numhitboxes; } if (debug) Logger::Info("Hitbox Count: %d\n", hitboxCount); return hitboxCount;

		case 49: for (int i = 0; i < mdl.mdlhdr.numhitboxsets; i++) { if (mdl.hitboxsets[i].numhitboxes > 0) hitboxCount += mdl.hitboxsets[i].numhitboxes; } if (debug) Logger::Info("Hitbox Count: %d\n", hitboxCount); return hitboxCount;

		default: return hitboxCount;
		}
	}
	Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type);
	return 0;
}

template <typename T> int GetModelCount(T mdl, bool debug)
{
	const char* type = typeid(mdl).name(); if (debug) Logger::Info("Type: %s\n", type);
	int modelCount = 0;

	if (type == typeid(MDL::v53Mdl).name() || type == typeid(MDL::v49Mdl).name() || type == typeid(MDL::v49Mdl).name())
	{
		int version = mdl.mdlhdr.version;
		switch (version)
		{
		case 53: for (int i = 0; i < mdl.mdlhdr.numbodyparts; i++) { if (mdl.bodyparts[i].nummodels > 0) modelCount += mdl.bodyparts[i].nummodels; } if (debug) Logger::Info("Model Count: %d\n", modelCount); return modelCount;

		case 52: for (int i = 0; i < mdl.mdlhdr.numbodyparts; i++) { if (mdl.bodyparts[i].nummodels > 0) modelCount += mdl.bodyparts[i].nummodels; } if (debug) Logger::Info("Model Count: %d\n", modelCount); return modelCount;

		case 49: for (int i = 0; i < mdl.mdlhdr.numbodyparts; i++) { if (mdl.bodyparts[i].nummodels > 0) modelCount += mdl.bodyparts[i].nummodels; } if (debug) Logger::Info("Model Count: %d\n", modelCount); return modelCount;

		default: return modelCount;
		}
	}
	Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type);
	return 0;
}

void AddDistToAnimData(mstudioanimdata_t_v49 vec, int extra)
{
	for (int i = 0; i < extra; i++)
	{
		vec.arry.push_back((std::byte)0);
	}
}

MDL::v53Mdl MDL::v53Mdl::_v53Mdl(BinaryReader* Stream, bool debug)
{
	studiohdr_t_v53										mdlhdr; Stream->Read(&mdlhdr);
	std::vector<mstudiobone_t_v53>						bones;
	std::vector<mstudiojigglebone_t_v49>				jigglebones;
	std::vector<mstudioboneflexdriver_t_v49>			boneflexdrivers;
	std::vector<mstudioattachment_t_v49>				attachments;
	std::vector<mstudiohitboxset_t_v49>					hitboxsets;
	std::vector<mstudiobbox_t_v53>						hitboxes;
	mstudiobonenametable_t_v49							bonenametable;
	std::vector<mstudioanimdesc_t_v53>					animdescs;
	std::vector<mstudioanim_t_v53>						anims;
	std::vector<sectionindexes_t_v53>					sectionindexes;
	std::vector<mstudioanim_t_v53>						sections;
	std::vector<mstudioikrule_t_v53>					ikrules;
	std::vector<mstudiocompressedikerror_t_v49>			compressedikerrors;
	std::vector<mstudiocompressedikerror_t_v52>			v52compressedikerrors;
	std::vector<mstudioikerror_t_v49>					ikerrors;
	std::vector<mstudioikrulezeroframe_t_v49>			ikrulezeroframe;
	std::vector<mstudioseqdesc_t_v53>					seqdescs;
	std::vector<blendgroup_t_v49>						blends;
	std::vector<posekey_t_v49>							posekeys;
	std::vector<mstudioevent_t_v49>						events;
	std::vector<mstudioautolayer_t_v49>					autolayers;
	std::vector<mstudioactivitymodifier_t_v53>			activitymodifiers;
	std::vector<seqweightlist_t_v49>					seqweightlist;
	std::vector<mstudionodename_t_v49>					nodenames;
	std::vector<mstudionodedata_v49>					nodes;
	std::vector<mstudiobodyparts_t_v49>					bodyparts;
	std::vector<mstudiomodel_t_v49>						models;
	std::vector<mstudiomesh_t_v49>						meshes;
	std::vector<mstudioikchain_t_v53>					ikchains;
	std::vector<mstudioiklink_t_v49>					iklinks;
	std::vector<mstudioposeparamdesc_t_v49>				poseparamdescs;
	std::vector<mstudiomodelgroup_t_v49>				includedmodels;
	std::vector<mstudiorruiheader_t>					ruiHdrs;
	std::vector<mstudioruimesh_t>						ruiMeshes;
	std::vector<mstudiotexture_t_v53>					textures;
	std::vector<mstudiotexturedir_t_v49>				cdtextures;
	std::vector<mstudioskingroup_t_v49>					skingroups;
	mstudiokeyvalues_t_v49								keyvalues;
	std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms;
	mstudiolinearbone_t_v49								linearbone;
	mstudiolinearbonedata_t_v53							linearbonedata;

	int numjigglebones = 0;

	int numhitboxes = 0;

	int nummodels = 0;

	int nummeshes = 0;

	int numiklinks = 0;

	int numiklocks = 0;

	int numikrules = 0;

	int numTextures = mdlhdr.numtextures;

	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 244 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v53 bone; Stream->Read(&bone);
			bones.push_back(bone);
			if (bone.proctype == 5) numjigglebones++;

			Logger::Info("Bone Read: %s\n", ReadMDLString(Stream, bonePos + bone.sznameindex).c_str());
			Logger::Info("Bone Read: %d\n", i);
		}
	}
	if (numjigglebones > 0)
	{
		int jiggleBonePos = mdlhdr.boneindex + 244 * mdlhdr.numbones;

		Stream->seek(jiggleBonePos);

		for (int i = 0; i < numjigglebones; i++)
		{
			mstudiojigglebone_t_v49 jigglebone; Stream->Read(&jigglebone);
			jigglebones.push_back(jigglebone);
			Logger::Info("JiggleBone Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}

	if (mdlhdr.numlocalattachments > 0)
	{
		Stream->seek(mdlhdr.localattachmentindex);
		for (int i = 0; i < mdlhdr.numlocalattachments; i++)
		{
			int attachmentPos = mdlhdr.localattachmentindex + 92 * i;
			Stream->seek(attachmentPos);

			mstudioattachment_t_v49 attachment; Stream->Read(&attachment);
			attachments.push_back(attachment);

			Logger::Info("Attachment Read: %s\n", ReadMDLString(Stream, attachmentPos + attachment.sznameindex).c_str());
			Logger::Info("Attachment Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
	if (mdlhdr.numhitboxsets > 0)
	{
		Stream->seek(mdlhdr.hitboxsetindex);
		for (int i = 0; i < mdlhdr.numhitboxsets; i++)
		{
			int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * i;
			Stream->seek(hitboxsetPos);

			mstudiohitboxset_t_v49 hitboxset; Stream->Read(&hitboxset);
			Logger::Info("HitboxSet Read: %s\n", ReadMDLString(Stream, hitboxsetPos + hitboxset.sznameindex).c_str());
			if (hitboxset.numhitboxes > 0) numhitboxes += hitboxset.numhitboxes;
			hitboxsets.push_back(hitboxset);
			Logger::Info("HitboxSet Read: %d\n", i);
		}
		if (numhitboxes > 0)
		{
			for (int i = 0; i < numhitboxes; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * mdlhdr.numhitboxsets;
				int hitboxPos = hitboxsetPos + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v53 hitbox; Stream->Read(&hitbox);
				Logger::Info("Hitbox Read: %s\n", ReadMDLString(Stream, hitboxPos + hitbox.szhitboxnameindex).c_str());
				Logger::Info("Hitbox Read: %s\n", ReadMDLString(Stream, hitboxPos + hitbox.keyvalueindex).c_str());
				hitboxes.push_back(hitbox);
				Logger::Info("Hitbox Read: %d\n", i);
			}
		}
		//Logger::Info("strPos: %d\n", Stream->Position());

	}
	if (mdlhdr.numbones > 0)
	{
		Stream->Read(&bonenametable, mdlhdr.numbones);
		Logger::Info("BoneTable Read: %d\n", mdlhdr.numbones);
	}
	if (mdlhdr.numlocalanim > 0)
	{
		Stream->seek(mdlhdr.localanimindex);
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int animDescPos = mdlhdr.localanimindex + 92 * i;
			Stream->seek(animDescPos);

			mstudioanimdesc_t_v53 animdesc; Stream->Read(&animdesc);
			animdescs.push_back(animdesc);

			Logger::Info("AnimDesc Read: %s\n", ReadMDLString(Stream, animDescPos + animdesc.sznameindex).c_str());
			Logger::Info("AnimDesc Read: %d\n", i);
			//Logger::Info("strPos: %d\n", Stream->Position());
		}


		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int boneHeaderNum = 0;
			int animDescPos = mdlhdr.localanimindex + 92 * i;
			int animPos = animDescPos + animdescs[i].animindex;

			if (animdescs[i].sectionindex == 0)
			{

				Stream->seek(animPos);
				//Logger::Info("strPos: %d\n", Stream->Position());
				for (int j = 0; j < mdlhdr.numlocalanim * mdlhdr.numbones; j++)
				{
					int dist = 0;
					int nextAnim = i + 1;
					int hdrStartPos = Stream->Position();
					//Logger::Info("hdrStartPos: %d\n", hdrStartPos);
					mstudioanim_t_v53 anim; Stream->Read(&anim);
					anims.push_back(anim);
					mstudioanimdata_t_v49 animData;

					//Logger::Info("HdrNum: %d\n", j);
					//Logger::Info("Bone: %d\n", anim.bone);
					//Logger::Info("Flags Read: %d\n", anim.flags);
					//Logger::Info("Unk Read: %d\n", anim.unk);
					//Logger::Info("RawRot Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot.x, anim.rawrot.y, anim.rawrot.z, anim.rawrot.wneg);
					//Logger::Info("RawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
					//Logger::Info("RawScale Read: X: %d Y: %d Z: %d\n", anim.rawscale.x, anim.rawscale.y, anim.rawscale.z);
					//Logger::Info("RotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
					//Logger::Info("PosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);
					//Logger::Info("ScalePtr Read: X: %d Y: %d Z: %d\n", anim.animscale.offset.x, anim.animscale.offset.y, anim.animscale.offset.z);
					//Logger::Info("NextOffset Read: %d\n", anim.nextoffset);

					int headerSize = 32;
					int hdrEndPos = Stream->Position();
					//Logger::Info("hdrEndPos: %d\n", hdrEndPos);
					boneHeaderNum++;
					if (anim.nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - hdrEndPos;
						if (nextAnim > mdlhdr.numlocalanim) dist = mdlhdr.localseqindex - hdrEndPos;

						//Logger::Info("SeqIndx: %d\n", mdlhdr.localseqindex);
						//Logger::Info("AnimDataSizeEnd: %d\n", dist);
						Stream->Read(&animData, dist);
						Logger::Info("Anim Read: %d\n", i);
						break;
					}
					dist = anim.nextoffset - headerSize;
					//Logger::Info("AnimDataSize: %d\n", dist);
					Stream->Read(&animData, dist);

				}

			}

			if (animdescs[i].sectionindex > 0)
			{
				int secBoneHeaderNum = 0;
				int animDescPos = mdlhdr.localanimindex + 92 * i;
				int animPos = animDescPos + animdescs[i].animindex;
				int secPos = animDescPos + animdescs[i].sectionindex;
				int frames = animdescs[i].numframes;
				int secFrames = animdescs[i].sectionframes;
				std::vector<sectionindexes_t_v53> idxs;

				int secNumber = (frames / secFrames) + 2;

				Stream->seek(secPos);

				for (int j = 0; j < secNumber; j++)
				{

					sectionindexes_t_v53 sectionIdx; Stream->Read(&sectionIdx);
					//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
					idxs.push_back(sectionIdx);
					sectionindexes.push_back(sectionIdx);
				}

				for (int j = 0; j < secNumber; j++)
				{
					sectionindexes_t_v53 sectionIdx = idxs[j];
					int nextSection = j + 1;
					//Logger::Info("secIdxPos: %d\n", animDescPos + sectionIdx.sectionoffset);
					Stream->seek(animDescPos + sectionIdx.sectionoffsets);


					for (int k = 0; k < mdlhdr.numlocalanim * mdlhdr.numbones; k++)
					{
						int dist = 0;
						int nextAnim = i + 1;
						int hdrStartPos = Stream->Position();
						//Logger::Info("secHdrStartPos: %d\n", hdrStartPos);
						mstudioanim_t_v53 anim; Stream->Read(&anim);
						sections.push_back(anim);
						mstudioanimdata_t_v49 animData;

						//Logger::Info("SecHdrNum: %d\n", j);
						//Logger::Info("SecAnimScale: %d\n", anim.animscale);
						//Logger::Info("SecBone: %d\n", anim.bone);
						//Logger::Info("SecFlags Read: %d\n", anim.flags);
						//Logger::Info("Unk Read: %d\n", anim.unk);
						//Logger::Info("SecRawRot Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot.x, anim.rawrot.y, anim.rawrot.z, anim.rawrot.wneg);
						//Logger::Info("SecRawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
						//Logger::Info("SecRawScale Read: X: %d Y: %d Z: %d\n", anim.rawscale.x, anim.rawscale.y, anim.rawscale.z);
						//Logger::Info("SecRotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
						//Logger::Info("SecPosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);
						//Logger::Info("SecScalePtr Read: X: %d Y: %d Z: %d\n", anim.animscale.offset.x, anim.animscale.offset.y, anim.animscale.offset.z);
						//Logger::Info("SecNextOffset Read: %d\n", anim.nextoffset);

						int headerSize = 32;
						int hdrEndPos = Stream->Position();
						//Logger::Info("secHdrEndPos: %d\n", hdrEndPos);
						secBoneHeaderNum++;
						if (anim.nextoffset == 0)
						{
							if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - hdrEndPos;  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - hdrEndPos;
							if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + idxs[nextSection].sectionoffsets - hdrEndPos;
							if (nextSection > secNumber && nextAnim > mdlhdr.numlocalanim && animdescs[i].numikrules == 0) dist = mdlhdr.localseqindex - hdrEndPos;
							Stream->Read(&animData, dist);
							//Logger::Info("AnimDataSizeEnd: %d\n", dist);
							Logger::Info("Section Read: %d\n", i);
							break;
						}
						dist = anim.nextoffset - headerSize;
						Stream->Read(&animData, dist);
					}

				}

			}

			if (animdescs[i].numikrules > 0)
			{
				int numOfCompressedIkErrors = 0;
				int numOfIkErrors = 0;
				std::vector<int> errorIdxs;
				int nextAnim = i + 1;

				for (int j = 0; j < animdescs[i].numikrules; j++)
				{
					Stream->seek(animDescPos + animdescs[i].ikruleindex + 140 * j);
					int pos = Stream->Position();
					mstudioikrule_t_v53 v53IkRule; Stream->Read(&v53IkRule);
					ikrules.push_back(v53IkRule);
					if (v53IkRule.compressedikerrorindex > 0) errorIdxs.push_back(pos + v53IkRule.compressedikerrorindex);
					if (v53IkRule.compressedikerrorindex > 0) numOfCompressedIkErrors++;
					if (v53IkRule.ikerrorindex > 0) numOfIkErrors++;

					Logger::Info("IkRule Read: %d\n", j);
					numikrules++;
				}

				if (numOfCompressedIkErrors > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 92 * i;
					for (int j = 0; j < numOfCompressedIkErrors; j++)
					{
						Stream->seek(errorIdxs[j]);
						int dist = 0;
						int next = j + 1;
						int pos = Stream->Position();
						mstudiocompressedikerror_t_v49 v49CompressedIkError; Stream->Read(&v49CompressedIkError);
						compressedikerrors.push_back(v49CompressedIkError);
						mstudioanimdata_t_v49 animData;
						int endPos = Stream->Position();
						if (j + 1 > numOfCompressedIkErrors - 1)
						{
							if (animdescs[i].compressedikerrorindex > 0) dist = animDescPos + animdescs[i].compressedikerrorindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - endPos;
							if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
							Stream->Read(&animData, dist);
							Logger::Info("CompressedIkError Read: %d\n", j);
							break;
						}
						Logger::Info("CompressedIkError Read: %d\n", j);
						dist = errorIdxs[next] - endPos;
						Stream->Read(&animData, dist);
					}
				}

				if (animdescs[i].compressedikerrorindex > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 92 * i;
					Stream->seek(animStartPos + animdescs[i].compressedikerrorindex);
					int dist = 0;
					int pos = Stream->Position();
					mstudiocompressedikerror_t_v52 v52CompressedIkError; Stream->Read(&v52CompressedIkError);
					v52compressedikerrors.push_back(v52CompressedIkError);
					mstudioanimdata_t_v49 animData;
					int endPos = Stream->Position();
					if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - endPos;
					if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - endPos;
					if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
					Stream->Read(&animData, dist);
					Logger::Info("v52CompressedIkError Read: %d\n");
				}

			}

		}
	}

	if (mdlhdr.numlocalseq > 0)
	{
		Stream->seek(mdlhdr.localseqindex);
		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 232 * i;
			Stream->seek(seqPos);
			mstudioseqdesc_t_v53 seqDesc; Stream->Read(&seqDesc);
			seqdescs.push_back(seqDesc);
			Logger::Info("SeqLabel Read: %s\n", ReadMDLString(Stream, seqPos + seqDesc.szlabelindex).c_str());
			Logger::Info("SeqActName Read: %s\n", ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex).c_str());
			Logger::Info("Seq Read: %d\n", i);
		}

		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 232 * i;
			int numBlends = seqdescs[i].groupsize[0] * seqdescs[i].groupsize[1];
			int posekeySize = seqdescs[i].groupsize[0] + seqdescs[i].groupsize[1];
			int numAutolayers = seqdescs[i].numautolayers;
			int numActMods = seqdescs[i].numactivitymodifiers;
			int numEvents = seqdescs[i].numevents;
			if (seqdescs[i].weightlistindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].weightlistindex);
				seqweightlist_t_v49 weightlist; Stream->Read(&weightlist, mdlhdr.numbones);
				seqweightlist.push_back(weightlist);
				Logger::Info("Weightlist Read: %d\n", i);
			}

			if (seqdescs[i].animindexindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].animindexindex);
				blendgroup_t_v49 blendgroup; Stream->Read(&blendgroup, seqdescs[i].numblends);
				blends.push_back(blendgroup);
				Logger::Info("BlendGroup Read: %d\n", i);
			}

			if (seqdescs[i].posekeyindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].posekeyindex);
				posekey_t_v49 posekey; Stream->Read(&posekey, posekeySize);
				posekeys.push_back(posekey);
				Logger::Info("Posekey Read: %d\n", i);
			}

			if (numAutolayers > 0)
			{
				Stream->seek(seqPos + seqdescs[i].autolayerindex);
				for (int j = 0; j < numAutolayers; j++)
				{
					mstudioautolayer_t_v49 autolayer; Stream->Read(&autolayer);
					autolayers.push_back(autolayer);
					Logger::Info("Autolayer Read: %d\n", j);
				}
			}

			if (numEvents > 0)
			{
				Stream->seek(seqPos + seqdescs[i].eventindex);
				for (int j = 0; j < seqdescs[i].numevents; j++)
				{
					mstudioevent_t_v49 _event; Stream->Read(&_event);
					events.push_back(_event);

					Logger::Info("Event Read: %s\n", ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex).c_str());
					Logger::Info("Event Read: %d\n", j);
				}
			}

			if (numActMods > 0)
			{
				Stream->seek(seqPos + seqdescs[i].activitymodifierindex);
				for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
				{
					mstudioactivitymodifier_t_v53 actmod; Stream->Read(&actmod);
					activitymodifiers.push_back(actmod);
					Logger::Info("ActMod Read: %s\n", ReadMDLString(Stream, seqPos + seqdescs[i].activitymodifierindex + 8 * j + actmod.sznameindex).c_str());
					Logger::Info("ActMod Read: %d\n", j);
				}
			}
		}
	}
	if (mdlhdr.numlocalnodes > 0)
	{
		Stream->seek(mdlhdr.localnodenameindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodename_t_v49 nodeName; Stream->Read(&nodeName);
			nodenames.push_back(nodeName);
			Logger::Info("NodeName Read: %s\n", ReadMDLString(Stream, nodenames[i].sznameindex).c_str());
			Logger::Info("NodeName Read: %d\n", i);
		}
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodedata_v49 node; Stream->Read(&node, mdlhdr.numlocalnodes * mdlhdr.numlocalnodes);
			nodes.push_back(node);
			Logger::Info("Node Read: %d\n", i);
		}
	}
	if (mdlhdr.numbodyparts > 0)
	{
		Stream->seek(mdlhdr.bodypartindex);
		for (int i = 0; i < mdlhdr.numbodyparts; i++)
		{
			mstudiobodyparts_t_v49 bodyPart; Stream->Read(&bodyPart);
			if (bodyPart.nummodels > 0) nummodels += bodyPart.nummodels;
			bodyparts.push_back(bodyPart);
			Logger::Info("Bodypart Read: %s\n", ReadMDLString(Stream, mdlhdr.bodypartindex + 16 * i + bodyparts[i].sznameindex).c_str());
			Logger::Info("Bodypart Read: %d\n", i);
		}
	}

	if (nummodels > 0)
	{
		Stream->seek(mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts);
		for (int i = 0; i < nummodels; i++)
		{
			mstudiomodel_t_v49 model; Stream->Read(&model);
			if (model.nummeshes > 0) nummeshes += model.nummeshes;
			models.push_back(model);

			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts + 148 * i;

			Logger::Info("Model Read: %s\n", ReadMDLString(Stream, modelPos).c_str());
			Logger::Info("Model Read: %d\n", i);
		}

	}

	if (mdlhdr.numikchains > 0)
	{
		Stream->seek(mdlhdr.ikchainindex);
		for (int i = 0; i < mdlhdr.numikchains; i++)
		{
			mstudioikchain_t_v53 ikchain; Stream->Read(&ikchain);
			if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;
			ikchains.push_back(ikchain);

			int ikChainPos = mdlhdr.ikchainindex + 32 * i;

			Logger::Info("IkChain Read: %s\n", ReadMDLString(Stream, ikChainPos + ikchains[i].sznameindex).c_str());
			Logger::Info("IkChain Read: %d\n", i);
		}

		for (int i = 0; i < numiklinks; i++)
		{
			int ikLinkPos = mdlhdr.ikchainindex + 16 * mdlhdr.numikchains;
			Stream->seek(ikLinkPos);
			mstudioiklink_t_v49 iklink; Stream->Read(&iklink);
			iklinks.push_back(iklink);
			Logger::Info("IkLink Read: %d\n", i);
		}
	}
	if (mdlhdr.localposeparamindex > 0)
	{
		Stream->seek(mdlhdr.localposeparamindex);
		for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
		{
			mstudioposeparamdesc_t_v49 poseparamdesc; Stream->Read(&poseparamdesc);
			poseparamdescs.push_back(poseparamdesc);

			int poseParamPos = mdlhdr.localposeparamindex + 20 * i;

			Logger::Info("PoseParamDesc Read: %s\n", ReadMDLString(Stream, poseParamPos + poseparamdescs[i].sznameindex).c_str());
			Logger::Info("PoseParamDesc Read: %d\n", i);
		}
	}

	if (nummeshes > 0)
	{
		for (int i = 0; i < nummeshes; i++)
		{
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts;
			int meshPos = modelPos + 148 * nummodels + 20 * mdlhdr.numlocalposeparameters + 32 * mdlhdr.numikchains + 28 * numiklinks + 116 * i;
			Stream->seek(meshPos);
			mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
			meshes.push_back(mesh);

			int texturePos = mdlhdr.textureindex + (44 * (int)mesh.material);
			Stream->seek(texturePos);
			mstudiotexture_t_v53 texture; Stream->Read(&texture);

			Logger::Info("MeshMat Read: %s\n", ReadMDLString(Stream, mdlhdr.textureindex + 44 * mesh.material + texture.sznameindex).c_str());
			Logger::Info("Mesh Read: %d\n", mesh.material);
		}
	}
	if (mdlhdr.numincludemodels > 0)
	{
		Stream->seek(mdlhdr.includemodelindex);
		for (int i = 0; i < mdlhdr.numincludemodels; i++)
		{
			mstudiomodelgroup_t_v49 includemodel; Stream->Read(&includemodel);
			includedmodels.push_back(includemodel);
			int includedModelPos = mdlhdr.includemodelindex + 8 * i;

			Logger::Info("IncludedModel Read: %s\n", ReadMDLString(Stream, includedModelPos + includedmodels[i].sznameindex).c_str());
			Logger::Info("IncludedModel Read: %d\n", i);
		}
	}

	Logger::Info("Textureidx: %d\n", mdlhdr.textureindex);
	if (mdlhdr.numtextures > 0)
	{
		Stream->seek(mdlhdr.textureindex);
		for (int i = 0; i < numTextures; i++)
		{
			mstudiotexture_t_v53 texture; Stream->Read(&texture);
			textures.push_back(texture);

			int texturePos = mdlhdr.textureindex + 44 * i;

			Logger::Info("Texture Read: %s\n", ReadMDLString(Stream, texturePos + textures[i].sznameindex).c_str());
			Logger::Info("Texture Read: %d\n", i);
		}
	}
	if (mdlhdr.numcdtextures > 0)
	{
		Stream->seek(mdlhdr.cdtextureindex);
		for (int i = 0; i < mdlhdr.numcdtextures; i++)
		{
			mstudiotexturedir_t_v49 cdtexture; Stream->Read(&cdtexture);
			cdtextures.push_back(cdtexture);

			Logger::Info("CdTexture Read: %s\n", ReadMDLString(Stream, cdtextures[i].sznameindex).c_str());
			Logger::Info("CdTexture Read: %d\n", i);
		}
	}
	if (mdlhdr.numskinfamilies > 0)
	{
		if (mdlhdr.numskinref > 0)
		{
			Stream->seek(mdlhdr.skinindex);
			for (int i = 0; i < mdlhdr.numskinfamilies; i++)
			{
				mstudioskingroup_t_v49 skingroup; Stream->Read(&skingroup, mdlhdr.numskinref);
				skingroups.push_back(skingroup);
				Logger::Info("SkinGroup Read: %d\n", i);
			}
		}

	}

	if (mdlhdr.keyvaluesize > 0)
	{
		Stream->seek(mdlhdr.keyvalueindex);
		Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
		Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
	}
	Logger::Info("Test\n");
	if (mdlhdr.numsrcbonetransform > 0)
	{
		Stream->seek(mdlhdr.srcbonetransformindex);
		for (int i = 0; i < mdlhdr.numsrcbonetransform; i++)
		{
			mstudiosrcbonetransform_t_v49 srcbonetransform; Stream->Read(&srcbonetransform);
			srcbonetransforms.push_back(srcbonetransform);

			int srcBonePos = mdlhdr.srcbonetransformindex + sizeof(mstudiosrcbonetransform_t_v49) * i;

			Logger::Info("SrcBoneTransform Read: %s\n", ReadMDLString(Stream, srcBonePos + srcbonetransforms[i].sznameindex).c_str());
			Logger::Info("SrcBoneTransform Read: %d\n", i);
		}
	}

	if (mdlhdr.numbones > 0)
	{

		Stream->Read(&linearbone);
		Stream->Read(&linearbonedata, mdlhdr.numbones);
		Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
	}

	if (mdlhdr.numruimeshes > 0)
	{
		Stream->seek(mdlhdr.ruimeshindex);
		for (int i = 0; i < mdlhdr.numruimeshes; i++)
		{
			mstudiorruiheader_t ruiHdr; Stream->Read(&ruiHdr);
			ruiHdrs.push_back(ruiHdr);
			Logger::Info("RuiHdr Read: %d\n", i);
		}

		Stream->seek(mdlhdr.ruimeshindex + ruiHdrs[0].ruimeshindex);
		for (int j = 0; j < ruiHdrs.size(); j++)
		{
			int ruiPos = Stream->Position();
			mstudioruimesh_t ruiMesh; Stream->Read(&ruiMesh);
			ruiMeshes.push_back(ruiMesh);
			//Logger::Info("RuiMesh Read: %d  %d\n", ruiMesh.vertexmap[0].vertstartid, ruiMesh.vertexmap[0].vertendid);
			//RUIMeshToSmd(ruiMesh);
			Logger::Info("RuiMesh Read: %s\n", ReadMDLString(Stream, ruiPos + 32).c_str());
			Logger::Info("RuiMesh Read: %d\n", j);
		}
	}

	MDL::v53Mdl _v53mdl{ mdlhdr,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,anims, sectionindexes, sections, ikrules,compressedikerrors,v52compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,textures,cdtextures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata };

	return _v53mdl;
}

MDL::v49Mdl MDL::v49Mdl::_v49Mdl(BinaryReader* Stream, bool debug)
{
	studiohdr_t_v49	 mdlhdr; Stream->Read(&mdlhdr);
	studiohdr2_t_v49 mdlsubhdr; Stream->Read(&mdlsubhdr);

	std::vector<mstudiobone_t_v49>						bones;
	std::vector<mstudiojigglebone_t_v49>				jigglebones;
	std::vector<mstudioboneflexdriver_t_v49>			boneflexdrivers;
	std::vector<mstudioattachment_t_v49>				attachments;
	std::vector<mstudiohitboxset_t_v49>					hitboxsets;
	std::vector<mstudiobbox_t_v49>						hitboxes;
	mstudiobonenametable_t_v49							bonenametable;
	std::vector<mstudioanimdesc_t_v49>					animdescs;
	std::vector<mstudioanim_t_v49>						anims;
	std::vector<sectionindexesindex_t_v49>				sectionindexes;
	std::vector<mstudioanim_t_v49>						sections;
	std::vector<mstudioikrule_t_v49>					ikrules;
	std::vector<mstudiocompressedikerror_t_v49>			compressedikerrors;
	std::vector<mstudioikerror_t_v49>					ikerrors;
	std::vector<mstudioikrulezeroframe_t_v49>			ikrulezeroframe;
	std::vector<mstudioseqdescv49_t>					seqdescs;
	std::vector<blendgroup_t_v49>						blends;
	std::vector<posekey_t_v49>							posekeys;
	std::vector<mstudioevent_t_v49>						events;
	std::vector<mstudioautolayer_t_v49>					autolayers;
	std::vector<mstudioactivitymodifier_t_v49>			activitymodifiers;
	std::vector<seqweightlist_t_v49>					seqweightlist;
	std::vector<mstudionodename_t_v49>					nodenames;
	std::vector<mstudionodedata_v49>					nodes;
	std::vector<mstudiobodyparts_t_v49>					bodyparts;
	std::vector<mstudiomodel_t_v49>						models;
	std::vector<mstudiomesh_t_v49>						meshes;
	std::vector<mstudioikchain_t_v49>					ikchains;
	std::vector<mstudioiklink_t_v49>					iklinks;
	std::vector<mstudioposeparamdesc_t_v49>				poseparamdescs;
	std::vector<mstudiomodelgroup_t_v49>				includedmodels;
	std::vector<mstudiotexturedir_t_v49>				cdtextures;
	std::vector<mstudiotexture_t_v49>					textures;
	std::vector<mstudioskingroup_t_v49>					skingroups;
	mstudiokeyvalues_t_v49								keyvalues;
	std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms;
	mstudiolinearbone_t_v49								linearbone;
	mstudiolinearbonedata_t_v49							linearbonedata;

	int numjigglebones = 0;

	int numhitboxes = 0;

	int nummodels = 0;

	int nummeshes = 0;

	int numiklinks = 0;

	int numiklocks = 0;

	int numikrules = 0;

	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 216 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v49 bone; Stream->Read(&bone);
			bones.push_back(bone);
			if (bone.proctype == 5) numjigglebones++;

			Logger::Info("Bone Read: %s\n", ReadMDLString(Stream, bonePos + bone.sznameindex).c_str());
			Logger::Info("Bone Read: %d\n", i);
		}
	}
	if (numjigglebones > 0)
	{
		int jiggleBonePos = mdlhdr.boneindex + 216 * mdlhdr.numbones;

		Stream->seek(jiggleBonePos);

		for (int i = 0; i < numjigglebones; i++)
		{
			mstudiojigglebone_t_v49 jigglebone; Stream->Read(&jigglebone);
			jigglebones.push_back(jigglebone);
			Logger::Info("JiggleBone Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}

	if (mdlhdr.numlocalattachments > 0)
	{
		Stream->seek(mdlhdr.localattachmentindex);
		for (int i = 0; i < mdlhdr.numlocalattachments; i++)
		{
			int attachmentPos = mdlhdr.localattachmentindex + 92 * i;
			Stream->seek(attachmentPos);

			mstudioattachment_t_v49 attachment; Stream->Read(&attachment);
			attachments.push_back(attachment);

			Logger::Info("Attachment Read: %s\n", ReadMDLString(Stream, attachmentPos + attachment.sznameindex).c_str());
			Logger::Info("Attachment Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
	if (mdlhdr.numhitboxsets > 0)
	{
		Stream->seek(mdlhdr.hitboxsetindex);
		for (int i = 0; i < mdlhdr.numhitboxsets; i++)
		{
			int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * i;
			Stream->seek(hitboxsetPos);

			mstudiohitboxset_t_v49 hitboxset; Stream->Read(&hitboxset);
			Logger::Info("HitboxSet Read: %s\n", ReadMDLString(Stream, hitboxsetPos + hitboxset.sznameindex).c_str());
			if (hitboxset.numhitboxes > 0) numhitboxes += hitboxset.numhitboxes;
			hitboxsets.push_back(hitboxset);
			Logger::Info("HitboxSet Read: %d\n", i);
		}
		if (numhitboxes > 0)
		{
			for (int i = 0; i < numhitboxes; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * mdlhdr.numhitboxsets;
				int hitboxPos = hitboxsetPos + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);
				Logger::Info("Hitbox Read: %s\n", ReadMDLString(Stream, hitboxPos + hitbox.szhitboxnameindex).c_str());
				hitboxes.push_back(hitbox);
				Logger::Info("Hitbox Read: %d\n", i);
			}
		}
		//Logger::Info("strPos: %d\n", Stream->Position());

	}
	if (mdlhdr.numbones > 0)
	{
		Stream->Read(&bonenametable, mdlhdr.numbones);
		Logger::Info("BoneTable Read: %d\n", mdlhdr.numbones);
	}
	if (mdlhdr.numlocalanim > 0)
	{
		Stream->seek(mdlhdr.localanimindex);
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			Stream->seek(animDescPos);

			mstudioanimdesc_t_v49 animdesc; Stream->Read(&animdesc);
			animdescs.push_back(animdesc);

			Logger::Info("AnimDesc Read: %s\n", ReadMDLString(Stream, animDescPos + animdesc.sznameindex).c_str());
			Logger::Info("AnimDesc Read: %d\n", i);
			//Logger::Info("strPos: %d\n", Stream->Position());
		}


		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int boneHeaderNum = 0;
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			int animPos = animDescPos + animdescs[i].animindex;

			if (animdescs[i].sectionindex == 0)
			{

				Stream->seek(animPos);
				//Logger::Info("strPos: %d\n", Stream->Position());
				for (int j = 0; j < mdlhdr.numlocalanim * mdlhdr.numbones; j++)
				{
					int dist = 0;
					int nextAnim = i + 1;
					int hdrStartPos = Stream->Position();
					//Logger::Info("hdrStartPos: %d\n", hdrStartPos);
					mstudioanim_t_v49 anim; Stream->Read(&anim);
					mstudioanimdata_t_v49 animData;

					//Logger::Info("HdrNum: %d\n", j);
					//Logger::Info("Bone: %d\n", anim.bone);
					//Logger::Info("Flags Read: %d\n", anim.flags);
					//Logger::Info("NextOffset Read: %d\n", anim.nextoffset);
					//Logger::Info("RawRot Read: One: %d I: %d J: %d K: %d\n", anim.rawrot.one, anim.rawrot.i, anim.rawrot.j, anim.rawrot.k);
					//Logger::Info("RawRot2 Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot2.x, anim.rawrot2.y, anim.rawrot2.z, anim.rawrot2.wneg);
					//Logger::Info("RawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
					//Logger::Info("RotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
					//Logger::Info("PosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);

					int headerSize = GetAnimHeaderSize((int)anim.flags);
					int hdrEndPos = Stream->Position();
					//Logger::Info("hdrEndPos: %d\n", hdrEndPos);
					boneHeaderNum++;
					if (anim.nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;
						if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
						if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - hdrEndPos;

						//Logger::Info("SeqIndx: %d\n", mdlhdr.localseqindex);
						//Logger::Info("AnimDataSizeEnd: %d\n", dist);
						Stream->Read(&animData, dist);
						anim.animdata = animData;
						anims.push_back(anim);
						Logger::Info("Anim Read: %d\n", i);
						break;
					}
					dist = anim.nextoffset - headerSize;
					//Logger::Info("AnimDataSize: %d\n", dist);
					Stream->Read(&animData, dist);
					anim.animdata = animData;
					anims.push_back(anim);

				}

			}

			if (animdescs[i].sectionindex > 0)
			{
				int secBoneHeaderNum = 0;
				int animDescPos = mdlhdr.localanimindex + 100 * i;
				int animPos = animDescPos + animdescs[i].animindex;
				int secPos = animDescPos + animdescs[i].sectionindex;
				int frames = animdescs[i].numframes;
				int secFrames = animdescs[i].sectionframes;
				std::vector<sectionindexesindex_t_v49> idxs;

				int secNumber = (frames / secFrames) + 2;

				Stream->seek(secPos);

				for (int j = 0; j < secNumber; j++)
				{

					sectionindexesindex_t_v49 sectionIdx; Stream->Read(&sectionIdx);
					//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
					idxs.push_back(sectionIdx);
					sectionindexes.push_back(sectionIdx);
				}

				for (int j = 0; j < secNumber; j++)
				{
					sectionindexesindex_t_v49 sectionIdx = idxs[j];
					int nextSection = j + 1;
					//Logger::Info("secIdxPos: %d\n", animDescPos + sectionIdx.sectionoffset);
					Stream->seek(animDescPos + sectionIdx.sectionoffset);


					for (int k = 0; k < mdlhdr.numlocalanim * mdlhdr.numbones; k++)
					{
						int dist = 0;
						int nextAnim = i + 1;
						int hdrStartPos = Stream->Position();
						//Logger::Info("secHdrStartPos: %d\n", hdrStartPos);
						mstudioanim_t_v49 anim; Stream->Read(&anim);
						mstudioanimdata_t_v49 animData;

						//Logger::Info("SecHdrNum: %d\n", j);
						//Logger::Info("SecBone: %d\n", anim.bone);
						//Logger::Info("SecFlags Read: %d\n", anim.flags);
						//Logger::Info("SecNextOffset Read: %d\n", anim.nextoffset);
						//Logger::Info("SecRawRot Read: One: %d I: %d J: %d K: %d\n", anim.rawrot.one, anim.rawrot.i, anim.rawrot.j, anim.rawrot.k);
						//Logger::Info("SecRawRot2 Read: X: %d Y: %d Z: %d W: %d\n", anim.rawrot2.x, anim.rawrot2.y, anim.rawrot2.z, anim.rawrot2.wneg);
						//Logger::Info("SecRawPos Read: X: %d Y: %d Z: %d\n", anim.rawpos.x, anim.rawpos.y, anim.rawpos.z);
						//Logger::Info("SecRotPtr Read: X: %d Y: %d Z: %d\n", anim.animrot.offset.x, anim.animrot.offset.y, anim.animrot.offset.z);
						//Logger::Info("SecPosPtr Read: X: %d Y: %d Z: %d\n", anim.animpos.offset.x, anim.animpos.offset.y, anim.animpos.offset.z);

						int headerSize = GetAnimHeaderSize((int)anim.flags);
						int hdrEndPos = Stream->Position();
						//Logger::Info("secHdrEndPos: %d\n", hdrEndPos);
						secBoneHeaderNum++;
						if (anim.nextoffset == 0)
						{
							if (animdescs[i].numikrules > 0) dist = animDescPos + animdescs[i].ikruleindex - hdrEndPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
							if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim - 1 && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + idxs[nextSection].sectionoffset - hdrEndPos;
							if (nextSection > secNumber && nextAnim > mdlhdr.numlocalanim - 1 && animdescs[i].numikrules == 0) dist = mdlhdr.localseqindex - hdrEndPos;
							Stream->Read(&animData, dist);
							anim.animdata = animData;
							sections.push_back(anim);
							//Logger::Info("AnimDataSizeEnd: %d\n", dist);
							Logger::Info("Section Read: %d\n", i);
							break;
						}
						dist = anim.nextoffset - headerSize;
						Stream->Read(&animData, dist);
						anim.animdata = animData;
						sections.push_back(anim);
					}

				}

			}

			if (animdescs[i].numikrules > 0)
			{
				int numOfCompressedIkErrors = 0;
				int numOfIkErrors = 0;
				std::vector<int> errorIdxs;
				int nextAnim = i + 1;

				for (int j = 0; j < animdescs[i].numikrules; j++)
				{
					Stream->seek(animDescPos + animdescs[i].ikruleindex + 152 * j);
					int pos = Stream->Position();
					mstudioikrule_t_v49 v49IkRule; Stream->Read(&v49IkRule);
					ikrules.push_back(v49IkRule);
					if (v49IkRule.compressedikerrorindex > 0) errorIdxs.push_back(pos + v49IkRule.compressedikerrorindex);
					if (v49IkRule.compressedikerrorindex > 0) numOfCompressedIkErrors++;
					if (v49IkRule.ikerrorindex > 0) numOfIkErrors++;

					Logger::Info("IkRule Read: %d\n", j);
					numikrules++;
				}

				if (numOfCompressedIkErrors > 0)
				{
					int animStartPos = mdlhdr.localanimindex + 100 * i;
					for (int j = 0; j < numOfCompressedIkErrors; j++)
					{
						Stream->seek(errorIdxs[j]);
						int dist = 0;
						int next = j + 1;
						int pos = Stream->Position();
						mstudiocompressedikerror_t_v49 v49CompressedIkError; Stream->Read(&v49CompressedIkError);
						compressedikerrors.push_back(v49CompressedIkError);
						mstudioanimdata_t_v49 animData;
						int endPos = Stream->Position();
						if (j + 1 > numOfCompressedIkErrors - 1)
						{
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0)  dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - endPos;
							if (nextAnim < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - endPos;
							if (nextAnim > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - endPos;
							Stream->Read(&animData, dist);
							v49CompressedIkError.animdata = animData;
							Logger::Info("CompressedIkError Read: %d\n", j);
							break;
						}
						Logger::Info("CompressedIkError Read: %d\n", j);
						dist = errorIdxs[next] - endPos;
						Stream->Read(&animData, dist);
						v49CompressedIkError.animdata = animData;
					}
				}

			}

		}
	}

	if (mdlhdr.numlocalseq > 0)
	{
		Stream->seek(mdlhdr.localseqindex);
		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 212 * i;
			Stream->seek(seqPos);
			mstudioseqdescv49_t seqDesc; Stream->Read(&seqDesc);
			seqdescs.push_back(seqDesc);
			Logger::Info("Seq Read: %s\n", ReadMDLString(Stream, seqPos + seqDesc.szlabelindex).c_str());
			Logger::Info("Seq Read: %s\n", ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex).c_str());
			Logger::Info("Seq Read: %d\n", i);
		}

		for (int i = 0; i < mdlhdr.numlocalseq; i++)
		{
			int seqPos = mdlhdr.localseqindex + 212 * i;
			int numBlends = seqdescs[i].groupsize[0] * seqdescs[i].groupsize[1];
			int posekeySize = seqdescs[i].groupsize[0] + seqdescs[i].groupsize[1];
			int numAutolayers = seqdescs[i].numautolayers;
			int numActMods = seqdescs[i].numactivitymodifiers;
			int numEvents = seqdescs[i].numevents;
			if (seqdescs[i].weightlistindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].weightlistindex);
				seqweightlist_t_v49 weightlist; Stream->Read(&weightlist, mdlhdr.numbones);
				seqweightlist.push_back(weightlist);
				Logger::Info("Weightlist Read: %d\n", i);
			}

			if (seqdescs[i].animindexindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].animindexindex);
				blendgroup_t_v49 blendgroup; Stream->Read(&blendgroup, numBlends);
				blends.push_back(blendgroup);
				Logger::Info("BlendGroup Read: %d\n", i);
			}

			if (seqdescs[i].posekeyindex > 0)
			{
				Stream->seek(seqPos + seqdescs[i].posekeyindex);
				posekey_t_v49 posekey; Stream->Read(&posekey, posekeySize);
				posekeys.push_back(posekey);
				Logger::Info("Posekey Read: %d\n", i);
			}

			if (numAutolayers > 0)
			{
				Stream->seek(seqPos + seqdescs[i].autolayerindex);
				for (int j = 0; j < numAutolayers; j++)
				{
					mstudioautolayer_t_v49 autolayer; Stream->Read(&autolayer);
					autolayers.push_back(autolayer);
					Logger::Info("Autolayer Read: %d\n", j);
				}
			}

			if (numEvents > 0)
			{
				Stream->seek(seqPos + seqdescs[i].eventindex);
				for (int j = 0; j < seqdescs[i].numevents; j++)
				{
					mstudioevent_t_v49 _event; Stream->Read(&_event);
					events.push_back(_event);

					Logger::Info("Event Read: %s\n", ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + 12).c_str());
					Logger::Info("Event Read: %s\n", ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex).c_str());
					Logger::Info("Event Read: %d\n", j);
				}
			}

			if (numActMods > 0)
			{
				Stream->seek(seqPos + seqdescs[i].activitymodifierindex);
				for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
				{
					mstudioactivitymodifier_t_v49 actmod; Stream->Read(&actmod);
					activitymodifiers.push_back(actmod);
					Logger::Info("ActMod Read: %s\n", ReadMDLString(Stream, seqPos + seqdescs[i].activitymodifierindex + 4 * j + actmod.sznameindex).c_str());
					Logger::Info("ActMod Read: %d\n", j);
				}
			}
		}
	}
	if (mdlhdr.numlocalnodes > 0)
	{
		Stream->seek(mdlhdr.localnodenameindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodename_t_v49 nodeName; Stream->Read(&nodeName);
			nodenames.push_back(nodeName);
			Logger::Info("NodeName Read: %s\n", ReadMDLString(Stream, nodenames[i].sznameindex).c_str());
			Logger::Info("NodeName Read: %d\n", i);
		}
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodedata_v49 node; Stream->Read(&node, mdlhdr.numlocalnodes * mdlhdr.numlocalnodes);
			nodes.push_back(node);
			Logger::Info("Node Read: %d\n", i);
		}
	}
	if (mdlhdr.numbodyparts > 0)
	{
		Stream->seek(mdlhdr.bodypartindex);
		for (int i = 0; i < mdlhdr.numbodyparts; i++)
		{
			mstudiobodyparts_t_v49 bodyPart; Stream->Read(&bodyPart);
			if (bodyPart.nummodels > 0) nummodels += bodyPart.nummodels;
			bodyparts.push_back(bodyPart);
			Logger::Info("Bodypart Read: %s\n", ReadMDLString(Stream, mdlhdr.bodypartindex + 16 * i + bodyparts[i].sznameindex).c_str());
			Logger::Info("Bodypart Read: %d\n", i);
		}
	}

	if (nummodels > 0)
	{
		Stream->seek(mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts);
		for (int i = 0; i < nummodels; i++)
		{
			mstudiomodel_t_v49 model; Stream->Read(&model);
			if (model.nummeshes > 0) nummeshes += model.nummeshes;
			models.push_back(model);
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts + 148 * i;

			Logger::Info("Model Read: %s\n", ReadMDLString(Stream, modelPos).c_str());
			Logger::Info("Model Read: %d\n", i);
		}

	}

	if (mdlhdr.numikchains > 0)
	{
		Stream->seek(mdlhdr.ikchainindex);
		for (int i = 0; i < mdlhdr.numikchains; i++)
		{
			mstudioikchain_t_v49 ikchain; Stream->Read(&ikchain);
			if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;
			ikchains.push_back(ikchain);
			Logger::Info("IkChain Read: %d\n", i);
		}

		for (int i = 0; i < numiklinks; i++)
		{
			int ikLinkPos = (mdlhdr.ikchainindex + 16 * mdlhdr.numikchains) + 28 * i;
			Stream->seek(ikLinkPos);
			mstudioiklink_t_v49 iklink; Stream->Read(&iklink);
			iklinks.push_back(iklink);
			Logger::Info("IkLink Read: %d\n", i);
		}
	}
	if (mdlhdr.localposeparamindex > 0)
	{
		Stream->seek(mdlhdr.localposeparamindex);
		for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
		{
			mstudioposeparamdesc_t_v49 poseparamdesc; Stream->Read(&poseparamdesc);
			poseparamdescs.push_back(poseparamdesc);
			int poseParamPos = mdlhdr.localposeparamindex + 20 * i;

			Logger::Info("PoseParamDesc Read: %s\n", ReadMDLString(Stream, poseParamPos + poseparamdescs[i].sznameindex).c_str());
			Logger::Info("PoseParamDesc Read: %d\n", i);
		}
	}

	if (nummeshes > 0)
	{
		for (int i = 0; i < nummeshes; i++)
		{
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts;
			int meshPos = (modelPos + 148 * nummodels) + (20 * mdlhdr.numlocalposeparameters) + (16 * mdlhdr.numikchains + 28 * numiklinks) + 116 * i;
			Stream->seek(meshPos);
			mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
			meshes.push_back(mesh);

			int texturePos = mdlhdr.textureindex + (64 * (int)mesh.material);
			Stream->seek(texturePos);
			mstudiotexture_t_v53 texture; Stream->Read(&texture);

			Logger::Info("MeshMat Read: %s\n", ReadMDLString(Stream, texturePos + texture.sznameindex).c_str());
			Logger::Info("Mesh Read: %d\n", i);
		}
	}
	if (mdlhdr.numincludemodels > 0)
	{
		Stream->seek(mdlhdr.includemodelindex);
		for (int i = 0; i < mdlhdr.numincludemodels; i++)
		{
			mstudiomodelgroup_t_v49 includemodel; Stream->Read(&includemodel);
			includedmodels.push_back(includemodel);
			Logger::Info("IncludedModel Read: %d\n", i);
		}
	}
	if (mdlhdr.numtextures > 0)
	{
		Stream->seek(mdlhdr.textureindex);
		for (int i = 0; i < mdlhdr.numtextures; i++)
		{
			mstudiotexture_t_v49 texture; Stream->Read(&texture);
			textures.push_back(texture);
			int texturePos = mdlhdr.textureindex + 64 * i;

			Logger::Info("Texture Read: %s\n", ReadMDLString(Stream, texturePos + textures[i].sznameindex).c_str());
			Logger::Info("Texture Read: %d\n", i);
		}
	}
	if (mdlhdr.numcdtextures > 0)
	{
		Stream->seek(mdlhdr.cdtextureindex);
		for (int i = 0; i < mdlhdr.numcdtextures; i++)
		{
			mstudiotexturedir_t_v49 cdtexture; Stream->Read(&cdtexture);
			cdtextures.push_back(cdtexture);
			Logger::Info("CdTexture Read: %s\n", ReadMDLString(Stream, cdtextures[i].sznameindex).c_str());
			Logger::Info("CdTexture Read: %d\n", i);
		}
	}
	if (mdlhdr.numskinfamilies > 0)
	{
		if (mdlhdr.numskinref > 0)
		{
			Stream->seek(mdlhdr.skinindex);
			for (int i = 0; i < mdlhdr.numskinfamilies; i++)
			{
				mstudioskingroup_t_v49 skingroup; Stream->Read(&skingroup, mdlhdr.numskinref);
				skingroups.push_back(skingroup);
				Logger::Info("SkinGroup Read: %d\n", i);
			}
		}
	}

	if (mdlhdr.keyvaluesize > 0)
	{
		Stream->seek(mdlhdr.keyvalueindex);
		Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
		Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
	}
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		Stream->seek(mdlsubhdr.srcbonetransformindex);
		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			mstudiosrcbonetransform_t_v49 srcbonetransform; Stream->Read(&srcbonetransform);
			srcbonetransforms.push_back(srcbonetransform);
			Logger::Info("SrcBoneTransform Read: %d\n", i);
		}
	}
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlsubhdr.linearboneindex + 408);
		Stream->Read(&linearbone);
		Stream->Read(&linearbonedata, mdlhdr.numbones);
		Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
	}

	Stream->seek(mdlsubhdr.sznameindex + 408);
	mstudiostringtable_t_v49 stringTable; Stream->Read(&stringTable, mdlhdr);
	std::string string(stringTable.mdlname.begin(), stringTable.mdlname.end());
	Logger::Info("mldName: %s\n", string.c_str());

	MDL::v49Mdl _v49mdl{ mdlhdr,mdlsubhdr,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,anims, sectionindexes, sections, ikrules,compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,cdtextures,textures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata, stringTable };

	return _v49mdl;
}

int MDL::v49Mdl::ConvertFlag(int flag)
{
	int test = 0;
	int flagNum = 0;

	if (flag & STUDIO_ANIM_ANIMROT)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
		flagNum++;
	}
	if (flag & STUDIO_ANIM_ANIMPOS)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
		flagNum++;
	}

	if (flag & STUDIO_ANIM_RAWROT)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
		test += STUDIO_ANIM_RAWROT_53;
		flagNum++;
	}
	if (flag & STUDIO_ANIM_RAWROT2)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
		test += STUDIO_ANIM_RAWROT_53;
		flagNum++;
	}
	if (flag & STUDIO_ANIM_RAWPOS)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWPOS);
		test += STUDIO_ANIM_RAWPOS_53;
		flagNum++;
	}

	if (flag & STUDIO_ANIM_DELTA)
	{
		//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_DELTA);
		test += STUDIO_ANIM_DELTA_53;
	}

	if (flagNum == 1)
	{
		if (flag & STUDIO_ANIM_ANIMROT)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_ANIMPOS)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
			test += STUDIO_ANIM_RAWROT_53;
		}

		if (flag & STUDIO_ANIM_RAWROT)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_RAWROT2)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
			test += STUDIO_ANIM_RAWPOS_53;
		}
		if (flag & STUDIO_ANIM_RAWPOS)
		{
			//Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWPOS);
			test += STUDIO_ANIM_READBONE_53;
		}
	}

	test += STUDIO_ANIM_RAWSCALE_53;
	if (flag == 0)
	{
		test = 0;
	}
	return test;
}

std::vector<int> MDL::v49Mdl::GetIkChainBones()
{
	std::vector<int> ikChainBones;
	std::vector<int> ikChainBonesChildren;
	std::vector<int> ikChainBonesFinal;

	for (int i = 0; i < iklinks.size(); i++)
	{
		ikChainBones.push_back(iklinks[i].bone);
	}

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < ikChainBones.size(); j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, ikChainBones[j]) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBones[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	int size = ikChainBonesChildren.size();

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 bone = bones[i];
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	for (int i = 0; i < ikChainBonesChildren.size(); i++)
	{
		ikChainBonesFinal.push_back(ikChainBonesChildren[i]);
	}

	for (int i = 0; i < ikChainBones.size(); i++)
	{
		ikChainBonesFinal.push_back(ikChainBones[i]);
	}

	return ikChainBonesFinal;
}

std::vector<int> MDL::v49Mdl::v53GetAnimHdrBytesAdded(bool zeroFirst)
{
	std::vector<int> animBytesAdded;
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int next = i + 1;
		animBytesAdded.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex == 0)
		{
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
		else animBytesAdded[i] = bytesAdded;
	}
	return animBytesAdded;
}

int MDL::v49Mdl::v53GetTotalAnimHdrBytesAdded()
{
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int next = i + 1;
		if (animdescs[i].sectionindex == 0)
		{
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 > mdlhdr.numlocalanim - 1)
					{
						int dist = (mdlhdr.localseqindex - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("secDist:  %d\n", dist);
						//Logger::Info("secFinalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
	}
	//Logger::Info("Total BytesAdded: %d\n", bytesAdded);
	return bytesAdded;
}

int MDL::v49Mdl::v53GetAnimHdrBytesAddedIdv(int anim)
{
	int bytesAdded = 0;
	int lastHdr = 0;

	for (int i = 0; i < anim + 1; i++)
	{
		int next = i + 1;
		if (animdescs[i].sectionindex == 0)
		{
			bytesAdded = 0;
			for (int j = lastHdr; j < anims.size(); j++)
			{
				int headerSize = GetAnimHeaderSize((int)anims[j].flags);
				lastHdr++;
				bytesAdded += (32 - headerSize);
				if (anims[j].nextoffset == 0)
				{

					if (animdescs[i].numikrules > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].ikruleindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						bytesAdded += 32 - dist;
					}

					if (i + 1 > mdlhdr.numlocalanim - 1)
					{
						int dist = (mdlhdr.localseqindex - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("secDist:  %d\n", dist);
						//Logger::Info("secFinalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex == 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					if (i + 1 < mdlhdr.numlocalanim - 1 && animdescs[next].sectionindex > 0)
					{
						int dist = (((animdescs[next].baseptr - animdescs[next].sectionindex) * -1) - (anims[j].strPos + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < anims[j].animdata.arry.size(); l++)
							{
								short animValue{ (short)anims[j].animdata.arry[l] };
								if (animValue == 0)
								{
									int newPos = l;
									dist = anims[j].animdata.arry.size() - newPos;
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						//AddDistToAnimData(anims[j].animdata, dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
			}
		}
	}
	return bytesAdded;
}

std::vector<int> MDL::v49Mdl::v53GetSecHdrBytesAdded(bool zeroFirst)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerAnim;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		bytesAddedPerAnim.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{

						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));
							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}
						if (i + 1 < mdlhdr.numlocalanim && j + 1 < num)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));
							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							//AddDistToAnimData(sections[j].animdata, dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerAnim;
}

int MDL::v49Mdl::v53GetTotalSecHdrBytesAdded()
{
	int bytesAdded = 0;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	//Logger::Info("Total BytesAdded: %d\n", bytesAdded);
	return bytesAdded;
}

int MDL::v49Mdl::v53GetSecHdrBytesAddedIdv(int anim)
{
	int bytesAdded = 0;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < anim + 1; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		//bytesAddedPerAnim.push_back(bytesAdded);
		//Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			bytesAdded = 0;
			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{

						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 > mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = (mdlhdr.localseqindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim - 1 && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAdded;
}

std::vector<int> MDL::v49Mdl::v53GetSecBytesAdded(bool zeroFirst)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerSec;
	int secNum = 0;
	int lastHdr = 0;

	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int nextAnim = i + 1;

		int startPos = mdlhdr.localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;

			for (int j = 0; j < num; j++)
			{
				int nextSec = j + 1;
				int bytesAddedPer = 0;
				bytesAddedPerSec.push_back(bytesAdded);
				//Logger::Info("secBytesAdded: %d, Anim: %d\n", bytesAdded, i);
				for (int k = lastHdr; k < sections.size(); k++)
				{
					int pos2 = sections[k].strPos;
					int headerSize = GetAnimHeaderSize((int)sections[k].flags);
					lastHdr++;
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (sections[k].nextoffset == 0)
					{
						if (animdescs[i].numikrules > 0 && j + 1 > num - 1)
						{
							int dist = (startPos + animdescs[i].ikruleindex - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							bytesAdded += 32 - dist;
						}

						if (i + 1 < mdlhdr.numlocalanim && j + 1 > num - 1)
						{
							int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						if (j + 1 < num)
						{
							int nextSection = secNum + 1;
							int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < sections[j].animdata.arry.size(); l++)
								{
									short animValue{ (short)sections[j].animdata.arry[l] };
									if (animValue == 0)
									{
										int newPos = l;
										dist = sections[j].animdata.arry.size() - newPos;
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						break;
					}
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerSec;
}

std::vector<int> MDL::v49Mdl::v53IkRuleStairsPerAnim()
{
	std::vector<int> ikRuleStairsPerAnim;
	int ikRuleNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];
		int ikStairs = -12 * (ikRuleNum);

		ikRuleStairsPerAnim.push_back(ikStairs);
		if (v49AnimDesc.numikrules > 0)
		{
			ikRuleNum += v49AnimDesc.numikrules;
		}

	}
	return ikRuleStairsPerAnim;
}

void MDL::v49Mdl::SetMdlInts()
{
	ikRuleStairsPerAnim = v53IkRuleStairsPerAnim();
	hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	secHdrBytesSecAdd = v53GetSecBytesAdded(true);

	animByteAddedTotal = v53GetTotalAnimHdrBytesAdded();
	animSecByteAddedTotal = v53GetTotalSecHdrBytesAdded();

	animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
	bytesAddedToRuiMesh = 0;
	bytesAddedToIkRules = -12 * ikrules.size();
	bytesAddedToHeader = 52;
	bytesAddedToBones = mdlhdr.numbones * 28;
	bytesAddedToAnims = -8 * mdlhdr.numlocalanim;
	bytesAddedToAnimData = mdlhdr.numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal + bytesAddedToIkRules + bytesAddedToSections : 0;
	bytesAddedToSections = -4 * secHdrBytesSecAdd.size();
	bytesAddedToSeqs = 20 * mdlhdr.numlocalseq;
	bytesAddedToTextures = -20 * mdlhdr.numtextures;
	bytesAddedToIkChains = 16 * mdlhdr.numikchains;
	bytesAddedToActMods = 4 * activitymodifiers.size();
	textureFiller = 0;
	strFiller = 60;
	allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + bytesAddedToRuiMesh;
	numOfLinks = 0;
}

studiohdr_t_v53 MDL::v49Mdl::ConvertHeader(FileInfo info)
{

	int ruiNum = 0;
	int mdlSize = 0;
	int phySize = 0;
	int phyBSize = 0;
	int vtxSize = 0;
	int vvdSize = 0;
	int vvcSize = 0;
	int ruiSize = 0;

	if (info.rui.has_value()) //This is a temp for rui testing. -Liberty
	{
		BinaryReader RUIStream = BinaryReader(info.rui.value().c_str());
		bytesAddedToRuiMesh = RUIStream.size;
		ruiSize = RUIStream.size;
		mstudiorruiheader_t ruiHeader;
		for (int i = 0; i < 64; i++)
		{
			RUIStream.Read(&ruiHeader);

			if (ruiHeader.ruiunk == 0 || ruiHeader.ruimeshindex == 0)
			{
				break;
			}
			ruiNum++;
		}
	}
	bytesAddedToRuiMesh = ruiSize;


	if (info.mdl.has_value())
	{
		BinaryReader MdlStream = BinaryReader(info.mdl.value().c_str());
		mdlSize = MdlStream.size;
	}
	if (info.phy.has_value())
	{
		BinaryReader PhyStream = BinaryReader(info.phy.value().c_str());
		phySize = PhyStream.size;
	}
	if (info.pfb.has_value())
	{
		BinaryReader PhyBStream = BinaryReader(info.pfb.value().c_str());
		phyBSize = PhyBStream.size;
	}
	if (info.vtx.has_value())
	{
		BinaryReader VtxStream = BinaryReader(info.vtx.value().c_str());
		vtxSize = VtxStream.size;
	}
	if (info.vvd.has_value())
	{
		BinaryReader VvdStream = BinaryReader(info.vvd.value().c_str());
		vvdSize = VvdStream.size;
	}

	if (info.vvc.has_value())
	{
		BinaryReader VvcStream = BinaryReader(info.vvc.value().c_str());
		vvcSize = VvcStream.size;
	}

	Logger::Notice("RuiSize %d\n", ruiSize);

	int			id;
	int			version;
	int			checksum;
	int			sznameindex = mdlsubhdr.sznameindex + allBytesAdded + strFiller + 408 + ruiSize;
	char		name[64];

	int			length;

	Vector3		eyeposition;

	Vector3		illumposition;

	Vector3		hull_min;
	Vector3		hull_max;

	Vector3		view_bbmin;
	Vector3		view_bbmax;

	mdlflags_t	flags;

	int			numbones;
	int			boneindex = mdlhdr.boneindex + bytesAddedToHeader;

	int			numbonecontrollers;
	int			bonecontrollerindex = mdlhdr.bonecontrollerindex + bytesAddedToHeader + bytesAddedToBones;

	int			numhitboxsets;
	int			hitboxsetindex = mdlhdr.hitboxsetindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalanim;
	int			localanimindex = mdlhdr.localanimindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalseq;
	int			localseqindex = mdlhdr.localseqindex + bytesAddedToAnims + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;

	int			activitylistversion;
	int			eventsindexed;

	int			numtextures;
	int			textureindex = mdlhdr.textureindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + ruiSize;

	int			numcdtextures;
	int			cdtextureindex = mdlhdr.cdtextureindex + allBytesAdded + ruiSize;

	int			numskinref;
	int			numskinfamilies;
	int			skinindex = mdlhdr.skinindex + allBytesAdded + ruiSize;

	int			numbodyparts;
	int			bodypartindex = mdlhdr.bodypartindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			numlocalattachments;
	int			localattachmentindex = mdlhdr.localattachmentindex + bytesAddedToHeader + bytesAddedToBones;

	int			numlocalnodes;
	int			localnodeindex = mdlhdr.localnodeindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;
	int			localnodenameindex = mdlhdr.localnodenameindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexdesc;
	int			flexdescindex = mdlhdr.flexdescindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexcontrollers;
	int			flexcontrollerindex = mdlhdr.flexcontrollerindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numflexrules;
	int			flexruleindex = mdlhdr.flexruleindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			numikchains;
	int			ikchainindex = mdlhdr.ikchainindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

	int			nummouths = ruiNum;
	int			mouthindex = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + 8 * mdlhdr.numincludemodels;

	int			numlocalposeparameters;
	int			localposeparamindex = mdlhdr.localposeparamindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	int			surfacepropindex = mdlhdr.surfacepropindex + allBytesAdded + strFiller + ruiSize;

	int			keyvalueindex = mdlhdr.keyvalueindex + allBytesAdded + ruiSize;
	int			keyvaluesize;

	int			numlocalikautoplaylocks;
	int			localikautoplaylockindex = mdlhdr.localikautoplaylockindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;


	float		mass;
	int			contents;

	int			numincludemodels;
	int			includemodelindex = mdlhdr.includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	uint32_t	virtualModel;

	int			bonetablebynameindex = mdlhdr.bonetablebynameindex + bytesAddedToHeader + bytesAddedToBones;

	std::byte	constdirectionallightdot;

	std::byte	rootLOD;

	std::byte	numAllowedRootLODs;

	std::byte	unused;

	float		fadedistance;

	int			numflexcontrollerui;
	int			flexcontrolleruiindex = mdlhdr.flexcontrolleruiindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;

	uint32_t	pVertexBase;
	uint32_t	pIndexBase;

	int			mayaindex = 0;

	int			numsrcbonetransform;
	int			srcbonetransformindex = mdlsubhdr.srcbonetransformindex + allBytesAdded + ruiSize;

	int			illumpositionattachmentindex;

	int			linearboneindex = mdlsubhdr.linearboneindex + allBytesAdded + 408 + ruiSize;

	int			m_nBoneFlexDriverCount;
	int			m_nBoneFlexDriverIndex;

	int			aabbindex = mdlsubhdr.sznameindex + allBytesAdded + strFiller + 408 - 61 + ruiSize;
	int			numaabb = 0;
	int			numaabb1 = 0;
	int			numaabb2 = 0;

	int			unkstringindex = mdlsubhdr.sznameindex + allBytesAdded + strFiller + ruiSize;

	int			vtxindex = vtxSize > 0 ? mdlSize + allBytesAdded + phySize + phyBSize + strFiller + ruiSize : 0; // VTX
	int			vvdindex = vvdSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize + strFiller + ruiSize : 0; // VVD / IDSV
	int			vvcindex = 0; // VVC / IDCV 
	int			vphyindex = phySize > 0 ? mdlSize + allBytesAdded + strFiller + ruiSize : 0; // VPHY / IVPS

	int			vtxsize = vtxSize; // VTX
	int			vvdsize = vvdSize; // VVD / IDSV
	int			vvcsize = 0; // VVC / IDCV 
	int			vphysize = phySize; // VPHY / IVPS

	int			unkmemberindex1 = mdlSize + allBytesAdded + phySize + strFiller + ruiSize;
	int			numunkmember1 = phyBSize > 0 ? 0 : 0;

	int			unk = 0;

	int			unkindex3 = mdlSize + allBytesAdded + phySize + strFiller + ruiSize;

	int			unused1[60];

	studiohdr_t_v53 v53Hdr = { mdlhdr.id, 53, mdlhdr.checksum, sznameindex, ArrayToVector(mdlhdr.name, 64), mdlhdr.length, mdlhdr.eyeposition, mdlhdr.illumposition, mdlhdr.hull_min, mdlhdr.hull_max, mdlhdr.view_bbmin, mdlhdr.view_bbmax, mdlhdr.flags, mdlhdr.numbones, boneindex, mdlhdr.numbonecontrollers, bonecontrollerindex, mdlhdr.numhitboxsets, hitboxsetindex, mdlhdr.numlocalanim, localanimindex, mdlhdr.numlocalseq, localseqindex, mdlhdr.activitylistversion, mdlhdr.eventsindexed, mdlhdr.numtextures, textureindex, mdlhdr.numcdtextures, cdtextureindex, mdlhdr.numskinref, mdlhdr.numskinfamilies, skinindex, mdlhdr.numbodyparts, bodypartindex, mdlhdr.numlocalattachments, localattachmentindex, mdlhdr.numlocalnodes, localnodeindex, localnodenameindex, mdlhdr.numflexdesc, flexdescindex, mdlhdr.numflexcontrollers, flexcontrollerindex, mdlhdr.numflexrules, flexruleindex, mdlhdr.numikchains, ikchainindex, ruiNum, mouthindex, mdlhdr.numlocalposeparameters, localposeparamindex, surfacepropindex, keyvalueindex, mdlhdr.keyvaluesize, mdlhdr.numlocalikautoplaylocks, localikautoplaylockindex, mdlhdr.mass, mdlhdr.contents, mdlhdr.numincludemodels, includemodelindex, mdlhdr.virtualModel, bonetablebynameindex, mdlhdr.constdirectionallightdot, mdlhdr.rootLOD, mdlhdr.numAllowedRootLODs, mdlhdr.unused, fadedistance, mdlhdr.numflexcontrollerui, flexcontrolleruiindex, mdlhdr.pVertexBase, mdlhdr.pIndexBase, mayaindex, mdlsubhdr.numsrcbonetransform, srcbonetransformindex, mdlsubhdr.illumpositionattachmentindex, linearboneindex, mdlsubhdr.m_nBoneFlexDriverCount, mdlsubhdr.m_nBoneFlexDriverIndex, aabbindex, numaabb, numaabb1, numaabb2, unkstringindex, vtxindex, vvdindex, vvcindex, vphyindex, vtxsize, vvdsize, vvcsize, vphysize, unkmemberindex1, numunkmember1, unk, unkindex3, unused1 };

	return v53Hdr;
}

std::vector<mstudiobone_t_v53> MDL::v49Mdl::BoneConversion()
{
	std::vector<mstudiobone_t_v53> v53Bones;
	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		int stairs = bytesAddedToAnims + bytesAddedToSeqs + (28 * mdlhdr.numbones) - (28 * i) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;

		mstudiobone_t_v49 v49Bone = bones[i];

		Vector3 unkvector{ 1,1,1 };

		Vector3 unkvector1{ 0.0004882813 ,0.0004882813 ,0.0004882813 };

		int unkindex = 0;

		Vector3 posscale{ 0,0,0 };

		if (v49Bone.procindex > 0) v49Bone.procindex += stairs;
		if (v49Bone.surfacepropidx > 0) v49Bone.surfacepropidx += stairs;
		if (v49Bone.surfacepropLookup > 0) v49Bone.surfacepropLookup += stairs;

		mstudiobone_t_v53 bone{ v49Bone.sznameindex + stairs, v49Bone.parent, -1,-1,-1,-1,-1,-1, v49Bone.pos, v49Bone.quat, v49Bone.rot, unkvector, posscale, v49Bone.rotscale, unkvector1, v49Bone.poseToBone, v49Bone.qAlignment, v49Bone.flags, v49Bone.proctype, v49Bone.procindex, v49Bone.physicsbone, v49Bone.surfacepropidx, v49Bone.contents, v49Bone.surfacepropLookup, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		v53Bones.push_back(bone);
	}
	return v53Bones;
}

std::vector<mstudiobbox_t_v53> MDL::v49Mdl::HitboxConversion()
{
	std::vector<mstudiobbox_t_v53> v53Hitboxes;
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < hitboxes.size(); i++)
	{
		mstudiobbox_t_v49 v49Hitbox = hitboxes[i];

		mstudiobbox_t_v53 hitbox{ v49Hitbox.bone, v49Hitbox.group, v49Hitbox.bbmin, v49Hitbox.bbmax, v49Hitbox.szhitboxnameindex + stairs, 0, 0, 0,0,0,0,0,0 };

		v53Hitboxes.push_back(hitbox);
	}

	return v53Hitboxes;
}

std::vector<mstudioanimdesc_t_v53> MDL::v49Mdl::AnimDescConversion()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<mstudioanimdesc_t_v53> v53AnimDescs;
	int ikRuleNum = 0;
	int animIdx = mdlhdr.localanimindex;
	//Logger::Info("animIdx: %d\n", animIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;
	int secNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		//Logger::Info("Test\n");
		mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];

		int frames = v49AnimDesc.numframes;
		int secFrames = v49AnimDesc.sectionframes;
		int outPos = animIdx + bytesAddedToHeader + bytesAddedToBones + 92 * i;
		int pos = animIdx + 100 * i;
		int stairs = (-((8 * (mdlhdr.numlocalanim - i))) + bytesAddedToSeqs) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
		int steps = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];


		if (v49AnimDesc.numikrules > 0)
		{
			int stairs = v53GetAnimHdrBytesAddedIdv(i);
			int stairs2 = 0;
			if (v49AnimDesc.sectionindex > 0) stairs2 = v53GetSecHdrBytesAddedIdv(i);
			v49AnimDesc.ikruleindex += stairs + stairs2;
			ikRuleNum += v49AnimDesc.numikrules;
		}
		int secStairs = -4 * secNum;
		if (v49AnimDesc.sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			secNum += num;
			int dist = (((pos + animdescs[i].sectionindex) + (8 * num)) - (pos + animdescs[i].animindex)) * -1;
		}
		int ikStairs = -12 * (ikRuleNum);

		if (v49AnimDesc.animindex > 0) v49AnimDesc.animindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.movementindex > 0) v49AnimDesc.movementindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.ikruleindex > 0) v49AnimDesc.ikruleindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.localhierarchyindex > 0) v49AnimDesc.localhierarchyindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.zeroframeindex > 0) v49AnimDesc.zeroframeindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.ikrulezeroframeindex > 0) v49AnimDesc.ikrulezeroframeindex += steps + ikStairs + secStairs;
		if (v49AnimDesc.sectionindex > 0) v49AnimDesc.sectionindex += steps + ikStairs + secStairs;

		int compressedIkRuleIdx = 0; //if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = ( basePtr - ( basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules) ) ) * -1 ;


		mstudioanimdesc_t_v53 animDesc = { -outPos, v49AnimDesc.sznameindex + stairs + ikStairs, v49AnimDesc.fps, v49AnimDesc.flags, v49AnimDesc.numframes, v49AnimDesc.nummovements, v49AnimDesc.movementindex, compressedIkRuleIdx * -1, v49AnimDesc.animindex, v49AnimDesc.numikrules, v49AnimDesc.ikruleindex, v49AnimDesc.numlocalhierarchy, v49AnimDesc.localhierarchyindex, v49AnimDesc.sectionindex, v49AnimDesc.sectionframes, v49AnimDesc.zeroframespan, v49AnimDesc.zeroframecount, v49AnimDesc.zeroframeindex, v49AnimDesc.zeroframestalltime, 0, 0, 0, 0, 0 };

		v53AnimDescs.push_back(animDesc);
	}
	return v53AnimDescs;
}

std::vector<mstudioseqdesc_t_v53> MDL::v49Mdl::SeqDescConversion()
{
	std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
	std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
	std::vector<mstudioseqdesc_t_v53> v53SeqDescs;
	int ikRuleNum = 0;
	int seqIdx = mdlhdr.localseqindex;
	//Logger::Info("animIdx: %d\n", seqIdx);
	//Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
	//Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
	int bytesAddedToHeader = 52;
	int bytesAddedToBones = mdlhdr.numbones * 28;
	int actModNumber = 0;

	for (int i = 0; i < mdlhdr.numlocalseq; i++)
	{
		//Logger::Info("Test\n");
		mstudioseqdescv49_t v49SeqDesc = seqdescs[i];
		int strPos = mdlhdr.localseqindex + 212 * i;

		int steps = 20 * (mdlhdr.numlocalseq - i);
		int steps2 = steps + 4 * actModNumber;
		int stairs1 = strPos + v49SeqDesc.weightlistindex == mdlhdr.localseqindex + seqdescs[0].weightlistindex ? 20 * (mdlhdr.numlocalseq - i) : 20 * (mdlhdr.numlocalseq - i) + 4 * actModNumber;
		int stairs = steps + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;

		int outPos = seqIdx + stairs;

		if (v49SeqDesc.eventindex > 0)				v49SeqDesc.eventindex += steps2;
		if (v49SeqDesc.animindexindex > 0)			v49SeqDesc.animindexindex += steps2;
		if (v49SeqDesc.movementindex > 0)			v49SeqDesc.movementindex += steps2;
		if (v49SeqDesc.autolayerindex > 0)			v49SeqDesc.autolayerindex += steps2;
		if (v49SeqDesc.weightlistindex > 0)			v49SeqDesc.weightlistindex += stairs1;
		if (v49SeqDesc.posekeyindex > 0)			v49SeqDesc.posekeyindex += steps2;
		if (v49SeqDesc.iklockindex > 0)				v49SeqDesc.iklockindex += steps2;
		if (v49SeqDesc.keyvalueindex > 0)			v49SeqDesc.keyvalueindex += steps2;
		if (v49SeqDesc.cycleposeindex > 0)			v49SeqDesc.cycleposeindex += steps2;
		if (v49SeqDesc.activitymodifierindex > 0)	v49SeqDesc.activitymodifierindex += steps2;

		int compressedIkRuleIdx = 0; //if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = ( basePtr - ( basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules) ) ) * -1 ;


		mstudioseqdesc_t_v53 v53SeqDesc{ -outPos, v49SeqDesc.szlabelindex + stairs, v49SeqDesc.szactivitynameindex + stairs, v49SeqDesc.flags, v49SeqDesc.activity, v49SeqDesc.actweight, v49SeqDesc.numevents, v49SeqDesc.eventindex, v49SeqDesc.bbmin, v49SeqDesc.bbmax, v49SeqDesc.numblends, v49SeqDesc.animindexindex, v49SeqDesc.movementindex, v49SeqDesc.groupsize[0], v49SeqDesc.groupsize[1], v49SeqDesc.paramindex[0], v49SeqDesc.paramindex[1], v49SeqDesc.paramstart[0], v49SeqDesc.paramstart[1], v49SeqDesc.paramend[0] , v49SeqDesc.paramend[1], v49SeqDesc.paramparent, v49SeqDesc.fadeintime, v49SeqDesc.fadeouttime, v49SeqDesc.localentrynode, v49SeqDesc.localexitnode, v49SeqDesc.nodeflags, v49SeqDesc.entryphase, v49SeqDesc.exitphase, v49SeqDesc.lastframe, v49SeqDesc.nextseq, v49SeqDesc.pose, v49SeqDesc.numikrules, v49SeqDesc.numautolayers, v49SeqDesc.autolayerindex, v49SeqDesc.weightlistindex, v49SeqDesc.posekeyindex, v49SeqDesc.numiklocks, v49SeqDesc.iklockindex, v49SeqDesc.keyvalueindex, v49SeqDesc.keyvaluesize, v49SeqDesc.cycleposeindex, v49SeqDesc.activitymodifierindex, v49SeqDesc.numactivitymodifiers, 0,0,0,0,0,0,0,0,0,0 };

		v53SeqDescs.push_back(v53SeqDesc);
		actModNumber += v49SeqDesc.numactivitymodifiers;
	}
	return v53SeqDescs;
}

std::vector<mstudioactivitymodifier_t_v53> MDL::v49Mdl::ActivityModifierConversion()
{
	std::vector<mstudioactivitymodifier_t_v53>	v53ActivityModifiers;
	for (int i = 0; i < activitymodifiers.size(); i++)
	{
		int unk = 0;
		int stairs = 4 * (activitymodifiers.size() - i) + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller;
		if (activitymodifiers[i].sznameindex > 0) activitymodifiers[i].sznameindex += stairs;

		mstudioactivitymodifier_t_v53 activityModifier = { activitymodifiers[i].sznameindex, unk };
		v53ActivityModifiers.push_back(activityModifier);
	}
	return v53ActivityModifiers;
}

std::vector<mstudioikrule_t_v53> MDL::v49Mdl::IkRuleConversion()
{
	std::vector<mstudioikrule_t_v53> v53IkRules;
	int numIkRules = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];
		if (v49AnimDesc.numikrules > 0)
		{
			for (int j = 0; j < v49AnimDesc.numikrules; j++)
			{
				int ikStairs = -12 * (v49AnimDesc.numikrules - j);

				mstudioikrule_t_v49 v49IkRule = ikrules[numIkRules];
				numIkRules++;

				if (v49IkRule.compressedikerrorindex > 0) v49IkRule.compressedikerrorindex += ikStairs;
				if (v49IkRule.ikerrorindex > 0) v49IkRule.ikerrorindex += ikStairs;
				if (v49IkRule.szattachmentindex > 0) v49IkRule.szattachmentindex += ikStairs;

				mstudioikrule_t_v53 ikRule = { v49IkRule.index, ikruletype(v49IkRule.type), v49IkRule.chain, v49IkRule.bone, v49IkRule.slot, v49IkRule.height, v49IkRule.radius, v49IkRule.floor, v49IkRule.pos, v49IkRule.q.one, v49IkRule.q.i, v49IkRule.q.j, v49IkRule.q.k, v49IkRule.compressedikerrorindex, v49IkRule.iStart, v49IkRule.ikerrorindex, v49IkRule.start, v49IkRule.peak, v49IkRule.tail, v49IkRule.end, v49IkRule.contact, v49IkRule.drop, v49IkRule.top, v49IkRule.szattachmentindex, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

				v53IkRules.push_back(ikRule);
			}
		}
	}
	return v53IkRules;
}

std::vector<mstudioanim_t_v53> MDL::v49Mdl::ConvertAnims()
{
	std::vector<mstudioanim_t_v49> _anims = anims;
	std::vector<mstudioanim_t_v53> v53Anims;
	mstudiolinearbonedata_t_v49 lbd = linearbonedata;

	for (int i = 0; i < anims.size(); i++)
	{

		bool hasAnimPosPtr =	(int)anims[i].flags & STUDIO_ANIM_ANIMPOS;
		bool hasAnimRotPtr =	(int)anims[i].flags & STUDIO_ANIM_ANIMROT;
		bool hasRawPos =		(int)anims[i].flags & STUDIO_ANIM_RAWPOS;
		bool hasRawRot =		(int)anims[i].flags & STUDIO_ANIM_RAWROT;
		bool hasRawRot2 =		(int)anims[i].flags & STUDIO_ANIM_RAWROT2;


		float _posscale = 0; // does what posscale is used for

		std::byte _bone; // unsigned byte, bone limit exceeds 128 so has to be. also means max bones is 255.
		std::byte _flags;

		int16_t _unk = 0; // normally null data


		QuaternionShort _rawrot = { 0,0,0, 0 }; // pQuat64

		mstudioanim_valueptr_t_v49 _animrot = { 0,0,0 }; // pRotV
		int16_t _unused = 0; // pretty sure


		Vector3Short _rawpos = { 0,0,0 }; // pPos
		mstudioanim_valueptr_t_v49 _animpos = { 0,0,0 }; // pPosV

		Vector3Short _rawscale = { 15360, 15360, 15360 }; // new in v53
		mstudioanim_valueptr_t_v49 _animscale = { 0,0,0 };

		int _nextOffset = (int)anims[i].nextoffset > 0 ? (int)anims[i].nextoffset + (32 - GetAnimHeaderSize((int)anims[i].flags)) : 0;

		int bone = (int)anims[i].bone;
		_bone = anims[i].bone;
		_flags = (std::byte)ConvertFlag((int)anims[i].flags);

		if (hasAnimPosPtr && hasAnimRotPtr) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
		if ((int)anims[i].flags & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr = anims[i].animrot.offset;
			if (!hasAnimPosPtr)
			{
				rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
				anims[i].animrot.offset = rotPtr;
			}
			else
			{
				rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
				anims[i].animrot.offset = rotPtr;
			}
			_animrot = anims[i].animrot;

		}

		if ((int)anims[i].flags & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr = anims[i].animpos.offset;
			if (!hasAnimRotPtr)
			{
				posPtr.x > 0 ? posPtr.x += 0 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 0 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 0 : posPtr.z = 0;
				anims[i].animpos.offset = posPtr;
			}
			else
			{
				posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
				anims[i].animpos.offset = posPtr;
			}
			_animpos = anims[i].animpos;
		}

		if ((int)anims[i].flags & STUDIO_ANIM_RAWROT) _rawrot = anims[i].rawrot;

		if ((int)anims[i].flags & STUDIO_ANIM_RAWROT2) _rawrot = anims[i].rawrot2;

		if ((int)anims[i].flags & STUDIO_ANIM_RAWPOS) _rawpos = anims[i].rawpos;
		else
			_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z) };

		mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, anims[i].animdata };
		v53Anims.push_back(v53Anim);
	}
	return v53Anims;
}

std::vector<sectionindexes_t_v53> MDL::v49Mdl::ConvertSectionIndexes()
{
	std::vector<sectionindexesindex_t_v49> _sectionIndexes = sectionindexes;
	std::vector<sectionindexes_t_v53> v53SectionIndexes;
	int secNum = 0;
	int secSetNum = 0;
	for (int i = 0; i < mdlhdr.numlocalanim; i++)
	{
		int pos = mdlhdr.localanimindex + 100 * i;
		int stairs = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i] - 8;

		int frames = animdescs[i].numframes;
		int secFrames = animdescs[i].sectionframes;

		if (animdescs[i].sectionindex > 0)
		{
			int num = (frames / secFrames) + 2;
			secSetNum += num;

			int dist = ( ( (pos + animdescs[i].sectionindex) + (8 * num) ) - (pos + animdescs[i].animindex) ) * -1;
			//Logger::Info("Dist: %d\n", dist);

			for (int j = 0; j < num; j++)
			{
				int steps = -4 * secSetNum;
				int offset = sectionindexes[secNum].sectionoffset + stairs + secHdrBytesSecAdd[secNum] + steps;

				sectionindexes_t_v53 v53SectionIndex{ offset };
				v53SectionIndexes.push_back(v53SectionIndex);
				secNum++;
			}
		}
	}
	return v53SectionIndexes;
}

std::vector<mstudioanim_t_v53> MDL::v49Mdl::ConvertSections()
{
	std::vector<mstudioanim_t_v49> _sections = sections;
	std::vector<mstudioanim_t_v53> v53Sections;
	mstudiolinearbonedata_t_v49 lbd = linearbonedata;

	for (int i = 0; i < sections.size(); i++)
	{


		bool hasAnimPosPtr =	(int)sections[i].flags & STUDIO_ANIM_ANIMPOS;
		bool hasAnimRotPtr =	(int)sections[i].flags & STUDIO_ANIM_ANIMROT;
		bool hasRawPos =		(int)sections[i].flags & STUDIO_ANIM_RAWPOS;
		bool hasRawRot =		(int)sections[i].flags & STUDIO_ANIM_RAWROT;
		bool hasRawRot2 =		(int)sections[i].flags & STUDIO_ANIM_RAWROT2;

		float _posscale = 0; // does what posscale is used for

		std::byte _bone; // unsigned byte, bone limit exceeds 128 so has to be. also means max bones is 255.
		std::byte _flags;

		int16_t _unk = 0; // normally null data


		QuaternionShort _rawrot = { 0,0,0, 0 }; // pQuat64

		mstudioanim_valueptr_t_v49 _animrot = { 0,0,0 }; // pRotV
		int16_t _unused = 0; // pretty sure


		Vector3Short _rawpos = { 0,0,0 }; // pPos
		mstudioanim_valueptr_t_v49 _animpos = { 0,0,0 }; // pPosV

		Vector3Short _rawscale = { 15360, 15360, 15360 }; // new in v53
		mstudioanim_valueptr_t_v49 _animscale = { 0,0,0 };

		int _nextOffset = (int)sections[i].nextoffset > 0 ? (int)sections[i].nextoffset + (32 - GetAnimHeaderSize((int)sections[i].flags)) : 0;

		int bone = (int)sections[i].bone;
		_bone = sections[i].bone;
		_flags = (std::byte)ConvertFlag((int)sections[i].flags);

		if ((int)sections[i].flags == 12) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
		if ((int)sections[i].flags & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr = sections[i].animrot.offset;
			if (!hasAnimPosPtr)
			{
				rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
				sections[i].animrot.offset = rotPtr;
			}
			else
			{
				rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
				sections[i].animrot.offset = rotPtr;
			}
			_animrot = sections[i].animrot;

		}

		if ((int)sections[i].flags & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr = sections[i].animpos.offset;
			if (!hasAnimRotPtr)
			{
				posPtr.x > 0 ? posPtr.x += 18 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 18 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 18 : posPtr.z = 0;
				sections[i].animpos.offset = posPtr;
			}
			else
			{
				posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
				sections[i].animpos.offset = posPtr;
			}
			_animpos = sections[i].animpos;
		}

		if (hasRawRot) _rawrot = sections[i].rawrot;

		if (hasRawRot2) _rawrot = sections[i].rawrot2;

		if (hasRawPos) _rawpos = sections[i].rawpos;
		else
			_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z) };

		mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, sections[i].animdata };
		v53Sections.push_back(v53Anim);
	}
	return v53Sections;
}

std::vector<mstudioikchain_t_v53> MDL::v49Mdl::IkChainConversion()
{
	std::vector<mstudioikchain_t_v53> v53IkChains;
	for (int i = 0; i < ikchains.size(); i++)
	{
		int stairs = 16 * (ikchains.size() - i);
		int offset = bytesAddedToTextures + textureFiller + strFiller + stairs + bytesAddedToRuiMesh;
		float unk = (float)0.707;
		if (ikchains[i].sznameindex > 0) ikchains[i].sznameindex += offset;
		if (ikchains[i].linkindex > 0) ikchains[i].linkindex += stairs;

		mstudioikchain_t_v53 ikChain = { ikchains[i].sznameindex, ikchains[i].linktype, ikchains[i].numlinks, ikchains[i].linkindex, unk, 0, 0, 0 };
		v53IkChains.push_back(ikChain);
	}
	return v53IkChains;
}

void MDL::v49Mdl::AttachmentConversion()
{
	int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < attachments.size(); i++)
	{
		attachments[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::SeqEventConversion()
{
	int actModNum = 0;
	int eventNum = 0;
	for (int i = 0; i < seqdescs.size(); i++)
	{
		for (int j = 0; j < seqdescs[i].numevents; j++)
		{
			int stairs = 4 * (activitymodifiers.size() - actModNum) + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller;
			if (events[eventNum].szeventindex > 0) events[eventNum].szeventindex += stairs;
			eventNum++;
		}
		if (seqdescs[i].numactivitymodifiers > 0) actModNum += seqdescs[i].numactivitymodifiers;
	}
}

void MDL::v49Mdl::ConvertNodeNames()
{
	for (int i = 0; i < nodenames.size(); i++)
	{
		if (nodenames[i].sznameindex > 0) nodenames[i].sznameindex += allBytesAdded + strFiller + bytesAddedToRuiMesh;
	}
}

void MDL::v49Mdl::ConvertBodyParts()
{
	int stairs = bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < bodyparts.size(); i++)
	{
		if (bodyparts[i].sznameindex > 0) bodyparts[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::ConvertModels()
{
	for (int i = 0; i < models.size(); i++)
	{
		models[i].eyeballindex = 0;
		models[i].unused[0] = 0;
		models[i].unused[1] = 0;
		if (mdlhdr.numikchains > 0)
		{
			if (models[i].meshindex > 0) models[i].meshindex += bytesAddedToIkChains;
			if (models[i].attachmentindex > 0) models[i].attachmentindex += bytesAddedToIkChains;
		}
	}
}

void MDL::v49Mdl::ConvertMeshes()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		if (mdlhdr.numikchains > 0)meshes[i].modelindex -= bytesAddedToIkChains;
		else break;
	}
}

void MDL::v49Mdl::ConvertPoseParamDescs()
{
	int stairs = bytesAddedToTextures + textureFiller + strFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < poseparamdescs.size(); i++)
	{
		if (poseparamdescs[i].sznameindex > 0) poseparamdescs[i].sznameindex += stairs;
	}
}

void MDL::v49Mdl::ConvertIncludeModel()
{
	int stairs = bytesAddedToTextures + textureFiller + strFiller + bytesAddedToRuiMesh;
	for (int i = 0; i < includedmodels.size(); i++)
	{
		if (includedmodels[i].sznameindex > 0) includedmodels[i].sznameindex += stairs;
	}
}

std::vector<mstudiotexture_t_v53> MDL::v49Mdl::ConvertTextures()
{
	std::vector<mstudiotexture_t_v53> v53Textures;

	for (int i = 0; i < textures.size(); i++)
	{
		int stairs = -((20) * (textures.size() - i));
		textures[i].sznameindex += stairs + strFiller;

		mstudiotexture_t_v53 texture = { textures[i].sznameindex, textures[i].flags, textures[i].used, textures[i].unused1, 0,0,0,0,0,0,0 };
		v53Textures.push_back(texture);
	}

	return v53Textures;
}

void MDL::v49Mdl::ConvertCDTextures()
{
	for (int i = 0; i < cdtextures.size(); i++)
	{
		cdtextures[i].sznameindex += allBytesAdded + strFiller + bytesAddedToRuiMesh;;
	}
}

void MDL::v49Mdl::ConvertSrcBoneTransforms()
{
	for (int i = 0; i < srcbonetransforms.size(); i++)
	{
		srcbonetransforms[i].sznameindex += strFiller;
	}
}

void MDL::v49Mdl::UpdateMdl()
{
	if (attachments.size() > 0) AttachmentConversion();
	if (events.size() > 0) SeqEventConversion();
	if (mdlhdr.numlocalnodes > 0) ConvertNodeNames();
	if (mdlhdr.numbodyparts > 0) ConvertBodyParts();
	if (models.size() > 0) ConvertModels();
	if (meshes.size() > 0) ConvertMeshes();
	if (mdlhdr.numlocalposeparameters > 0) ConvertPoseParamDescs();
	if (mdlhdr.numincludemodels > 0) ConvertIncludeModel();
	if (mdlhdr.numcdtextures > 0) ConvertCDTextures();
	if (mdlsubhdr.numsrcbonetransform > 0) ConvertSrcBoneTransforms();
}