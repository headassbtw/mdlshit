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
			mstudiolinearbonedata_t_v49							linearbonedata;

			v53Mdl(studiohdr_t_v53 _mdlhdr, std::vector<mstudiobone_t_v53> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v53> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v53> _animdescs, std::vector<mstudioikrule_t_v53> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudiocompressedikerror_t_v52> _v52compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdesc_t_v53> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v53> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v53> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexture_t_v53> _textures, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata)
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

			v49Mdl(studiohdr_t_v49 _mdlhdr, studiohdr2_t_v49 _mdlsubhdr, std::vector<mstudiobone_t_v49> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets,std::vector<mstudiobbox_t_v49> _hitboxes,mstudiobonenametable_t_v49 _bonenametable,std::vector<mstudioanimdesc_t_v49> _animdescs,std::vector<mstudioikrule_t_v49> _ikrules,std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors,std::vector<mstudioikerror_t_v49> _ikerrors,std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe,std::vector<mstudioseqdescv49_t> _seqdescs,std::vector<blendgroup_t_v49> _blends,std::vector<posekey_t_v49> _posekeys,std::vector<mstudioevent_t_v49> _events,std::vector<mstudioautolayer_t_v49> _autolayers,std::vector<mstudioactivitymodifier_t_v49> _activitymodifiers,std::vector<seqweightlist_t_v49> _seqweightlist,std::vector<mstudionodename_t_v49> _nodenames,std::vector<mstudionodedata_v49> _nodes,std::vector<mstudiobodyparts_t_v49> _bodyparts,std::vector<mstudiomodel_t_v49>	_models,std::vector<mstudiomesh_t_v49> _meshes,std::vector<mstudioikchain_t_v49> _ikchains,std::vector<mstudioiklink_t_v49> _iklinks,std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs,std::vector<mstudiomodelgroup_t_v49> _includedmodels,std::vector<mstudiotexturedir_t_v49> _cdtextures,std::vector<mstudiotexture_t_v49> _textures,std::vector<mstudioskingroup_t_v49> _skingroups,mstudiokeyvalues_t_v49 _keyvalues,std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata )
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
			mstudiolinearbonedata_t_v49							linearbonedata;

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

					Logger::Info("Bone Read: %s\n", func::ReadMDLString(Stream, bonePos + bone.sznameindex).c_str());
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
						mstudiobbox_t_v53 hitbox; Stream->Read(&hitbox);
						Logger::Info("Hitbox Read: %s\n", func::ReadMDLString(Stream, hitboxPos + hitbox.szhitboxnameindex).c_str());
						Logger::Info("Hitbox Read: %s\n", func::ReadMDLString(Stream, hitboxPos + hitbox.keyvalueindex).c_str());
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

					Logger::Info("AnimDesc Read: %s\n", func::ReadMDLString(Stream, animDescPos + animdesc.sznameindex).c_str());
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
								if (nextAnim + 1 < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].sectionindex - hdrEndPos;
								if (nextAnim + 1 < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 92 + animdescs[nextAnim].animindex - hdrEndPos;
								if (nextAnim + 1 > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - hdrEndPos;

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
						std::vector<sectionindexes_t_v53> sectionIdxArry;

						int secNumber = (frames / secFrames) + 2;

						Stream->seek(secPos);

						for (int j = 0; j < secNumber; j++)
						{

							sectionindexes_t_v53 sectionIdx; Stream->Read(&sectionIdx);
							//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
							sectionIdxArry.push_back(sectionIdx);
						}

						for (int j = 0; j < secNumber; j++)
						{
							sectionindexes_t_v53 sectionIdx = sectionIdxArry[j];
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
									if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + sectionIdxArry[nextSection].sectionoffsets - hdrEndPos;
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
					Logger::Info("SeqLabel Read: %s\n", func::ReadMDLString(Stream, seqPos + seqDesc.szlabelindex).c_str());
					Logger::Info("SeqActName Read: %s\n", func::ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex).c_str());
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

							Logger::Info("Event Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex).c_str());
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
							Logger::Info("ActMod Read: %s\n", func::ReadMDLString(Stream, seqPos + seqdescs[i].activitymodifierindex + 8 * j + actmod.sznameindex).c_str());
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
					mstudioikchain_t_v53 ikchain; Stream->Read(&ikchain);
					if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;
					ikchains.push_back(ikchain);

					int ikChainPos = mdlhdr.ikchainindex + 32 * i;

					Logger::Info("IkChain Read: %s\n", func::ReadMDLString(Stream, ikChainPos + ikchains[i].sznameindex).c_str());
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
					int meshPos = modelPos + 148 * nummodels + 20 * mdlhdr.numlocalposeparameters + 32 * mdlhdr.numikchains + 28 * numiklinks + 116 * i;
					Stream->seek(meshPos);
					mstudiomesh_t_v49 mesh; Stream->Read(&mesh);
					meshes.push_back(mesh);

					int texturePos = mdlhdr.textureindex + (44 * (int)mesh.material);
					Stream->seek(texturePos);
					mstudiotexture_t_v53 texture; Stream->Read(&texture);

					Logger::Info("MeshMat Read: %s\n", func::ReadMDLString(Stream, mdlhdr.textureindex + 44 * mesh.material + texture.sznameindex).c_str());
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

					Logger::Info("IncludedModel Read: %s\n", func::ReadMDLString(Stream, includedModelPos + includedmodels[i].sznameindex).c_str());
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
			}

			if (mdlhdr.keyvaluesize > 0)
			{
				Stream->seek(mdlhdr.keyvalueindex);
				Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
				Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
			}
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
			if (mdlhdr.numbones > 0)
			{
				Stream->Read(&linearbone);
				Stream->Read(&linearbonedata, linearbone);
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
					Logger::Info("RuiMesh Read: %s\n", func::ReadMDLString(Stream, ruiPos + 32).c_str());
					Logger::Info("RuiMesh Read: %d\n", j);
				}
			}

			v53Mdl _v53mdl{ mdlhdr,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,ikrules,compressedikerrors,v52compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,textures,cdtextures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata };

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
								if (nextAnim + 1 < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].sectionindex - hdrEndPos;
								if (nextAnim + 1 < mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex == 0 && animdescs[i].numikrules == 0) dist = animDescPos + 100 + animdescs[nextAnim].animindex - hdrEndPos;
								if (nextAnim + 1 > mdlhdr.numlocalanim - 1) dist = mdlhdr.localseqindex - hdrEndPos;

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
						int animDescPos = mdlhdr.localanimindex + 100 * i;
						int animPos = animDescPos + animdescs[i].animindex;
						int secPos = animDescPos + animdescs[i].sectionindex;
						int frames = animdescs[i].numframes;
						int secFrames = animdescs[i].sectionframes;
						std::vector<sectionindexesindex_t_v49> sectionIdxArry;

						int secNumber = (frames / secFrames) + 2;

						Stream->seek(secPos);

						for (int j = 0; j < secNumber; j++)
						{

							sectionindexesindex_t_v49 sectionIdx; Stream->Read(&sectionIdx);
							//Logger::Info("secIdx: %d\n", sectionIdx.sectionoffset);
							sectionIdxArry.push_back(sectionIdx);
						}

						for (int j = 0; j < secNumber; j++)
						{
							sectionindexesindex_t_v49 sectionIdx = sectionIdxArry[j];
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
									if (nextSection < secNumber && nextAnim > mdlhdr.numlocalanim && animdescs[nextAnim].sectionindex > 0 && animdescs[i].numikrules == 0) dist = animDescPos + sectionIdxArry[nextSection].sectionoffset - hdrEndPos;
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
							Stream->seek(animDescPos + animdescs[i].ikruleindex + 152 * j);
							int pos = Stream->Position();
							mstudioikrule_t_v49 v49IkRule; Stream->Read(&v49IkRule);
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
				Stream->Read(&linearbonedata, linearbone);
				Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
			}
			v49Mdl _v49mdl{ mdlhdr,mdlsubhdr,bones,jigglebones,boneflexdrivers,attachments,hitboxsets,hitboxes,bonenametable,animdescs,ikrules,compressedikerrors,ikerrors,ikrulezeroframe,seqdescs,blends,posekeys,events,autolayers,activitymodifiers,seqweightlist,nodenames,nodes,bodyparts,models,meshes,ikchains,iklinks,poseparamdescs,includedmodels,cdtextures,textures,skingroups,keyvalues,srcbonetransforms,linearbone,linearbonedata};

			return _v49mdl;
		}

	}












}



