#pragma once
#include <cstddef>
#include <string>
#include <optional>

struct FileInfo{
	std::optional<std::string> mdl;
	std::optional<std::string> vtx;
	std::optional<std::string> vvd;
	std::optional<std::string> phy;
	std::optional<std::string> out;

	std::optional<int> attachment_override;
	std::optional<int> sequence_override;
	std::optional<int> animation_override;
	std::optional<int> flags_override;

  bool disable_bones = false;
  bool disable_attachments = false;
  bool disable_hitboxsets = false;
  bool disable_animations = false;
  bool disable_sequences = false;
  bool disable_bodyparts = false;
  bool disable_includemodels = false;
  bool disable_textures = false;
};

struct Vector{
  float x, y, z;
};
struct v49_Header{
  int id;
	int version;
	int checksum; 
	char name[64];
	int fileSize;
	Vector				eyeposition;
	Vector				illumposition;
	Vector				hull_min;
	Vector				hull_max;
	Vector				view_bbmin;
	Vector				view_bbmax;
	int					flags;
	int					numbones;
	int					boneindex;
	int					numbonecontrollers;
	int					bonecontrollerindex;
	int					numhitboxsets;
	int					hitboxsetindex;
 	int					numlocalanim;
 	int					localanimindex;

 	int					numlocalseq;
 	int					localseqindex;
 	int			activitylistversion;
 	int			eventsindexed;
 	int					numtextures;
 	int					textureindex;
 	int					numcdtextures;
 	int					cdtextureindex;
 	int					numskinref;
 	int					numskinfamilies;
 	int					skinindex; 
 	int					numbodyparts;
 	int					bodypartindex;
 	int					numlocalattachments;
 	int					localattachmentindex;
 	int					numlocalnodes;
 	int					localnodeindex;
	int					localnodenameindex;
	int					numflexdesc;
	int					flexdescindex;
	int					numflexcontrollers;
	int					flexcontrollerindex;
	int					numflexrules;
	int					flexruleindex;
	int					numikchains;
	int					ikchainindex;
	int					nummouths;
	int					mouthindex;
	int					numlocalposeparameters;
	int					localposeparamindex;
	int					surfacepropindex;
	int					keyvalueindex;
	int					keyvaluesize;
	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	float				mass;
	int					contents;
	int					numincludemodels;
	int					includemodelindex;
	/*was long*/ float virtualModel;
	int					szanimblocknameindex;
	int					numanimblocks;
	int					animblockindex;
	/*was long*/ float animblockModel;
	int					bonetablebynameindex;
	/*was long*/ float pVertexBase;
	/*was long*/ float pIndexBase;
	std::byte				constdirectionallightdot;
	std::byte				rootLOD; 
	std::byte				numAllowedRootLODs;
	std::byte				unused[1];
	int					unused4;
	int					numflexcontrollerui;
	int					flexcontrolleruiindex;
	float				flVertAnimFixedPointScale;
	int			surfacepropLookup;
	int					studiohdr2index;
	int					unused2[1];
};

struct v53_Header{
  int id;
	int version;
	int checksum; 
	int nameCopyOffset; //This is the extra four bytes Respawn added. It is an offset that leads to the start of the string chunk. - Liberty
	char name[64];
	int fileSize;
	Vector				eyeposition;	// ideal eye position
	Vector				illumposition;	// illumination center
	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;
	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;
	int					flags;          //Anything above 0 will activate glow and make vmts useless. - Liberty
	int					numbones;			// bones
	int					boneindex;
	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	int					numhitboxsets;
	int					hitboxsetindex;
 	int					numlocalanim;			// animations/poses
 	int					localanimindex;		// animation descriptions

 	int					numlocalseq;				// sequences
 	int					localseqindex;

 	int			activitylistversion;
 	int			eventsindexed;
// raw textures
 	int					numtextures;
 	int					textureindex;

// raw textures search paths
 	int					numcdtextures; //This is always set to 1 in v53. - Liberty
 	int					cdtextureindex; //This leads to where the cd texture structs are stored which are just offsets that lead to their name. Make sure to have the offset of the first cd texture (the offset this index leads to) lead to the start of the string chunk. -Liberty

// replaceable textures tables
 	int					numskinref;
 	int					numskinfamilies; //Set to 1 for saftey concerns. - Liberty
 	int					skinindex; 

 	int					numbodyparts;
 	int					bodypartindex;

 	int					numlocalattachments;
 	int					localattachmentindex;
 	int					numlocalnodes;
 	int					localnodeindex;
	int					localnodenameindex;
	int					numflexdesc;
	int					flexdescindex;
	int					numflexcontrollers;
	int					flexcontrollerindex;
	int					numflexrules;
	int					flexruleindex;
	int					numikchains;        //ALWAYS SET THIS TO "0" - Liberyu
	int					ikchainindex;
	int					nummouths;
	int					mouthindex;
	int					numlocalposeparameters;
	int					localposeparamindex;
	int					surfacepropindex;
	int					keyvalueindex;
	int					keyvaluesize;
	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	float				mass;       //Depends on type of model. Zero until proper document is made. - Liberty
	int					contents;
	int					numincludemodels;
	int					includemodelindex;
	/*was long*/ float virtualModel;
	int					szanimblocknameindex; //They moved this to the 1shyOfStringChunk and replaced it with the bonetablebynameindex. - Liberty
	int					numanimblocks;
	int					animblockindex; //Static value "-1082130432" - Liberty
	/*was long*/ float animblockModel;
//NOTE: Probably not used or used for something else.
	int					bonetablebynameindex; //Respawn moved this to szanimblocknameindex and replaced it with the unknown block after the model data. -Liberty
	/*was long*/ float pVertexBase;
	/*was long*/ float pIndexBase;
	std::byte				constdirectionallightdot; //Always set to "-52" - Liberty
	std::byte				rootLOD;                 //Always set to "2" - Liberty
	std::byte				numAllowedRootLODs;
	std::byte				unused[1];
	int					unused4; // zero out if version < 47
	int					numflexcontrollerui; //These are tricky so null them until further instructions. - Liberty
	int					flexcontrolleruiindex; //These are tricky so null them until further instructions. - Liberty
	float				flVertAnimFixedPointScale;

	int			surfacepropLookup;	// this index must be cached by the loader, not saved in the file

	int					studiohdr2index; //These are tricky so null them until further instructions. - Liberty 
	int					unused2[1]; //This is usually 61 -> 63 bytes short of the string chunk. - Liberty
};

struct v53_Header_Part2{

    int staticValues1[3];
    int oneshyOfStringChunk;
    int vtxOffset1;

    int vvdOffset;
    int idcvOffset;
    int phyOffset;
    
    int staticValues2[4]; //Static values which solve certain bugs when put in correctly. - Liberty
    int unknownBlock; //This int is an unknown block that is located right after the phy. It can be substituded with the vtx. - Liberty
    int unknown2[3]; //These Don't matter. Zero them. - Liberty
    int vtxOffset2;
};