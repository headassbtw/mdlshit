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

using namespace rapidjson;

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

std::vector<float> CreateQuad(std::vector<std::vector<float>> tris)
{
	return tris[0];
}


bool IsWhole(double n) {
	if (n == (int)n)
		return true;
	else
		return false;
}



mstudioruimesh_t CreateRuiMesh(ruiRecipe recipe)
{
	mstudioruimesh_t* ruiMesh = new mstudioruimesh_t();
	int numOfVerts = 0;
	ruiMesh->numparents = 1;
	ruiMesh->numfaces = recipe.faces.size();
	ruiMesh->szruimeshname = recipe.name;
	ruiMesh->parent.push_back(recipe.parent);

	for (int i = 0; i < ruiMesh->numfaces; i++)
	{
		mstudioruimesface_t faceData{recipe.faces[i].uvMin[0], recipe.faces[i].uvMin[1], recipe.faces[i].uvMax[0], recipe.faces[i].uvMax[1], recipe.faces[i].faceScaleMin[0], recipe.faces[i].faceScaleMin[1], recipe.faces[i].faceScaleMax[0], recipe.faces[i].faceScaleMax[1] };

		mstudioruivertmap_t vertMap{ 0 + numOfVerts, 2 + numOfVerts, 3 + numOfVerts };
		ruiMesh->vertexmap.push_back(vertMap);
		mstudioruivert_t vert1{ 0, recipe.faces[i].one.x, recipe.faces[i].one.y, recipe.faces[i].one.z };
		mstudioruivert_t vert2{ 0, recipe.faces[i].two.x, recipe.faces[i].two.y, recipe.faces[i].two.z };
		mstudioruivert_t vert3{ 0, recipe.faces[i].three.x, recipe.faces[i].three.y, recipe.faces[i].three.z };
		mstudioruivert_t vert4{ 0, recipe.faces[i].four.x, recipe.faces[i].four.y, recipe.faces[i].four.z };
		ruiMesh->vertex.push_back(vert1);
		numOfVerts++;
		ruiMesh->vertex.push_back(vert2);
		numOfVerts++;
		ruiMesh->vertex.push_back(vert3);
		numOfVerts++;

		if (recipe.faces[i].four.x != 0 && recipe.faces[i].four.y != 0 && recipe.faces[i].four.z != 0)
		{
			ruiMesh->vertex.push_back(vert4);
			numOfVerts++;
		}
		ruiMesh->numvertices = recipe.faces[i].numOfVerts;
		Logger::Info("verts: %d\n", recipe.faces[i].numOfVerts);
		ruiMesh->facedata.push_back(faceData);
	}

	mstudioruimesh_t _ruiMesh{ ruiMesh->numparents, ruiMesh->numvertices, ruiMesh->numfaces, 0, 0, 0, 0, (std::byte)0, (std::byte)0, (std::byte)0, (std::byte)0, ruiMesh->szruimeshname, ruiMesh->parent, ruiMesh->vertexmap, ruiMesh->vertex, ruiMesh->facedata };

	return _ruiMesh;

}


Face HandleFace(rapidjson::Value& value)
{
	std::vector<std::vector<float>> tris;
	int i = 0;
	std::vector<float> _tris;
	Face* face = new Face();
	int numOfVerts = 0;

	if (value.HasMember("uvMin") && value["uvMin"].IsArray())
	{
		for (auto& it : value["uvMin"].GetArray()) // Now we setup the second TextureGUID Map.
		{
			if (it.GetStdString() != "")
			{
				Logger::Info("uvMin: %f\n", stof(it.GetStdString()));
				face->uvMin[i] = stof(it.GetStdString());
				i++;
			}
		}
		i = 0;
	}

	if (value.HasMember("uvMax") && value["uvMax"].IsArray())
	{
		for (auto& it : value["uvMax"].GetArray()) // Now we setup the second TextureGUID Map.
		{
			if (it.GetStdString() != "")
			{
				Logger::Info("uvMax: %f\n", stof(it.GetStdString()));
				face->uvMax[i] = stof(it.GetStdString());
				i++;
			}
		}
		i = 0;
	}

	if (value.HasMember("faceScaleMin") && value["faceScaleMin"].IsArray())
	{
		for (auto& it : value["faceScaleMin"].GetArray()) // Now we setup the second TextureGUID Map.
		{
			if (it.GetStdString() != "")
			{
				Logger::Info("FaceScaleMax: %f\n", stof(it.GetStdString()));
				face->faceScaleMin[i] = stof(it.GetStdString());
				i++;
			}
		}
		i = 0;
	}

	if (value.HasMember("faceScaleMax") && value["faceScaleMax"].IsArray())
	{
		for (auto& it : value["faceScaleMax"].GetArray()) // Now we setup the second TextureGUID Map.
		{
			if (it.GetStdString() != "")
			{
				Logger::Info("FaceScaleMax: %f\n", stof(it.GetStdString()));
				face->faceScaleMax[i] = stof(it.GetStdString());
				i++;
			}
		}
		i = 0;
	}


	if (value.HasMember("verts") && value["verts"].IsArray())
	{
		for (auto& it : value["verts"].GetArray()) // Now we setup the second TextureGUID Map.
		{
			if (it.GetStdString() != "")
			{
				//Logger::Info("PassedJson3\n");
				Logger::Info("vertFloat: %f\n", stof(it.GetStdString()));

				if (i < 9) _tris.push_back(stof(it.GetStdString()));

				i++;
				if (i > 8)
				{
					i = 0;
					tris.push_back(_tris);
					_tris.clear();
				}
			}
		}
		Logger::Info("vertCount: %d\n", value["verts"].GetArray().Size() / 3);
		Logger::Info("trisCount: %d\n", tris.size());

		for (int j = 0; j < tris.size(); j++)
		{
			face->one.x = tris[j][0];
			face->one.y = tris[j][1];
			face->one.z = tris[j][2];
			face->two.x = tris[j][6];
			face->two.y = tris[j][7];
			face->two.z = tris[j][8];
			face->three.x = tris[j][3];
			face->three.y = tris[j][4];
			face->three.z = tris[j][5];
			if (j + 1 < tris.size())
			{
				face->four.x = tris[j + 1][0];
				face->four.y = tris[j + 1][1];
				face->four.z = tris[j + 1][2];
			}
		}
		Face _face{ face->uvMin[0], face->uvMin[1], face->uvMax[0], face->uvMax[1], face->faceScaleMin[0], face->faceScaleMin[1], face->faceScaleMax[0], face->faceScaleMax[1], face->one.x, face->one.y, face->one.z, face->two.x, face->two.y, face->two.z, face->three.x, face->three.y, face->three.z, face->four.x, face->four.y, face->four.z, (value["verts"].GetArray().Size() / 3) - (2 * (tris.size() / 2)), tris.size()};
		return _face;
	}
}

