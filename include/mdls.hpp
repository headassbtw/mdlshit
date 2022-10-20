#pragma once
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
using namespace std;
namespace MDL
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

		v53Mdl _v53Mdl(BinaryReader* Stream, bool debug);

		v53Mdl::v53Mdl(studiohdr_t_v53 _mdlhdr, std::vector<mstudiobone_t_v53> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v53> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v53> _animdescs, std::vector<mstudioanim_t_v53> _anims, std::vector<sectionindexes_t_v53> _sectionindexes, std::vector<mstudioanim_t_v53> _sections, std::vector<mstudioikrule_t_v53> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudiocompressedikerror_t_v52> _v52compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdesc_t_v53> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v53> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v53> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexture_t_v53> _textures, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v53 _linearbonedata)
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

		std::vector<int> ikRuleStairsPerAnim;
		std::vector<int> hdrBytesAnimDescAdd;
		std::vector<int> secHdrBytesAnimDescAdd;
		std::vector<int> secHdrBytesSecAdd;

		int animByteAddedTotal = v53GetTotalAnimHdrBytesAdded();
		int animSecByteAddedTotal = v53GetTotalSecHdrBytesAdded();

		int animBytesAdded = 0;
		int bytesAddedToRuiMesh = 0;
		int bytesAddedToIkRules = 0;
		int bytesAddedToHeader = 52;
		int bytesAddedToBones = 0;
		int bytesAddedToAnims = 0;
		int bytesAddedToAnimData = 0;
		int bytesAddedToSections = 0;
		int bytesAddedToSeqs = 0;
		int bytesAddedToTextures = 0;
		int bytesAddedToIkChains = 0;
		int bytesAddedToActMods = 0;
		int textureFiller = 0;
		int strFiller = 60;
		int allBytesAdded = 0;
		int numOfLinks = 0;

		void SetMdlInts();

		int ConvertFlag(int flag);

		std::vector<int> GetIkChainBones();

		std::vector<int> v53GetAnimHdrBytesAdded(bool zeroFirst);

		int v53GetTotalAnimHdrBytesAdded();

		int v53GetAnimHdrBytesAddedIdv(int anim);

		std::vector<int> v53GetSecHdrBytesAdded(bool zeroFirst);

		int v53GetTotalSecHdrBytesAdded();

		int v53GetSecHdrBytesAddedIdv(int anim);

		std::vector<int> v53GetSecBytesAdded(bool zeroFirst);

		std::vector<int> v53IkRuleStairsPerAnim();

		studiohdr_t_v53 ConvertHeader(FileInfo info);

		std::vector<mstudiobone_t_v53> BoneConversion();

		std::vector<mstudiobbox_t_v53> HitboxConversion();

		std::vector<mstudioanimdesc_t_v53> AnimDescConversion();

		std::vector<mstudioseqdesc_t_v53> SeqDescConversion();

		std::vector<mstudioactivitymodifier_t_v53> ActivityModifierConversion();

		std::vector<mstudioikrule_t_v53> IkRuleConversion();

		std::vector<mstudioanim_t_v53> ConvertAnims();

		std::vector<sectionindexes_t_v53> ConvertSectionIndexes();

		std::vector<mstudioanim_t_v53> ConvertSections();

		std::vector<mstudioikchain_t_v53> IkChainConversion();

		void AttachmentConversion();

		void SeqEventConversion();

		void ConvertNodeNames();

		void ConvertBodyParts();

		void ConvertModels();

		void ConvertMeshes();

		void ConvertPoseParamDescs();

		void ConvertIncludeModel();

		void ConvertCDTextures();

		std::vector<mstudiotexture_t_v53> ConvertTextures();

		void ConvertSrcBoneTransforms();

		void UpdateMdl();

		v49Mdl _v49Mdl(BinaryReader* Stream, bool debug);

		v49Mdl(studiohdr_t_v49 _mdlhdr, studiohdr2_t_v49 _mdlsubhdr, std::vector<mstudiobone_t_v49> _bones, std::vector<mstudiojigglebone_t_v49> _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, std::vector<mstudioattachment_t_v49> _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v49> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v49> _animdescs, std::vector<mstudioanim_t_v49> _anims, std::vector<sectionindexesindex_t_v49> _sectionindexes, std::vector<mstudioanim_t_v49> _sections, std::vector<mstudioikrule_t_v49> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdescv49_t> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v49> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v49> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudiotexture_t_v49> _textures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata)
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
		}
	};
}
