#pragma once
#include <cstddef>
#include <string>
#include <optional>

struct FileInfo {
	std::optional<std::string> mdl; //main mdl file
	std::optional<std::string> vtx; //vertices
	std::optional<std::string> vvd; //vertex data
	std::optional<std::string> phy; //physics data
	std::optional<std::string> pfb; //extra physics stuff
	std::optional<std::string> str; //extra structs
	std::optional<std::string> aabb; //aabb trees
	std::optional<std::string> out; //output file

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

struct Vector2
{
	float x;
	float y;
};

struct Vector3
{
	float x, y, z;
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

struct Quaternion
{
	float one;
	float i;
	float j;
	float k;
};

struct RadianEuler
{
	// for when you want radians in float.
	float x;
	float y;
	float z;
};

struct matrix3x4_t
{
	// unsure if that's how it actually works

	// row 1, x
	float c0r0; // x
	float c1r0; // x
	float c2r0; // x
	float c3r0; // x

	// row 2, y
	float c0r1; // y
	float c1r1; // y
	float c2r1; // y
	float c3r1; // y

	// row 3, z
	float c0r2; // z
	float c1r2; // z
	float c2r2; // z
	float c3r2; // z
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
 	int					activitylistversion;
 	int					eventsindexed;
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
	/*was long*/ float	virtualModel;
	int					szanimblocknameindex;
	int					numanimblocks;
	int					animblockindex;
	/*was long*/ float	animblockModel;
	int					bonetablebynameindex;
	/*was long*/ float	pVertexBase;
	/*was long*/ float	pIndexBase;
	std::byte			constdirectionallightdot;
	std::byte			rootLOD; 
	std::byte			numAllowedRootLODs;
	std::byte			unused[1];
	float				unused4;
	int					numflexcontrollerui;
	int					flexcontrolleruiindex;
	float				flVertAnimFixedPointScale;
	int					surfacepropLookup;
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

 	int					activitylistversion;
 	int					eventsindexed;
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

	/*
	int					szanimblocknameindex; //They moved this to the 1shyOfStringChunk and replaced it with the bonetablebynameindex. - Liberty
	int					numanimblocks;
	int					animblockindex; //Static value "-1082130432" - Liberty
	float animblockModel;
	*/


//NOTE: Probably not used or used for something else.
	int					bonetablebynameindex; //Respawn moved this to szanimblocknameindex and replaced it with the unknown block after the model data. -Liberty
	/*was long*/ //float pVertexBase;
	/*was long*/ //float pIndexBase;
	std::byte				constdirectionallightdot; //Always set to "-52" - Liberty
	std::byte				rootLOD;                 //Always set to "2" - Liberty
	std::byte				numAllowedRootLODs;
	std::byte				unused[1];
	float					unused4; // zero out if version < 47
	int					numflexcontrollerui; //These are tricky so null them until further instructions. - Liberty
	int					flexcontrolleruiindex; //These are tricky so null them until further instructions. - Liberty

	//float				flVertAnimFixedPointScale;

	//int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file

	//int					studiohdr2index; //These are tricky so null them until further instructions. - Liberty 
	float			pVertexBase;
	float			pIndexBase;

	// this is in all shipped models, probably part of their asset bakery. it should be 0x2CC.
	int					mayaindex; // doesn't actually need to be written pretty sure, only four bytes when not present.


	int					unused2[1]; //This is usually 61 -> 63 bytes short of the string chunk. - Liberty
};

struct v53_Header_Part2{
	/*
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
	*/
	int numsrcbonetransform;
	int srcbonetransformindex;

	int	illumpositionattachmentindex;

	int linearboneindex;

	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;

	// v52 has an identical set of unks
	// this section is seemingly always 60 bytes, unless the following unks are not 0.
	// of note this does not appear in v49, while it does appear in v52 and v53.
	int unkindex; // goes to a section just after linear bone data, but just before string data.

	int numunk[3]; // used in envballs, looks like a count of sort.

	// maybe this is for the string table since it always has one byte padding it?
	// this is probably for some section I haven't seen or a string that hasn't been filled out.
	int unkindex1; // byte before string table start?

	// start of model combination stuff.
	// anis are no longer used from what I can tell, v52s that had them don't in v53.
	int vtxindex; // VTX
	int vvdindex; // VVD / IDSV
	int vvcindex; // VVC / IDCV 
	int vphyindex; // VPHY / IVPS