mstudioruimesh_t HandleJsonRUI(rapidjson::Value& value)
{
		std::vector<std::vector<float>> tris;
		std::string _name = "";
		std::vector<Face> faces;
		std::vector<mstudioruimesh_t> ruiMeshes;
		int _parent = 0;

		int i = 0;
		std::vector<float> _tris;

		if (value.HasMember("name") && value["name"].IsString())
		{
			if (value["name"].GetStdString() != "")
			{
				Logger::Info("Name: %s\n", value["name"].GetStdString().c_str());
				_name = value["name"].GetStdString();
			}
		}

		if (value.HasMember("parent") && value["parent"].IsString())
		{
			if (value["parent"].GetStdString() != "")
			{
				Logger::Info("Parent: %d\n", stoi(value["parent"].GetStdString()));
				_parent = stoi(value["parent"].GetStdString());
			}
		}

		if (value.HasMember("faces") && value["faces"].IsArray())
		{
			Logger::Info("PassedJson2\n");
			for (auto& it : value["faces"].GetArray())
			{
				faces.push_back(HandleFace(it));
			}
		}

		ruiRecipe recipe{ _name, _parent, faces };

		return CreateRuiMesh(recipe);
}

std::vector<mstudioruimesh_t> GetRuiMeshes(FileInfo info)
{
	std::vector<mstudioruimesh_t> ruiMeshes;
	char** argv;
	std::ifstream ifs(info.str.value().c_str());

	// begin json parsing
	rapidjson::IStreamWrapper isw{ ifs };

	rapidjson::Document doc{};

	doc.ParseStream(isw);

	if (doc.HasMember("rui") && doc["rui"].IsArray())
	{
		for (auto& file : doc["rui"].GetArray())
		{
			Logger::Info("PassedJson1\n");
			ruiMeshes.push_back(HandleJsonRUI(file));
		}
	}

	return ruiMeshes;

}

int GetRuiBytesAdded(std::vector<mstudioruimesh_t> ruiMeshes)
{
	int bytesAdded = 0;
	for (int i = 0; i < ruiMeshes.size(); i++)
	{
		bytesAdded += 32;
		bytesAdded += 2 * ruiMeshes[i].numparents;
		bytesAdded += 32 * ruiMeshes[i].numfaces;
		bytesAdded += 16 * ruiMeshes[i].numvertices;
		bytesAdded += 6 * ruiMeshes[i].numfaces;
		bytesAdded += ruiMeshes[i].szruimeshname.size();

		double sizeCheck = bytesAdded / 16.0;
		int idk = 0;
		if (!IsWhole(sizeCheck))
		{
			double sizeCheck2 = 16 * ((int)sizeCheck + 1);
			idk = sizeCheck2 - bytesAdded;
			bytesAdded += idk;
		}

		bytesAdded += 8;

	}

	return bytesAdded;
}

int GetRuiBytesAddedIdv(std::vector<mstudioruimesh_t> ruiMeshes, int num)
{
	int bytesAdded = 0;
	bytesAdded += 32;
	bytesAdded += 2 * ruiMeshes[num].numparents;
	bytesAdded += 32 * ruiMeshes[num].numfaces;
	bytesAdded += 16 * ruiMeshes[num].numvertices;
	bytesAdded += 6 * ruiMeshes[num].numfaces;
	bytesAdded += ruiMeshes[num].szruimeshname.size();
	return bytesAdded;
}

std::vector<int> GetRuiBytesAddedPer(std::vector<mstudioruimesh_t> ruiMeshes)
{
	int bytesAdded = 0;
	std::vector<int> bytesAddedPerRui;
	for (int i = 0; i < ruiMeshes.size(); i++)
	{
		bytesAddedPerRui.push_back(bytesAdded);

		bytesAdded += 32;
		bytesAdded += 2 * ruiMeshes[i].numparents;
		bytesAdded += 32 * ruiMeshes[i].numfaces;
		bytesAdded += 16 * ruiMeshes[i].numvertices;
		bytesAdded += 6 * ruiMeshes[i].numfaces;
		bytesAdded += ruiMeshes[i].szruimeshname.size();

		double sizeCheck = bytesAdded / 16.0;
		int idk = 0;
		if (!IsWhole(sizeCheck))
		{
			double sizeCheck2 = 16 * ((int)sizeCheck + 1);
			idk = sizeCheck2 - bytesAdded;
			bytesAdded += idk;
		}

		bytesAdded += 8;

	}
	return bytesAddedPerRui;
}



