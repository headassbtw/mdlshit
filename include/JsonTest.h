#pragma once
#include <structs.hpp>
using namespace rapidjson;
class JsonTest
{
	public:
		struct ExtraBoneData
		{
			std::vector<int> bones;
			int bone;
			int unkBoolean;
			int unkPhySec;
		};

		struct ExtraHitboxData
		{
			std::string			groupName;
			std::vector<int>	hboxArry;
			bool				isCrit;
			std::string			kvName;
		};

		struct ExtraSeqData
		{
			int seq;
			int unk1;
			int unk2;
		};

		struct ExtraModelData
		{
			int model;
			int unkIdx1;
			int unkIdx2;
		};

		struct ExtraIkChainData
		{
			std::string			name;
			float				unk;
		};

		struct ExtraMeshData
		{
			int mesh;
			int unk1;
			int unk2;
		};

		struct RuiMeshData
		{
			int											numparents; // apparently you can have meshes parented to more than one bone(?)    
			int											numvertices; // number of verts
			int											numfaces; // number of faces (quads)

			int											parentindex; // this gets padding out front of it to even off the struct

			int											vertexindex; // offset into smd style vertex data
			int											vertmapindex; // offsets into a vertex map for each quad
			int											facedataindex; // offset into uv section

			std::byte									unk[4]; // zero sometimes, others not. has to do with face clipping.

			std::string									szruimeshname; // have to subtract header to get actual size (padding included)

			std::vector <int16_t>						parent; // parent(s) bone of mesh

			std::vector <mstudioruivertmap_t>			vertexmap; // vertex map for each face  
			std::vector <mstudioruivert_t>				vertex;

			std::vector <mstudioruimesface_t>			facedata;
		};

		std::vector<ExtraHitboxData>	extraHitboxData;
		std::vector<ExtraIkChainData>	extraIkChainData;
		std::vector<RuiMeshData>		ruiMeshData;

		Document doc{};

		std::string types[7]{"bone", "hitbox", "seq", "model", "ikChain", "mesh", "ruiMesh"};

		void ReadExtraStructs(FileInfo info);

		void HandleType();

		void HandleBone(Value& value, bool debug);

		void HandleHitbox(Value& value, bool debug);

		void HandleSeq(Value& value, bool debug);

		void HandleModel(Value& value, bool debug);

		void HandleIkChain(Value& value, bool debug);

		void HandleMesh(Value& value, bool debug);

		void HandleRuiMesh(Value& value, bool debug);
};