	int vtxsize;
	int vvdsize;
	int vvcsize;
	int vphysize;

	// the following 'unks' could actually be indexs.
	// one of these is probably the ANI/IDAG index
	// vertAnimFixedPointScale might be in here but I doubt it.

	// big block of floats I think
	// not present with no vphy??
	int phybindex; // section between vphy and vtx. probably combined loose file, but what?

	int numofphyb;

	int unk;

	int unkindex3; // goes to the same spot as vtx normally.

	int unused1[60]; // god I hope


};

struct studiohdr2_t
{
	// NOTE: For forward compat, make sure any methods in this struct
	// are also available in studiohdr_t so no leaf code ever directly references
	// a studiohdr2_t structure
	int numsrcbonetransform;
	int srcbonetransformindex;

	int	illumpositionattachmentindex;

	float flMaxEyeDeflection; // default to cos(30) if not set

	int linearboneindex;

	int sznameindex;

	int m_nBoneFlexDriverCount;
	int m_nBoneFlexDriverIndex;

	int reserved[56];
};

struct mstudiobonev49_t
{
	int sznameindex;

	int parent; // parent bone
	int bonecontroller[6]; // bone controller index, -1 == none

	// default values
	Vector3 pos;
	Quaternion quat;
	RadianEuler rot;
	// compression scale
	Vector3 posscale;
	Vector3 rotscale;

	matrix3x4_t poseToBone;
	Quaternion qAlignment;

	int flags;
	int proctype;
	int procindex; // procedural rule
	int physicsbone; // index into physically simulated bone

	int surfacepropidx; // index into string tablefor property name

	int contents; // See BSPFlags.h for the contents flags

	int surfacepropLookup; // this index must be cached by the loader, not saved in the file

	int unused[7]; // remove as appropriate
};

struct mstudiobonev53_t
{
	int sznameindex;

	int parent; // parent bone
	int bonecontroller[6]; // bone controller index, -1 == none

	// default values
	Vector3 pos;
	Quaternion quat;
	RadianEuler rot;

	Vector3 unkvector;

	// compression scale
	Vector3 posscale;
	Vector3 rotscale;

	Vector3 unkvector1;

	matrix3x4_t poseToBone;
	Quaternion qAlignment;

	int flags;
	int proctype;
	int procindex; // procedural rule
	int physicsbone; // index into physically simulated bone

	int surfacepropidx; // index into string tablefor property name

	int contents; // See BSPFlags.h for the contents flags

	int surfacepropLookup; // this index must be cached by the loader, not saved in the file

	int unkindex; // sometimes there is something here, 0xFFFF in many cases.
				  // probably for adding to an offset in a file.

	int unused[7]; // remove as appropriate
};

struct mstudioattachment_t
{
	int sznameindex;
	int		flags;

	int					localbone; // parent bone

	matrix3x4_t			localmatrix; // attachment point

	int					unused[8];
};

struct mstudioevent_t
{
	float				cycle;
	int					event;
	int					type;
	char				options[64];

	int					szeventindex;

};

struct mstudiomodelv53_t
{
	char name[64];

	int type;

	float boundingradius;

	int nummeshes;
	uint32_t meshindex;

	// cache purposes
	int numvertices; // number of unique vertices/normals/texcoords
	uint32_t vertexindex; // vertex Vector
	uint32_t tangentsindex; // tangents Vector

	int numattachments;
	uint32_t attachmentindex;

	int numeyeballs;
	uint32_t eyeballindex;

	//mstudio_modelvertexdata_t vertexdata;

	uint32_t unk[4];

	uint32_t unkindex;
	uint32_t unkindex1;

	uint32_t unk1[4];
};

struct mstudioikchainv53_t
{
	int sznameindex;

	int linktype;
	int numlinks;
	int linkindex;

	Vector3	kneeDir;

	int unk;
};

struct mstudioposeparamdesc_t
{
	uint32_t sznameindex;

	uint32_t flags; // ????
	float start; // starting value
	float end; // ending value
	float loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};

struct BoneHeaderv49
{
	std::byte bone;
	std::byte flag;
	short nextOffset;
	short transX;
	short transY;
	short transZ;
	int dataSize;
};