LinearBoneData* GetLinearBoneData(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int bone)
{
	LinearBoneData* boneData = new LinearBoneData();
	int pos = Stream->Position();
	if (bone < Initial_Header->numbones)
	{
		int filler = 4 * 6; 
		Stream->seek(Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex);
		int idx = Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex;
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
		//Logger::Info("idx: %d %d\n", Initial_Header->studiohdr2index + Initial_Header_Part2->linearboneindex);
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

uint16_t float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
	const uint32_t b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
	const uint32_t e = (b & 0x7F800000) >> 23; // exponent
	const uint32_t m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
	return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}

mstudiobone_t_v53 BoneConversion(mstudiobone_t_v49 v49Bone, int stairs)
{
	Vector3 unkvector{ 1,1,1 };

	Vector3 unkvector1{ 0.0004882813 ,0.0004882813 ,0.0004882813 };

	int unkindex = 0;

	Vector3 posscale{ 0,0,0 };

	if (v49Bone.procindex > 0) v49Bone.procindex += stairs;
	if (v49Bone.surfacepropidx > 0) v49Bone.surfacepropidx += stairs;
	if (v49Bone.surfacepropLookup > 0) v49Bone.surfacepropLookup += stairs;

	mstudiobone_t_v53 bone{ v49Bone.sznameindex + stairs, v49Bone.parent, -1,-1,-1,-1,-1,-1, v49Bone.pos, v49Bone.quat, v49Bone.rot, unkvector, posscale, v49Bone.rotscale, unkvector1, v49Bone.poseToBone, v49Bone.qAlignment, v49Bone.flags, v49Bone.proctype, v49Bone.procindex, v49Bone.physicsbone, v49Bone.surfacepropidx, v49Bone.contents, v49Bone.surfacepropLookup, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	return bone;
}

mstudiobbox_tv53 HitboxConversion(mstudiobbox_t_v49 v49Hitbox, int stairs)
{
	
	mstudiobbox_tv53 hitbox{ v49Hitbox.bone, v49Hitbox.group, v49Hitbox.bbmin, v49Hitbox.bbmax, v49Hitbox.szhitboxnameindex + stairs, 0, 0, 0,0,0,0,0,0 };

	return hitbox;
}

bool contains(std::vector<int> arry, int trgt)
{
	for (int i = 0; i < arry.size(); i++)
	{
		if (trgt == arry[i]) return true;
	}
	return false;
}

std::vector<int> GetIkChainBones(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	std::vector<int> ikChainBones;
	std::vector<int> ikChainBonesChildren;
	std::vector<int> ikChainBonesFinal;
	int numOfIkLinks = 0;
	Stream->seek(Initial_Header->ikchainindex);

	for (int i = 0; i < Initial_Header->numikchains; i++)
	{
		mstudioikchain_t_v49 ikChain; Stream->Read(&ikChain);
		if (ikChain.numlinks > 0) numOfIkLinks += ikChain.numlinks;
	}

	for (int i = 0; i < numOfIkLinks; i++)
	{
		mstudioiklink_t_v49 ikLink; Stream->Read(&ikLink);
		ikChainBones.push_back(ikLink.bone);
	}

	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++)
	{
		mstudiobone_t_v49 bone; Stream->Read(&bone);
		for (int j = 0; j < ikChainBones.size(); j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, ikChainBones[j]) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBones[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	int size = ikChainBonesChildren.size();
	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++)
	{
		mstudiobone_t_v49 bone; Stream->Read(&bone);
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}
	size = ikChainBonesChildren.size();
	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++)
	{
		mstudiobone_t_v49 bone; Stream->Read(&bone);
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++)
	{
		mstudiobone_t_v49 bone; Stream->Read(&bone);
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++)
	{
		mstudiobone_t_v49 bone; Stream->Read(&bone);
		for (int j = 0; j < size; j++)
		{
			if (!contains(ikChainBones, i) && !contains(ikChainBonesChildren, i) && bone.parent == ikChainBonesChildren[j])
			{
				ikChainBonesChildren.push_back(i);
			}
		}
	}

	size = ikChainBonesChildren.size();
	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++)
	{
		mstudiobone_t_v49 bone; Stream->Read(&bone);
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

int GetAnimBoneHeaderBytesAddedIndv(BinaryReader* Stream, v49_Header* Initial_Header, int anim, bool debug)
{
	int pos = Stream->Position();
	int bytesAdded = 0;
	int bytesAddedPer = 0;
	int strPos = Initial_Header->localanimindex + 100 * anim;
	int animPos = strPos + 56;
	int secIndexPos = strPos + 80;
	Stream->seek(animPos);
	int animIndex; Stream->Read(&animIndex);
	Stream->seek(secIndexPos);
	int sectionIndex; Stream->Read(&sectionIndex);
	Stream->seek(strPos + animIndex);

	if (sectionIndex == 0)
	{
		for (int j = 0; j < 100000; j++)
		{
			int pos2 = Stream->Position();
			std::byte bone; Stream->Read(&bone); 
			std::byte flag; Stream->Read(&flag); 
			short nextOffset; Stream->Read(&nextOffset);
			int headerSize = GetAnimHeaderSize((int)flag);
			bytesAddedPer += (32 - headerSize);
			bytesAdded += (32 - headerSize);
			if (nextOffset == 0)
			{
				break;
			}
			Stream->seek(pos2 + nextOffset);

		}
	}
	Stream->seek(pos);
	return bytesAdded;
}

mstudioanimdesc_tv53 AnimDescConversion( mstudioanimdesc_t_v49 v49AnimDesc, int stairs, int stairs2, int stairs3, int basePtr)
{
	if (v49AnimDesc.animindex > 0) v49AnimDesc.animindex += stairs + stairs3;
	if (v49AnimDesc.movementindex > 0) v49AnimDesc.movementindex += stairs + stairs3;
	if (v49AnimDesc.ikruleindex > 0) v49AnimDesc.ikruleindex += stairs + stairs3;
	if (v49AnimDesc.localhierarchyindex > 0) v49AnimDesc.localhierarchyindex += stairs + stairs3;
	if (v49AnimDesc.zeroframeindex > 0) v49AnimDesc.zeroframeindex += stairs + stairs3;
	if (v49AnimDesc.ikrulezeroframeindex > 0) v49AnimDesc.ikrulezeroframeindex += stairs + stairs3;
	if (v49AnimDesc.sectionindex > 0) v49AnimDesc.sectionindex += stairs + stairs3;
	int compressedIkRuleIdx = 0; if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules);


	mstudioanimdesc_tv53 animDesc = { basePtr, v49AnimDesc.sznameindex + stairs2 + stairs3, v49AnimDesc.fps, v49AnimDesc.flags, v49AnimDesc.numframes, v49AnimDesc.nummovements, v49AnimDesc.movementindex, compressedIkRuleIdx, v49AnimDesc.animindex, v49AnimDesc.numikrules, v49AnimDesc.ikruleindex, v49AnimDesc.numlocalhierarchy, v49AnimDesc.localhierarchyindex, v49AnimDesc.sectionindex, v49AnimDesc.sectionframes, v49AnimDesc.zeroframespan, v49AnimDesc.zeroframecount, v49AnimDesc.zeroframeindex, v49AnimDesc.zeroframestalltime, 0, 0, 0, 0, 0 };
	
	return animDesc;
}

mstudioikrule_tv53 IkRuleConversion(mstudioikrule_t_v49 v49IkRule, int stairs)
{
	if (v49IkRule.compressedikerrorindex > 0) v49IkRule.compressedikerrorindex += stairs;
	if (v49IkRule.ikerrorindex > 0) v49IkRule.ikerrorindex += stairs;
	if (v49IkRule.szattachmentindex > 0) v49IkRule.szattachmentindex += stairs;

	mstudioikrule_tv53 ikRule = {v49IkRule.index, v49IkRule.type, v49IkRule.chain, v49IkRule.bone, v49IkRule.slot, v49IkRule.height, v49IkRule.radius, v49IkRule.floor, v49IkRule.pos, v49IkRule.q, v49IkRule.compressedikerrorindex, v49IkRule.iStart, v49IkRule.ikerrorindex, v49IkRule.start, v49IkRule.peak, v49IkRule.tail, v49IkRule.end, v49IkRule.contact, v49IkRule.drop, v49IkRule.top, v49IkRule.szattachmentindex, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	return ikRule;
}


std::vector<int> GetAnimSectionCount(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int secNum = 0;
	std::vector<int> secPerAnim;
	int pos = Stream->Position();

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
			secPerAnim.push_back(secNumber);
		}
		else secPerAnim.push_back(0);



	}
	Stream->seek(pos);
	return secPerAnim;
}


int GetAnimBoneHeaderTotal(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int hdrNum = 0;
	int* hdrNumPerAnim = new int[Initial_Header->numlocalanim];
	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int bHdrCount = 0;
		int strPos = Initial_Header->localanimindex + 100 * i;
		int animPos = strPos + 56;
		Stream->seek(animPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(strPos + animIndex);

		for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++)
		{
			int pos2 = Stream->Position();
			std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
			std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
			short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
			bHdrCount++;
			hdrNum++;
			if (nextOffset == 0)
			{
				break;
			}
			Stream->seek(pos2 + nextOffset);

		}
		if (debug) Logger::Info("AnimNum: %d, hdrNum:  %d\n", i, bHdrCount);
		hdrNumPerAnim[i] = bHdrCount;


	}
	return hdrNum;
}

