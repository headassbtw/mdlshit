#pragma once
#include <cstdio>
#include <cstring>
#include <fstream>
#include <half.hpp>
#include <iostream>
#include <structs.hpp>
using namespace std;
class BinaryReader{
  //const char* _filename;
  public:
  std::fstream Stream;
  const char* _filename;
    BinaryReader(const char* filename);
    BinaryReader(const BinaryReader&);
    ~BinaryReader();
    const char* _pathName = _filename;
    int size;
    void read(char* data, int size);
    void seek(int pos);
    
    void Read(char** data);
    //void Read(const char** data, int size);
    void Read(char** data[], int size);
    //void Read(char** data, int size);
    void Read(int* data);
    void Read(int data);
    void Read(int data[], int size);
    void Read(float* data);
    std::string ReadNullTermStr(bool debug);
    std::string ReadNullTermStrTrgt(int pos, bool debug);
    std::string ReadNullTermStrTrgtNoReturn(int pos, bool debug);
    std::string ReadNullTermStrTrgtNoSep(int pos, bool debug);

    template <typename T> void Read(std::vector<T>* data, int groupSize);
    template <typename T> void Read(std::vector<T>* data, int groupSize, bool special);

    //void Read(long* data);
    int Position();
    void Read(Vector* data);
    void Read(Vector3Short* data);
    void Read(Vector3* data);
    void Read(RadianEuler* data);
    void Read(matrix3x4_t* data);
    void Read(QuaternionShort* data);
    void Read(Quaternion* data);
    void Read(short* data);
    void Read(uint16_t* data);
    void Read(byte* data);
    void Read(byte* data[], int size);
    void Read(uint32_t* data);
    void Read(studiohdr_t_v49* data);
    void Read(studiohdr2_t_v49* data);
    void Read(mstudiobone_t_v49* data);
    void Read(mstudiojigglebone_t_v49* data);
    void Read(mstudioboneflexdriver_t_v49* data);
    void Read(mstudioattachment_t_v49* data);
    void Read(mstudioikchain_t_v49* data);
    void Read(mstudioiklink_t_v49* data);
    void Read(mstudioiklock_t_v49* data); 
    void Read(mstudioposeparamdesc_t_v49* data);
    void Read(mstudioanimblock_t* data);
    void Read(mstudiosrcbonetransform_t_v49* data);
    void Read(mstudiolinearbone_t_v49* data);
    void Read(mstudiolinearbonedata_t_v49* data, int numbones);
    void Read(mstudiolinearbonedata_t_v53* data, int numbones);
    void Read(mstudiohitboxset_t_v49* data);
    void Read(mstudiobbox_t_v49* data);
    void Read(mstudiobonenametable_t_v49* data, int numbones);
    void Read(mstudioanimdesc_t_v49* data);
    void Read(sectionindexesindex_t_v49* data);
    void Read(mstudioanim_valueptr_t_v49* data);
    void Read(mstudioanim_t_v49* data);
    void Read(mstudioanimvalue_t* data, mstudioanim_t_v49* hdr);
    void Read(mstudioanimvalues_t_v49* data, mstudioanim_t_v49* hdr, int maxFrames);
    void Read(mstudioanimdata_t_v49* data, int datasize);
    void Read(mstudioikrule_t_v49* data);
    void Read(mstudiocompressedikerror_t_v49* data, int maxFrames);
    void Read(mstudiocompressedikerror_t_v49* data);
    void Read(mstudioikerror_t_v49* data);
    void Read(mstudioikrulezeroframe_t_v49* data);
    void Read(mstudioseqdesc_t_v49* data);
    void Read(mstudioseqdescv49_t* data); //legacy stuff kept to keep unused stuff from breaking. -Liberty

