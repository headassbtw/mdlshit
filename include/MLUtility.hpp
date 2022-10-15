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

namespace Utility
{
	namespace check
	{
		bool IsWhole(double n) {
			if (n == (int)n)
				return true;
			else
				return false;
		}

		template <typename T> bool Contains(T* arry, int trgt)
		{
			int count = sizeof(arry);

			for (int i = 0; i < count; i++)
			{
				if (trgt == arry[i]) return true;
			}
			return false;
		}

		bool IsTris(mstudioruivertmap_t vertMap)
		{
			short v1 = vertMap.vertstartid;
			short v2 = vertMap.vertendid;
			short v3 = vertMap.vertextraid;

			bool _132 = v2 == v1 + (short)2 && v3 == v1 + (short)1 || v2 == v1 - (short)2 && v3 == v1 - (short)1;
			bool _123 = v2 == v1 + (short)1 && v3 == v2 + (short)1 || v2 == v1 - (short)1 && v3 == v2 - (short)1;
			bool _134 = v2 == v1 + (short)2 && v3 != v1 + (short)1 || v2 == v1 - (short)2 && v3 != v1 - (short)1;
			bool _tris = !_134;


			if (_tris) return true;
			else return false;




			return true;
		}
	}

	namespace func
	{
		std::string ReadMDLString(BinaryReader* Stream, int pos)
		{
			int startPos = Stream->Position();
			int size = 0;

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
					size++;
				}
			}
			std::string string(vec.begin(), vec.end());
			Stream->seek(startPos);
			return string;
			
		}

		int FaceType(mstudioruivertmap_t vertMap)
		{

			for (int i = 0; i < 3; i++)
			{
				if (vertMap.vertendid == vertMap.vertstartid + 2) return 0;
				if (vertMap.vertextraid == vertMap.vertstartid + 2) return 1;
				if (vertMap.vertendid == vertMap.vertstartid + 2) return 0;
			}


			//Logger::Info("vert1: %d vert2: %d\n", vertMap.vertstartid, vertMap.vertendid);

			//if (vertMap.vertendid == vertMap.vertstartid + (short)1 && vertMap.vertextraid == vertMap.vertendid + (short)1) return 2; //Is 1 2 3 pattern
			//
			//if (vertMap.vertendid == vertMap.vertstartid + (short)2 && vertMap.vertextraid == vertMap.vertstartid + (short)1 || vertMap.vertendid == vertMap.vertstartid - (short)2 && vertMap.vertextraid == vertMap.vertstartid - (short)1) return 2; //Is 1 3 2 pattern
			//
			//if (vertMap.vertendid == vertMap.vertstartid + (short)1 && vertMap.vertextraid == vertMap.vertendid + (short)1) return 2; //Is 1 2 3 pattern
			//
			//if (vertMap.vertendid != vertMap.vertstartid + (short)2 && vertMap.vertendid != vertMap.vertstartid + (short)1) return 1; //Is 1 3 2 pattern
			//
			//if (vertMap.vertendid == vertMap.vertstartid + (short)2 && vertMap.vertextraid == vertMap.vertstartid + (short)1) return 1; //Is 1 3 2 pattern
			//
			//if (vertMap.vertendid = vertMap.vertstartid + (short)2 && vertMap.vertextraid != vertMap.vertstartid + (short)1)
			//{
			//	return 0; //Is quad
			//}


		}

		std::vector<short> ReadVertMap(mstudioruivertmap_t vertMap)
		{
			//Logger::Info("RuiMesh Read: %d  %d\n", vertMap.vertstartid, vertMap.vertendid);
			int tpye = FaceType(vertMap);
			std::vector<short> vertOrder;

			short v1 = vertMap.vertstartid;
			short v2 = vertMap.vertendid;
			short v3 = vertMap.vertextraid;


			//for (int i = 0; i < 3; i++)
			//{
			//	//if(v2 == v1 + 2 || v2 == v1 - 2) Logger::Info("Is 1 3\n");
			//	//if(v1 == v2 + 2 || v1 == v2 - 2) Logger::Info("Is 3 1\n");
			//}


			bool _132Pos = v2 == v1 + (short)2 && v3 == v1 + (short)1;
			bool _123Pos = v2 == v1 + (short)1 && v3 == v2 + (short)1;
			bool _132Neg = v2 == v1 - (short)2 && v3 == v1 - (short)1;
			bool _123Neg = v2 == v1 - (short)1 && v3 == v2 - (short)1;
			bool _134Pos = v2 == v1 + (short)2 && v3 != v1 + (short)1;
			bool _134Neg = v2 == v1 - (short)2 && v3 != v1 - (short)1;
			//bool _143Pos = v3 == v1 + (short)2 && v2 != v1 + (short)1;
			//bool _143Neg = v3 == v1 - (short)2 && v2 != v1 - (short)1;
			//bool _413Pos = v3 == v2 + (short)2 && v1 != v2 + (short)1;
			//bool _413Neg = v3 == v2 - (short)2 && v1 != v2 - (short)1;
			bool _142Pos = v3 == v1 + (short)1 && v2 != v1 + (short)2 && v2 != v1 + 1;
			bool _142Neg = v3 == v1 - (short)1 && v2 != v1 - (short)2 && v2 != v1 - 1;
			bool _default = !_132Pos && !_123Pos && !_132Neg && !_123Neg && !_134Pos && !_134Neg;


			if (_134Pos && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			{
				//Logger::Info("Is Quad\n");
				vertOrder.push_back(vertMap.vertstartid);
				vertOrder.push_back(vertMap.vertendid);
				vertOrder.push_back(vertMap.vertstartid + (short)1);
				vertOrder.push_back(vertMap.vertextraid);
				vertOrder.push_back(vertMap.vertstartid + (short)1);
				vertOrder.push_back(vertMap.vertendid);
				//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
				return vertOrder;
			}
			//if (_134Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			//{
			//	//Logger::Info("Is Quad\n");
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertstartid - (short)1);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertstartid - (short)1);
			//	vertOrder.push_back(vertMap.vertendid);
			//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
			//	return vertOrder;
			//}
			if (_142Pos && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			{
				//Logger::Info("Is Quad\n");
				vertOrder.push_back(vertMap.vertstartid);
				vertOrder.push_back(vertMap.vertendid);
				vertOrder.push_back(vertMap.vertextraid);
				vertOrder.push_back(vertMap.vertstartid);
				vertOrder.push_back(vertMap.vertextraid);
				vertOrder.push_back(vertMap.vertextraid + (short)1);
				//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
				return vertOrder;
			}
			//if (_142Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			//{
			//	//Logger::Info("Is Quad\n");
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertextraid - (short)1);
			//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
			//	return vertOrder;
			//}
			//if (_143Pos && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			//{
			//	//Logger::Info("Is Quad\n");
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertstartid + (short)1);
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertstartid + (short)1);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
			//	return vertOrder;
			//}
			//if (_143Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			//{
			//	//Logger::Info("Is Quad\n");
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertstartid - (short)1);
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertstartid - (short)1);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
			//	return vertOrder;
			//}
			//if (_413Pos && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			//{
			//	//Logger::Info("Is Quad\n");
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertendid + (short)1);
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertendid + (short)1);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
			//	return vertOrder;
			//}
			//if (_413Neg && !_123Pos && !_123Neg && !_132Neg && !_132Pos)
			//{
			//	//Logger::Info("Is Quad\n");
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	vertOrder.push_back(vertMap.vertendid - (short)1);
			//	vertOrder.push_back(vertMap.vertstartid);
			//	vertOrder.push_back(vertMap.vertendid);
			//	vertOrder.push_back(vertMap.vertextraid);
			//	//Logger::Info("Vert4 Read: %d \n", vertOrder[3]);
			//	return vertOrder;
			//}
			if (_132Pos || _123Pos )
			{
				//Logger::Info("Is Tris\n");
				vertOrder.push_back(vertMap.vertstartid);
				vertOrder.push_back(vertMap.vertendid);
				vertOrder.push_back(vertMap.vertextraid);
				return vertOrder;
			}
			
			if (_132Neg || _123Neg)
			{
				//Logger::Info("Is Tris\n");
				vertOrder.push_back(vertMap.vertstartid);
				vertOrder.push_back(vertMap.vertendid);
				vertOrder.push_back(vertMap.vertextraid);
				return vertOrder;
			}
			
			if (_default)
			{
				vertOrder.push_back(vertMap.vertstartid);
				vertOrder.push_back(vertMap.vertendid);
				vertOrder.push_back(vertMap.vertextraid);
				return vertOrder;
			}

			vertOrder.push_back(vertMap.vertstartid);
			vertOrder.push_back(vertMap.vertendid);
			vertOrder.push_back(vertMap.vertextraid);
			return vertOrder;
		}

		namespace update
		{
			int GetAnimHdrBytesAddedIndv(BinaryReader* Stream, v49_Header* Initial_Header, int anim, bool debug)
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
		}
	}

	namespace mdl
	{

		class v53Mdl
		{
		public:
			studiohdr_t_v53										mdlhdr;
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
			std::vector<mstudiotexture_t_v53>					textures;
			std::vector<mstudiotexturedir_t_v49>				cdtextures;
			std::vector<mstudioskingroup_t_v49>					skingroups;
			mstudiokeyvalues_t_v49								keyvalues;
			std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms;
			mstudiolinearbone_t_v49								linearbone;
			mstudiolinearbonedata_t_v53							linearbonedata;

			std::vector<int> GetAnimHdrBytesAdded()
			{
				for (int i = 0; i < anims.size(); i++)
				{

				}
			}

			std::vector<mstudiobone_t_v49> ConvertBonesV49(int extra)
			{
				std::vector<mstudiobone_t_v49> v49Bones;

				for (int i = 0; i < mdlhdr.numbones; i++)
				{
					mstudiobone_t_v53 bone = bones[i];
					int stairs = -28 * (mdlhdr.numbones - i) + extra;


					mstudiobone_t_v49 v49Bone{ bone.sznameindex + stairs, bone.parent, -1,-1,-1,-1,-1,-1,bone.pos, bone.quat, bone.rot, bone.unkvector, bone.rotscale, bone.poseToBone, bone.qAlignment, bone.flags, bone.proctype, bone.procindex, bone.physicsbone, bone.surfacepropidx + stairs, bone.contents, bone.surfacepropLookup, bone.unused[0], bone.unused[1], bone.unused[2], bone.unused[3], bone.unused[4], bone.unused[5], bone.unused[6]};
					v49Bones.push_back(v49Bone);
				}

			}

			std::vector<mstudioanimdesc_t_v49> ConvertAnimDescsV49(int extra)
			{
				int ikRuleNum = 0;

				for (int i = 0; i < mdlhdr.numlocalanim; i++)
				{
					mstudioanimdesc_t_v53 animDesc = animdescs[i];
					int stairs = 8 * (mdlhdr.numlocalanim - i) + extra;
					int ikstairs = 12 * (mdlhdr.numlocalanim - i);
					int steps = 8 * (mdlhdr.numlocalanim - i);
					//if (animDesc.numikrules > 0)
					//{
					//	int stairs = GetAnimBoneHeaderBytesAddedIndv(&Stream, Initial_Header, i, false);
					//	int stairs2 = 0;
					//	if (animDesc.sectionindex > 0) stairs2 = GetAnimSectionBytesAddedIdv(&Stream, Initial_Header, i, false);
					//	animDesc.ikruleindex += stairs + stairs2;
					//	ikRuleNum += animDesc.numikrules;
					//}
					animDesc.sznameindex + stairs;
					if (animDesc.animindex > 0) animDesc.animindex += steps + ikstairs;
					if (animDesc.movementindex > 0) animDesc.movementindex += steps + ikstairs;
					if (animDesc.ikruleindex > 0) animDesc.ikruleindex += steps + ikstairs;
					if (animDesc.localhierarchyindex > 0) animDesc.localhierarchyindex += steps + ikstairs;
					if (animDesc.zeroframeindex > 0) animDesc.zeroframeindex += steps + ikstairs;
					//if (animDesc.ikrulezeroframeindex > 0) animDesc.ikrulezeroframeindex += stairs + stairs3;
					if (animDesc.sectionindex > 0) animDesc.sectionindex += steps + ikstairs;


					mstudioanimdesc_t_v49 v49AnimDesc{ animDesc.baseptr, animDesc.sznameindex, animDesc.fps, animDesc.flags, animDesc.numframes, animDesc.nummovements, animDesc.movementindex, 0, 0, 0, 0, 0, 0, 0, animDesc.animindex + steps, animDesc.numikrules, animDesc.ikruleindex, 0, animDesc.numlocalhierarchy, animDesc.localhierarchyindex, animDesc.sectionindex, animDesc.sectionframes, animDesc.zeroframespan, animDesc.zeroframecount, animDesc.zeroframeindex, animDesc.zeroframestalltime };
				}
			}

//			std::vector<mstudiobone_t_v49> ConvertBonesV49(int extra)
//			{
//
//			}


			v53Mdl(studiohdr_t_v53 _mdlhdr, std::vector<mstudiobone_t_v53> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v53> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v53> _animdescs, std::vector<mstudioanim_t_v53> _anims, std::vector<sectionindexes_t_v53> _sectionindexes, std::vector<mstudioanim_t_v53> _sections, std::vector<mstudioikrule_t_v53> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudiocompressedikerror_t_v52> _v52compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdesc_t_v53> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v53> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v53> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexture_t_v53> _textures, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v53 _linearbonedata)
			{
				mdlhdr = _mdlhdr;
				bones = _bones;
				jigglebones = _jigglebones;
				boneflexdrivers = _boneflexdrivers;
				attachments = _attachments;
				hitboxsets = _hitboxsets;
				hitboxes = _hitboxes;
				bonenametable = _bonenametable;
				animdescs = _animdescs;
				anims = _anims;
				sectionindexes = _sectionindexes;
				sections = _sections;
				ikrules = _ikrules;
				compressedikerrors = _compressedikerrors;
				v52compressedikerrors = _v52compressedikerrors;
				ikerrors = _ikerrors;
				ikrulezeroframe = _ikrulezeroframe;
				seqdescs = _seqdescs;
				blends = _blends;
				posekeys = _posekeys;
				events = _events;
				autolayers = _autolayers;
				activitymodifiers = _activitymodifiers;
				seqweightlist = _seqweightlist;
				nodenames = _nodenames;
				nodes = _nodes;
				bodyparts = _bodyparts;
				models = _models;
				meshes = _meshes;
				ikchains = _ikchains;
				iklinks = _iklinks;
				poseparamdescs = _poseparamdescs;
				includedmodels = _includedmodels;
				cdtextures = _cdtextures;
				textures = _textures;
				skingroups = _skingroups;
				keyvalues = _keyvalues;
				srcbonetransforms = _srcbonetransforms;
				linearbone = _linearbone;
				linearbonedata = _linearbonedata;

				//return v49Mdl( _mdlhdr,  _mdlsubhdr, _bones, _jigglebones, _boneflexdrivers, _attachments, _hitboxsets, _hitboxes, _bonenametable, _animdescs, _ikrules, _compressedikerrors, _ikerrors,  _ikrulezeroframe, _seqdescs, _blends, _posekeys, _events, _autolayers, _activitymodifiers, _seqweightlist, _nodenames, _nodes, _bodyparts, _models, _meshes, _ikchains, _iklinks, _poseparamdescs, _includedmodels, _cdtextures, _textures, _skingroups, _keyvalues, _srcbonetransforms, _linearbone, _linearbonedata)

			}

		};

		class v49Mdl
		{
			public:
			studiohdr_t_v49										mdlhdr;
			studiohdr2_t_v49									mdlsubhdr;
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


			int ConvertFlag(int flag)
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
					Logger::Info("Has Flag: %d\n", STUDIO_ANIM_DELTA);
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

			std::vector<int> v53GetAnimHdrBytesAdded(bool zeroFirst)
			{
				std::vector<int> animBytesAdded;
				int bytesAdded = 0;
				int lastHdr = 0;

				for (int i = 0; i < mdlhdr.numlocalanim; i++)
				{
					int next = i + 1;
					animBytesAdded.push_back(bytesAdded);
					Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
					if (animdescs[i].sectionindex == 0)
					{
						for (int j = lastHdr; j < anims.size(); j++)
						{
							int headerSize = GetAnimHeaderSize((int)anims[j].flags);
							lastHdr++;
							bytesAdded += (32 - headerSize);
							if (anims[j].nextoffset == 0)
							{

								if (i + 1 < mdlhdr.numlocalanim && animdescs[next].sectionindex == 0)
								{
									int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

									if (dist > 18 || dist < 0)
									{
										for (int l = 0; l < 10000000; l++)
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
									Logger::Info("dist:  %d\n", dist);
									Logger::Info("finalDist:  %d\n", 32 - dist);
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

			int v53GetAnimHdrBytesAddedIdv(int anim)
			{
				int bytesAdded = 0;
				int lastHdr = 0;

				for (int i = 0; i < anim; i++)
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

								if (i + 1 < mdlhdr.numlocalanim && animdescs[next].sectionindex == 0)
								{
									int dist = (((animdescs[next].baseptr - animdescs[next].animindex) * -1) - (anims[j].strPos + headerSize));

									if (dist > 18 || dist < 0)
									{
										for (int l = 0; l < 10000000; l++)
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

								break;
							}
						}
					}
				}
				return bytesAdded;
			}

			std::vector<int> v53GetSecHdrBytesAdded(bool zeroFirst)
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
					Logger::Info("BytesAdded: %d, Anim: %d\n", bytesAdded, i);
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
									if (i + 1 < mdlhdr.numlocalanim && j + 1 > num - 1)
									{
										int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

										if (dist > 18 || dist < 0)
										{
											for (int l = 0; l < 1000; l++)
											{
												short animValue{ (short)sections[j].animdata.arry[l] };
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
										Logger::Info("secDist:  %d\n", dist);
										Logger::Info("secFinalDist:  %d\n", 32 - dist);
										bytesAdded += 32 - dist;
									}

									if (j + 1 < num)
									{
										int nextSection = secNum + 1;
										int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

										if (dist > 18 || dist < 0)
										{
											for (int l = 0; l < 1000; l++)
											{
												short animValue{ (short)sections[j].animdata.arry[l] };
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
										Logger::Info("secDist:  %d\n", dist);
										Logger::Info("secFinalDist:  %d\n", 32 - dist);
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

			int v53GetSecHdrBytesAddedIdv(int anim)
			{
				int bytesAdded = 0;
				int secNum = 0;
				int lastHdr = 0;

				for (int i = 0; i < anim; i++)
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
									if (i + 1 < mdlhdr.numlocalanim && j + 1 > num - 1)
									{
										int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

										if (dist > 18 || dist < 0)
										{
											for (int l = 0; l < 1000; l++)
											{
												short animValue{ (short)sections[j].animdata.arry[l] };
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

									if (j + 1 < num)
									{
										int nextSection = secNum + 1;
										int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

										if (dist > 18 || dist < 0)
										{
											for (int l = 0; l < 1000; l++)
											{
												short animValue{ (short)sections[j].animdata.arry[l] };
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
										Logger::Info("secDist:  %d\n", dist);
										Logger::Info("secFinalDist:  %d\n", 32 - dist);
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

			std::vector<int> v53GetSecBytesAdded(bool zeroFirst)
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
							Logger::Info("secBytesAdded: %d, Anim: %d\n", bytesAdded, i);
							for (int k = lastHdr; k < sections.size(); k++)
							{
								int pos2 = sections[k].strPos;
								int headerSize = GetAnimHeaderSize((int)sections[k].flags);
								lastHdr++;
								bytesAddedPer += (32 - headerSize);
								bytesAdded += (32 - headerSize);
								if (sections[k].nextoffset == 0)
								{
									if (i + 1 < mdlhdr.numlocalanim && j + 1 > num - 1)
									{
										int dist = animdescs[nextAnim].sectionindex > 0 ? ((startPos + 100 + animdescs[nextAnim].sectionindex) - (pos2 + headerSize)) : ((startPos + 100 + animdescs[nextAnim].animindex) - (pos2 + headerSize));

										if (dist > 18 || dist < 0)
										{
											for (int l = 0; l < 1000; l++)
											{
												short animValue{ (short)sections[j].animdata.arry[l] };
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
										Logger::Info("secDist:  %d\n", dist);
										Logger::Info("secFinalDist:  %d\n", 32 - dist);
										bytesAdded += 32 - dist;
									}

									if ( j + 1 < num )
									{
										int nextSection = secNum + 1;
										int dist = ((startPos + sectionindexes[nextSection].sectionoffset) - (pos2 + headerSize));

										if (dist > 18 || dist < 0)
										{
											for (int l = 0; l < 1000; l++)
											{
												short animValue{ (short)sections[j].animdata.arry[l] };
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
										Logger::Info("secDist:  %d\n", dist);
										Logger::Info("secFinalDist:  %d\n", 32 - dist);
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

			int v53GetTotalAnimHdrBytesAdded(std::vector<int> animBytes)
			{
				int bytesAdded = 0;

				for (int i = 0; i < animBytes.size(); i++)
				{
					bytesAdded += animBytes[i];
				}
				Logger::Info("Total BytesAdded: %d\n", bytesAdded);
				return bytesAdded;
			}

			int v53GetTotalSecHdrBytesAdded(std::vector<int> animBytes) //Temp. Gonna make this just take the array and do the calculation doing that only requiring one function. -Liberty
			{
				int bytesAdded = 0;

				for (int i = 0; i < animBytes.size(); i++)
				{
					bytesAdded += animBytes[i];
				}
				Logger::Info("Total BytesAdded: %d\n", bytesAdded);
				return bytesAdded;
			}

			std::vector<int> v53IkRuleStairsPerAnim()
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

			std::vector<int> ikRuleStairsPerAnim = v53IkRuleStairsPerAnim();
			std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
			std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
			std::vector<int> secHdrBytesSecAdd = v53GetSecBytesAdded(true);

			int animByteAddedTotal = v53GetTotalAnimHdrBytesAdded(hdrBytesAnimDescAdd);
			int animSecByteAddedTotal = v53GetTotalSecHdrBytesAdded(secHdrBytesAnimDescAdd);

			int animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
			int bytesAddedToRuiMesh = 0;
			int bytesAddedToIkRules = 0;// -12 * animData->numOfIkRules;
			int bytesAddedToHeader = 52;
			int bytesAddedToBones = mdlhdr.numbones * 28;
			int bytesAddedToAnims = -8 * mdlhdr.numlocalanim;
			int bytesAddedToAnimData = mdlhdr.numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal + bytesAddedToIkRules : 0;
			int bytesAddedToSeqs = 20 * mdlhdr.numlocalseq;
			int bytesAddedToTextures = -20 * mdlhdr.numtextures;
			int bytesAddedToIkChains = 16 * mdlhdr.numikchains;
			int bytesAddedToActMods = 4 * activitymodifiers.size();
			int textureFiller = 0;
			int strFiller = 60;
			int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + bytesAddedToIkRules + textureFiller + bytesAddedToRuiMesh;
			int numOfLinks = 0;

			void SetMdlInts()
			{
				ikRuleStairsPerAnim = v53IkRuleStairsPerAnim();
				hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
				secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
				secHdrBytesSecAdd = v53GetSecBytesAdded(true);

				animByteAddedTotal = v53GetTotalAnimHdrBytesAdded(hdrBytesAnimDescAdd);
				animSecByteAddedTotal = v53GetTotalSecHdrBytesAdded(secHdrBytesAnimDescAdd);

				animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
				bytesAddedToRuiMesh = 0;
				bytesAddedToIkRules = 0;// -12 * animData->numOfIkRules;
				bytesAddedToHeader = 52;
				bytesAddedToBones = mdlhdr.numbones * 28;
				bytesAddedToAnims = -8 * mdlhdr.numlocalanim;
				bytesAddedToAnimData = mdlhdr.numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal + bytesAddedToIkRules : 0;
				bytesAddedToSeqs = 20 * mdlhdr.numlocalseq;
				bytesAddedToTextures = -20 * mdlhdr.numtextures;
				bytesAddedToIkChains = 16 * mdlhdr.numikchains;
				bytesAddedToActMods = 4 * activitymodifiers.size();
				textureFiller = 0;
				strFiller = 60;
				allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + bytesAddedToIkRules + textureFiller + bytesAddedToRuiMesh;
				numOfLinks = 0;
			}

			std::vector<mstudiobone_t_v53> BoneConversion()
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

			std::vector<mstudiobbox_t_v53> HitboxConversion()
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

			std::vector<mstudioanimdesc_t_v53> AnimDescConversion()
			{
				std::vector<int> hdrBytesAnimDescAdd = v53GetAnimHdrBytesAdded(true);
				std::vector<int> secHdrBytesAnimDescAdd = v53GetSecHdrBytesAdded(true);
				std::vector<mstudioanimdesc_t_v53> v53AnimDescs;
				int ikRuleNum = 0;
				int animIdx = mdlhdr.localanimindex;
				Logger::Info("animIdx: %d\n", animIdx);
				Logger::Info("Hdr BytesAdded: %d\n", bytesAddedToHeader);
				Logger::Info("Bone BytesAdded: %d\n", bytesAddedToBones);
				int bytesAddedToHeader = 52;
				int bytesAddedToBones = mdlhdr.numbones * 28;
				for (int i = 0; i < mdlhdr.numlocalanim; i++)
				{
					//Logger::Info("Test\n");
					mstudioanimdesc_t_v49 v49AnimDesc = animdescs[i];


					int outPos = animIdx + bytesAddedToHeader + bytesAddedToBones + 92 * i;
					int stairs = (-((8 * (mdlhdr.numlocalanim - i))) + bytesAddedToSeqs) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
					int steps = -((8 * (mdlhdr.numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];
					int ikStairs = -12 * (ikRuleNum);
					if (v49AnimDesc.numikrules > 0)
					{
						int stairs = v53GetAnimHdrBytesAddedIdv(i);
						int stairs2 = 0;
						if (v49AnimDesc.sectionindex > 0) stairs2 = v53GetSecHdrBytesAddedIdv(i);
						v49AnimDesc.ikruleindex += stairs + stairs2;
						ikRuleNum += v49AnimDesc.numikrules;
					}

					if (v49AnimDesc.animindex > 0) v49AnimDesc.animindex += steps + ikStairs;
					if (v49AnimDesc.movementindex > 0) v49AnimDesc.movementindex += steps + ikStairs;
					if (v49AnimDesc.ikruleindex > 0) v49AnimDesc.ikruleindex += steps + ikStairs;
					if (v49AnimDesc.localhierarchyindex > 0) v49AnimDesc.localhierarchyindex += steps + ikStairs;
					if (v49AnimDesc.zeroframeindex > 0) v49AnimDesc.zeroframeindex += steps + ikStairs;
					if (v49AnimDesc.ikrulezeroframeindex > 0) v49AnimDesc.ikrulezeroframeindex += steps + ikStairs;
					if (v49AnimDesc.sectionindex > 0) v49AnimDesc.sectionindex += steps + ikStairs;
					int compressedIkRuleIdx = 0; //if (v49AnimDesc.numikrules > 0) compressedIkRuleIdx = ( basePtr - ( basePtr - (v49AnimDesc.ikruleindex + 140 * v49AnimDesc.numikrules) ) ) * -1 ;


					mstudioanimdesc_t_v53 animDesc = { -outPos, v49AnimDesc.sznameindex + stairs + ikStairs, v49AnimDesc.fps, v49AnimDesc.flags, v49AnimDesc.numframes, v49AnimDesc.nummovements, v49AnimDesc.movementindex, compressedIkRuleIdx * -1, v49AnimDesc.animindex, v49AnimDesc.numikrules, v49AnimDesc.ikruleindex, v49AnimDesc.numlocalhierarchy, v49AnimDesc.localhierarchyindex, v49AnimDesc.sectionindex, v49AnimDesc.sectionframes, v49AnimDesc.zeroframespan, v49AnimDesc.zeroframecount, v49AnimDesc.zeroframeindex, v49AnimDesc.zeroframestalltime, 0, 0, 0, 0, 0 };

					v53AnimDescs.push_back(animDesc);
				}
				return v53AnimDescs;
			}

			std::vector<mstudioikrule_t_v53> IkRuleConversion(mstudioikrule_t_v49 v49IkRule, int stairs)
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

							if (v49IkRule.compressedikerrorindex > 0) v49IkRule.compressedikerrorindex += stairs;
							if (v49IkRule.ikerrorindex > 0) v49IkRule.ikerrorindex += stairs;
							if (v49IkRule.szattachmentindex > 0) v49IkRule.szattachmentindex += stairs;

							mstudioikrule_t_v53 ikRule = { v49IkRule.index, ikruletype(v49IkRule.type), v49IkRule.chain, v49IkRule.bone, v49IkRule.slot, v49IkRule.height, v49IkRule.radius, v49IkRule.floor, v49IkRule.pos, v49IkRule.q.one, v49IkRule.q.i, v49IkRule.q.j, v49IkRule.q.k, v49IkRule.compressedikerrorindex, v49IkRule.iStart, v49IkRule.ikerrorindex, v49IkRule.start, v49IkRule.peak, v49IkRule.tail, v49IkRule.end, v49IkRule.contact, v49IkRule.drop, v49IkRule.top, v49IkRule.szattachmentindex, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

							v53IkRules.push_back(ikRule);
						}
					}
				}
				return v53IkRules;
			}

			std::vector<mstudioanim_t_v53> ConvertAnims()
			{
				std::vector<mstudioanim_t_v49> _anims = anims;
				std::vector<mstudioanim_t_v53> v53Anims;
				mstudiolinearbonedata_t_v49 lbd = linearbonedata;

				for (int i = 0; i < anims.size(); i++)
				{
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

					if ((int)anims[i].flags == 12) _posscale = GetLargestNumber(lbd.posScale[bone].x, lbd.posScale[bone].y, lbd.posScale[bone].z);
					if ((int)anims[i].flags & STUDIO_ANIM_ANIMROT)
					{
						Vector3Short rotPtr = anims[i].animrot.offset;
						Vector3Short posPtr = anims[i].animpos.offset;
						_animrot = anims[i].animrot;
						if (posPtr.x == 0, posPtr.y == 0, posPtr.z == 0)
						{
							rotPtr.x > 0 ? rotPtr.x += 18 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 18 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 18 : rotPtr.z = 0;
							anims[i].animrot.offset = rotPtr;
						}
						else
						{
							rotPtr.x > 0 ? rotPtr.x += 12 : rotPtr.x = 0; rotPtr.y > 0 ? rotPtr.y += 12 : rotPtr.y = 0; rotPtr.z > 0 ? rotPtr.z += 12 : rotPtr.z = 0;
							anims[i].animrot.offset = rotPtr;
						}

					}

					if ((int)anims[i].flags & STUDIO_ANIM_ANIMPOS)
					{
						Vector3Short rotPtr = anims[i].animrot.offset;
						Vector3Short posPtr = anims[i].animpos.offset;
						if (rotPtr.x == 0, rotPtr.y == 0, rotPtr.z == 0)
						{
							posPtr.x > 0 ? posPtr.x += 18 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 18 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 18 : posPtr.z = 0;
							anims[i].animpos.offset = posPtr;
						}
						else
						{
							posPtr.x > 0 ? posPtr.x += 10 : posPtr.x = 0; posPtr.y > 0 ? posPtr.y += 10 : posPtr.y = 0; posPtr.z > 0 ? posPtr.z += 10 : posPtr.z = 0;
							anims[i].animpos.offset = posPtr;
						}
						_animpos = anims[i].animpos;
					}

					if ((int)anims[i].flags & STUDIO_ANIM_RAWROT) _rawrot = anims[i].rawrot;//{ anims[i].rawrot.one, anims[i].rawrot.i, anims[i].rawrot.j, anims[i].rawrot.k };

					if ((int)anims[i].flags & STUDIO_ANIM_RAWROT2) _rawrot = anims[i].rawrot2;

					if ((int)anims[i].flags & STUDIO_ANIM_RAWPOS) _rawpos = anims[i].rawpos;
					else
						_rawpos = { (short)float_to_half(lbd.bonePos[bone].x), (short)float_to_half(lbd.bonePos[bone].y), (short)float_to_half(lbd.bonePos[bone].z)};

					mstudioanim_t_v53 v53Anim{ _posscale, _bone, _flags, _unk, _rawrot, _animrot, _unused, _rawpos, _animpos, _rawscale, _animscale, _nextOffset, anims[i].animdata };
					v53Anims.push_back(v53Anim);
				}
				return v53Anims;
			}

			v49Mdl(studiohdr_t_v49 _mdlhdr, studiohdr2_t_v49 _mdlsubhdr, std::vector<mstudiobone_t_v49> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets,std::vector<mstudiobbox_t_v49> _hitboxes,mstudiobonenametable_t_v49 _bonenametable,std::vector<mstudioanimdesc_t_v49> _animdescs,std::vector<mstudioanim_t_v49> _anims,std::vector<sectionindexesindex_t_v49> _sectionindexes, std::vector<mstudioanim_t_v49> _sections,std::vector<mstudioikrule_t_v49> _ikrules,std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors,std::vector<mstudioikerror_t_v49> _ikerrors,std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe,std::vector<mstudioseqdescv49_t> _seqdescs,std::vector<blendgroup_t_v49> _blends,std::vector<posekey_t_v49> _posekeys,std::vector<mstudioevent_t_v49> _events,std::vector<mstudioautolayer_t_v49> _autolayers,std::vector<mstudioactivitymodifier_t_v49> _activitymodifiers,std::vector<seqweightlist_t_v49> _seqweightlist,std::vector<mstudionodename_t_v49> _nodenames,std::vector<mstudionodedata_v49> _nodes,std::vector<mstudiobodyparts_t_v49> _bodyparts,std::vector<mstudiomodel_t_v49>	_models,std::vector<mstudiomesh_t_v49> _meshes,std::vector<mstudioikchain_t_v49> _ikchains,std::vector<mstudioiklink_t_v49> _iklinks,std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs,std::vector<mstudiomodelgroup_t_v49> _includedmodels,std::vector<mstudiotexturedir_t_v49> _cdtextures,std::vector<mstudiotexture_t_v49> _textures,std::vector<mstudioskingroup_t_v49> _skingroups,mstudiokeyvalues_t_v49 _keyvalues,std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata )
			{
				mdlhdr = _mdlhdr;
				mdlsubhdr = _mdlsubhdr;
				bones = _bones;
				jigglebones = _jigglebones;
				boneflexdrivers = _boneflexdrivers;
				attachments = _attachments;
				hitboxsets = _hitboxsets;
				hitboxes = _hitboxes;
				bonenametable = _bonenametable;
				animdescs = _animdescs;
				anims = _anims;
				sectionindexes = _sectionindexes;
				sections = _sections;
				ikrules = _ikrules;
				compressedikerrors = _compressedikerrors;
				ikerrors = _ikerrors;
				ikrulezeroframe = _ikrulezeroframe;
				seqdescs = _seqdescs;
				blends = _blends;
				posekeys = _posekeys;
				events = _events;
				autolayers = _autolayers;
				activitymodifiers = _activitymodifiers;
				seqweightlist = _seqweightlist;
				nodenames = _nodenames;
				nodes = _nodes;
				bodyparts = _bodyparts;
				models = _models;
				meshes = _meshes;
				ikchains = _ikchains;
				iklinks = _iklinks;
				poseparamdescs = _poseparamdescs;
				includedmodels = _includedmodels;
				cdtextures = _cdtextures;
				textures = _textures;
				skingroups = _skingroups;
				keyvalues = _keyvalues;
				srcbonetransforms = _srcbonetransforms;
				linearbone = _linearbone;
				linearbonedata = _linearbonedata;

				//return v49Mdl( _mdlhdr,  _mdlsubhdr, _bones, _jigglebones, _boneflexdrivers, _attachments, _hitboxsets, _hitboxes, _bonenametable, _animdescs, _ikrules, _compressedikerrors, _ikerrors,  _ikrulezeroframe, _seqdescs, _blends, _posekeys, _events, _autolayers, _activitymodifiers, _seqweightlist, _nodenames, _nodes, _bodyparts, _models, _meshes, _ikchains, _iklinks, _poseparamdescs, _includedmodels, _cdtextures, _textures, _skingroups, _keyvalues, _srcbonetransforms, _linearbone, _linearbonedata)

			}

		};

		template <typename T> int GetJiggleBoneCount(T mdl, bool debug)
		{
			const char* type = typeid(mdl).name(); if (debug) Logger::Info("Type: %s\n", (std::string)type);
			int jiggleBoneCount = 0;

			if (type == typeid(v53Mdl).name() || type == typeid(v49Mdl).name() || type == typeid(v49Mdl).name())
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
			Logger::Error("No compatible mdl class was passed. Returning 0. Wrong Type: \n", type );
			return 0;
		}

		template <typename T> int GetHitboxCount(T mdl, bool debug)
		{
			const char* type = typeid(mdl).name();
			int hitboxCount = 0;

			if ( type == typeid(v53Mdl).name() || type == typeid(v49Mdl).name() || type == typeid(v49Mdl).name() )
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

			if (type == typeid(v53Mdl).name() || type == typeid(v49Mdl).name() || type == typeid(v49Mdl).name())
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

		void v53_ReadMdl()
		{

		}

		void v52_ReadMdl()
		{

		}

		void v49_ReadMdl()
		{

		}

		void RUIMeshToSmd(mstudioruimesh_t ruiMesh)
		{
			int faces = ruiMesh.numfaces;
			Logger::Info("version 1\n");
			Logger::Info("nodes\n");
			Logger::Info("%d 'Bone' %d\n", 0, -1);
			Logger::Info("end\n");
			Logger::Info("skeleton\n");
			Logger::Info("time 0\n");
			Logger::Info("0  0.000000 0.000000 0.000000  0.000000 -0.000000 0.000000\n");
			Logger::Info("end\n");
			Logger::Info("triangles\n");
			//Logger::Info("RuiMesh Read: %d  %d\n", ruiMesh.vertexmap[0].vertstartid, ruiMesh.vertexmap[0].vertendid);
			int faceNum = 0;
			int vertNum = 0;
			//for (int i = 0; i < ruiMesh.numvertices; i++)
			//{
			//	//Logger::Info("FACE%d \n", faceNum);
			//	if (vertNum >= 3)
			//	{
			//		Logger::Info("FACE%d \n", faceNum);
			//		faceNum++;
			//		vertNum = 0;
			//	}
			//	mstudioruivert_t vert1 = ruiMesh.vertex[i];
			//	Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z);
			//	vertNum++;
			//	//for (int j = 0; j < ruiMesh.numvertices; j++)
			//	//{
			//	//	mstudioruivert_t vert1 = ruiMesh.vertex[vertNum];
			//	//	Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z);
			//	//	vertNum++;
			//	//}
			//	//faceNum++;
			//}
			for (int i = 0; i < ruiMesh.numfaces; i++)
			{
				mstudioruivertmap_t map = ruiMesh.vertexmap[i];
				//Logger::Info("RuiMesh Read: %d  %d\n", ruiMesh.vertexmap[i].vertstartid, ruiMesh.vertexmap[i].vertendid);
				std::vector<short> vertMap = func::ReadVertMap(map);
				Logger::Info("FACE%d \n", i);
				if (vertMap.size() > 3)
				{
					int v1 = vertMap[0];
					int v2 = vertMap[1];
					int v3 = vertMap[2];
					int v4 = vertMap[3];
					int v5 = vertMap[4];
					int v6 = vertMap[5];
					//Logger::Info("Vert4 Read: %d \n", vertMap[3]);
					//Logger::Info("Vert4 Read: %d \n", v4);
			
					mstudioruivert_t vert1 = ruiMesh.vertex[v1];
					mstudioruivert_t vert2 = ruiMesh.vertex[v2];
					mstudioruivert_t vert3 = ruiMesh.vertex[v3];
			
					mstudioruivert_t vert4 = ruiMesh.vertex[v4];
					mstudioruivert_t vert5 = ruiMesh.vertex[v5];
					mstudioruivert_t vert6 = ruiMesh.vertex[v6];
			
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z );
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert2.vertexpos.x, vert2.vertexpos.y, vert2.vertexpos.z);
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert3.vertexpos.x, vert3.vertexpos.y, vert3.vertexpos.z);
					Logger::Info("FACE%d Quad \n", i);
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert4.vertexpos.x, vert4.vertexpos.y, vert4.vertexpos.z);
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert5.vertexpos.x, vert5.vertexpos.y, vert5.vertexpos.z);
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert6.vertexpos.x, vert6.vertexpos.y, vert6.vertexpos.z);
				}
				else
				{
					int v1 = vertMap[0];
					int v2 = vertMap[1];
					int v3 = vertMap[2];
				
					mstudioruivert_t vert1 = ruiMesh.vertex[v1];
					mstudioruivert_t vert2 = ruiMesh.vertex[v2];
					mstudioruivert_t vert3 = ruiMesh.vertex[v3];
				
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert1.vertexpos.x, vert1.vertexpos.y, vert1.vertexpos.z);
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert2.vertexpos.x, vert2.vertexpos.y, vert2.vertexpos.z);
					Logger::Info("0 %f %f %f 0.000000 0.000000 0.000000  0.009463 0.997711 1 9 1.000000\n", vert3.vertexpos.x, vert3.vertexpos.y, vert3.vertexpos.z);
				}
			}
			Logger::Info("end\n");

			
		}

		v53Mdl _v53Mdl(BinaryReader* Stream, bool debug)
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

					//Logger::Info("Bone Read: %s\n", func::ReadMDLString(Stream, bonePos + bone.sznameindex).c_str());
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

					//Logger::Info("Attachment Read: %s\n", func::ReadMDLString(Stream, attachmentPos + attachment.sznameindex).c_str());
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
					//Logger::Info("HitboxSet Read: %s\n", func::ReadMDLString(Stream, hitboxsetPos + hitboxset.sznameindex).c_str());
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
						//Logger::Info("Hitbox Read: %s\n", func::ReadMDLString(Stream, hitboxPos + hitbox.szhitboxnameindex).c_str());
						//Logger::Info("Hitbox Read: %s\n", func::ReadMDLString(Stream, hitboxPos + hitbox.keyvalueindex).c_str());
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

					//Logger::Info("AnimDesc Read: %s\n", func::ReadMDLString(Stream, animDescPos + animdesc.sznameindex).c_str());
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
								if (nextAnim > mdlhdr.numlocalanim ) dist = mdlhdr.localseqindex - hdrEndPos;

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
					//Logger::Info("SeqLabel Read: %s\n", func::ReadMDLString(Stream, seqPos + seqDesc.szlabelindex).c_str());
					//Logger::Info("SeqActName Read: %s\n", func::ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex).c_str());
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

							//Logger::Info("Event Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex).c_str());
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
							//Logger::Info("ActMod Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].activitymodifierindex + 8 * j + actmod.sznameindex).c_str());
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
					//Logger::Info("NodeName Read: %s\n", func::ReadMDLString(Stream, nodenames[i].sznameindex).c_str());
					Logger::Info("NodeName Read: %d\n", i);
				}
				Stream->seek(mdlhdr.localnodeindex);
				for (int i = 0; i < mdlhdr.numlocalnodes; i++)
				{
					mstudionodedata_v49 node; Stream->Read(&node);
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
					//Logger::Info("Bodypart Read: %s\n", func::ReadMDLString(Stream, mdlhdr.bodypartindex + 16 * i + bodyparts[i].sznameindex).c_str());
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

					//Logger::Info("Model Read: %s\n", func::ReadMDLString(Stream, modelPos).c_str());
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

					//Logger::Info("IkChain Read: %s\n", func::ReadMDLString(Stream, ikChainPos + ikchains[i].sznameindex).c_str());
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

					//Logger::Info("PoseParamDesc Read: %s\n", func::ReadMDLString(Stream, poseParamPos + poseparamdescs[i].sznameindex).c_str());
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

					//Logger::Info("MeshMat Read: %s\n", func::ReadMDLString(Stream, mdlhdr.textureindex + 44 * mesh.material + texture.sznameindex).c_str());
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

					//Logger::Info("IncludedModel Read: %s\n", func::ReadMDLString(Stream, includedModelPos + includedmodels[i].sznameindex).c_str());
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

					Logger::Info("Texture Read: %s\n", func::ReadMDLString(Stream, texturePos + textures[i].sznameindex).c_str());
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

					Logger::Info("CdTexture Read: %s\n", func::ReadMDLString(Stream, cdtextures[i].sznameindex).c_str());
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
				Logger::Info("Test\n");
			}
			Logger::Info("Test\n");
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

					Logger::Info("SrcBoneTransform Read: %s\n", func::ReadMDLString(Stream, srcBonePos + srcbonetransforms[i].sznameindex).c_str());
					Logger::Info("SrcBoneTransform Read: %d\n", i);
				}
			}
			Logger::Info("Test\n");
			if (mdlhdr.numbones > 0)
			{
				Logger::Info("Test\n");
				Stream->Read(&linearbone);
				Stream->Read(&linearbonedata, mdlhdr.numbones);
				Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
			}
			Logger::Info("Test\n");
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
					RUIMeshToSmd(ruiMesh);
					Logger::Info("RuiMesh Read: %s\n", func::ReadMDLString(Stream, ruiPos + 32).c_str());
					Logger::Info("RuiMesh Read: %d\n", j);
				}
			}

			v53Mdl _v53mdl{ mdlhdr,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,anims, sectionindexes, sections, ikrules,compressedikerrors,v52compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,textures,cdtextures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata };

			return _v53mdl;
		}

		v49Mdl _v49Mdl(BinaryReader* Stream, bool debug)
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

					Logger::Info("Bone Read: %s\n", func::ReadMDLString(Stream, bonePos + bone.sznameindex).c_str());
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

					Logger::Info("Attachment Read: %s\n", func::ReadMDLString(Stream, attachmentPos + attachment.sznameindex).c_str());
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
					Logger::Info("HitboxSet Read: %s\n", func::ReadMDLString(Stream, hitboxsetPos + hitboxset.sznameindex).c_str());
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
						Logger::Info("Hitbox Read: %s\n", func::ReadMDLString(Stream, hitboxPos + hitbox.szhitboxnameindex).c_str());
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

					Logger::Info("AnimDesc Read: %s\n", func::ReadMDLString(Stream, animDescPos + animdesc.sznameindex).c_str());
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
								if (nextAnim > mdlhdr.numlocalanim) dist = mdlhdr.localseqindex - hdrEndPos;

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
									Logger::Info("CompressedIkError Read: %d\n", j);
									break;
								}
								Logger::Info("CompressedIkError Read: %d\n", j);
								dist = errorIdxs[next] - endPos;
								Stream->Read(&animData, dist);
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
					Logger::Info("Seq Read: %s\n", func::ReadMDLString(Stream, seqPos + seqDesc.szlabelindex).c_str());
					Logger::Info("Seq Read: %s\n", func::ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex).c_str());
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

							Logger::Info("Event Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + 12 ).c_str());
							Logger::Info("Event Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex).c_str());
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
							Logger::Info("ActMod Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].activitymodifierindex + 4 * j + actmod.sznameindex).c_str());
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
					Logger::Info("NodeName Read: %s\n", func::ReadMDLString(Stream, nodenames[i].sznameindex).c_str());
					Logger::Info("NodeName Read: %d\n", i);
				}
				Stream->seek(mdlhdr.localnodeindex);
				for (int i = 0; i < mdlhdr.numlocalnodes; i++)
				{
					mstudionodedata_v49 node; Stream->Read(&node);
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
					Logger::Info("Bodypart Read: %s\n", func::ReadMDLString(Stream, mdlhdr.bodypartindex + 16 * i + bodyparts[i].sznameindex).c_str());
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

					Logger::Info("Model Read: %s\n", func::ReadMDLString(Stream, modelPos).c_str());
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

					Logger::Info("PoseParamDesc Read: %s\n", func::ReadMDLString(Stream, poseParamPos + poseparamdescs[i].sznameindex).c_str());
					Logger::Info("PoseParamDesc Read: %d\n", i);
				}
			}

			if (nummeshes > 0)
			{
				for (int i = 0; i < nummeshes; i++)
				{
					int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts;
					int meshPos = modelPos + 148 * nummodels + 20 * mdlhdr.numlocalposeparameters + 16 * mdlhdr.numikchains + 28 * numiklinks + 116 * i;
					Stream->seek(meshPos);
					mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
					meshes.push_back(mesh);

					int texturePos = mdlhdr.textureindex + (64 * (int)mesh.material);
					Stream->seek(texturePos);
					mstudiotexture_t_v53 texture; Stream->Read(&texture);

					Logger::Info("MeshMat Read: %s\n", func::ReadMDLString(Stream, texturePos + texture.sznameindex).c_str());
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

					Logger::Info("Texture Read: %s\n", func::ReadMDLString(Stream, texturePos + textures[i].sznameindex).c_str());
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
					Logger::Info("CdTexture Read: %s\n", func::ReadMDLString(Stream, cdtextures[i].sznameindex).c_str());
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
				Stream->Read(&linearbone);
				Stream->Read(&linearbonedata, &linearbone);
				Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
			}
			v49Mdl _v49mdl{ mdlhdr,mdlsubhdr,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,anims, sectionindexes, sections, ikrules,compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,cdtextures,textures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata};

			return _v49mdl;
		}

	}












}