std::vector<int> GetAnimBoneHeaderCount(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int hdrNum = 0;
	std::vector<int> hdrNumPerAnim;
	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int bHdrCount = 0;
		int strPos = Initial_Header->localanimindex + 100 * i;
		int animPos = strPos + 56;
		Stream->seek(animPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(strPos + animIndex);

		for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++)
		{
			int pos2 = Stream->Position();
			std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
			std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
			short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
			bHdrCount++;
			hdrNum++;
			if (nextOffset == 0)
			{
				break;
			}
			Stream->seek(pos2 + nextOffset);

		}
		if (debug) Logger::Info("AnimNum: %d, hdrNum:  %d\n", i, bHdrCount);
		hdrNumPerAnim.push_back(bHdrCount);


	}
	return hdrNumPerAnim;
}

int GetAnimBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	int hdrCount = GetAnimBoneHeaderTotal(Stream, Initial_Header, false);

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int strPos = Initial_Header->localanimindex + 100 * i;
		int animPos = strPos + 56;
		int secIndexPos = strPos + 80;

		Stream->seek(animPos + 100);
		int nextAnimIndex; Stream->Read(&nextAnimIndex);
		Stream->seek(secIndexPos + 100);
		int nextSectionIndex; Stream->Read(&nextSectionIndex);

		Stream->seek(animPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(secIndexPos);
		int sectionIndex; Stream->Read(&sectionIndex);
		Stream->seek(strPos + animIndex);
		if (sectionIndex == 0)
		{
			for (int j = 0; j < 1000000; j++)
			{
				int pos2 = Stream->Position();
				std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
				std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
				short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
				int headerSize = GetAnimHeaderSize((int)flag);
				bytesAdded += (32 - headerSize);
				if (nextOffset == 0)
				{
					if (i + 1 < Initial_Header->numlocalanim && nextSectionIndex == 0)
					{
						int dist = ( ( strPos + 100 + nextAnimIndex ) - ( pos2 + headerSize ) );

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < 1000; l++)
							{
								short animValue; Stream->Read(&animValue);
								if (animValue == 0)
								{
									int newPos = Stream->Position() - 2;
									dist = ((strPos + 100 + nextAnimIndex) - (newPos));
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}

						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("start:  %d\n", pos2 + headerSize);
						//Logger::Info("end:  %d\n", strPos + 100 + nextAnimIndex);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}
					if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, bytesAdded);
					break;
				}
				Stream->seek(pos2 + nextOffset);

			}
		}

	}
	return bytesAdded;
}


int GetAnimSectionCountTotal(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
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
			secNum += secNumber;
		}
		else secPerAnim[i] = 0;



	}
	return secNum;
}

std::vector<int> GetAnimSectionBoneHeaderCount(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	int  secTotal = GetAnimSectionCountTotal(Stream, Initial_Header, false);
	int* hdrsPerSec = new int[secTotal];
	std::vector<int> secHdrsPerAnim;
	int secHdrNum = 0;
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

			for (int j = 0; j < secPerAnim[i]; j++)
			{
				//secHdrNum = 0;

				Stream->seek(startPos + sectionIndex);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				for (int k = 0; k < Initial_Header->numbones * Initial_Header->numlocalanim; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					secHdrNum++;
					if (nextOffset == 0)
					{
						Stream->seek(pos2 + headerSize);
						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
			}
			secHdrsPerAnim.push_back(secHdrNum);


		}
		else secHdrsPerAnim.push_back(0);



	}
	return secHdrsPerAnim;

}

int GetAnimSectionBoneHeaderCountTotal(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	int secHdrNum = 0;
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

			for (int j = 0; j < secPerAnim[i]; j++)
			{
				//secHdrNum = 0;

				Stream->seek(startPos + sectionIndex);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				for (int k = 0; k < Initial_Header->numbones * Initial_Header->numlocalanim; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					secHdrNum++;
					if (nextOffset == 0)
					{
						Stream->seek(pos2 + headerSize);
						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
			}
			if (debug) Logger::Info("AnimNum: %d, secHdrNum:  %d\n", i, secHdrNum);
		}



	}
	return secHdrNum;

}

std::vector<int> GetAnimSectionBoneHeaderCountPerSec(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	int  secTotal = GetAnimSectionCountTotal(Stream, Initial_Header, false);
	std::vector<int> hdrsPerSec;
	int* secHdrsPerAnim = new int[secTotal];
	int secNum = 0;
	int test = 0;
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
			int secHdrNum = 0;

			for (int j = 0; j < secPerAnim[i]; j++)
			{
				secHdrNum = 0;

				Stream->seek(startPos + sectionIndex);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				for (int k = 0; k < 100000000; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					secHdrNum++;
					if (nextOffset == 0)
					{
						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
				hdrsPerSec.push_back(secHdrNum);
				if (debug) Logger::Info("SecNum: %d, secHdrNum:  %d\n", j, secHdrNum);
				secNum++;
			}
		}
	}
	return hdrsPerSec;

}

int GetAnimSectionBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	std::vector<int> secHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, false);
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	int secNum = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int startPos = Initial_Header->localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		Stream->seek(animIndexPos + 100);
		int nextAnimIndex; Stream->Read(&nextAnimIndex);
		Stream->seek(secIndexPos + 100);
		int nextSectionIndex; Stream->Read(&nextSectionIndex);

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
			for (int j = 0; j < secPerAnim[i]; j++)
			{
				int bytesAddedPer = 0;
				Stream->seek(startPos + sectionIndex + 8 * j);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				for (int k = 0; k < 1000000; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (nextOffset == 0)
					{
						if (i + 1 < Initial_Header->numlocalanim && j == secPerAnim[i])
						{
							int dist = ((startPos + 100 + nextAnimIndex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < 1000; l++)
								{
									short animValue; Stream->Read(&animValue);
									if (animValue == 0)
									{
										int newPos = Stream->Position() - 2;
										dist = ((startPos + 100 + nextAnimIndex) - (newPos));
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secStart:  %d\n", pos2 + headerSize);
							//Logger::Info("secEnd:  %d\n", startPos + 100 + nextAnimIndex);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}

						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
				secNum++;
			}
		}
	}
	return bytesAdded;
}

std::vector<int> GetAnimSectionBytesAdded2(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	std::vector<int> secHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, false);
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	std::vector<int> bytesAddedPerAnim;
	int secNum = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int startPos = Initial_Header->localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		Stream->seek(animIndexPos + 100);
		int nextAnimIndex; Stream->Read(&nextAnimIndex);
		Stream->seek(secIndexPos + 100);
		int nextSectionIndex; Stream->Read(&nextSectionIndex);

		Stream->seek(framePos);
		int frames; Stream->Read(&frames);
		Stream->seek(secFramPos);
		int secFrames; Stream->Read(&secFrames);

		Stream->seek(animIndexPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(secIndexPos);
		int sectionIndex; Stream->Read(&sectionIndex);
		bytesAddedPerAnim.push_back(bytesAdded);
		if (sectionIndex > 0)
		{
			int num = (frames / secFrames) + 2;

			for (int j = 0; j < num; j++)
			{
				int bytesAddedPer = 0;
				Stream->seek(startPos + sectionIndex + 8 * j);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				for (int k = 0; k < secHdrsPerAnim[i]; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (nextOffset == 0)
					{
						if (i + 1 < Initial_Header->numlocalanim && j + 1 > num )
						{
							int dist = nextSectionIndex > 0 ? ((startPos + 100 + nextSectionIndex) - (pos2 + headerSize)) : ((startPos + 100 + nextAnimIndex) - (pos2 + headerSize));

							if (dist > 18 || dist < 0)
							{
								for (int l = 0; l < 1000; l++)
								{
									short animValue; Stream->Read(&animValue);
									if (animValue == 0)
									{
										int newPos = Stream->Position() - 2;
										nextSectionIndex > 0 ? dist = ((startPos + 100 + nextSectionIndex) - (newPos)) : dist = ((startPos + 100 + nextAnimIndex) - (newPos));
										if (dist <= 18)
										{
											break;
										}
									}
								}
							}
							Logger::Info("secDist:  %d\n", dist);
							Logger::Info("secStart:  %d\n", pos2 + headerSize);
							Logger::Info("secEnd:  %d\n", startPos + 100 + nextAnimIndex);
							Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
				secNum++;
			}
		}
	}
	return bytesAddedPerAnim;
}

std::vector<int> GetAnimSectionBoneHeaderBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	std::vector<int> secHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(Stream, Initial_Header, false);
	std::vector<int> secBytesAddedPerAnim;
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	std::vector<int> secBytesAddedPerSec;
	int secNum = 0;
	int test = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int startPos = Initial_Header->localanimindex + 100 * i;
		int animIndexPos = startPos + 56;
		int secIndexPos = startPos + 80;
		int framePos = startPos + 16;
		int secFramPos = startPos + 84;

		Stream->seek(animIndexPos + 100);
		int nextAnimIndex; Stream->Read(&nextAnimIndex);
		Stream->seek(secIndexPos + 100);
		int nextSectionIndex; Stream->Read(&nextSectionIndex);

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
			secBytesAddedPerAnim.push_back(bytesAdded);
			for (int j = 0; j < secPerAnim[i]; j++)
			{
				int bytesAddedPer = 0;
				Stream->seek(startPos + sectionIndex + 8 * j);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				for (int k = 0; k < secHdrsPerSec[i]; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (nextOffset == 0)
					{
						if (i + 1 < Initial_Header->numlocalanim && nextSectionIndex == 0)
						{
							int dist = ((startPos + 100 + nextAnimIndex) - (pos2 + headerSize));
							//Logger::Info("secDist:  %d\n", dist);
							//Logger::Info("secStart:  %d\n", pos2 + headerSize);
							//Logger::Info("secEnd:  %d\n", startPos + 100 + nextAnimIndex);
							//Logger::Info("secFinalDist:  %d\n", 32 - dist);
							bytesAdded += 32 - dist;
						}
						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
				secNum++;
			}
		}

	}
	return secBytesAddedPerAnim;
}

std::vector<int> GetAnimBoneHeaderBytesAdded2(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	std::vector<int> hdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, false);
	std::vector<int> hdrBytesAddedPerAnim;
	int passedSec = 0;
	int test = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int bytesAddedPer = 0;
		int strPos = Initial_Header->localanimindex + 100 * i;
		int animPos = strPos + 56;
		int secIndexPos = strPos + 80;
		Stream->seek(animPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(secIndexPos);
		int sectionIndex; Stream->Read(&sectionIndex);
		Stream->seek(strPos + animIndex);

		hdrBytesAddedPerAnim.push_back(bytesAdded);
		if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, hdrBytesAddedPerAnim[i]);

		for (int j = 0; j < hdrsPerAnim[i]; j++)
		{
			int pos2 = Stream->Position();
			std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
			std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
			short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
			int headerSize = GetAnimHeaderSize((int)flag);
			bytesAddedPer += (32 - headerSize);
			bytesAdded += (32 - headerSize);
			if (nextOffset == 0)
			{
				break;
			}
			Stream->seek(pos2 + nextOffset);

		}

	}
	return hdrBytesAddedPerAnim;
}


std::vector<int> GetAnimBoneHeaderBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	std::vector<int> hdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, false);
	std::vector<int> hdrBytesAddedPerAnim;
	int passedSec = 0;
	int test = 0;

	for (int i = 0; i < Initial_Header->numlocalanim; i++)
	{
		int bytesAddedPer = 0;
		int strPos = Initial_Header->localanimindex + 100 * i;
		int animPos = strPos + 56;
		int secIndexPos = strPos + 80;

		Stream->seek(animPos + 100);
		int nextAnimIndex; Stream->Read(&nextAnimIndex);
		Stream->seek(secIndexPos + 100);
		int nextSectionIndex; Stream->Read(&nextSectionIndex);

		Stream->seek(animPos);
		int animIndex; Stream->Read(&animIndex);
		Stream->seek(secIndexPos);
		int sectionIndex; Stream->Read(&sectionIndex);
		Stream->seek(strPos + animIndex);
		hdrBytesAddedPerAnim.push_back(bytesAdded);
		if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, hdrBytesAddedPerAnim[i]);

		if (sectionIndex == 0)
		{
			for (int j = 0; j < hdrsPerAnim[i]; j++)
			{
				int pos2 = Stream->Position();
				std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
				std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
				short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
				int headerSize = GetAnimHeaderSize((int)flag);
				bytesAddedPer += (32 - headerSize);
				bytesAdded += (32 - headerSize);
				if (nextOffset == 0)
				{
					if (i + 1 < Initial_Header->numlocalanim && nextSectionIndex == 0)
					{
						int dist = ((strPos + 100 + nextAnimIndex) - (pos2 + headerSize));

						if (dist > 18 || dist < 0)
						{
							for (int l = 0; l < 1000; l++)
							{
								short animValue; Stream->Read(&animValue);
								if (animValue == 0)
								{
									int newPos = Stream->Position() - 2;
									dist = ((strPos + 100 + nextAnimIndex) - (newPos));
									if (dist <= 18)
									{
										break;
									}
								}
							}
						}
						//Logger::Info("dist:  %d\n", dist);
						//Logger::Info("start:  %d\n", pos2 + headerSize);
						//Logger::Info("end:  %d\n", strPos + 100 + nextAnimIndex);
						//Logger::Info("finalDist:  %d\n", 32 - dist);
						bytesAdded += 32 - dist;
					}

					break;
				}
				Stream->seek(pos2 + nextOffset);

			}
		}
		else hdrBytesAddedPerAnim[i] = bytesAdded;

	}
	return hdrBytesAddedPerAnim;
}