    void Read(blendgroup_t_v49* data, int groupSize);
    void Read(posekey_t_v49* data, int groupSize);
    void Read(mstudioevent_t_v49* data);
    void Read(mstudioautolayer_t_v49* data);
    void Read(mstudioactivitymodifier_t_v49* data);
    void Read(seqweightlist_t_v49* data, int numbones);
    void Read(mstudiomodelgroup_t_v49* data);
    void Read(mstudiobodyparts_t_v49* data);
    void Read(mstudiomodel_t_v49* data);
    void Read(mstudio_meshvertexdata_t_v49* data);
    void Read(mstudiomesh_t_v49* data);
    void Read(mstudioboneweight_t_v49* data);
    void Read(mstudiovertex_t_v49* data);
    void Read(mstudiotexturedir_t_v49* data);
    void Read(mstudiotexture_t_v49* data);
    void Read(mstudioskingroup_t_v49* data, int groupSize);
    void Read(mstudionodedata_v49* data, int groupSize);
    void Read(mstudionodename_t_v49* data);
    void Read(pertriheader_t_v49* data);
    void Read(mstudiokeyvalues_t_v49* data, int groupSize);
    void Read(mstudiostringtable_t_v49* data, studiohdr_t_v49 _mdl, std::vector<mstudioseqdescv49_t> seqs, std::vector<mstudiohitboxset_t_v49>	hitboxsets, mstudioattachment_t_v49* attachments, std::vector< mstudionodename_t_v49> nodes, std::vector<mstudiobodyparts_t_v49> bodyparts, std::vector<mstudioikchain_t_v49> ikchains, std::vector<mstudioanimdesc_t_v49> animdescs, std::vector<mstudiotexture_t_v49> textures, std::vector<mstudiomodelgroup_t_v49> includemodels, std::vector<mstudiotexturedir_t_v49> cdmaterials, std::vector<mstudioposeparamdesc_t_v49> poseparamdescs, std::vector<mstudiosrcbonetransform_t_v49> srcbonetransforms);
    //void Read(mstudiostringtable_t_v49* data, MDL::SourceModel49 mdl);
    void Read(mstudiostringtable_t_v49* data, studiohdr_t_v49 mdlhdr, std::vector<mstudioseqdesc_t_v49> seqs, mstudiohitboxset_t_v49* hitboxsets, mstudioattachment_t_v49* attachments, mstudionodename_t_v49* nodes, mstudiobodyparts_t_v49* bodyparts, mstudioikchain_t_v49* ikchains, mstudioanimdesc_t_v49* animdescs, mstudiotexture_t_v49* textures, mstudiomodelgroup_t_v49* includemodels, mstudiotexturedir_t_v49* cdmaterials, mstudioposeparamdesc_t_v49* poseparamdescs, mstudiosrcbonetransform_t_v49* srcbonetransforms);

    void Read(studiohdr_t_v52* data);
    void Read(studiohdr2_t_v52* data);
    void Read(mayabakery_t* data, int groupSize);
    void Read(mstudiobone_t_v52* data);
    void Read(mstudioanimdesc_t_v52* data);
    void Read(mstudiocompressedikerror_t_v52* data);
    void Read(mstudioseqdesc_t_v52* data);
    void Read(mstudiostringtable_t_v52* data, studiohdr_t_v52 _mdl, studiohdr2_t_v52 mdlsubhdr, std::vector<mstudioseqdesc_t_v52> seqs, std::vector<mstudiohitboxset_t_v49>	hitboxsets, std::vector<mstudioattachment_t_v49> attachments, std::vector< mstudionodename_t_v49> nodes, std::vector<mstudiobodyparts_t_v49> bodyparts, std::vector<mstudioikchain_t_v49> ikchains, std::vector<mstudioanimdesc_t_v52> animdescs, std::vector<mstudiotexture_t_v49> textures, std::vector<mstudiomodelgroup_t_v49> includemodels, std::vector<mstudiotexturedir_t_v49> cdmaterials, std::vector<mstudioposeparamdesc_t_v49> poseparamdescs, std::vector<mstudiosrcbonetransform_t_v49> srcbonetransforms);

