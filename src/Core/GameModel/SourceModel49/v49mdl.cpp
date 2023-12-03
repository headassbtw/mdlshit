/*#include <mdls.hpp>
#include <JsonTest.h>
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
#include <MLUtil.h>
#pragma region MDLV49

void MDL::v49Mdl::ReadMdl(BinaryReader* Stream)
{
	studiohdr_t_v49	 mdlhdr; Stream->Read(&mdlhdr);
	studiohdr2_t_v49 mdlsubhdr; Stream->Read(&mdlsubhdr);

	mstudiobone_t_v49*									bones = new mstudiobone_t_v49[mdlhdr.numbones];
	mstudiojigglebone_t_v49*							jigglebones = {};
	mstudioboneflexdriver_t_v49*						boneflexdrivers = new mstudioboneflexdriver_t_v49[mdlsubhdr.m_nBoneFlexDriverCount];
	mstudioattachment_t_v49*							attachments = new mstudioattachment_t_v49[mdlhdr.numlocalattachments];
	mstudiohitboxset_t_v49*								hitboxsets = new mstudiohitboxset_t_v49[mdlhdr.numhitboxsets];
	mstudiobbox_t_v49*									hitboxes = GetHitboxes(Stream); //Add function for getting number of hitboxes from hitbox sets. - Liberty
	unsigned char*										bonenametable = new unsigned char[mdlhdr.numbones]; //Replace with unsigned list instead of struct by itself. - Liberty
	mstudioanimdesc_t_v49*								animdescs = new mstudioanimdesc_t_v49[mdlhdr.numlocalanim];
	std::vector<mstudioanim_t_v49>						anims; //Add function for gathering anims. - Liberty
	std::vector<sectionindexesindex_t_v49>				sectionindexes; //Add function for gathering anims. - Liberty
	std::vector<mstudioanim_t_v49>						sections; //Add function for gathering anims. - Liberty
	std::vector<mstudioikrule_t_v49>					ikrules; //Add function for gathering anims. - Liberty
	std::vector<mstudiocompressedikerror_t_v49>			compressedikerrors; //Add function for gathering anims. - Liberty
	std::vector<mstudioikerror_t_v49>					ikerrors; //Add function for gathering anims. - Liberty
	std::vector<mstudioikrulezeroframe_t_v49>			ikrulezeroframe; //Add function for gathering anims. - Liberty
	mstudioseqdescv49_t*								seqdescs = new mstudioseqdescv49_t[mdlhdr.numlocalseq];
	std::vector<blendgroup_t_v49>						blends; //Add function to get seq data - Liberty
	std::vector<posekey_t_v49>							posekeys; //Add function to get seq data - Liberty
	std::vector<mstudioevent_t_v49>						events; //Add function to get seq data - Liberty
	std::vector<mstudioautolayer_t_v49>					autolayers; //Add function to get seq data - Liberty
	std::vector<mstudioactivitymodifier_t_v49>			activitymodifiers; //Add function to get seq data - Liberty
	std::vector<seqweightlist_t_v49>					seqweightlist; //Add function to get seq data - Liberty
	mstudionodename_t_v49*								nodenames = new mstudionodename_t_v49[mdlhdr.numlocalnodes];
	mstudionodedata_v49*								nodes = new mstudionodedata_v49[mdlhdr.numlocalnodes];
	mstudiobodyparts_t_v49*								bodyparts = new mstudiobodyparts_t_v49[mdlhdr.numbodyparts];
	std::vector<mstudiomodel_t_v49>						models; //Make function to get model data - Liberty
	std::vector<mstudiomesh_t_v49>						meshes; //Make function to get model data - Liberty
	mstudioikchain_t_v49*								ikchains = new mstudioikchain_t_v49[mdlhdr.numikchains];
	std::vector<mstudioiklink_t_v49>					iklinks; //Make function to get ikchain data - Liberty
	mstudioposeparamdesc_t_v49*							poseparamdescs = new mstudioposeparamdesc_t_v49[mdlhdr.numlocalposeparameters];
	mstudioanimblock_t*									animBlocks = new mstudioanimblock_t[mdlhdr.numanimblocks];
	mstudiomodelgroup_t_v49*							includedmodels = new mstudiomodelgroup_t_v49[mdlhdr.numincludemodels];
	mstudiotexturedir_t_v49*							cdtextures = new mstudiotexturedir_t_v49[mdlhdr.numcdtextures];
	mstudiotexture_t_v49*								textures = new mstudiotexture_t_v49[mdlhdr.numtextures];
	mstudioskingroup_t_v49*								skingroups = new mstudioskingroup_t_v49[mdlhdr.numskinfamilies];
	mstudiokeyvalues_t_v49								keyvalues;
	mstudiosrcbonetransform_t_v49*						srcbonetransforms = new mstudiosrcbonetransform_t_v49[mdlsubhdr.numsrcbonetransform];
	mstudiolinearbone_t_v49								linearbone;
	mstudiolinearbonedata_t_v49							linearbonedata;
}

mstudiobbox_t_v49* MDL::v49Mdl::GetHitboxes(BinaryReader* Stream)
{
	bool debug = true;
	bool dumpData = true;
	Stream->seek(0); //Just in case of wonky bs with seeking. - Liberty

	studiohdr_t_v49	 mdlhdr; Stream->Read(&mdlhdr);
	studiohdr2_t_v49 mdlsubhdr; Stream->Read(&mdlsubhdr);

	int tempNumHitboxes = 0;

	Stream->seek(mdlhdr.hitboxsetindex);

	if (DEBUG)
	{
		Logger::Debug("HITBOX GETTER INFO:\n");
		Logger::Info(":--------------------------------: \n");
	}


	for (int i = 0; i < mdlhdr.numhitboxsets; i++)
	{
		if (DEBUG)
		{
			Logger::Notice(":----------- DATA START -----------: \n");
			Logger::Info("HITBOX TOTAL FROM SET AMOUNT BEFORE: %d | Idx: %d\n", tempNumHitboxes, i);

			mstudiohitboxset_t_v49 hboxSet; Stream->Read(&hboxSet);
			tempNumHitboxes += hboxSet.numhitboxes;

			//Logger::Notice(":----------- DATA SEPARATOR -----------: \n");
			Logger::Info(":--------------------------------: \n");

			Logger::Info("HITBOX TOTAL FROM SET AMOUNT AFTER: %d | Idx: %d\n", tempNumHitboxes, i);

			Logger::Notice(":----------- DATA END -----------: \n");

			Logger::Info(":--------------------------------: \n");
		}
		else
		{
			mstudiohitboxset_t_v49 hboxSet; Stream->Read(&hboxSet);
			tempNumHitboxes += hboxSet.numhitboxes;
		}
	}

	if (DEBUG)
	{
		Logger::Notice(":----------- DATA START -----------: \n");
		Logger::Info("HITBOX AMOUNT TOTAL: %d\n", tempNumHitboxes);
		Logger::Notice(":----------- DATA END -----------: \n");
		Logger::Info(":--------------------------------: \n");
	}

	mstudiobbox_t_v49*	hitboxes = new mstudiobbox_t_v49[tempNumHitboxes]; //Add function for getting number of hitboxes from hitbox sets. - Liberty

	if (DEBUG)
	{
		Logger::Notice(":----------- DATA START -----------: \n");
		Logger::Info("HITBOX ASSIGNMENT TO RETURN ARRAY DATA: \n");
		Logger::Notice(":----------- DATA END -----------: \n");
		Logger::Info  (":--------------------------------: \n");
	}

	for (int i = 0; i < tempNumHitboxes; i++)
	{
		if (DEBUG)
		{
			Logger::Debug(":----------- DATA START -----------: \n");
			Logger::Info("HITBOX SETTER AMOUNT SYNC CHECK: Idx: %d | Max: %d\n", i, tempNumHitboxes);

			//Logger::Notice(":----------- DATA SEPARATOR -----------: \n");
			Logger::Info(":--------------------------------: \n");

			Logger::Info("HITBOX SETTER DATA CHECK BEFORE: DATA: %d | Idx: %d\n", hitboxes[i].bone <= 255 && hitboxes[i].bone > 0  ? hitboxes[i].bone : -1, i);
			mstudiobbox_t_v49 hbox; Stream->Read(&hbox);
			hitboxes[i] = hbox;

			//Logger::Notice(":----------- DATA SEPARATOR -----------: \n");
			Logger::Info(":--------------------------------: \n");

			Logger::Info("HITBOX SETTER DATA CHECK AFTER:  DATA: %d | Idx: %d\n", hitboxes[i].bone, i);
			Logger::Notice(":----------- DATA END -----------: \n");
			Logger::Info(":--------------------------------: \n");

			if (dumpData)
			{
				int hitboxPos = mdlhdr.hitboxsetindex + (12 * mdlhdr.numhitboxsets) + (68 * i);

				Logger::Debug(":----------- DUMPING HITBOX DATA #%d -----------: \n", i);
				Logger::Info(":--------------------------------: \n");
				Logger::Notice(":----------- DATA START -----------: \n");
				Logger::Info("HITBOX[ %d ].bone: %d  \n", i, hbox.bone);
				Logger::Info("HITBOX[ %d ].group: %d  \n", i, hbox.group);
				Logger::Info("HITBOX[ %d ].bbmin: < x: %f, y: %f, z: %f >  \n", i, hbox.bbmin.x, hbox.bbmin.y, hbox.bbmin.z);
				Logger::Info("HITBOX[ %d ].bbmax: < x: %f, y: %f, z: %f >  \n", i, hbox.bbmax.x, hbox.bbmax.y, hbox.bbmax.z);
				Logger::Info("HITBOX[ %d ].szhitboxnameindex: %d StrNamePos: %d | %s\n", i, hbox.szhitboxnameindex, hitboxPos + hbox.szhitboxnameindex, hbox.szhitboxname != "" ? hbox.szhitboxname.c_str() : "Empty");
				Logger::Info("HITBOX[ %d ].Unused: int[%d]  \n", i, 8);
				Logger::Notice(":----------- DATA END -----------: \n");
				Logger::Info(":--------------------------------: \n");
			}


		}
		else
		{
			mstudiobbox_t_v49 hbox; Stream->Read(&hbox);
			hitboxes[i] = hbox;
		}
	}

	return hitboxes;
}

mstudiobone_t_v49* GetBones(BinaryReader* Stream)
{
	Stream->seek(0); //Just in case of wonky bs with seeking. - Liberty

	studiohdr_t_v49	 mdlhdr; Stream->Read(&mdlhdr);
	studiohdr2_t_v49 mdlsubhdr; Stream->Read(&mdlsubhdr);
	mstudiobone_t_v49* bones = new mstudiobone_t_v49[mdlhdr.numbones];

	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 216 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v49 bone; Stream->Read(&bone);
			//bones.push_back(bone);
			bones[i] = bone;
			//if (bone.proctype == 5) numjigglebones++;

			std::string szName = Stream->ReadNullTermStrTrgt(bonePos + bone.sznameindex, false);

			Logger::Info("Bone Read: %s : %d\n", szName.c_str(), i);
		}
	}
}

/*
std::vector <int> MDL::v49Mdl::GetIKBones()
{
	std::vector <int> chain;
	std::vector <int> chainBones;
	std::vector <int> roots;

	for (int i = 0; i < iklinks.size(); i++)
	{
		mstudioiklink_t_v49 iklink = iklinks[i];
		roots.push_back(iklink.bone);
	}

	for (int i = 0; i < roots.size(); i++)
	{
//		Logger::Info("Root Chain Bone Read: %d\n", roots[i]);
		chain.push_back(roots[i]);
	}

	for (int i = 0; i < mdlhdr.numbones; i++)
	{
		mstudiobone_t_v49 _bone = bones[i];

		if ( contains(roots, _bone.parent) && !contains(chain, i))
		{
			chain.push_back(i);
//			Logger::Info("Child of Root Chain Bone Read: %d | Root: %d\n", i, _bone.parent);
		}

		if (contains(chain, _bone.parent) && !contains(chain, i))
		{
			chain.push_back(i);
//			Logger::Info("Child of Root Chain Bone Child Read: %d | Parent: %d\n", i, _bone.parent);
		}

	}

	for (int i = 0; i < chain.size(); i++)
	{
		if (!contains(chainBones, chain[i]))
		{
			chainBones.push_back(chain[i]);
		}
	}
//	Logger::Info("Total ChainBones: %d\n", chainBones.size());

	return chainBones;
}

void MDL::v49Mdl::v49ExtractSrcBoneTransforms(BinaryReader* Stream)
{
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		std::string fileName = Stream->_filename;
		fileName.resize(fileName.size() - 4);
		fileName.insert(fileName.size(), ".srcbtf");
		BinaryWriter Bing = BinaryWriter(fileName.c_str());

		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			Bing.Write(srcbonetransforms[i]);
		}
	}
}

MDL::v49Mdl::v49Mdl()
{

}

MDL::v49Mdl::v49Mdl(studiohdr_t_v49 _mdlhdr, studiohdr2_t_v49 _mdlsubhdr, mstudiobone_t_v49* _bones, mstudiojigglebone_t_v49* _jigglebones, std::vector<mstudioboneflexdriver_t_v49>	_boneflexdrivers, mstudioattachment_t_v49* _attachments, std::vector<mstudiohitboxset_t_v49> _hitboxsets, std::vector<mstudiobbox_t_v49> _hitboxes, mstudiobonenametable_t_v49 _bonenametable, std::vector<mstudioanimdesc_t_v49> _animdescs, std::vector<mstudioanim_t_v49> _anims, std::vector<sectionindexesindex_t_v49> _sectionindexes, std::vector<mstudioanim_t_v49> _sections, std::vector<mstudioikrule_t_v49> _ikrules, std::vector<mstudiocompressedikerror_t_v49> _compressedikerrors, std::vector<mstudioikerror_t_v49> _ikerrors, std::vector<mstudioikrulezeroframe_t_v49> _ikrulezeroframe, std::vector<mstudioseqdescv49_t> _seqdescs, std::vector<blendgroup_t_v49> _blends, std::vector<posekey_t_v49> _posekeys, std::vector<mstudioevent_t_v49> _events, std::vector<mstudioautolayer_t_v49> _autolayers, std::vector<mstudioactivitymodifier_t_v49> _activitymodifiers, std::vector<seqweightlist_t_v49> _seqweightlist, std::vector<mstudionodename_t_v49> _nodenames, std::vector<mstudionodedata_v49> _nodes, std::vector<mstudiobodyparts_t_v49> _bodyparts, std::vector<mstudiomodel_t_v49>	_models, std::vector<mstudiomesh_t_v49> _meshes, std::vector<mstudioikchain_t_v49> _ikchains, std::vector<mstudioiklink_t_v49> _iklinks, std::vector<mstudioposeparamdesc_t_v49> _poseparamdescs, std::vector<mstudioanimblock_t> _animblocks, std::vector<mstudiomodelgroup_t_v49> _includedmodels, std::vector<mstudiotexturedir_t_v49> _cdtextures, std::vector<mstudiotexture_t_v49> _textures, std::vector<mstudioskingroup_t_v49> _skingroups, mstudiokeyvalues_t_v49 _keyvalues, std::vector<mstudiosrcbonetransform_t_v49> _srcbonetransforms, mstudiolinearbone_t_v49 _linearbone, mstudiolinearbonedata_t_v49 _linearbonedata, mstudiostringtable_t_v49 _stringtable)
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
	animblocks = _animblocks;
	includedmodels = _includedmodels;
	cdtextures = _cdtextures;
	textures = _textures;
	skingroups = _skingroups;
	keyvalues = _keyvalues;
	srcbonetransforms = _srcbonetransforms;
	linearbone = _linearbone;
	linearbonedata = _linearbonedata;
	stringtable = _stringtable;
}

MDL::v49Mdl MDL::v49Mdl::_v49Mdl(BinaryReader* Stream, bool debug)
{
	Stream->seek(0);
#pragma region VARS
	studiohdr_t_v49	 mdlhdr; Stream->Read(&mdlhdr);
	studiohdr2_t_v49 mdlsubhdr; Stream->Read(&mdlsubhdr);
	
	mstudiobone_t_v49*									bones = new mstudiobone_t_v49[mdlhdr.numbones];
	mstudiojigglebone_t_v49*							jigglebones = {};
	std::vector<mstudioboneflexdriver_t_v49>			boneflexdrivers;
	mstudioattachment_t_v49*							attachments = new mstudioattachment_t_v49[mdlhdr.numlocalattachments];
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
	std::vector<mstudioanimblock_t>						animBlocks;
	std::vector<mstudiomodelgroup_t_v49>				includedmodels;
	std::vector<mstudiotexturedir_t_v49>				cdtextures;
	std::vector<mstudiotexture_t_v49>					textures;
	std::vector<mstudioskingroup_t_v49>					skingroups;
	mstudiokeyvalues_t_v49								keyvalues;
	std::vector<mstudiosrcbonetransform_t_v49>			srcbonetransforms;
	mstudiolinearbone_t_v49								linearbone;
	mstudiolinearbonedata_t_v49							linearbonedata;

	std::vector<int>									weightlistidxs;

	std::vector<int>									bytesAddedToWeightLists;

//	int weightListPos = 0;

	int numjigglebones = 0;

	int numhitboxes = 0;

	int nummodels = 0;

	int nummeshes = 0;

	int numiklinks = 0;

	int numiklocks = 0;

	int numikrules = 0;

#pragma endregion
#pragma region BONES
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlhdr.boneindex);
		for (int i = 0; i < mdlhdr.numbones; i++)
		{
			int bonePos = mdlhdr.boneindex + 216 * i;
			Stream->seek(bonePos);

			mstudiobone_t_v49 bone; Stream->Read(&bone);
			//bones.push_back(bone);
			bones[i] = bone;
			if (bone.proctype == 5) numjigglebones++;

			std::string szName = Stream->ReadNullTermStrTrgt(bonePos + bone.sznameindex, false);

			Logger::Info("Bone Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region JIGGLEBONES
	if (numjigglebones > 0)
	{
		jigglebones = new mstudiojigglebone_t_v49[numjigglebones];

		int jiggleBonePos = mdlhdr.boneindex + 216 * mdlhdr.numbones;

		Stream->seek(jiggleBonePos);

		for (int i = 0; i < numjigglebones; i++)
		{
			mstudiojigglebone_t_v49 jigglebone; Stream->Read(&jigglebone);
			Logger::Info("JiggleBoneFlags Read: %s\n", Utility::func::ReadJiggleFlags(jigglebone.flags).c_str());
			jigglebones[i] = jigglebone; //jigglebones.push_back(jigglebone);
			Logger::Info("JiggleBone Read: %d\n", i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
#pragma endregion
#pragma region ATTACHMENTS
	if (mdlhdr.numlocalattachments > 0)
	{
		Stream->seek(mdlhdr.localattachmentindex);
		for (int i = 0; i < mdlhdr.numlocalattachments; i++)
		{
			int attachmentPos = mdlhdr.localattachmentindex + 92 * i;
			Stream->seek(attachmentPos);

			mstudioattachment_t_v49 attachment; Stream->Read(&attachment);

			std::string szName = Stream->ReadNullTermStrTrgt(attachmentPos + attachment.sznameindex, false);
			attachment.szname = szName;

			attachments[i] = attachment;//attachments.push_back(attachment);
			Logger::Info("Attachment Read: %s : %d\n", attachment.szname.c_str(), i);
		}
		//Logger::Info("strPos: %d\n", Stream->Position());
	}
#pragma endregion
#pragma region HITBOXSETS
	if (mdlhdr.numhitboxsets > 0)
	{
		Logger::Info("Made it!\n");
		Stream->seek(mdlhdr.hitboxsetindex);
		Logger::Info("Made it!\n");
		for (int i = 0; i < mdlhdr.numhitboxsets; i++)
		{
			int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * i;
			Stream->seek(hitboxsetPos);
			Logger::Info("Made it2!\n");

			mstudiohitboxset_t_v49 hitboxset; Stream->Read(&hitboxset);

			mstudiobbox_t_v49* _temp = new mstudiobbox_t_v49[hitboxset.numhitboxes];

			if (hitboxset.numhitboxes > 0)
			{
				numhitboxes += hitboxset.numhitboxes;

				Stream->seek(hitboxsetPos + hitboxset.hitboxindex);
				for (int j = 0; j < hitboxset.numhitboxes; j++)
				{
					int hitboxPos = hitboxsetPos + hitboxset.hitboxindex + 68 * j;
					Stream->seek(hitboxPos);
					mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);

					std::string szHitboxName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
					hitbox.szhitboxname = szHitboxName;

					_temp[j] = hitbox;
				}

				hitboxset.hitboxes = _temp;
			}
			else
			{
				Stream->seek(hitboxsetPos + hitboxset.hitboxindex);
				delete[] _temp;
			}


			std::string szName = Stream->ReadNullTermStrTrgt(hitboxsetPos + hitboxset.sznameindex, false);
			hitboxset.szname = szName;

			hitboxsets.push_back(hitboxset);
			Logger::Info("HitboxSet Read: %s : %d\n", szName.c_str(), i);
		}
#pragma endregion
#pragma region HITBOXES
		if (numhitboxes > 0)
		{
			for (int i = 0; i < numhitboxes; i++)
			{
				int hitboxsetPos = mdlhdr.hitboxsetindex + 12 * mdlhdr.numhitboxsets;
				int hitboxPos = hitboxsetPos + 68 * i;
				Stream->seek(hitboxPos);
				mstudiobbox_t_v49 hitbox; Stream->Read(&hitbox);

				std::string szName = Stream->ReadNullTermStrTrgt(hitboxPos + hitbox.szhitboxnameindex, false);
				hitbox.szhitboxname = szName;

				hitboxes.push_back(hitbox);
				Logger::Info("Hitbox Read: %s : %d\n", hitbox.szhitboxname.c_str(), i);
			}
		}
		//Logger::Info("strPos: %d\n", Stream->Position());

	}
#pragma endregion
#pragma region BONENAMETABLE
	if (mdlhdr.numbones > 0)
	{
		Stream->Read(&bonenametable, mdlhdr.numbones);
		Logger::Info("BoneTable Read: %d\n", mdlhdr.numbones);
	}
#pragma endregion
#pragma region ANIMDESCS
	if (mdlhdr.numlocalanim > 0)
	{
		Stream->seek(mdlhdr.localanimindex);
		for (int i = 0; i < mdlhdr.numlocalanim; i++)
		{
			int animDescPos = mdlhdr.localanimindex + 100 * i;
			Stream->seek(animDescPos);

			mstudioanimdesc_t_v49 animdesc; Stream->Read(&animdesc);

			std::string szName = Stream->ReadNullTermStrTrgt(animDescPos + animdesc.sznameindex, false);
			animdesc.szname = szName;

			animdescs.push_back(animdesc);

			Logger::Info("AnimDesc Read: %s : %d\n", szName.c_str(), i);
			//Logger::Info("strPos: %d\n", Stream->Position());
		}
#pragma endregion
#pragma region ANIMS
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
#pragma endregion
#pragma region SECTIONS
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
#pragma endregion
#pragma region IKRULES
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
#pragma endregion
#pragma region COMPRESSEDIKERRORS
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
							v49CompressedIkError.animdata = animData;
							compressedikerrors.push_back(v49CompressedIkError);
							Logger::Info("CompressedIkError Read: %d\n", j);
							break;
						}
//						Logger::Info("CompressedIkError Read: %d\n", j);
						dist = errorIdxs[next] - endPos;
						Stream->Read(&animData, dist);
						v49CompressedIkError.animdata = animData;
						compressedikerrors.push_back(v49CompressedIkError);
					}
				}

			}

		}
	}
#pragma endregion
#pragma region SEQDESCS
	#pragma region SEQDESC
		if (mdlhdr.numlocalseq > 0)
		{
			int actModNum = 0;
			int eventNum = 0;
	
			Stream->seek(mdlhdr.localseqindex);
			for (int i = 0; i < mdlhdr.numlocalseq; i++)
			{
				int seqPos = mdlhdr.localseqindex + 212 * i;
				Stream->seek(seqPos);
				mstudioseqdescv49_t seqDesc; Stream->Read(&seqDesc);
				seqDesc.szlabel = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szlabelindex, false);
				seqDesc.szactivityname = Stream->ReadNullTermStrTrgt(seqPos + seqDesc.szactivitynameindex, false);//ReadMDLString(Stream, seqPos + seqDesc.szactivitynameindex);
	
				seqdescs.push_back(seqDesc);
	
				Logger::Info("Seq Read: %s : %s : %d\n", seqDesc.szlabel.c_str(), seqDesc.szactivityname.c_str(), i);
			}
	#pragma endregion
	#pragma region SEQDATA
			for (int i = 0; i < mdlhdr.numlocalseq; i++)
			{
				#pragma region VARS
							int seqPos = mdlhdr.localseqindex + 212 * i;
							int numBlends = seqdescs[i].groupsize[0] * seqdescs[i].groupsize[1];
							int posekeySize = seqdescs[i].groupsize[0] + seqdescs[i].groupsize[1];
							int numAutolayers = seqdescs[i].numautolayers;
							int numActMods = seqdescs[i].numactivitymodifiers;
							int numEvents = seqdescs[i].numevents;
				#pragma endregion
				#pragma region WEIGHTLIST
							if (seqdescs[i].weightlistindex > 0)
							{
								Stream->seek(seqPos + seqdescs[i].weightlistindex);
								seqweightlist_t_v49 weightlist; Stream->Read(&weightlist, mdlhdr.numbones);
								seqweightlist.push_back(weightlist);
								Logger::Info("Weightlist Read: %d\n", i);
							}
				#pragma endregion
				#pragma region BLENDS
							if (seqdescs[i].animindexindex > 0)
							{
								Stream->seek(seqPos + seqdescs[i].animindexindex);
								blendgroup_t_v49 blendgroup; Stream->Read(&blendgroup, numBlends);
								blends.push_back(blendgroup);
								seqdescs[i].blends = blendgroup;
								Logger::Info("BlendGroup Read: %d\n", i);
							}
				#pragma endregion
				#pragma region POSEKEY
							if (seqdescs[i].posekeyindex > 0)
							{
								Stream->seek(seqPos + seqdescs[i].posekeyindex);
								posekey_t_v49 posekey; Stream->Read(&posekey, posekeySize);
								if (posekeySize == 3)posekey.unk[2] == 0.0;
				//				Logger::Info("PosekeyF Read: %f\n", posekey.unk[2]);
								posekeys.push_back(posekey);
								Logger::Info("Posekey Read: %d\n", i);
							}
				#pragma endregion
				#pragma region AUTOLAYER
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
				#pragma endregion
				#pragma region EVENTS
							string* _tempEvents = new string[seqdescs[i].numevents];
							if (numEvents > 0)
							{
								Stream->seek(seqPos + seqdescs[i].eventindex);
								for (int j = 0; j < seqdescs[i].numevents; j++)
								{
									mstudioevent_t_v49 _event; Stream->Read(&_event);
									events.push_back(_event);
				
									std::string szEventName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + _event.szeventindex, false);
									std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].eventindex + 80 * j + 12, false);
									Logger::Info("Pos: %d\n",Stream->Position());
									if (szEventName != "")_tempEvents[j] = szEventName;
									else _tempEvents[j] = (std::string)"Empty";
									seqdescs[i].events.push_back(eventNum);
				
									Logger::Info("Event Read: %s : %s : %d\n", szName.c_str(), _tempEvents[j].c_str(), j);
									eventNum++;
								}
							}
							if (seqdescs[i].szeventnames != nullptr)
							{
								delete[] seqdescs[i].szeventnames;
							}

							seqdescs[i].szeventnames = _tempEvents;
				#pragma endregion
				#pragma region ACTIVITYMODIFIERS
							if (numActMods > 0)
							{
								Stream->seek(seqPos + seqdescs[i].activitymodifierindex);
								for (int j = 0; j < seqdescs[i].numactivitymodifiers; j++)
								{
									mstudioactivitymodifier_t_v49 actmod; Stream->Read(&actmod);
									
									std::string szName = Stream->ReadNullTermStrTrgt(seqPos + seqdescs[i].activitymodifierindex + 4 * j + actmod.sznameindex, false);
									seqdescs[i].szactivitymodifiernames.push_back(szName);
									actmod.szname = szName;
									activitymodifiers.push_back(actmod);
									seqdescs[i].actmods.push_back(actModNum);
									Logger::Info("ActMod Read: %s : %d\n", seqdescs[i].szactivitymodifiernames[j].c_str(), j);
									actModNum++;
								}
							}


				#pragma endregion
			}
		}
	#pragma endregion
#pragma endregion
#pragma region NODENAMES
	if (mdlhdr.numlocalnodes > 0)
	{
		Stream->seek(mdlhdr.localnodenameindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodename_t_v49 nodeName; Stream->Read(&nodeName);

			std::string szName = Stream->ReadNullTermStrTrgt(nodeName.sznameindex, false);
			nodeName.szname = szName;
			nodenames.push_back(nodeName);
			Logger::Info("NodeName Read: %s : %d\n", nodeName.szname.c_str(), i);
		}
#pragma endregion
#pragma region NODEDATA
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numlocalnodes; i++)
		{
			mstudionodedata_v49 node; Stream->Read(&node, mdlhdr.numlocalnodes * mdlhdr.numlocalnodes);
			nodes.push_back(node);
			Logger::Info("Node Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region BODYPARTS
	if (mdlhdr.numbodyparts > 0)
	{
		Stream->seek(mdlhdr.bodypartindex);
		for (int i = 0; i < mdlhdr.numbodyparts; i++)
		{
			mstudiobodyparts_t_v49 bodyPart; Stream->Read(&bodyPart);
			if (bodyPart.nummodels > 0) nummodels += bodyPart.nummodels;

			std::string szName = Stream->ReadNullTermStrTrgt(mdlhdr.bodypartindex + 16 * i + bodyPart.sznameindex, false);
			bodyPart.szname = szName;

			bodyparts.push_back(bodyPart);
			Logger::Info("Bodypart Read: %s : %d\n", bodyPart.szname.c_str(), i);
		}
	}
#pragma endregion
#pragma region MODELS
	if (nummodels > 0)
	{
		Stream->seek(mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts);
		for (int i = 0; i < nummodels; i++)
		{
			mstudiomodel_t_v49 model; Stream->Read(&model);
			if (model.nummeshes > 0) nummeshes += model.nummeshes;
			models.push_back(model);
			int modelPos = mdlhdr.bodypartindex + 16 * mdlhdr.numbodyparts + 148 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(modelPos, false);

			Logger::Info("Model Read: %s : %d\n", szName.c_str(), i);
		}

	}
#pragma endregion
#pragma region IKCHAINS
	if (mdlhdr.numikchains > 0)
	{
		std::vector <int> ikRoots;
		Stream->seek(mdlhdr.ikchainindex);
		for (int i = 0; i < mdlhdr.numikchains; i++)
		{
			mstudioikchain_t_v49 ikchain; Stream->Read(&ikchain);
			if (ikchain.numlinks > 0) numiklinks += ikchain.numlinks;

			int ikChainPos = mdlhdr.ikchainindex + 16 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(ikChainPos + ikchain.sznameindex, false);
			ikchain.szname = szName;

			ikchains.push_back(ikchain);
			Logger::Info("IkChain Read: %s : %d\n", szName.c_str(), i);
		}
#pragma endregion
#pragma region IKLINKS
		for (int i = 0; i < numiklinks; i++)
		{
			int ikLinkPos = (mdlhdr.ikchainindex + 16 * mdlhdr.numikchains) + 28 * i;
			Stream->seek(ikLinkPos);
			mstudioiklink_t_v49 iklink; Stream->Read(&iklink);
			iklinks.push_back(iklink);
			Logger::Info("IkLink Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region POSEPARAMDESCS
	if (mdlhdr.localposeparamindex > 0)
	{
		Stream->seek(mdlhdr.localposeparamindex);
		for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
		{
			mstudioposeparamdesc_t_v49 poseparamdesc; Stream->Read(&poseparamdesc);
			int poseParamPos = mdlhdr.localposeparamindex + 20 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(poseParamPos + poseparamdesc.sznameindex, false);
			poseparamdesc.szname = szName;

			poseparamdescs.push_back(poseparamdesc);
			Logger::Info("PoseParamDesc Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region ANIMBLOCKS
	if (mdlhdr.numanimblocks > 0)
	{
		Stream->seek(mdlhdr.localnodeindex);
		for (int i = 0; i < mdlhdr.numanimblocks; i++)
		{
			mstudioanimblock_t animBlock; Stream->Read(&animBlock);
			animblocks.push_back(animBlock);
			Logger::Info("AnimBlock Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region MESHES
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

			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);

			Logger::Info("MeshMat Read: %s : %d\n", szName.c_str(), i);
			Logger::Info("Mesh Read: %d\n", i);
		}
	}
#pragma endregion
#pragma region INCLUDEMODEL
	if (mdlhdr.numincludemodels > 0)
	{
		Stream->seek(mdlhdr.includemodelindex);
		for (int i = 0; i < mdlhdr.numincludemodels; i++)
		{
			mstudiomodelgroup_t_v49 includemodel; Stream->Read(&includemodel);
			int includeModelPos = mdlhdr.includemodelindex + 8 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(includeModelPos + includemodel.sznameindex, false);
			includemodel.szname = szName;

			includedmodels.push_back(includemodel);
			Logger::Info("IncludedModel Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region TEXTURES
	if (mdlhdr.numtextures > 0)
	{
		Stream->seek(mdlhdr.textureindex);
		for (int i = 0; i < mdlhdr.numtextures; i++)
		{
			mstudiotexture_t_v49 texture; Stream->Read(&texture);
			int texturePos = mdlhdr.textureindex + 64 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(texturePos + texture.sznameindex, false);
			texture.szname = szName;

			textures.push_back(texture);

			Logger::Info("Texture Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region CDMATERIALS
	if (mdlhdr.numcdtextures > 0)
	{
		Stream->seek(mdlhdr.cdtextureindex);
		for (int i = 0; i < mdlhdr.numcdtextures; i++)
		{
			mstudiotexturedir_t_v49 cdtexture; Stream->Read(&cdtexture);

			std::string szName = Stream->ReadNullTermStrTrgt(cdtexture.sznameindex, false);
			cdtexture.szname = szName;

			cdtextures.push_back(cdtexture);
			Logger::Info("CdTexture Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region SKINFAMILIES
	if (mdlhdr.numskinfamilies > 0 && mdlhdr.numtextures > 0)
	{
		if (mdlhdr.numskinref > 0 && mdlhdr.numtextures > 0)
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
#pragma endregion
#pragma region KEYVALUES
	if (mdlhdr.keyvaluesize > 0)
	{
		Stream->seek(mdlhdr.keyvalueindex);
		Stream->Read(&keyvalues, mdlhdr.keyvaluesize);
		Logger::Info("Keyvalues Read: %d\n", mdlhdr.keyvaluesize);
	}
#pragma endregion
#pragma region SRCBONETRANSFORMS
	if (mdlsubhdr.numsrcbonetransform > 0)
	{
		Stream->seek(mdlsubhdr.srcbonetransformindex);
		for (int i = 0; i < mdlsubhdr.numsrcbonetransform; i++)
		{
			mstudiosrcbonetransform_t_v49 srcbonetransform; Stream->Read(&srcbonetransform);
			int srcBonePos = mdlsubhdr.srcbonetransformindex + 100 * i;

			std::string szName = Stream->ReadNullTermStrTrgt(srcBonePos + srcbonetransform.sznameindex, false);

			srcbonetransforms.push_back(srcbonetransform);
			Logger::Info("SrcBoneTransform Read: %s : %d\n", szName.c_str(), i);
		}
	}
#pragma endregion
#pragma region LINEARBONE
	if (mdlhdr.numbones > 0)
	{
		Stream->seek(mdlsubhdr.linearboneindex + 408);
		Stream->Read(&linearbone);
		Stream->Read(&linearbonedata, mdlhdr.numbones);
		Logger::Info("LinearBone Read: %d\n", mdlhdr.numbones);
	}
#pragma endregion
#pragma region STRINGTABLE
	Stream->seek(mdlsubhdr.sznameindex + 408);
	mstudiostringtable_t_v49 stringTable; Stream->Read(&stringTable, mdlhdr, seqdescs, hitboxsets, attachments, nodenames, bodyparts, ikchains,animdescs, textures, includedmodels, cdtextures, poseparamdescs, srcbonetransforms);
	//std::string string(stringTable.mdlname.begin(), stringTable.mdlname.end());
	//Logger::Info("mldName: %s\n", string.c_str());
#pragma endregion

	MDL::v49Mdl _v49mdl = { mdlhdr, mdlsubhdr, bones, jigglebones, boneflexdrivers, attachments, hitboxsets, hitboxes, bonenametable, animdescs, anims, sectionindexes, sections, ikrules, compressedikerrors, ikerrors, ikrulezeroframe, seqdescs, blends, posekeys, events, autolayers, activitymodifiers, seqweightlist, nodenames, nodes, bodyparts, models, meshes, ikchains, iklinks, poseparamdescs, animBlocks, includedmodels, cdtextures, textures, skingroups, keyvalues, srcbonetransforms, linearbone, linearbonedata, stringTable };
	_v49mdl.numjigglebones = numjigglebones;


	return _v49mdl;
}
#pragma endregion
*/