std::vector<int> GetAnimSectionBoneHeaderBytesAddedPerSec(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	int bytesAdded = 0;
	int* secBytesAddedPerAnim = new int[Initial_Header->numlocalanim];
	std::vector<int> secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	std::vector<int> secBytesAddedPerSec;
	std::vector<int> secHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(Stream, Initial_Header, false);
	int secNum = 0;
	int test = 0;

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
			int num = (frames / secFrames) + 2;
			for (int j = 0; j < num; j++)
			{
				int bytesAddedPer = 0;
				Stream->seek(startPos + sectionIndex + 8 * j);
				int animBlock; Stream->Read(&animBlock);
				int animOffset; Stream->Read(&animOffset);
				Stream->seek(startPos + animOffset);
				secBytesAddedPerSec.push_back(bytesAdded);
				for (int k = 0; k < 10000000; k++)
				{
					int pos2 = Stream->Position();
					std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
					std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
					short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
					int headerSize = GetAnimHeaderSize((int)flag);
					bytesAddedPer += (32 - headerSize);
					bytesAdded += (32 - headerSize);
					if (nextOffset == 0)
					{
						Stream->seek(pos2 + headerSize);
						//if (debug) Logger::Info("SecNum: %d, secBytesAddedTtl:  %d\n", secNum, bytesAdded);
						//if (debug) Logger::Info("SecNum: %d, secBytesAddePerd:  %d\n", secNum, bytesAddedPer);
						break;
					}
					Stream->seek(pos2 + nextOffset);
				}
				secNum++;
			}
		}
	}
	return secBytesAddedPerSec;
}

std::vector<int> GetBytesAddedPerSeq(BinaryReader* Stream, v49_Header* Initial_Header, SequenceData* sequenceData, bool debug)
{
	std::vector<int> bytesAddedPerSeq;
	int curSeq = 0;
	int numOfActMods = 0;
	int bytesAdded = 0;
	std::vector<int> actMods;
	for (int i = 0; i < Initial_Header->numlocalseq; i++)
	{
		mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];

		if (sequence.numactivitymodifiers > 0)
		{
			actMods.push_back(i);
		}
	}

	for (int i = 0; i < Initial_Header->numlocalseq; i++)
	{
		int num = 0;
		for (int j = 0; j < actMods.size(); j++)
		{
			if (i < actMods[j])
			{
				bytesAdded += 4 * sequenceData->sequences[actMods[j]].numactivitymodifiers;
				num++;
			}
		}
		if (i == actMods[0]) bytesAddedPerSeq.push_back(0);
		if (i != actMods[0] && num > 0)bytesAddedPerSeq.push_back(bytesAdded);
		else if (i != actMods[0] && num == 0)bytesAddedPerSeq.push_back(0);
	}
	return bytesAddedPerSeq;
}

std::vector<int> GetBytesAddedPerSeqIdx(BinaryReader* Stream, v49_Header* Initial_Header, SequenceData* sequenceData, bool debug)
{
	std::vector<int> bytesAddedPerSeq;
	int curSeq = 0;
	int numOfActMods = 0;
	int bytesAdded = 0;
	std::vector<int> actMods;
	for (int i = 0; i < Initial_Header->numlocalseq; i++)
	{
		mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];

		if (sequence.numactivitymodifiers > 0)
		{
			actMods.push_back(i);
		}
	}

	for (int i = 0; i < Initial_Header->numlocalseq; i++)
	{
		int num = 0;
		for (int j = 0; j < actMods.size(); j++)
		{
			if (i > actMods[j])
			{
				bytesAdded += 4 * sequenceData->sequences[actMods[j]].numactivitymodifiers;
				num++;
			}
		}
		if (i == actMods[0]) bytesAddedPerSeq.push_back(0);
		if (i != actMods[0] && num > 0)bytesAddedPerSeq.push_back(bytesAdded);
		else if (i != actMods[0] && num == 0)bytesAddedPerSeq.push_back(0);
	}
	return bytesAddedPerSeq;
}