    void Read(studiohdr_t_v53* data);
    void Read(mstudiobone_t_v53* data);
    void Read(mstudiobbox_t_v53* data);
    void Read(mstudiohitboxset_t_v53* data);
    void Read(mstudioanimdesc_t_v53* data);
    void Read(mstudioanim_t_v53* data);
    void Read(sectionindexes_t_v53* data);
    void Read(mstudioikrule_t_v53* data);
    void Read(mstudioseqdesc_t_v53* data);
    void Read(mstudioactivitymodifier_t_v53* data);
    void Read(mstudioikchain_t_v53* data);
    void Read(mstudiotexture_t_v53* data);
    void Read(mstudiostringtable_t_v53 data, studiohdr_t_v53 _mdl);
    void Read(mstudiostringtable_t_v52* data, studiohdr_t_v53 mdlhdr, std::vector<mstudioseqdesc_t_v53> seqs, std::vector<mstudiohitboxset_t_v53>	hitboxsets, std::vector<mstudioattachment_t_v49> attachments, std::vector< mstudionodename_t_v49> nodes, std::vector<mstudiobodyparts_t_v49> bodyparts, std::vector<mstudioikchain_t_v53> ikchains, std::vector<mstudioanimdesc_t_v53> animdescs, std::vector<mstudiotexture_t_v53> textures, std::vector<mstudiomodelgroup_t_v49> includemodels, std::vector<mstudiotexturedir_t_v49> cdmaterials, std::vector<mstudioposeparamdesc_t_v49> poseparamdescs, std::vector<mstudiosrcbonetransform_t_v49> srcbonetransforms);

    void Read(mstudiorruiheader_t* data);
    void Read(mstudioruivertmap_t* data);
    void Read(mstudioruivert_t* data);
    void Read(mstudioruimesface_t* data);
    void Read(mstudioruimesh_t* data);
    void Read(pertriheader_t* data);
    void Read(aabbnode* data);
    void Read(aabbcollmaskmap* data);
    void Read(aabbleaf* data);
    void Read(aabbcollisionmask* data, int numVerts);

    void Read(phyheader_t* data);
    void Read(compactsurfaceheader_t* data);
    void Read(legacysurfaceheader_t* data);
    void Read(trianglefaceheader_t* data);
    void Read(trianglevertmap_t* data);
    void Read(phyvertex_t* data);
    void Read(phynode_t* data);
    void Read(physection_t* data);


};
class BinaryWriter{
  const char* _filename;
  public:
  std::fstream Stream;
    BinaryWriter(const char* filename);
    BinaryWriter(const BinaryWriter&);
    ~BinaryWriter();
    int size;
    void write(const char* data, int size);
    void seek(int pos);
    void Write(char* data);
    void Write(char* data[]);
    void Write(int data);
    void Write(short data);
    void Write(int data[]);
    void Write(float data);
    void Padding(int count);

    //void Write(long data);
    int Position();
    void Write(uint32_t data);
    void Write(Vector data);
    void Write(Vector3Short data);
    void Write(Vector3 data);
    void Write(RadianEuler data);
    void Write(matrix3x4_t data);
    void Write(Quaternion data);
    void Write(QuaternionShort data);
    void Write(byte data);
    void Write(byte* data);
    void Write(byte data[], int size);
    void Write(studiohdr_t_v49 data);
    void Write(studiohdr2_t_v49 data);
    void Write(mstudiobone_t_v49 data);
    void Write(mstudiojigglebone_t_v49 data);
    void Write(mstudioboneflexdriver_t_v49 data);
    void Write(mstudioattachment_t_v49 data);
    void Write(mstudioikchain_t_v49 data);
    void Write(mstudioiklink_t_v49 data);
    void Write(mstudioiklock_t_v49 data);
    void Write(mstudioposeparamdesc_t_v49 data);
    void Write(mstudiosrcbonetransform_t_v49 data);
    void Write(mstudiolinearbone_t_v49 data);
    void Write(mstudiolinearbonedata_t_v49 data, int numbones);
    void Write(mstudiohitboxset_t_v49 data);
    void Write(mstudiobbox_t_v49 data);
    void Write(mstudiobonenametable_t_v49 data, int numbones);
    void Write(mstudioanimdesc_t_v49 data);
    void Write(sectionindexesindex_t_v49 data);
    void Write(mstudioanim_valueptr_t_v49 data);
    void Write(mstudioanim_t_v49 data);
    void Write(mstudioanimdata_t_v49 data);
    void Write(mstudioanimvalues_t_v49 data);
    void Write(mstudioanimvalue_t data);
    void Write(mstudioikrule_t_v49 data);
    void Write(mstudiocompressedikerror_t_v49 data);
    void Write(mstudioikerror_t_v49 data);
    void Write(mstudioikrulezeroframe_t_v49 data);
    void Write(mstudioseqdesc_t_v49 data);
    void Write(mstudioseqdescv49_t data);

    void Write(blendgroup_t_v49 data);
    void Write(posekey_t_v49 data, int groupSize);
    void Write(mstudioevent_t_v49 data);
    void Write(mstudioautolayer_t_v49 data);
    void Write(mstudioactivitymodifier_t_v49 data);
    void Write(seqweightlist_t_v49 data);
    void Write(mstudiomodelgroup_t_v49 data);
    void Write(mstudiobodyparts_t_v49 data);
    void Write(mstudiomodel_t_v49 data);
    void Write(mstudio_meshvertexdata_t_v49 data);
    void Write(mstudiomesh_t_v49 data);
    void Write(mstudioboneweight_t_v49 data);
    void Write(mstudiovertex_t_v49 data);
    void Write(mstudiotexturedir_t_v49 data);
    void Write(mstudiotexture_t_v49 data);
    void Write(mstudioskingroup_t_v49 data, int groupSize);
    void Write(mstudionodedata_v49 data);
    void Write(mstudionodename_t_v49 data);
    void Write(mstudioanimblock_t data);
    void Write(pertriheader_t_v49 data);
    void Write(mstudiokeyvalues_t_v49 data);
    void Write(mstudiostringtable_t_v49 data, studiohdr_t_v49 hdr);
    template <typename T> void Write(std::vector<T> data);

    //void Write(studiohdr_t_v52 data);
    //void Write(studiohdr2_t_v52 data);
    void Write(mayabakery_t data);
    //void Write(mstudiobone_t_v52 data);
    //void Write(mstudioanimdesc_t_v52 data);
    void Write(mstudiocompressedikerror_t_v52 data);
    //void Write(mstudioseqdesc_t_v52 data);
    void Write(mstudiostringtable_t_v52 data);

    void Write(studiohdr_t_v53 data);
    void Write(mstudiobone_t_v53 data);
    void Write(mstudiobbox_t_v53 data);
    void Write(mstudioanimdesc_t_v53 data);
    void Write(mstudioanim_t_v53 data);
    void Write(sectionindexes_t_v53 data);
    void Write(mstudioikrule_t_v53 data);
    void Write(mstudioseqdesc_t_v53 data);
    void Write(mstudioactivitymodifier_t_v53 data);
    void Write(mstudioikchain_t_v53 data);
    void Write(mstudiotexture_t_v53 data);

    void Write(mstudiorruiheader_t data);
    void Write(mstudioruivertmap_t data);
    void Write(mstudioruivert_t data);
    void Write(mstudioruimesface_t data);
    void Write(mstudioruimesh_t data);
    void Write(mstudiolinearbonedata_t_v53 data, int numbones);

    void Write(pertriheader_t data);
    void Write(aabbnode data);
    void Write(aabbcollmaskmap data);
    void Write(aabbleaf data);
    void Write(aabbcollisionmask data);

    void Write(phyheader_t data);
    void Write(compactsurfaceheader_t data);
    void Write(legacysurfaceheader_t data);
    void Write(trianglefaceheader_t data);
    void Write(trianglevertmap_t data);
    void Write(phyvertex_t data);
    void Write(phynode_t data);
    void Write(physection_t data);

    void Write(std::string data);
};