void ConvertBoneHeader(BinaryReader* Stream, int num, BoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2) //hacky way of converting the headers
{
	int pos2 = Stream->Position();
	std::byte bone; Stream->Read(&bone);
	std::byte flag; Stream->Read(&flag);
	short nextOffset; Stream->Read(&nextOffset);
	short origOffset = nextOffset;
	int strEndPos = Stream->Position();
	nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;

	switch ((int)flag)
	{
	case 12:
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
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
	Stream->seek(pos2 + origOffset);
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

void ReadBoneHeader2(BinaryReader* Stream, BoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int num, int dataSize, bool debug)
{
	int pos2 = Stream->Position();
	std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
	std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
	short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
	int strEndPos = Stream->Position();
	nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;

	LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
	Stream->seek(strEndPos);

	boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
	boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);

	if ((int)flag & STUDIO_ANIM_ANIMROT)
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
	}
	if ((int)flag & STUDIO_ANIM_ANIMPOS)
	{
		Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
		boneHeaders_v53[num].rawPos = posPtr;
	}
	if ((int)flag & STUDIO_ANIM_RAWPOS)
	{
		Vector3Short rawPos; Stream->Read(&rawPos);
		boneHeaders_v53[num].rawPos = rawPos;
	}
	if ((int)flag & STUDIO_ANIM_RAWROT)
	{
		Vector3Short rawRot; Stream->Read(&rawRot);
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.x; boneHeaders_v53[num].packedRotation.i = (short)rawRot.y; boneHeaders_v53[num].packedRotation.j = (short)rawRot.z; boneHeaders_v53[num].packedRotation.k = (short)0;
	}
	if ((int)flag & STUDIO_ANIM_RAWROT2)
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
	}
	strEndPos = Stream->Position();
	if ((int)flag == 12)boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
	else boneHeaders_v53[num].translationScale = 0;
	boneHeaders_v53[num].bone = bone;
	boneHeaders_v53[num].r2Flag = flag;
	boneHeaders_v53[num].flags = 0;
	boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
	boneHeaders_v53[num].nextOffset = nextOffset;
	boneHeaders_v53[num].dataSize = dataSize;
	boneHeaders_v53[num].rawStrPos = strEndPos;

	if ((int)flag == 0)
	{
		strEndPos = Stream->Position();
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)0; boneHeaders_v53[num].rawPos.y = (short)0; boneHeaders_v53[num].rawPos.z = (short)0;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 0; boneHeaders_v53[num].rawScale.y = 0; boneHeaders_v53[num].rawScale.z = 0;
		boneHeaders_v53[num].nextOffset = 0;
		boneHeaders_v53[num].dataSize = 0;
		boneHeaders_v53[num].rawStrPos = strEndPos;
	}
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
	case 0:
	{
		strEndPos = Stream->Position();
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)0; boneHeaders_v53[num].rawPos.y = (short)0; boneHeaders_v53[num].rawPos.z = (short)0;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 0; boneHeaders_v53[num].rawScale.y = 0; boneHeaders_v53[num].rawScale.z = 0;
		boneHeaders_v53[num].nextOffset = 0;
		boneHeaders_v53[num].dataSize = 0;
		boneHeaders_v53[num].rawStrPos = strEndPos;

		break;
	}

		case 12:
		{
			Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
			Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
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

		case 2:
		{
			Vector3Short rawRot; Stream->Read(&rawRot);
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.x; boneHeaders_v53[num].packedRotation.i = (short)rawRot.y; boneHeaders_v53[num].packedRotation.j = (short)rawRot.z; boneHeaders_v53[num].packedRotation.k = (short)0; //boneHeaders_v53[num].packedRotation.one = (short)boneData->boneRot.x; boneHeaders_v53[num].packedRotation.i = (short)boneData->boneRot.y; boneHeaders_v53[num].packedRotation.j = (short)boneData->boneRot.z; boneHeaders_v53[num].packedRotation.k = (short)0;
			boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}

		case 28:
		{
			Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
			Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
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

		case 48:
		{
			QuaternionShort rawRot; Stream->Read(&rawRot);
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
			boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}

		case 49:
		{
			QuaternionShort rawRot; Stream->Read(&rawRot);
			Vector3Short rawPos; Stream->Read(&rawPos);
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
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

		case 24:
		{
			Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
			strEndPos = Stream->Position();
			LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
			boneHeaders_v53[num].translationScale = 0;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
			boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
			boneHeaders_v53[num].nextOffset = nextOffset;
			boneHeaders_v53[num].dataSize = dataSize;
			boneHeaders_v53[num].rawStrPos = strEndPos;
			break;
		}

		case 17:
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
}

void ReadSecBoneHeader2(BinaryReader* Stream, SectionBoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int num, int dataSize, bool debug)
{
		int pos2 = Stream->Position();
		std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
		std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
		short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
		int strEndPos = Stream->Position();
		nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;

		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		Stream->seek(strEndPos);

		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);

		if ((int)flag & STUDIO_ANIM_ANIMROT)
		{
			Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
		}
		if ((int)flag & STUDIO_ANIM_ANIMPOS)
		{
			Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
			boneHeaders_v53[num].rawPos = posPtr;
		}
		if ((int)flag & STUDIO_ANIM_RAWPOS)
		{
			Vector3Short rawPos; Stream->Read(&rawPos);
			boneHeaders_v53[num].rawPos = rawPos;
		}
		if ((int)flag & STUDIO_ANIM_RAWROT)
		{
			Vector3Short rawRot; Stream->Read(&rawRot);
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.x; boneHeaders_v53[num].packedRotation.i = (short)rawRot.y; boneHeaders_v53[num].packedRotation.j = (short)rawRot.z; boneHeaders_v53[num].packedRotation.k = (short)0;
		}
		if ((int)flag & STUDIO_ANIM_RAWROT2)
		{
			QuaternionShort rawRot; Stream->Read(&rawRot);
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
		}
		strEndPos = Stream->Position();
		if ((int)flag == 12)boneHeaders_v53[num].translationScale = GetSecondLargest(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
		else boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;

		if ((int)flag == 0)
		{
			strEndPos = Stream->Position();
			boneHeaders_v53[num].translationScale = 0;
			boneHeaders_v53[num].bone = bone;
			boneHeaders_v53[num].r2Flag = flag;
			boneHeaders_v53[num].flags = 0;
			boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
			boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)0; boneHeaders_v53[num].rawPos.y = (short)0; boneHeaders_v53[num].rawPos.z = (short)0;
			boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 0; boneHeaders_v53[num].rawScale.y = 0; boneHeaders_v53[num].rawScale.z = 0;
			boneHeaders_v53[num].nextOffset = 0;
			boneHeaders_v53[num].dataSize = 0;
			boneHeaders_v53[num].rawStrPos = strEndPos;
		}

	}

void ReadSecBoneHeader(BinaryReader* Stream, SectionBoneHeaderv53* boneHeaders_v53, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int num, int dataSize, bool debug)
{
	int pos2 = Stream->Position();
	std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
	std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
	short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
	int strEndPos = Stream->Position();
	nextOffset = nextOffset > 0 ? nextOffset + (32 - GetAnimHeaderSize((int)flag)) : 0;
	switch ((int)flag)
	{
	case 0:
	{
		strEndPos = Stream->Position();
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)0; boneHeaders_v53[num].packedRotation.i = (short)0; boneHeaders_v53[num].packedRotation.j = (short)0; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)0; boneHeaders_v53[num].rawPos.y = (short)0; boneHeaders_v53[num].rawPos.z = (short)0;
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 0; boneHeaders_v53[num].rawScale.y = 0; boneHeaders_v53[num].rawScale.z = 0;
		boneHeaders_v53[num].nextOffset = 0;
		boneHeaders_v53[num].dataSize = 0;
		boneHeaders_v53[num].rawStrPos = strEndPos;

		break;
	}

	case 12:
	{
		
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
		Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
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
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
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
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
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

	case 2:
	{
		Vector3Short rawRot; Stream->Read(&rawRot);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.x; boneHeaders_v53[num].packedRotation.i = (short)rawRot.y; boneHeaders_v53[num].packedRotation.j = (short)rawRot.z; boneHeaders_v53[num].packedRotation.k = (short)0; //boneHeaders_v53[num].packedRotation.one = (short)boneData->boneRot.x; boneHeaders_v53[num].packedRotation.i = (short)boneData->boneRot.y; boneHeaders_v53[num].packedRotation.j = (short)boneData->boneRot.z; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 28:
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
		Vector3Short posPtr; Stream->Read(&posPtr); posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = GetLargestNumber(boneData->posScale.x, boneData->posScale.y, boneData->posScale.z);
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

	case 48:
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rawRot.one; boneHeaders_v53[num].packedRotation.i = (short)rawRot.i; boneHeaders_v53[num].packedRotation.j = (short)rawRot.j; boneHeaders_v53[num].packedRotation.k = (short)rawRot.k;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 49:
	{
		QuaternionShort rawRot; Stream->Read(&rawRot);
		Vector3Short rawPos; Stream->Read(&rawPos);
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
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

	case 24:
	{
		Vector3Short rotPtr; Stream->Read(&rotPtr); rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
		strEndPos = Stream->Position();
		LinearBoneData* boneData = GetLinearBoneData(Stream, Initial_Header, Initial_Header_Part2, (int)bone);
		boneHeaders_v53[num].translationScale = 0;
		boneHeaders_v53[num].bone = bone;
		boneHeaders_v53[num].r2Flag = flag;
		boneHeaders_v53[num].flags = 0;
		boneHeaders_v53[num].packedRotation; boneHeaders_v53[num].packedRotation.one = (short)rotPtr.x; boneHeaders_v53[num].packedRotation.i = (short)rotPtr.y; boneHeaders_v53[num].packedRotation.j = (short)rotPtr.z; boneHeaders_v53[num].packedRotation.k = (short)0;
		boneHeaders_v53[num].rawPos; boneHeaders_v53[num].rawPos.x = (short)float_to_half(boneData->bonePos.x); boneHeaders_v53[num].rawPos.y = (short)float_to_half(boneData->bonePos.y); boneHeaders_v53[num].rawPos.z = (short)float_to_half(boneData->bonePos.z);
		boneHeaders_v53[num].rawScale; boneHeaders_v53[num].rawScale.x = 15360; boneHeaders_v53[num].rawScale.y = 15360; boneHeaders_v53[num].rawScale.z = 15360;
		boneHeaders_v53[num].nextOffset = nextOffset;
		boneHeaders_v53[num].dataSize = dataSize;
		boneHeaders_v53[num].rawStrPos = strEndPos;
		break;
	}

	case 17:
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
}

BoneHeaderData* GetBoneHeaderData(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, bool debug)
{
	int numOfBoneHeaders = GetBoneHeaderNum(Stream, Initial_Header, 0, false);
	int numOfSecBoneHeaders = GetBoneHeaderNum(Stream, Initial_Header, 1, false);
	int numOfSecs = GetBoneHeaderNum(Stream, Initial_Header, 2, false);

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

	//boneHeaderData->secOffsets = sectionOffsets;
//	//boneHeaderData->hdrBytesAnimDescAdd = GetAnimBoneHeaderBytesAdded(Stream, Initial_Header, true);;
//	//boneHeaderData->secHdrBytesAnimDescAdd = GetAnimSectionBoneHeaderBytesAdded(Stream, Initial_Header, 0, true);
//	//boneHeaderData->secHdrBytesSecAdd = GetAnimSectionBoneHeaderBytesAdded(Stream, Initial_Header, 1, true);
	//boneHeaderData->numOfBoneHeaders = numOfBoneHeaders;
	//boneHeaderData->numOfSecBoneHeaders = numOfSecBoneHeaders;
//	//boneHeaderData->numOfBoneHdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, true);
//	//boneHeaderData->numOfSecBoneHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, 0, true);
//	//boneHeaderData->numOfSecBoneHdrsPerSec = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, 1, true);//secHdrBytesAddedToSec;//GetSecBytesAddedPerSec(Stream,Initial_Header, Initial_Header_Part2, false);
	//boneHeaderData->boneHeaders = boneHeaders;
	//boneHeaderData->secBoneheaders = secBoneHeaders;
	//boneHeaderData->secBytesAdded = secBytesAdded;
	//boneHeaderData->animBytesAdded = bytesAdded;
	//boneHeaderData->strStartPos = secStartStrPos;
	return boneHeaderData;
}

AnimData* GetAnimData(BinaryReader* Stream, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, bool debug)
{
	int numOfIkRules = 0;
	int startPos = Stream->Position();
	AnimData* animData = new AnimData();
	int animIdx = Initial_Header->localanimindex;
	animData->numOfAnims = Initial_Header->localanimindex;
	animData->secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
	animData->bytesAddedToAnimDescs = -8 * Initial_Header->numlocalanim;
	animData->bytesAddedToSec = GetAnimBoneHeaderBytesAdded2(Stream, Initial_Header, false);
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
		std::vector<int> bytesAddedPerSec;
		numOfIkRules += numikrules;

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


	}
	animData->numOfIkRules = numOfIkRules;
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
	mstudiobone_t_v49* initialBones = new mstudiobone_t_v49[Initial_Header->numbones];
	Stream->seek(Initial_Header->boneindex);
	for (int i = 0; i < Initial_Header->numbones; i++) 
	{
		mstudiobone_t_v49 bone2;  Stream->Read(&bone2);

		Vector3 unkvector {0,0,0};

		Vector3 unkvector1 { 0.0004882813 ,0.0004882813 ,0.0004882813 };

		int unkindex = 0;

		mstudiobonev53_t bone{ bone2.sznameindex, bone2.parent, -1,-1,-1,-1,-1,-1, bone2.pos, bone2.quat, bone2.rot, unkvector, bone2.posscale, bone2.rotscale, unkvector1, bone2.poseToBone, bone2.qAlignment, bone2.flags, bone2.proctype, bone2.procindex, bone2.physicsbone, bone2.surfacepropidx, bone2.contents, bone2.surfacepropidx, unkindex, 0, 0, 0, 0, 0, 0, 0};
		bones[i] = bone;
	}
	boneData->bones = bones;
	return boneData;
}

AttachmentData* GetAttachmentData(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	AttachmentData* attachmentData = new AttachmentData();
	mstudioattachment_t_v49* attachments = new mstudioattachment_t_v49[Initial_Header->numlocalattachments];
	attachmentData->numOfAttachments = Initial_Header->numlocalattachments;

	for (int i = 0; i < Initial_Header->numlocalattachments; i++)
	{
		Stream->seek(Initial_Header->localattachmentindex + 92 * i);
		mstudioattachment_t_v49 attachment2; Stream->Read(&attachment2);

		int					sznameindex; Stream->Read(&sznameindex);
		int					flags; Stream->Read(&flags);

		int					localbone; Stream->Read(&localbone); // parent bone

		matrix3x4_t			localmatrix; Stream->Read(&localmatrix); // attachment point

		int					unused[8]{0,0,0,0,0,0,0,0};

		mstudioattachment_t attachment{sznameindex, flags, localbone, localmatrix, 0,0,0,0,0,0,0,0 };
		attachments[i] = attachment2;
	}
	attachmentData->attachments = attachments;
	return attachmentData;
}

HitboxData* GetHitboxData(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
	HitboxData* hitboxData = new HitboxData();
	v53_HitboxSet* hitboxsets = new v53_HitboxSet[Initial_Header->numhitboxsets];
	std::vector<int> numOfHitboxesPerSet;
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
		numOfHitboxesPerSet.push_back(numofhitboxes);
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
	std::vector<int> numOfEvntPerSeq;
	std::vector<int> evntIdxs;
	std::vector<int> numOfActModsPerSeq;
	std::vector<int> actModIdxs;
	int numOfEvents = 0;
	int numOfActMods = 0;

	mstudioseqdesc_t_v53* sequences = new mstudioseqdesc_t_v53[Initial_Header->numlocalseq];

	int bytesAdded = 0;
	std::vector<int> bytesAddedPerSeq;

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

		bytesAddedPerSeq.push_back(bytesAdded);
		numOfEvntPerSeq.push_back(numevents);
		evntIdxs.push_back(eventindex);
		numOfActModsPerSeq.push_back(numactivitymodifiers);
		actModIdxs.push_back(activitymodifierindex);
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


