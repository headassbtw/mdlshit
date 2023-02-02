#include "structs.hpp"
#include <binarystream.hpp>
#include <fstream>
#include <ios>
#include <string>
#include <sys/stat.h>
#include <logger.hpp>
#include <half.hpp>
#include "mdls.hpp"
using namespace std;

bool Contains(std::vector<int> arry, int trgt)
{
    for (int i = 0; i < arry.size(); i++)
    {
        if (trgt == arry[i]) return true;
    }
    return false;
}

bool ContainsString(std::vector<std::vector<char>> arry, std::vector<char> trgt)
{
    for (int i = 0; i < arry.size(); i++)
    {
        if (trgt == arry[i]) return true;
    }
    return false;
}

bool ContainsString2(std::vector<std::string> arry, std::string trgt)
{
    for (int i = 0; i < arry.size(); i++)
    {
        if (trgt == arry[i])
        {
            //Logger::Info("Arry(%d): %s Target: %s\n", i, arry[i].c_str(), trgt.c_str());
            return true;
        }
    }
    return false;
}

#pragma region READ_UTILITY

std::string BinaryReader::ReadNullTermStr(bool debug)
{
        std::vector<char> vec;
        char value = Stream.get();
        if (value != '\0') seek(Position() - 1);

        while (value != '\0')
        {
            Stream.read((char*)&value, 1);
            vec.push_back(value);
        }

        std::string string(vec.begin(), vec.end());

        //if(debug) Logger::Info("Debug_NullTermStr: %s\n", string.c_str());

        return string;
}

std::string BinaryReader::ReadNullTermStrTrgt(int pos, bool debug)
{
    int startPos = Position();
    seek(pos);
    std::vector<char> vec;
    char value = Stream.get();
    if (value != '\0') seek(Position() - 1);

    while (value != '\0')
    {
        Stream.read((char*)&value, 1);
        vec.push_back(value);
    }

    std::string string(vec.begin(), vec.end());

   // if (debug) Logger::Info("Debug_NullTermStr: %s\n", string.c_str());
    seek(startPos);
    return string;
}

std::string BinaryReader::ReadNullTermStrTrgtNoSep(int pos, bool debug)
{
    int startPos = Position();
    int moe = 0;
    seek(pos);
    std::vector<char> vec;
    char value = Stream.get();
    if (value != '\0') seek(Position() - 1);

    while (value != '\0')
    {
        Stream.read((char*)&value, 1);
        vec.push_back(value);
    }
    if (vec.size() > 1) moe = 1;

    std::string string(vec.begin(), vec.end() - moe);

    // if (debug) Logger::Info("Debug_NullTermStr: %s\n", string.c_str());
    seek(startPos);
    return string;
}

BinaryReader::BinaryReader(const char* filename)
{
  _filename = filename;
  Stream = fstream(_filename, ios::binary | ios::in);
  struct stat results;
  if (stat(filename, &results) == 0){
    size = results.st_size;
  }
  else{
    Logger::Critical("Could not get stats of input file!\n");
  }

}
BinaryReader::~BinaryReader(){
  Stream.close();
  Logger::Info("BinaryReader for file \"%s\" closed\n", _filename);
}
void BinaryReader::Read(int* data)
{
  Stream.read((char*)data, sizeof(int));
  //printf("read an int\n");
}
void BinaryReader::Read(int data[], int size)
{
  for(int i = 0; i < size;i++){
    Stream.read((char*)data[i], sizeof(int));
  }
  
  //printf("read an int array\n");
}
void BinaryReader::Read(char** data[], int size)
{
  Stream.read((char*)*data, size);
  //printf("read an array of char\n");
}
void BinaryReader::read(char* data, int size)
{
  Stream.read(data, size);
  //printf("read a char\n");
}

void BinaryReader::Read(char** data)
{
  Stream.read(*data, sizeof(char*));
  //printf("read a char*\n");
}
void BinaryReader::Read(byte* data)
{
  
  Stream.read((char*) data, 1);
  
  
}
void BinaryReader::Read(byte* data[] , int size){
  Stream.read((char*) data, size);
  //printf("read a byte\n");
}

void BinaryReader::Read(float* data)
{
  Stream.read((char*)data, sizeof(float));
  //printf("read a float\n");
}
int BinaryReader::Position(){
  return Stream.tellg();
}
void BinaryReader::Read(Vector* data)
{
  Stream.read((char*)&data->x, sizeof(float));
  Stream.read((char*)&data->y, sizeof(float));
  Stream.read((char*)&data->z, sizeof(float));
  //printf("read a Vector\n");
}
void BinaryReader::Read(short* data) 
{
    Stream.read((char*)data, sizeof(short));
}

void BinaryReader::Read(uint32_t* data) 
{
    Stream.read((char*)data, sizeof(int));
}

void BinaryReader::Read(uint16_t* data) 
{
    Stream.read((char*)data, sizeof(short));
}

void BinaryReader::Read(Vector3* data) 
{
    Stream.read((char*)&data->x, sizeof(float));
    Stream.read((char*)&data->y, sizeof(float));
    Stream.read((char*)&data->z, sizeof(float));
    //printf("read a Vector\n");
}

void BinaryReader::Read(Vector3Short* data) 
{
    Stream.read((char*)&data->x, sizeof(short));
    Stream.read((char*)&data->y, sizeof(short));
    Stream.read((char*)&data->z, sizeof(short));
    //printf("read a Vector\n");
}

void BinaryReader::Read(RadianEuler* data) 
{
    Stream.read((char*)&data->x, sizeof(float));
    Stream.read((char*)&data->y, sizeof(float));
    Stream.read((char*)&data->z, sizeof(float));
    //printf("read a Vector\n");
}

void BinaryReader::Read(matrix3x4_t* data) 
{
    Stream.read((char*)&data->c0r0, sizeof(float));
    Stream.read((char*)&data->c1r0, sizeof(float));
    Stream.read((char*)&data->c2r0, sizeof(float));
    Stream.read((char*)&data->c3r0, sizeof(float));

    Stream.read((char*)&data->c0r1, sizeof(float));
    Stream.read((char*)&data->c1r1, sizeof(float));
    Stream.read((char*)&data->c2r1, sizeof(float));
    Stream.read((char*)&data->c3r1, sizeof(float));

    Stream.read((char*)&data->c0r2, sizeof(float));
    Stream.read((char*)&data->c1r2, sizeof(float));
    Stream.read((char*)&data->c2r2, sizeof(float));
    Stream.read((char*)&data->c3r2, sizeof(float));
}

void BinaryReader::Read(Quaternion* data) 
{
    Stream.read((char*)&data->one, sizeof(float));
    Stream.read((char*)&data->i, sizeof(float));
    Stream.read((char*)&data->j, sizeof(float));
    Stream.read((char*)&data->k, sizeof(float));
    //printf("read a Vector\n");
}

void BinaryReader::Read(QuaternionShort* data) 
{
    Stream.read((char*)&data->one, 2);
    Stream.read((char*)&data->i, 2);
    Stream.read((char*)&data->j, 2);
    Stream.read((char*)&data->k, 2);
    //printf("read a Vector\n");
}

template <typename T> void BinaryReader::Read(std::vector<T>* data, int groupSize)
{
    for (int i = 0; i < groupSize; i++)
    {
        T value; Stream.read((char*)&value, sizeof(T));
        data->push_back(value);
    }
}

#pragma endregion

#pragma region V49READ
void BinaryReader::Read(studiohdr_t_v49* data) 
{

    Stream.read((char*)&data->id, sizeof(int));                         
    Stream.read((char*)&data->version, sizeof(int));                    
    Stream.read((char*)&data->checksum, sizeof(int));                   
    Stream.read((char*)&data->name, 64);                                

    Stream.read((char*)&data->length, sizeof(int));                     

    Stream.read((char*)&data->eyeposition, sizeof(Vector3));	        

    Stream.read((char*)&data->illumposition, sizeof(Vector3));	        

    Stream.read((char*)&data->hull_min, sizeof(Vector3));		        
    Stream.read((char*)&data->hull_max, sizeof(Vector3));

    Stream.read((char*)&data->view_bbmin, sizeof(Vector3));		        
    Stream.read((char*)&data->view_bbmax, sizeof(Vector3));

    Stream.read((char*)&data->flags, sizeof(int));

    Stream.read((char*)&data->numbones, sizeof(int));                   
    Stream.read((char*)&data->boneindex, sizeof(int));

    Stream.read((char*)&data->numbonecontrollers, sizeof(int));         
    Stream.read((char*)&data->bonecontrollerindex, sizeof(int));

    Stream.read((char*)&data->numhitboxsets, sizeof(int));
    Stream.read((char*)&data->hitboxsetindex, sizeof(int));

    Stream.read((char*)&data->numlocalanim, sizeof(int));               
    Stream.read((char*)&data->localanimindex, sizeof(int));             

    Stream.read((char*)&data->numlocalseq, sizeof(int));                
    Stream.read((char*)&data->localseqindex, sizeof(int));

    Stream.read((char*)&data->activitylistversion, sizeof(int));
    Stream.read((char*)&data->eventsindexed, sizeof(int));


    Stream.read((char*)&data->numtextures, sizeof(int));
    Stream.read((char*)&data->textureindex, sizeof(int));


    Stream.read((char*)&data->numcdtextures, sizeof(int));
    Stream.read((char*)&data->cdtextureindex, sizeof(int));


    Stream.read((char*)&data->numskinref, sizeof(int));
    Stream.read((char*)&data->numskinfamilies, sizeof(int));
    Stream.read((char*)&data->skinindex, sizeof(int));

    Stream.read((char*)&data->numbodyparts, sizeof(int));
    Stream.read((char*)&data->bodypartindex, sizeof(int));

    Stream.read((char*)&data->numlocalattachments, sizeof(int));
    Stream.read((char*)&data->localattachmentindex, sizeof(int));

    Stream.read((char*)&data->numlocalnodes, sizeof(int));
    Stream.read((char*)&data->localnodeindex, sizeof(int));
    Stream.read((char*)&data->localnodenameindex, sizeof(int));

    Stream.read((char*)&data->numflexdesc, sizeof(int));
    Stream.read((char*)&data->flexdescindex, sizeof(int));

    Stream.read((char*)&data->numflexcontrollers, sizeof(int));
    Stream.read((char*)&data->flexcontrollerindex, sizeof(int));

    Stream.read((char*)&data->numflexrules, sizeof(int));
    Stream.read((char*)&data->flexruleindex, sizeof(int));

    Stream.read((char*)&data->numikchains, sizeof(int));
    Stream.read((char*)&data->ikchainindex, sizeof(int));

    Stream.read((char*)&data->nummouths, sizeof(int));
    Stream.read((char*)&data->mouthindex, sizeof(int));

    Stream.read((char*)&data->numlocalposeparameters, sizeof(int));
    Stream.read((char*)&data->localposeparamindex, sizeof(int));

    Stream.read((char*)&data->surfacepropindex, sizeof(int));

    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->keyvaluesize, sizeof(int));

    Stream.read((char*)&data->numlocalikautoplaylocks, sizeof(int));
    Stream.read((char*)&data->localikautoplaylockindex, sizeof(int));


    Stream.read((char*)&data->mass, sizeof(float));
    Stream.read((char*)&data->contents, sizeof(int));


    Stream.read((char*)&data->numincludemodels, sizeof(int));
    Stream.read((char*)&data->includemodelindex, sizeof(int));

    Stream.read((char*)&data->virtualModel, sizeof(uint32_t));

    Stream.read((char*)&data->szanimblocknameindex, sizeof(int));
    Stream.read((char*)&data->numanimblocks, sizeof(int));
    Stream.read((char*)&data->animblockindex, sizeof(int));
    Stream.read((char*)&data->animblockModel, sizeof(uint32_t));

    Stream.read((char*)&data->bonetablebynameindex, sizeof(int));

    Stream.read((char*)&data->pVertexBase, sizeof(uint32_t));
    Stream.read((char*)&data->pIndexBase, sizeof(uint32_t));

    Stream.read((char*)&data->constdirectionallightdot, sizeof(std::byte));

    Stream.read((char*)&data->rootLOD, sizeof(std::byte));

    Stream.read((char*)&data->numAllowedRootLODs, sizeof(std::byte));

    Stream.read((char*)&data->unused, sizeof(std::byte));

    Stream.read((char*)&data->unused4, sizeof(int));         

    Stream.read((char*)&data->numflexcontrollerui, sizeof(int));
    Stream.read((char*)&data->flexcontrolleruiindex, sizeof(int));

    Stream.read((char*)&data->flVertAnimFixedPointScale, sizeof(float));
    Stream.read((char*)&data->surfacepropLookup, sizeof(int)); 

    Stream.read((char*)&data->studiohdr2index, sizeof(int));

    Stream.read((char*)&data->unused2, sizeof(int));
}

void BinaryReader::Read(studiohdr2_t_v49* data) 
{

    Stream.read((char*)&data->numsrcbonetransform, sizeof(int));
    Stream.read((char*)&data->srcbonetransformindex, sizeof(int));

    Stream.read((char*)&data->illumpositionattachmentindex, sizeof(int));

    Stream.read((char*)&data->flMaxEyeDeflection, sizeof(float));

    Stream.read((char*)&data->linearboneindex, sizeof(int));

    Stream.read((char*)&data->sznameindex, sizeof(int));

    Stream.read((char*)&data->m_nBoneFlexDriverCount, sizeof(int));
    Stream.read((char*)&data->m_nBoneFlexDriverIndex, sizeof(int));

    Stream.read((char*)&data->reserved, sizeof(int) * 56);
}

void BinaryReader::Read(mstudiobone_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->parent, sizeof(int));
    Stream.read((char*)&data->bonecontroller, sizeof(int) * 6);
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->quat, sizeof(Quaternion));
    Stream.read((char*)&data->rot, sizeof(RadianEuler));
    Stream.read((char*)&data->posscale, sizeof(Vector3));
    Stream.read((char*)&data->rotscale, sizeof(Vector3));
    Stream.read((char*)&data->poseToBone, sizeof(matrix3x4_t));
    Stream.read((char*)&data->qAlignment, sizeof(Quaternion));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->proctype, sizeof(int));
    Stream.read((char*)&data->procindex, sizeof(int));
    Stream.read((char*)&data->physicsbone, sizeof(int));
    Stream.read((char*)&data->surfacepropidx, sizeof(int));
    Stream.read((char*)&data->contents, sizeof(int));
    Stream.read((char*)&data->surfacepropLookup, sizeof(int));
    Stream.read((char*)&data->unused, sizeof(int) * 7);
    //printf("read a Vector\n");
}

void BinaryReader::Read(mstudiojigglebone_t_v49* data) 
{
        Stream.read((char*)&data->flags, sizeof(int));
        Stream.read((char*)&data->length, sizeof(float));
        Stream.read((char*)&data->tipMass, sizeof(float));
        

        Stream.read((char*)&data->yawStiffness, sizeof(float));
        Stream.read((char*)&data->yawDamping, sizeof(float));
        Stream.read((char*)&data->pitchStiffness, sizeof(float));
        Stream.read((char*)&data->pitchDamping, sizeof(float));
        Stream.read((char*)&data->alongStiffness, sizeof(float));
        Stream.read((char*)&data->alongDamping, sizeof(float));
        

        Stream.read((char*)&data->angleLimit, sizeof(float));
        

        Stream.read((char*)&data->minYaw, sizeof(float));
        Stream.read((char*)&data->maxYaw, sizeof(float));
        Stream.read((char*)&data->yawFriction, sizeof(float));
        Stream.read((char*)&data->yawBounce, sizeof(float));
        

        Stream.read((char*)&data->minPitch, sizeof(float));
        Stream.read((char*)&data->maxPitch, sizeof(float));
        Stream.read((char*)&data->pitchFriction, sizeof(float));
        Stream.read((char*)&data->pitchBounce, sizeof(float));
        

        Stream.read((char*)&data->baseMass, sizeof(float));
        Stream.read((char*)&data->baseStiffness, sizeof(float));
        Stream.read((char*)&data->baseDamping, sizeof(float));
        Stream.read((char*)&data->baseMinLeft, sizeof(float));
        Stream.read((char*)&data->baseMaxLeft, sizeof(float));
        Stream.read((char*)&data->baseLeftFriction, sizeof(float));
        Stream.read((char*)&data->baseMinUp, sizeof(float));
        Stream.read((char*)&data->baseMaxUp, sizeof(float));
        Stream.read((char*)&data->baseUpFriction, sizeof(float));
        Stream.read((char*)&data->baseMinForward, sizeof(float));
        Stream.read((char*)&data->baseMaxForward, sizeof(float));
        Stream.read((char*)&data->baseForwardFriction, sizeof(float));
}

void BinaryReader::Read(mstudioboneflexdriver_t_v49* data) 
{
    Stream.read((char*)&data->m_nBoneIndex, sizeof(int));
    Stream.read((char*)&data->m_nControlCount, sizeof(int));
    Stream.read((char*)&data->m_nControlIndex, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 3);
}

void BinaryReader::Read(mstudioattachment_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->localbone, sizeof(int));
    Stream.read((char*)&data->localmatrix, sizeof(matrix3x4_t));
    Stream.read((char*)&data->unused, sizeof(int) * 8);
}

void BinaryReader::Read(mstudioikchain_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->linktype, sizeof(int));
    Stream.read((char*)&data->numlinks, sizeof(int));

    Stream.read((char*)&data->linkindex, sizeof(int));
}

void BinaryReader::Read(mstudioiklink_t_v49* data) 
{
    Stream.read((char*)&data->bone, sizeof(int));
    Stream.read((char*)&data->kneeDir, sizeof(Vector3));
    Stream.read((char*)&data->unused0, sizeof(Vector3));
}

void BinaryReader::Read(mstudioiklock_t_v49* data) 
{
    Stream.read((char*)&data->chain, sizeof(int));
    Stream.read((char*)&data->flPosWeight, sizeof(float));
    Stream.read((char*)&data->flLocalQWeight, sizeof(float));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->unused, sizeof(int) * 4);
}

void BinaryReader::Read(mstudioposeparamdesc_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->start, sizeof(float));
    Stream.read((char*)&data->end, sizeof(float));
    Stream.read((char*)&data->loop, sizeof(float));
}

void BinaryReader::Read(mstudioanimblock_t* data)
{
    Stream.read((char*)&data->datastart, sizeof(int));
    Stream.read((char*)&data->dataend, sizeof(int));
}

void BinaryReader::Read(mstudiosrcbonetransform_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->pretransform, sizeof(matrix3x4_t));
    Stream.read((char*)&data->posttransform, sizeof(matrix3x4_t));
}

void BinaryReader::Read(mstudiolinearbone_t_v49* data) 
{

    Stream.read((char*)&data->numbones, sizeof(int));

    Stream.read((char*)&data->flagsindex, sizeof(int));

    Stream.read((char*)&data->parentindex, sizeof(int));

    Stream.read((char*)&data->posindex, sizeof(int));

    Stream.read((char*)&data->quatindex, sizeof(int));

    Stream.read((char*)&data->rotindex, sizeof(int));

    Stream.read((char*)&data->posetoboneindex, sizeof(int));

    Stream.read((char*)&data->posscaleindex, sizeof(int));

    Stream.read((char*)&data->rotscaleindex, sizeof(int));

    Stream.read((char*)&data->qalignmentindex, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 6);
}

void BinaryReader::Read(mstudiolinearbonedata_t_v49* data, int numbones)
{
    Stream.read((char*)&data->flags, sizeof(uint32_t) * numbones);

    Stream.read((char*)&data->parents, sizeof(uint32_t) * numbones);

    Stream.read((char*)&data->bonePos, sizeof(Vector3) * numbones);

    Stream.read((char*)&data->boneQuat, sizeof(Quaternion) * numbones);

    Stream.read((char*)&data->boneRot, sizeof(RadianEuler) * numbones);

    Stream.read((char*)&data->poseToBone, sizeof(matrix3x4_t) * numbones);

    Stream.read((char*)&data->posScale, sizeof(Vector3) * numbones);

    Stream.read((char*)&data->rotScale, sizeof(Vector3) * numbones);

    Stream.read((char*)&data->boneAlignment, sizeof(Quaternion) * numbones);
}

void BinaryReader::Read(mstudiolinearbonedata_t_v53* data, int numbones)
{
    Stream.read((char*)&data->flags, sizeof(uint32_t) * numbones);
    Stream.read((char*)&data->parents, sizeof(uint32_t) * numbones);
    Stream.read((char*)&data->bonePos, sizeof(Vector3) * numbones);
    Stream.read((char*)&data->boneQuat, sizeof(Quaternion) * numbones);
    Stream.read((char*)&data->boneRot, sizeof(RadianEuler) * numbones);
    Stream.read((char*)&data->poseToBone, sizeof(matrix3x4_t) * numbones);
    Stream.read((char*)&data->rotScale, sizeof(Vector3) * numbones);
    Stream.read((char*)&data->boneAlignment, sizeof(Quaternion) * numbones);

}

void BinaryReader::Read(mstudiohitboxset_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->numhitboxes, sizeof(int));
    Stream.read((char*)&data->hitboxindex, sizeof(int));
}

void BinaryReader::Read(mstudiobbox_t_v49* data) 
{
    Stream.read((char*)&data->bone, sizeof(int));
    Stream.read((char*)&data->group, sizeof(int));
    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));
    Stream.read((char*)&data->szhitboxnameindex, sizeof(int));
    Stream.read((char*)&data->unused, sizeof(int) * 8);

}

void BinaryReader::Read(mstudiobonenametable_t_v49* data, int numbones) 
{
    for (int i = 0; i < numbones; i++)
    {
        Stream.read((char*)&data->bone, sizeof(byte));
        data->bones.push_back(data->bone);
    }
}

void BinaryReader::Read(mstudioanimdesc_t_v49* data) 
{
    Stream.read((char*)&data->baseptr, sizeof(int));
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->fps, sizeof(float));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->numframes, sizeof(int));
    Stream.read((char*)&data->nummovements, sizeof(int));
    Stream.read((char*)&data->movementindex, sizeof(int));
    Stream.read((char*)&data->ikrulezeroframeindex, sizeof(int));
    Stream.read((char*)&data->unused1, sizeof(int) * 5);
    Stream.read((char*)&data->animblock, sizeof(int));
    Stream.read((char*)&data->animindex, sizeof(int));
    Stream.read((char*)&data->numikrules, sizeof(int));
    Stream.read((char*)&data->ikruleindex, sizeof(int));
    Stream.read((char*)&data->animblockikruleindex, sizeof(int));
    Stream.read((char*)&data->numlocalhierarchy, sizeof(int));
    Stream.read((char*)&data->localhierarchyindex, sizeof(int));
    Stream.read((char*)&data->sectionindex, sizeof(int));
    Stream.read((char*)&data->sectionframes, sizeof(int));
    Stream.read((char*)&data->zeroframespan, sizeof(short));
    Stream.read((char*)&data->zeroframecount, sizeof(short));
    Stream.read((char*)&data->zeroframeindex, sizeof(int));
    Stream.read((char*)&data->zeroframestalltime, sizeof(float));
}

void BinaryReader::Read(sectionindexesindex_t_v49* data) 
{
    Stream.read((char*)&data->anioffset, sizeof(int));
    Stream.read((char*)&data->sectionoffset, sizeof(int));
}

void BinaryReader::Read(mstudioanim_valueptr_t_v49* data) 
{
    Stream.read((char*)&data->offset, sizeof(Vector3Short));
}

void BinaryReader::Read(mstudioanim_t_v49* data) 
{
    data->strPos = Position();
    Stream.read((char*)&data->bone, sizeof(std::byte));
    Stream.read((char*)&data->flags, sizeof(std::byte));
    Stream.read((char*)&data->nextoffset, sizeof(short));
    if((int)data->flags & STUDIO_ANIM_ANIMROT) Stream.read((char*)&data->animrot, sizeof(Vector3Short));

    if((int)data->flags & STUDIO_ANIM_ANIMPOS) Stream.read((char*)&data->animpos, sizeof(Vector3Short));

    if((int)data->flags & STUDIO_ANIM_RAWROT) Stream.read((char*)&data->rawrot, sizeof(QuaternionShort));

    if((int)data->flags & STUDIO_ANIM_RAWROT2) Stream.read((char*)&data->rawrot2, sizeof(QuaternionShort));

    if((int)data->flags & STUDIO_ANIM_RAWPOS) Stream.read((char*)&data->rawpos, sizeof(Vector3Short));
}

void BinaryReader::Read(mstudioanimdata_t_v49* data, int datasize)
{
    for (int i = 0; i < datasize; i++)
    {
        char value;
        Stream.read((char*)&value, 1);
        data->arry.push_back( (std::byte)value);
    }
}

void BinaryReader::Read(mstudioikrule_t_v49* data) 
{
    Stream.read((char*)&data->index, sizeof(int));

    Stream.read((char*)&data->type, sizeof(int));
    Stream.read((char*)&data->chain, sizeof(int));

    Stream.read((char*)&data->bone, sizeof(int));

    Stream.read((char*)&data->slot, sizeof(int));	
    Stream.read((char*)&data->height, sizeof(float));
    Stream.read((char*)&data->radius, sizeof(float));
    Stream.read((char*)&data->floor, sizeof(float));
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->q, 16);

    Stream.read((char*)&data->compressedikerrorindex, sizeof(int));

    Stream.read((char*)&data->unused2, sizeof(int));

    Stream.read((char*)&data->iStart, sizeof(int));
    Stream.read((char*)&data->ikerrorindex, sizeof(int));

    Stream.read((char*)&data->start, sizeof(float));	
    Stream.read((char*)&data->peak, sizeof(float));	    
    Stream.read((char*)&data->tail, sizeof(float));	    
    Stream.read((char*)&data->end, sizeof(float));		

    Stream.read((char*)&data->unused3, sizeof(float));
    Stream.read((char*)&data->contact, sizeof(float));
    Stream.read((char*)&data->drop, sizeof(float));	
    Stream.read((char*)&data->top, sizeof(float));

    Stream.read((char*)&data->unused6, sizeof(int));
    Stream.read((char*)&data->unused7, sizeof(int));
    Stream.read((char*)&data->unused8, sizeof(int));
    
    Stream.read((char*)&data->szattachmentindex, sizeof(int));		
    
    Stream.read((char*)&data->unused, sizeof(int) * 7);

}

void BinaryReader::Read(mstudiocompressedikerror_t_v49* data) 
{
    Stream.read((char*)&data->scale, sizeof(float) * 6);
    Stream.read((char*)&data->index, sizeof(short) * 6);
}

void BinaryReader::Read(mstudioikerror_t_v49* data) 
{
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->q, sizeof(Quaternion));
}

void BinaryReader::Read(mstudioikrulezeroframe_t_v49* data) 
{
    Stream.read((char*)&data->chain, sizeof(short));
    Stream.read((char*)&data->slot, sizeof(short));
    Stream.read((char*)&data->start, sizeof(short));
    Stream.read((char*)&data->peak, sizeof(short));
    Stream.read((char*)&data->tail, sizeof(short));
    Stream.read((char*)&data->end, sizeof(short));

}

void BinaryReader::Read(mstudioseqdescv49_t* data) 
{

    Stream.read((char*)&data->baseptr, sizeof(int));

    Stream.read((char*)&data->szlabelindex, sizeof(int));

    Stream.read((char*)&data->szactivitynameindex, sizeof(int));

    Stream.read((char*)&data->flags, sizeof(int));

    Stream.read((char*)&data->activity, sizeof(int));
    Stream.read((char*)&data->actweight, sizeof(int));

    Stream.read((char*)&data->numevents, sizeof(int));
    Stream.read((char*)&data->eventindex, sizeof(int));

    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));

    Stream.read((char*)&data->numblends, sizeof(int));


    Stream.read((char*)&data->animindexindex, sizeof(int));

    Stream.read((char*)&data->movementindex, sizeof(int));		
    Stream.read((char*)&data->groupsize, sizeof(int) * 2);
    Stream.read((char*)&data->paramindex, sizeof(int) * 2);	    
    Stream.read((char*)&data->paramstart, sizeof(float) * 2);	
    Stream.read((char*)&data->paramend, sizeof(float) * 2);		
    Stream.read((char*)&data->paramparent, sizeof(int));

    Stream.read((char*)&data->fadeintime, sizeof(float));		
    Stream.read((char*)&data->fadeouttime, sizeof(float));		

    Stream.read((char*)&data->localentrynode, sizeof(int));		
    Stream.read((char*)&data->localexitnode, sizeof(int));		
    Stream.read((char*)&data->nodeflags, sizeof(int));			

    Stream.read((char*)&data->entryphase, sizeof(float));		
    Stream.read((char*)&data->exitphase, sizeof(float));		

    Stream.read((char*)&data->lastframe, sizeof(float));		

    Stream.read((char*)&data->nextseq, sizeof(int));			
    Stream.read((char*)&data->pose, sizeof(int));			    

    Stream.read((char*)&data->numikrules, sizeof(int));

    Stream.read((char*)&data->numautolayers, sizeof(int));
    Stream.read((char*)&data->autolayerindex, sizeof(int));

    Stream.read((char*)&data->weightlistindex, sizeof(int));

    Stream.read((char*)&data->posekeyindex, sizeof(int));

    Stream.read((char*)&data->numiklocks, sizeof(int));
    Stream.read((char*)&data->iklockindex, sizeof(int));


    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->keyvaluesize, sizeof(int));

    Stream.read((char*)&data->cycleposeindex, sizeof(int));		

    Stream.read((char*)&data->activitymodifierindex, sizeof(int));
    Stream.read((char*)&data->numactivitymodifiers, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 5);

}

void BinaryReader::Read(blendgroup_t_v49* data, int groupSize)
{
    if (groupSize > 1)
    {
        Stream.read((char*)&data->blends, sizeof(short) * groupSize);
    }
    else
    {
        Stream.read((char*)&data->blends, sizeof(short) * 2);
    }
}

void BinaryReader::Read(posekey_t_v49* data, int groupSize) 
{
    Stream.read((char*)&data->unk, sizeof(float) * groupSize);
}

void BinaryReader::Read(mstudioevent_t_v49* data) 
{
    Stream.read((char*)&data->cycle, sizeof(float));
    Stream.read((char*)&data->event, sizeof(int));
    Stream.read((char*)&data->type, sizeof(int));
    Stream.read((char*)&data->options, 64);

    Stream.read((char*)&data->szeventindex, sizeof(int));

}

void BinaryReader::Read(mstudioautolayer_t_v49* data) 
{
    Stream.read((char*)&data->iSequence, sizeof(short));
    Stream.read((char*)&data->iPose, sizeof(short));

    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->start, sizeof(float));
    Stream.read((char*)&data->peak, sizeof(float));
    Stream.read((char*)&data->tail, sizeof(float));
    Stream.read((char*)&data->end, sizeof(float));

}

void BinaryReader::Read(mstudioactivitymodifier_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
}

void BinaryReader::Read(seqweightlist_t_v49* data, int numbones ) 
{

    for (int i = 0; i < numbones; i++)
    {
        float weight = 0;
        Stream.read((char*)&weight, sizeof(float));
        data->boneweight.push_back(weight);
    }
}

void BinaryReader::Read(mstudiomodelgroup_t_v49* data) 
{
    Stream.read((char*)&data->szlabelindex, sizeof(int));
    Stream.read((char*)&data->sznameindex, sizeof(int));

}

void BinaryReader::Read(mstudiobodyparts_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->nummodels, sizeof(int));
    Stream.read((char*)&data->base, sizeof(int));
    Stream.read((char*)&data->modelindex, sizeof(int));

}

void BinaryReader::Read(mstudiomodel_t_v49* data) 
{
    Stream.read((char*)&data->name, 64);

    Stream.read((char*)&data->type, sizeof(int));

    Stream.read((char*)&data->boundingradius, sizeof(float));

    Stream.read((char*)&data->nummeshes, sizeof(int));
    Stream.read((char*)&data->meshindex, sizeof(int));


    Stream.read((char*)&data->numvertices, sizeof(int));
    Stream.read((char*)&data->vertexindex, sizeof(int));
    Stream.read((char*)&data->tangentsindex, sizeof(int));



    Stream.read((char*)&data->numattachments, sizeof(int));
    Stream.read((char*)&data->attachmentindex, sizeof(int));

    Stream.read((char*)&data->numeyeballs, sizeof(int));
    Stream.read((char*)&data->eyeballindex, sizeof(int));




    Stream.read((char*)&data->unused, sizeof(int) * 10);

}

void BinaryReader::Read(mstudio_meshvertexdata_t_v49* data) 
{
    Stream.read((char*)&data->unk, sizeof(int));

    Stream.read((char*)&data->numLODVertexes, sizeof(int) * MAX_NUM_LODS);

}

void BinaryReader::Read(mstudiomesh_t_v49* data) 
{
    Stream.read((char*)&data->material, sizeof(int));

    Stream.read((char*)&data->modelindex, sizeof(int));

    Stream.read((char*)&data->numvertices, sizeof(int));
    Stream.read((char*)&data->vertexoffset, sizeof(int));



    Stream.read((char*)&data->numflexes, sizeof(int));
    Stream.read((char*)&data->flexindex, sizeof(int));


    Stream.read((char*)&data->materialtype, sizeof(int));
    Stream.read((char*)&data->materialparam, sizeof(int));


    Stream.read((char*)&data->meshid, sizeof(int));

    Stream.read((char*)&data->center, sizeof(Vector3));

    Stream.read((char*)&data->vertexdata.unk, sizeof(int));

    Stream.read((char*)&data->vertexdata.numLODVertexes, sizeof(int) * MAX_NUM_LODS);

    Stream.read((char*)&data->unused, sizeof(int) * 8);

}

void BinaryReader::Read(mstudioboneweight_t_v49* data) 
{
    Stream.read((char*)&data->weight, sizeof(float) * MAX_NUM_BONES_PER_VERT);
    Stream.read((char*)&data->bone, sizeof(char) * MAX_NUM_BONES_PER_VERT);
    Stream.read((char*)&data->numbones, sizeof(byte));

}

void BinaryReader::Read(mstudiovertex_t_v49* data) 
{
    Stream.read((char*)&data->m_BoneWeights, sizeof(mstudioboneweight_t_v49));
    Stream.read((char*)&data->m_vecPosition, sizeof(Vector3));
    Stream.read((char*)&data->m_vecNormal, sizeof(Vector3));
    Stream.read((char*)&data->m_vecTexCoord, sizeof(Vector2));

}

void BinaryReader::Read(mstudiotexturedir_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
}

void BinaryReader::Read(mstudiotexture_t_v49* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));

    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->used, sizeof(int));
    Stream.read((char*)&data->unused1, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 12);

}

void BinaryReader::Read(mstudioskingroup_t_v49* data, int groupSize) 
{
    Stream.read((char*)&data->textureId, sizeof(short) * groupSize);
}

void BinaryReader::Read(mstudionodedata_v49* data, int groupSize) 
{
    for (int i = 0; i < groupSize; i++)
    {
        char value = 0;
        Stream.read((char*)&value, 1);
        data->unk.push_back(value);
    }
}

void BinaryReader::Read(mstudionodename_t_v49* data) {
    Stream.read((char*)&data->sznameindex, sizeof(int));
}

void BinaryReader::Read(pertriheader_t_v49* data) {
    Stream.read((char*)&data->version, sizeof(int));



    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));

    Stream.read((char*)&data->unused, sizeof(int) * 8);

}

void BinaryReader::Read(mstudiokeyvalues_t_v49* data, int groupSize)
{
    for (int i = 0; i < groupSize; i++)
    {
        char value = 0;
        Stream.read((char*)&value, 1);
        data->value.push_back(value);
    }
}

void BinaryReader::Read(mstudiostringtable_t_v49* data, studiohdr_t_v49 mdlhdr, std::vector<mstudioseqdescv49_t> seqs, std::vector<mstudiohitboxset_t_v49>	hitboxsets, std::vector<mstudioattachment_t_v49> attachments, std::vector< mstudionodename_t_v49> nodes, std::vector<mstudiobodyparts_t_v49> bodyparts, std::vector<mstudioikchain_t_v49> ikchains, std::vector<mstudioanimdesc_t_v49> animdescs, std::vector<mstudiotexture_t_v49> textures, std::vector<mstudiomodelgroup_t_v49> includemodels, std::vector<mstudiotexturedir_t_v49> cdmaterials, std::vector<mstudioposeparamdesc_t_v49> poseparamdescs, std::vector<mstudiosrcbonetransform_t_v49> srcbonetransforms)
{
    std::vector<std::string> stringsUsed;
    int startPos = Position();
    int eventNum = 0;
    int actModNum = 0;
    int stringTableSize = mdlhdr.length - startPos;


    data->stringtablesize = stringTableSize;
    Logger::Info("String Table Size: %d\n", data->stringtablesize);

    std::string mdlName = ReadNullTermStr(true);
    data->mdlname = mdlName;
    Logger::Info("MdlName: %s\n", mdlName.c_str());

    std::string surfacePropName = ReadNullTermStr(true);
    data->surfaceprop = surfacePropName;
    Logger::Info("SurfacePropName: %s\n", surfacePropName.c_str());
    stringsUsed.push_back(surfacePropName.c_str());


    for (int i = 0; i < mdlhdr.numbones; i++)
    {
        std::string str = ReadNullTermStr(true);
        Logger::Info("BoneName: %s\n", str.c_str());
        stringsUsed.push_back(str.c_str());
        data->bones.push_back(str);
    }

    for (int i = 0; i < mdlhdr.numlocalattachments; i++)
    {
        if (!attachments[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, attachments[i].szname.c_str()))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("AttachmentName: %s\n", str.c_str());
                data->attachments.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("AttachmentReusedName: %s\n", attachments[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("AttachmentName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numhitboxsets; i++)
    {
        if (!hitboxsets[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, hitboxsets[i].szname.c_str()))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("HitboxSetName: %s\n", str.c_str());
                data->hitboxsets.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("HitboxSetReusedName: %s\n", hitboxsets[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("HitboxSetName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numlocalanim; i++)
    {
        if (!animdescs[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, animdescs[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("AnimDescName: %s\n", str.c_str());
                data->anims.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("AnimDescReusedName: %s\n", animdescs[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("AnimDescName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numlocalseq; i++)
    {
        std::vector<std::string> _activities;
        std::vector<std::string> _activityEvents;
        std::vector<std::string> _activityModifiers;

        int seqPos = (seqs[i].baseptr - seqs[i].activitymodifierindex) * -1;

        std::string szName = ReadNullTermStr(true);
        Logger::Info("SeqName: %s\n", szName.c_str());
        std::string szActivity = seqs[i].szactivityname;

        //Logger::Info("TestSeqName: %s\n", seqs[i].szlabel.c_str());
        //Logger::Info("TestSeqActivity: %s\n", seqs[i].szactivityname.c_str());
        //Logger::Info("TestSeqActivitySize: %d\n", seqs[i].szactivityname.size());
        bool test = seqs[i].szactivityname.empty();

        //if(test)Logger::Info("true\n");
        //else Logger::Info("false\n");

        if (!seqs[i].szactivityname.empty())
        {
            if (!ContainsString2(stringsUsed, szActivity))
            {
                szActivity = ReadNullTermStr(true);
                Logger::Info("SeqActivity: %s\n", szActivity.c_str());
                stringsUsed.push_back(seqs[i].szactivityname);
                _activities.push_back(seqs[i].szactivityname);
            }
            else
            {
                szActivity = "";
                Logger::Info("SeqActivityReusedName: %s\n", seqs[i].szactivityname.c_str());
            }
        }
        else
        {
            Logger::Info("SeqActivity: %s\n", "Empty");
        }

        for (int j = 0; j < seqs[i].numevents; j++)
        {
            int pos = Position();
            std::string eventName = seqs[i].szeventnames[j];
            //Logger::Info("TestSeqEvent: %s\n", seqs[i].szeventnames[j].c_str());

//            if (ContainsString2(stringsUsed, seqs[i].szeventnames[j]) || eventName == "")
//            {
//                _activityEvents.push_back("");
//            }
            if (!seqs[i].szeventnames[j].empty())
            {
                if (!ContainsString2(stringsUsed, seqs[i].szeventnames[j]))
                {
                    eventName = ReadNullTermStr(true);
                    Logger::Info("eventName: %s\n", eventName.c_str());
                    stringsUsed.push_back(seqs[i].szeventnames[j]);
                    _activityEvents.push_back(seqs[i].szeventnames[j]);
                }
                else
                {
                    _activityEvents.push_back("");
                    Logger::Info("eventNameReusedName: %s\n", seqs[i].szeventnames[j].c_str());
                }
            }
            else
            {
                _activityEvents.push_back("");
                Logger::Info("SeqActivity: %s\n", "Empty");
            }
        }

        for (int j = 0; j < seqs[i].numactivitymodifiers; j++)
        {
            int pos = Position();
            std::string actModName = seqs[i].szactivitymodifiernames[j];
            if (!seqs[i].szactivitymodifiernames[j].empty())
            {
                if (!ContainsString2(stringsUsed, seqs[i].szactivitymodifiernames[j]))
                {
                    actModName = ReadNullTermStr(true);
                    Logger::Info("actModName: %s\n", actModName.c_str());
                    stringsUsed.push_back(seqs[i].szactivitymodifiernames[j]);
                    _activityModifiers.push_back(seqs[i].szactivitymodifiernames[j]);
                }
                else
                {
                    _activityModifiers.push_back("");
                    Logger::Info("actModReusedName: %s\n", seqs[i].szactivitymodifiernames[j].c_str());
                }
            }
            else
            {
                _activityModifiers.push_back("");
                Logger::Info("actModName: %s\n", "Empty");
            }
        }
        mstudioseqstring_t_v49 _seq = { szName, szActivity, _activityEvents, _activityModifiers };

        data->seqs.push_back(_seq);
    }

    for (int i = 0; i < mdlhdr.numlocalnodes; i++)
    {
        if (!nodes[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, nodes[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("NodeName: %s\n", str.c_str());
                data->nodes.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("NodeReusedName: %s\n", nodes[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("NodeName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numbodyparts; i++)
    {
        if (!bodyparts[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, bodyparts[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("BodyPartName: %s\n", str.c_str());
                data->bodyparts.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("BodyPartReusedName: %s\n", bodyparts[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("BodyPartName: %s\n", "Empty");
        }

    }

    for (int i = 0; i < mdlhdr.numhitboxsets; i++)
    {
        if (hitboxsets[i].numhitboxes > 0)
        {
            for (int j = 0; j < hitboxsets[i].numhitboxes; j++)
            {
                if (!hitboxsets[i].hitboxes[j].szhitboxname.empty())
                {
                    if (!ContainsString2(stringsUsed, hitboxsets[i].hitboxes[j].szhitboxname))
                    {
                        std::string str2 = ReadNullTermStr(true);
                        Logger::Info("HitboxName: %s\n", str2.c_str());
                        data->hitboxes.push_back(str2);
                        stringsUsed.push_back(str2);
                    }
                    else
                    {
                            Logger::Info("HitboxReusedName: %s\n", hitboxsets[i].hitboxes[j].szhitboxname.c_str());
                    }
                }
                else
                {
                    Logger::Info("HitboxName: %s\n", "Empty");
                }
            }
        }
    }

    for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
    {
        if (!poseparamdescs[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, poseparamdescs[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("PoseParamName: %s\n", str.c_str());
                data->poseparams.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("PoseParamReusedName: %s\n", poseparamdescs[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("PoseParamName: %s\n", "Empty");
        }

    }

    for (int i = 0; i < mdlhdr.numikchains; i++)
    {
        if (!ikchains[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, ikchains[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("IkChainName: %s\n", str.c_str());
                data->ikchains.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("IkChainReusedName: %s\n", ikchains[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("IkChainName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numincludemodels; i++)
    {
        if (!includemodels[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, includemodels[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("IncludeModelName: %s\n", str.c_str());
                data->includemodel.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("IncludeModelReusedName: %s\n", includemodels[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("IncludeModelName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numtextures; i++)
    {
        if (!textures[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, textures[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("TextureName: %s\n", str.c_str());
                data->textures.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("TextureReusedName: %s\n", textures[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("TextureName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numcdtextures; i++)
    {
        if (!cdmaterials[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, cdmaterials[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("CdMaterialName: %s\n", str.c_str());
                data->cdmaterials.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("CdMaterialReusedName: %s\n", cdmaterials[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("CdMaterialName: %s\n", "Empty");
        }
    }
}

#pragma endregion

#pragma region V52READ

void BinaryReader::Read(studiohdr_t_v52* data) {

    Stream.read((char*)&data->id, sizeof(int));
    Stream.read((char*)&data->version, sizeof(int));
    Stream.read((char*)&data->checksum, sizeof(int));
    Stream.read((char*)&data->name, 64);

    Stream.read((char*)&data->length, sizeof(int));

    Stream.read((char*)&data->eyeposition, sizeof(Vector3));

    Stream.read((char*)&data->illumposition, sizeof(Vector3));

    Stream.read((char*)&data->hull_min, sizeof(Vector3));
    Stream.read((char*)&data->hull_max, sizeof(Vector3));

    Stream.read((char*)&data->view_bbmin, sizeof(Vector3));
    Stream.read((char*)&data->view_bbmax, sizeof(Vector3));

    Stream.read((char*)&data->flags, sizeof(int));

    Stream.read((char*)&data->numbones, sizeof(int));
    Stream.read((char*)&data->boneindex, sizeof(int));

    Stream.read((char*)&data->numbonecontrollers, sizeof(int));
    Stream.read((char*)&data->bonecontrollerindex, sizeof(int));

    Stream.read((char*)&data->numhitboxsets, sizeof(int));
    Stream.read((char*)&data->hitboxsetindex, sizeof(int));

    Stream.read((char*)&data->numlocalanim, sizeof(int));
    Stream.read((char*)&data->localanimindex, sizeof(int));

    Stream.read((char*)&data->numlocalseq, sizeof(int));
    Stream.read((char*)&data->localseqindex, sizeof(int));

    Stream.read((char*)&data->activitylistversion, sizeof(int));
    Stream.read((char*)&data->eventsindexed, sizeof(int));


    Stream.read((char*)&data->numtextures, sizeof(int));
    Stream.read((char*)&data->textureindex, sizeof(int));


    Stream.read((char*)&data->numcdtextures, sizeof(int));
    Stream.read((char*)&data->cdtextureindex, sizeof(int));


    Stream.read((char*)&data->numskinref, sizeof(int));
    Stream.read((char*)&data->numskinfamilies, sizeof(int));
    Stream.read((char*)&data->skinindex, sizeof(int));

    Stream.read((char*)&data->numbodyparts, sizeof(int));
    Stream.read((char*)&data->bodypartindex, sizeof(int));

    Stream.read((char*)&data->numlocalattachments, sizeof(int));
    Stream.read((char*)&data->localattachmentindex, sizeof(int));

    Stream.read((char*)&data->numlocalnodes, sizeof(int));
    Stream.read((char*)&data->localnodeindex, sizeof(int));
    Stream.read((char*)&data->localnodenameindex, sizeof(int));

    Stream.read((char*)&data->numflexdesc, sizeof(int));
    Stream.read((char*)&data->flexdescindex, sizeof(int));

    Stream.read((char*)&data->numflexcontrollers, sizeof(int));
    Stream.read((char*)&data->flexcontrollerindex, sizeof(int));

    Stream.read((char*)&data->numflexrules, sizeof(int));
    Stream.read((char*)&data->flexruleindex, sizeof(int));

    Stream.read((char*)&data->numikchains, sizeof(int));
    Stream.read((char*)&data->ikchainindex, sizeof(int));

    Stream.read((char*)&data->nummouths, sizeof(int));
    Stream.read((char*)&data->mouthindex, sizeof(int));

    Stream.read((char*)&data->numlocalposeparameters, sizeof(int));
    Stream.read((char*)&data->localposeparamindex, sizeof(int));

    Stream.read((char*)&data->surfacepropindex, sizeof(int));

    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->keyvaluesize, sizeof(int));

    Stream.read((char*)&data->numlocalikautoplaylocks, sizeof(int));
    Stream.read((char*)&data->localikautoplaylockindex, sizeof(int));


    Stream.read((char*)&data->mass, sizeof(float));
    Stream.read((char*)&data->contents, sizeof(int));


    Stream.read((char*)&data->numincludemodels, sizeof(int));
    Stream.read((char*)&data->includemodelindex, sizeof(int));

    Stream.read((char*)&data->virtualModel, sizeof(uint32_t));

    Stream.read((char*)&data->szanimblocknameindex, sizeof(int));
    Stream.read((char*)&data->numanimblocks, sizeof(int));
    Stream.read((char*)&data->animblockindex, sizeof(int));
    Stream.read((char*)&data->animblockModel, sizeof(uint32_t));

    Stream.read((char*)&data->bonetablebynameindex, sizeof(int));

    Stream.read((char*)&data->pVertexBase, sizeof(uint32_t));
    Stream.read((char*)&data->pIndexBase, sizeof(uint32_t));

    Stream.read((char*)&data->constdirectionallightdot, sizeof(std::byte));

    Stream.read((char*)&data->rootLOD, sizeof(std::byte));

    Stream.read((char*)&data->numAllowedRootLODs, sizeof(std::byte));

    Stream.read((char*)&data->unused, sizeof(std::byte));

    Stream.read((char*)&data->fadedistance, sizeof(int));

    Stream.read((char*)&data->numflexcontrollerui, sizeof(int));
    Stream.read((char*)&data->flexcontrolleruiindex, sizeof(int));

    Stream.read((char*)&data->flVertAnimFixedPointScale, sizeof(float));
    Stream.read((char*)&data->surfacepropLookup, sizeof(int));

    Stream.read((char*)&data->studiohdr2index, sizeof(int));

    Stream.read((char*)&data->mayaindex, sizeof(int));
}

void BinaryReader::Read(studiohdr2_t_v52* data) 
{

    Stream.read((char*)&data->numsrcbonetransform, sizeof(int));
    Stream.read((char*)&data->srcbonetransformindex, sizeof(int));

    Stream.read((char*)&data->illumpositionattachmentindex, sizeof(int));

    Stream.read((char*)&data->flMaxEyeDeflection, sizeof(float));

    Stream.read((char*)&data->linearboneindex, sizeof(int));

    Stream.read((char*)&data->sznameindex, sizeof(int));

    Stream.read((char*)&data->m_nBoneFlexDriverCount, sizeof(int));
    Stream.read((char*)&data->m_nBoneFlexDriverIndex, sizeof(int));

    Stream.read((char*)&data->aabbindex, sizeof(int));
    Stream.read((char*)&data->numaabbnodes, sizeof(int));
    Stream.read((char*)&data->numaabbleaves, sizeof(int));
    Stream.read((char*)&data->numaabbverts, sizeof(int));

    Stream.read((char*)&data->unkstringindex, sizeof(int));


    Stream.read((char*)&data->reserved, sizeof(int) * 39);
}

void BinaryReader::Read(mayabakery_t* data, int size)
{
    for (int i = 0; i < size; i++)
    {
        char value = 0;
        Stream.read((char*)&value, 1);
        data->bakery.push_back(value);
    }
}

void BinaryReader::Read(mstudiobone_t_v52* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->parent, sizeof(int));
    Stream.read((char*)&data->bonecontroller, sizeof(int) * 6);
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->quat, sizeof(Quaternion));
    Stream.read((char*)&data->rot, sizeof(RadianEuler));
    Stream.read((char*)&data->posscale, sizeof(Vector3));
    Stream.read((char*)&data->rotscale, sizeof(Vector3));
    Stream.read((char*)&data->poseToBone, sizeof(matrix3x4_t));
    Stream.read((char*)&data->qAlignment, sizeof(Quaternion));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->proctype, sizeof(int));
    Stream.read((char*)&data->procindex, sizeof(int));
    Stream.read((char*)&data->physicsbone, sizeof(int));
    Stream.read((char*)&data->surfacepropidx, sizeof(int));
    Stream.read((char*)&data->contents, sizeof(int));
    Stream.read((char*)&data->surfacepropLookup, sizeof(int));
    Stream.read((char*)&data->unkvector, sizeof(Vector3));
    Stream.read((char*)&data->unkvector1, sizeof(Vector3));
    Stream.read((char*)&data->unused, sizeof(int) * 1);
    //printf("read a Vector\n");
}

void BinaryReader::Read(mstudioanimdesc_t_v52* data) 
{
    Stream.read((char*)&data->baseptr, sizeof(int));
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->fps, sizeof(float));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->numframes, sizeof(int));
    Stream.read((char*)&data->nummovements, sizeof(int));
    Stream.read((char*)&data->movementindex, sizeof(int));
    Stream.read((char*)&data->ikrulezeroframeindex, sizeof(int));
    Stream.read((char*)&data->compressedikerrorindex, sizeof(int));
    Stream.read((char*)&data->unused1, sizeof(int) * 4);
    Stream.read((char*)&data->animblock, sizeof(int));
    Stream.read((char*)&data->animindex, sizeof(int));
    Stream.read((char*)&data->numikrules, sizeof(int));
    Stream.read((char*)&data->ikruleindex, sizeof(int));
    Stream.read((char*)&data->animblockikruleindex, sizeof(int));
    Stream.read((char*)&data->numlocalhierarchy, sizeof(int));
    Stream.read((char*)&data->localhierarchyindex, sizeof(int));
    Stream.read((char*)&data->sectionindex, sizeof(int));
    Stream.read((char*)&data->sectionframes, sizeof(int));
    Stream.read((char*)&data->zeroframespan, sizeof(short));
    Stream.read((char*)&data->zeroframecount, sizeof(short));
    Stream.read((char*)&data->zeroframeindex, sizeof(int));
    Stream.read((char*)&data->zeroframestalltime, sizeof(float));
}

void BinaryReader::Read(mstudiocompressedikerror_t_v52* data) 
{
    Stream.read((char*)&data->scale, sizeof(float) * 4);
    Stream.read((char*)&data->offset, sizeof(short) * 4);
}

void BinaryReader::Read(mstudioseqdesc_t_v52* data)
{

    Stream.read((char*)&data->baseptr, sizeof(int));

    Stream.read((char*)&data->szlabelindex, sizeof(int));

    Stream.read((char*)&data->szactivitynameindex, sizeof(int));

    Stream.read((char*)&data->flags, sizeof(int));

    Stream.read((char*)&data->activity, sizeof(int));
    Stream.read((char*)&data->actweight, sizeof(int));

    Stream.read((char*)&data->numevents, sizeof(int));
    Stream.read((char*)&data->eventindex, sizeof(int));

    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));

    Stream.read((char*)&data->numblends, sizeof(int));


    Stream.read((char*)&data->animindexindex, sizeof(int));

    Stream.read((char*)&data->movementindex, sizeof(int));
    Stream.read((char*)&data->groupsize, sizeof(int) * 2);
    Stream.read((char*)&data->paramindex, sizeof(int) * 2);
    Stream.read((char*)&data->paramstart, sizeof(float) * 2);
    Stream.read((char*)&data->paramend, sizeof(float) * 2);
    Stream.read((char*)&data->paramparent, sizeof(int));

    Stream.read((char*)&data->fadeintime, sizeof(float));
    Stream.read((char*)&data->fadeouttime, sizeof(float));

    Stream.read((char*)&data->localentrynode, sizeof(int));
    Stream.read((char*)&data->localexitnode, sizeof(int));
    Stream.read((char*)&data->nodeflags, sizeof(int));

    Stream.read((char*)&data->entryphase, sizeof(float));
    Stream.read((char*)&data->exitphase, sizeof(float));

    Stream.read((char*)&data->lastframe, sizeof(float));

    Stream.read((char*)&data->nextseq, sizeof(int));
    Stream.read((char*)&data->pose, sizeof(int));

    Stream.read((char*)&data->numikrules, sizeof(int));

    Stream.read((char*)&data->numautolayers, sizeof(int));
    Stream.read((char*)&data->autolayerindex, sizeof(int));

    Stream.read((char*)&data->weightlistindex, sizeof(int));

    Stream.read((char*)&data->posekeyindex, sizeof(int));

    Stream.read((char*)&data->numiklocks, sizeof(int));
    Stream.read((char*)&data->iklockindex, sizeof(int));


    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->keyvaluesize, sizeof(int));

    Stream.read((char*)&data->cycleposeindex, sizeof(int));

    Stream.read((char*)&data->activitymodifierindex, sizeof(int));
    Stream.read((char*)&data->numactivitymodifiers, sizeof(int));

    Stream.read((char*)&data->unk, sizeof(int));
    Stream.read((char*)&data->unk1, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 3);

}

void BinaryReader::Read(mstudiostringtable_t_v52* data, studiohdr_t_v52 mdlhdr, studiohdr2_t_v52 mdlsubhdr, std::vector<mstudioseqdesc_t_v52> seqs, std::vector<mstudiohitboxset_t_v49>	hitboxsets, std::vector<mstudioattachment_t_v49> attachments, std::vector< mstudionodename_t_v49> nodes, std::vector<mstudiobodyparts_t_v49> bodyparts, std::vector<mstudioikchain_t_v49> ikchains, std::vector<mstudioanimdesc_t_v52> animdescs, std::vector<mstudiotexture_t_v49> textures, std::vector<mstudiomodelgroup_t_v49> includemodels, std::vector<mstudiotexturedir_t_v49> cdmaterials, std::vector<mstudioposeparamdesc_t_v49> poseparamdescs, std::vector<mstudiosrcbonetransform_t_v49> srcbonetransforms)
{
    std::vector<std::string> stringsUsed;
    int startPos = Position();
    int eventNum = 0;
    int actModNum = 0;
    int stringTableSize = mdlhdr.length - startPos;


    data->stringtablesize = stringTableSize;
    Logger::Info("String Table Size: %d\n", data->stringtablesize);

    std::string mdlName = ReadNullTermStr(true);
    data->mdlname = mdlName;
    Logger::Info("MdlName: %s\n", mdlName.c_str());

    std::string surfacePropName = ReadNullTermStr(true);
    data->surfaceprop = surfacePropName;
    Logger::Info("SurfacePropName: %s\n", surfacePropName.c_str());
    stringsUsed.push_back(surfacePropName.c_str());
    if (mdlsubhdr.unkstringindex + 408 > mdlsubhdr.sznameindex + 407)
    {
        std::string unkString = ReadNullTermStr(true); //ReadNullTermStrTrgt(mdlsubhdr.unkstringindex + 408, false);
        data->unk = unkString;
        Logger::Info("UnkString: %s\n", unkString.c_str());
    }



    for (int i = 0; i < mdlhdr.numbones; i++)
    {
        std::string str = ReadNullTermStr(true);
        Logger::Info("BoneName: %s\n", str.c_str());
        stringsUsed.push_back(str.c_str());
        data->bones.push_back(str);
    }

    for (int i = 0; i < mdlhdr.numlocalattachments; i++)
    {
        if (!attachments[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, attachments[i].szname.c_str()))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("AttachmentName: %s\n", str.c_str());
                data->attachments.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("AttachmentReusedName: %s\n", attachments[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("AttachmentName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numhitboxsets; i++)
    {
        if (!hitboxsets[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, hitboxsets[i].szname.c_str()))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("HitboxSetName: %s\n", str.c_str());
                data->hitboxsets.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("HitboxSetReusedName: %s\n", hitboxsets[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("HitboxSetName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numlocalanim; i++)
    {
        if (!animdescs[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, animdescs[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("AnimDescName: %s\n", str.c_str());
                data->anims.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("AnimDescReusedName: %s\n", animdescs[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("AnimDescName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numlocalseq; i++)
    {
        std::vector<std::string> _activities;
        std::vector<std::string> _activityEvents;
        std::vector<std::string> _activityModifiers;

        int seqPos = (seqs[i].baseptr - seqs[i].activitymodifierindex) * -1;

        std::string szName = ReadNullTermStr(true);
        Logger::Info("SeqName: %s\n", szName.c_str());
        std::string szActivity = seqs[i].szactivityname;

        //Logger::Info("TestSeqName: %s\n", seqs[i].szlabel.c_str());
        //Logger::Info("TestSeqActivity: %s\n", seqs[i].szactivityname.c_str());
        //Logger::Info("TestSeqActivitySize: %d\n", seqs[i].szactivityname.size());
        bool test = seqs[i].szactivityname.empty();

        //if(test)Logger::Info("true\n");
        //else Logger::Info("false\n");

        if (!seqs[i].szactivityname.empty())
        {
            if (!ContainsString2(stringsUsed, szActivity))
            {
                szActivity = ReadNullTermStr(true);
                Logger::Info("SeqActivity: %s\n", szActivity.c_str());
                stringsUsed.push_back(seqs[i].szactivityname);
                _activities.push_back(seqs[i].szactivityname);
            }
            else
            {
                szActivity = "";
                Logger::Info("SeqActivityReusedName: %s\n", seqs[i].szactivityname.c_str());
            }
        }
        else
        {
            Logger::Info("SeqActivity: %s\n", "Empty");
        }

        for (int j = 0; j < seqs[i].numevents; j++)
        {
            int pos = Position();
            std::string eventName = seqs[i].szeventnames[j];
            //Logger::Info("TestSeqEvent: %s\n", seqs[i].szeventnames[j].c_str());

//            if (ContainsString2(stringsUsed, seqs[i].szeventnames[j]) || eventName == "")
//            {
//                _activityEvents.push_back("");
//            }
            if (!seqs[i].szeventnames[j].empty())
            {
                if (!ContainsString2(stringsUsed, seqs[i].szeventnames[j]))
                {
                    eventName = ReadNullTermStr(true);
                    Logger::Info("eventName: %s\n", eventName.c_str());
                    stringsUsed.push_back(seqs[i].szeventnames[j]);
                    _activityEvents.push_back(seqs[i].szeventnames[j]);
                }
                else
                {
                    _activityEvents.push_back("");
                    Logger::Info("eventNameReusedName: %s\n", seqs[i].szeventnames[j].c_str());
                }
            }
            else
            {
                _activityEvents.push_back("");
                Logger::Info("SeqActivity: %s\n", "Empty");
            }
        }

        for (int j = 0; j < seqs[i].numactivitymodifiers; j++)
        {
            int pos = Position();
            std::string actModName = seqs[i].szactivitymodifiernames[j];
            if (!seqs[i].szactivitymodifiernames[j].empty())
            {
                if (!ContainsString2(stringsUsed, seqs[i].szactivitymodifiernames[j]))
                {
                    actModName = ReadNullTermStr(true);
                    Logger::Info("actModName: %s\n", actModName.c_str());
                    stringsUsed.push_back(seqs[i].szactivitymodifiernames[j]);
                    _activityModifiers.push_back(seqs[i].szactivitymodifiernames[j]);
                }
                else
                {
                    _activityModifiers.push_back("");
                    Logger::Info("actModReusedName: %s\n", seqs[i].szactivitymodifiernames[j].c_str());
                }
            }
            else
            {
                _activityModifiers.push_back("");
                Logger::Info("actModName: %s\n", "Empty");
            }
        }
        mstudioseqstring_t_v49 _seq = { szName, szActivity, _activityEvents, _activityModifiers };

        data->seqs.push_back(_seq);
    }

    for (int i = 0; i < mdlhdr.numlocalnodes; i++)
    {
        if (!nodes[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, nodes[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("NodeName: %s\n", str.c_str());
                data->nodes.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("NodeReusedName: %s\n", nodes[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("NodeName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numbodyparts; i++)
    {
        if (!bodyparts[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, bodyparts[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("BodyPartName: %s\n", str.c_str());
                data->bodyparts.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("BodyPartReusedName: %s\n", bodyparts[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("BodyPartName: %s\n", "Empty");
        }

    }

    for (int i = 0; i < mdlhdr.numhitboxsets; i++)
    {
        if (hitboxsets[i].numhitboxes > 0)
        {
            for (int j = 0; j < hitboxsets[i].numhitboxes; j++)
            {
                if (!hitboxsets[i].hitboxes[j].szhitboxname.empty())
                {
                    if (!ContainsString2(stringsUsed, hitboxsets[i].hitboxes[j].szhitboxname))
                    {
                        std::string str2 = ReadNullTermStr(true);
                        Logger::Info("HitboxName: %s\n", str2.c_str());
                        data->hitboxes.push_back(str2);
                        stringsUsed.push_back(str2);
                    }
                    else
                    {
                        Logger::Info("HitboxReusedName: %s\n", hitboxsets[i].hitboxes[j].szhitboxname.c_str());
                    }
                }
                else
                {
                    Logger::Info("HitboxName: %s\n", "Empty");
                }
            }
        }
    }

    for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
    {
        if (!poseparamdescs[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, poseparamdescs[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("PoseParamName: %s\n", str.c_str());
                data->poseparams.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("PoseParamReusedName: %s\n", poseparamdescs[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("PoseParamName: %s\n", "Empty");
        }

    }

    for (int i = 0; i < mdlhdr.numikchains; i++)
    {
        if (!ikchains[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, ikchains[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("IkChainName: %s\n", str.c_str());
                data->ikchains.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("IkChainReusedName: %s\n", ikchains[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("IkChainName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numincludemodels; i++)
    {
        if (!includemodels[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, includemodels[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("IncludeModelName: %s\n", str.c_str());
                data->includemodel.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("IncludeModelReusedName: %s\n", includemodels[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("IncludeModelName: %s\n", "Empty");
        }
    }

    if (mdlhdr.szanimblocknameindex > mdlsubhdr.sznameindex + 407)
    {
        std::string str = ReadNullTermStr(true);
        Logger::Info("AnimBlockName: %s\n", str.c_str());
        data->animblockname = str;
        stringsUsed.push_back(str);
    }

    for (int i = 0; i < mdlhdr.numtextures; i++)
    {
        if (!textures[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, textures[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("TextureName: %s\n", str.c_str());
                data->textures.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("TextureReusedName: %s\n", textures[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("TextureName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numcdtextures; i++)
    {
        if (!cdmaterials[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, cdmaterials[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("CdMaterialName: %s\n", str.c_str());
                data->cdmaterials.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("CdMaterialReusedName: %s\n", cdmaterials[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("CdMaterialName: %s\n", "Empty");
        }
    }
}
#pragma endregion

#pragma region V53READ

void BinaryReader::Read(studiohdr_t_v53* data) 
{

    char name[64];

    Stream.read((char*)&data->id, sizeof(int));                        
    Stream.read((char*)&data->version, sizeof(int));                   
    Stream.read((char*)&data->checksum, sizeof(int));                  
    Stream.read((char*)&data->sznameindex, sizeof(int));               
    Stream.read((char*)&name, 64);                                     
    std::vector<char> _vec;
    for (int i = 0; i < 64; i++)
    {
        _vec.push_back(name[i]);
    }
    data->name = _vec;

    Stream.read((char*)&data->length, sizeof(int));                    

    Stream.read((char*)&data->eyeposition, sizeof(Vector3));	       

    Stream.read((char*)&data->illumposition, sizeof(Vector3));	       

    Stream.read((char*)&data->hull_min, sizeof(Vector3));		       
    Stream.read((char*)&data->hull_max, sizeof(Vector3));

    Stream.read((char*)&data->view_bbmin, sizeof(Vector3));		       
    Stream.read((char*)&data->view_bbmax, sizeof(Vector3));

    Stream.read((char*)&data->flags, sizeof(int));

    Stream.read((char*)&data->numbones, sizeof(int));                  
    Stream.read((char*)&data->boneindex, sizeof(int));

    Stream.read((char*)&data->numbonecontrollers, sizeof(int));        
    Stream.read((char*)&data->bonecontrollerindex, sizeof(int));

    Stream.read((char*)&data->numhitboxsets, sizeof(int));
    Stream.read((char*)&data->hitboxsetindex, sizeof(int));

    Stream.read((char*)&data->numlocalanim, sizeof(int));               
    Stream.read((char*)&data->localanimindex, sizeof(int));             

    Stream.read((char*)&data->numlocalseq, sizeof(int));                
    Stream.read((char*)&data->localseqindex, sizeof(int));

    Stream.read((char*)&data->activitylistversion, sizeof(int));
    Stream.read((char*)&data->eventsindexed, sizeof(int));

    Stream.read((char*)&data->numtextures, sizeof(int));
    Stream.read((char*)&data->textureindex, sizeof(int));

    Stream.read((char*)&data->numcdtextures, sizeof(int));
    Stream.read((char*)&data->cdtextureindex, sizeof(int));


    Stream.read((char*)&data->numskinref, sizeof(int));
    Stream.read((char*)&data->numskinfamilies, sizeof(int));
    Stream.read((char*)&data->skinindex, sizeof(int));

    Stream.read((char*)&data->numbodyparts, sizeof(int));
    Stream.read((char*)&data->bodypartindex, sizeof(int));

    Stream.read((char*)&data->numlocalattachments, sizeof(int));
    Stream.read((char*)&data->localattachmentindex, sizeof(int));

    Stream.read((char*)&data->numlocalnodes, sizeof(int));
    Stream.read((char*)&data->localnodeindex, sizeof(int));
    Stream.read((char*)&data->localnodenameindex, sizeof(int));

    Stream.read((char*)&data->numflexdesc, sizeof(int));
    Stream.read((char*)&data->flexdescindex, sizeof(int));

    Stream.read((char*)&data->numflexcontrollers, sizeof(int));
    Stream.read((char*)&data->flexcontrollerindex, sizeof(int));

    Stream.read((char*)&data->numflexrules, sizeof(int));
    Stream.read((char*)&data->flexruleindex, sizeof(int));

    Stream.read((char*)&data->numikchains, sizeof(int));
    Stream.read((char*)&data->ikchainindex, sizeof(int));

    Stream.read((char*)&data->numruimeshes, sizeof(int));
    Stream.read((char*)&data->ruimeshindex, sizeof(int));

    Stream.read((char*)&data->numlocalposeparameters, sizeof(int));
    Stream.read((char*)&data->localposeparamindex, sizeof(int));

    Stream.read((char*)&data->surfacepropindex, sizeof(int));

    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->keyvaluesize, sizeof(int));

    Stream.read((char*)&data->numlocalikautoplaylocks, sizeof(int));
    Stream.read((char*)&data->localikautoplaylockindex, sizeof(int));


    Stream.read((char*)&data->mass, sizeof(float));
    Stream.read((char*)&data->contents, sizeof(int));


    Stream.read((char*)&data->numincludemodels, sizeof(int));
    Stream.read((char*)&data->includemodelindex, sizeof(int));

    Stream.read((char*)&data->virtualModel, sizeof(uint32_t));

    Stream.read((char*)&data->bonetablebynameindex, sizeof(int));

    Stream.read((char*)&data->constdirectionallightdot, sizeof(std::byte));

    Stream.read((char*)&data->rootLOD, sizeof(std::byte));

    Stream.read((char*)&data->numAllowedRootLODs, sizeof(std::byte));

    Stream.read((char*)&data->unused, sizeof(std::byte));

    Stream.read((char*)&data->fadedistance, sizeof(float));                       

    Stream.read((char*)&data->numflexcontrollerui, sizeof(int));
    Stream.read((char*)&data->flexcontrolleruiindex, sizeof(int));

    Stream.read((char*)&data->pVertexBase, sizeof(uint32_t));
    Stream.read((char*)&data->pIndexBase, sizeof(uint32_t));

    Stream.read((char*)&data->mayaindex, sizeof(int));

    Stream.read((char*)&data->numsrcbonetransform, sizeof(int));
    Stream.read((char*)&data->srcbonetransformindex, sizeof(int));

    Stream.read((char*)&data->illumpositionattachmentindex, sizeof(int));

    Stream.read((char*)&data->linearboneindex, sizeof(int));

    Stream.read((char*)&data->m_nBoneFlexDriverCount, sizeof(int));
    Stream.read((char*)&data->m_nBoneFlexDriverIndex, sizeof(int));

    Stream.read((char*)&data->aabbindex, sizeof(int));
    Stream.read((char*)&data->numaabbnodes, sizeof(int));
    Stream.read((char*)&data->numaabbleaves, sizeof(int));
    Stream.read((char*)&data->numaabbverts, sizeof(int));

    Stream.read((char*)&data->unkstringindex, sizeof(int));

    Stream.read((char*)&data->vtxindex, sizeof(int)); // VTX
    Stream.read((char*)&data->vvdindex, sizeof(int)); // VVD
    Stream.read((char*)&data->vvcindex, sizeof(int)); // VVC
    Stream.read((char*)&data->vphyindex, sizeof(int)); // VP

    Stream.read((char*)&data->vtxsize, sizeof(int)); // VTX
    Stream.read((char*)&data->vvdsize, sizeof(int)); // VVD 
    Stream.read((char*)&data->vvcsize, sizeof(int)); // VVC 
    Stream.read((char*)&data->vphysize, sizeof(int)); // VPH

    Stream.read((char*)&data->unkmemberindex1, sizeof(int));
    Stream.read((char*)&data->numunkmember1, sizeof(int));

    Stream.read((char*)&data->unk, sizeof(int));

    Stream.read((char*)&data->unkindex3, sizeof(int));

    Stream.read((char*)&data->unused1, sizeof(int) * 60);

}

void BinaryReader::Read(mstudiobone_t_v53* data)
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->parent, sizeof(int));
    Stream.read((char*)&data->bonecontroller, sizeof(int) * 6);
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->quat, sizeof(Quaternion));
    Stream.read((char*)&data->rot, sizeof(RadianEuler));
    Stream.read((char*)&data->unkvector, sizeof(Vector3));
    Stream.read((char*)&data->posscale, sizeof(Vector3));
    Stream.read((char*)&data->rotscale, sizeof(Vector3));
    Stream.read((char*)&data->unkvector1, sizeof(Vector3));
    Stream.read((char*)&data->poseToBone, sizeof(matrix3x4_t));
    Stream.read((char*)&data->qAlignment, sizeof(Quaternion));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->proctype, sizeof(int));
    Stream.read((char*)&data->procindex, sizeof(int));
    Stream.read((char*)&data->physicsbone, sizeof(int));
    Stream.read((char*)&data->surfacepropidx, sizeof(int));
    Stream.read((char*)&data->contents, sizeof(int));
    Stream.read((char*)&data->surfacepropLookup, sizeof(int));
    Stream.read((char*)&data->unktoggle, sizeof(int));
    Stream.read((char*)&data->unused, sizeof(int) * 7);
    //printf("read a Vector\n");
}

void BinaryReader::Read(mstudiobbox_t_v53* data) 
{
    Stream.read((char*)&data->bone, sizeof(int));
    Stream.read((char*)&data->group, sizeof(int));
    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));
    Stream.read((char*)&data->szhitboxnameindex, sizeof(int));
    Stream.read((char*)&data->critoverride, sizeof(int));
    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->unused, sizeof(int) * 6);
}

void BinaryReader::Read(mstudiohitboxset_t_v53* data)
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->numhitboxes, sizeof(int));
    Stream.read((char*)&data->hitboxindex, sizeof(int));
}

void BinaryReader::Read(mstudioanimdesc_t_v53* data) 
{
    Stream.read((char*)&data->baseptr, sizeof(int));
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->fps, sizeof(float));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->numframes, sizeof(int));
    Stream.read((char*)&data->nummovements, sizeof(int));
    Stream.read((char*)&data->movementindex, sizeof(int));
    Stream.read((char*)&data->compressedikerrorindex, sizeof(int));
    Stream.read((char*)&data->animindex, sizeof(int));
    Stream.read((char*)&data->numikrules, sizeof(int));
    Stream.read((char*)&data->ikruleindex, sizeof(int));
    Stream.read((char*)&data->numlocalhierarchy, sizeof(int));
    Stream.read((char*)&data->localhierarchyindex, sizeof(int));
    Stream.read((char*)&data->sectionindex, sizeof(int));
    Stream.read((char*)&data->sectionframes, sizeof(int));
    Stream.read((char*)&data->zeroframespan, sizeof(short));
    Stream.read((char*)&data->zeroframecount, sizeof(short));
    Stream.read((char*)&data->zeroframeindex, sizeof(int));
    Stream.read((char*)&data->zeroframestalltime, sizeof(float));
    Stream.read((char*)&data->unused, sizeof(int) * 5);
}

void BinaryReader::Read(mstudioanim_t_v53* data) 
{
    Stream.read((char*)&data->animscale, sizeof(float));
    Stream.read((char*)&data->bone, sizeof(byte));
    Stream.read((char*)&data->flags, sizeof(byte));
    Stream.read((char*)&data->unk, sizeof(short));

    bool hasRawPosFlag = (int)data->flags & STUDIO_ANIM_RAWPOS_53;
    bool hasRawRotFlag = (int)data->flags & STUDIO_ANIM_RAWROT_53;
    bool hasRawScaleFlag = (int)data->flags & STUDIO_ANIM_RAWSCALE_53;

    if (!hasRawPosFlag)
    {
        Stream.read((char*)&data->animpos, sizeof(Vector3Short));
    }
    else
    {
        Stream.read((char*)&data->rawpos, sizeof(Vector3Short));
    }

    if (!hasRawRotFlag)
    {
        Stream.read((char*)&data->animrot, sizeof(Vector3Short));
        Stream.read((char*)&data->unused, sizeof(short));
    }
    else
    {
        Stream.read((char*)&data->rawrot, sizeof(QuaternionShort));
    }

    if (!hasRawScaleFlag)
    {
        Stream.read((char*)&data->animscale, sizeof(Vector3Short));
    }
    else
    {
        Stream.read((char*)&data->rawscale, sizeof(Vector3Short));
    }

    Stream.read((char*)&data->nextoffset, sizeof(int));
}

void BinaryReader::Read(sectionindexes_t_v53* data)
{
    Stream.read((char*)&data->sectionoffsets, sizeof(int));
}


void BinaryReader::Read(mstudioikrule_t_v53* data) 
{
    Stream.read((char*)&data->index, sizeof(int));

    Stream.read((char*)&data->type, sizeof(int));
    Stream.read((char*)&data->chain, sizeof(int));

    Stream.read((char*)&data->bone, sizeof(int));

    Stream.read((char*)&data->slot, sizeof(int));
    Stream.read((char*)&data->height, sizeof(float));
    Stream.read((char*)&data->radius, sizeof(float));
    Stream.read((char*)&data->floor, sizeof(float));
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->q, sizeof(Quaternion));

    Stream.read((char*)&data->compressedikerrorindex, sizeof(int));

    Stream.read((char*)&data->iStart, sizeof(int));
    Stream.read((char*)&data->ikerrorindex, sizeof(int));

    Stream.read((char*)&data->start, sizeof(float));
    Stream.read((char*)&data->peak, sizeof(float));	
    Stream.read((char*)&data->tail, sizeof(float));
    Stream.read((char*)&data->end, sizeof(float));

    Stream.read((char*)&data->contact, sizeof(float));
    Stream.read((char*)&data->drop, sizeof(float));
    Stream.read((char*)&data->top, sizeof(float));	

    Stream.read((char*)&data->szattachmentindex, sizeof(int));	

    Stream.read((char*)&data->unused, sizeof(int) * 9);
}

void BinaryReader::Read(mstudioseqdesc_t_v53* data) 
{

    Stream.read((char*)&data->baseptr, sizeof(int));

    Stream.read((char*)&data->szlabelindex, sizeof(int));

    Stream.read((char*)&data->szactivitynameindex, sizeof(int));

    Stream.read((char*)&data->flags, sizeof(int));

    Stream.read((char*)&data->activity, sizeof(int));
    Stream.read((char*)&data->actweight, sizeof(int));

    Stream.read((char*)&data->numevents, sizeof(int));
    Stream.read((char*)&data->eventindex, sizeof(int));

    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));

    Stream.read((char*)&data->numblends, sizeof(int));


    Stream.read((char*)&data->animindexindex, sizeof(int));

    Stream.read((char*)&data->movementindex, sizeof(int));		
    Stream.read((char*)&data->groupsize, sizeof(int) * 2);
    Stream.read((char*)&data->paramindex, sizeof(int) * 2);	    
    Stream.read((char*)&data->paramstart, sizeof(float) * 2);	
    Stream.read((char*)&data->paramend, sizeof(float) * 2);		
    Stream.read((char*)&data->paramparent, sizeof(int));

    Stream.read((char*)&data->fadeintime, sizeof(float));		
    Stream.read((char*)&data->fadeouttime, sizeof(float));		

    Stream.read((char*)&data->localentrynode, sizeof(int));		
    Stream.read((char*)&data->localexitnode, sizeof(int));		
    Stream.read((char*)&data->nodeflags, sizeof(int));			

    Stream.read((char*)&data->entryphase, sizeof(float));		
    Stream.read((char*)&data->exitphase, sizeof(float));		

    Stream.read((char*)&data->lastframe, sizeof(float));		

    Stream.read((char*)&data->nextseq, sizeof(int));			
    Stream.read((char*)&data->pose, sizeof(int));			    

    Stream.read((char*)&data->numikrules, sizeof(int));

    Stream.read((char*)&data->numautolayers, sizeof(int));
    Stream.read((char*)&data->autolayerindex, sizeof(int));

    Stream.read((char*)&data->weightlistindex, sizeof(int));

    Stream.read((char*)&data->posekeyindex, sizeof(int));

    Stream.read((char*)&data->numiklocks, sizeof(int));
    Stream.read((char*)&data->iklockindex, sizeof(int));


    Stream.read((char*)&data->keyvalueindex, sizeof(int));
    Stream.read((char*)&data->keyvaluesize, sizeof(int));

    Stream.read((char*)&data->cycleposeindex, sizeof(int));

    Stream.read((char*)&data->activitymodifierindex, sizeof(int));
    Stream.read((char*)&data->numactivitymodifiers, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 10);

}

void BinaryReader::Read(mstudioactivitymodifier_t_v53* data)
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->unk, sizeof(int));
}

void BinaryReader::Read(mstudioikchain_t_v53* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));
    Stream.read((char*)&data->linktype, sizeof(int));
    Stream.read((char*)&data->numlinks, sizeof(int));

    Stream.read((char*)&data->linkindex, sizeof(int));

    Stream.read((char*)&data->unk, sizeof(float));

    Stream.read((char*)&data->unused, sizeof(int) * 3);
}

void BinaryReader::Read(mstudiotexture_t_v53* data) 
{
    Stream.read((char*)&data->sznameindex, sizeof(int));

    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->used, sizeof(int));
    Stream.read((char*)&data->unused1, sizeof(int));

    Stream.read((char*)&data->unused, sizeof(int) * 7);

}

void BinaryReader::Read(mstudiorruiheader_t* data)
{
    Stream.read((char*)&data->ruiunk, 4);
    Stream.read((char*)&data->ruimeshindex, sizeof(int));
}

void BinaryReader::Read(mstudioruivertmap_t* data)
{
    Stream.read((char*)&data->vertstartid, sizeof(short));
    Stream.read((char*)&data->vertendid, sizeof(short));

    Stream.read((char*)&data->vertextraid, sizeof(short));

}

void BinaryReader::Read(mstudioruivert_t* data)
{
    Stream.read((char*)&data->parent, sizeof(int));
    Stream.read((char*)&data->vertexpos, sizeof(Vector3));

}

void BinaryReader::Read(mstudioruimesface_t* data)
{
    Stream.read((char*)&data->faceuvmin, sizeof(Vector2));
    Stream.read((char*)&data->faceuvmax, sizeof(Vector2));

    Stream.read((char*)&data->facescalemin, sizeof(Vector2));
    Stream.read((char*)&data->facescalemax, sizeof(Vector2));

}

template <typename T> void BinaryReader::Read(std::vector<T>* data, int groupSize, bool special)
{
    for (int i = 0; i < groupSize; i++)
    {
        //Logger::Info("Test Read: %d\n", i);
        T value; BinaryReader::Read(&value);
        data->push_back(value);
    }
}

void BinaryReader::Read(mstudioruimesh_t* data)
{
    Stream.read((char*)&data->numparents, sizeof(int));
    Stream.read((char*)&data->numvertices, sizeof(int));
    Stream.read((char*)&data->numfaces, sizeof(int));

    Stream.read((char*)&data->parentindex, sizeof(int));

    Stream.read((char*)&data->vertexindex, sizeof(int));
    Stream.read((char*)&data->vertmapindex, sizeof(int));
    Stream.read((char*)&data->facedataindex, sizeof(int));

    Stream.read((char*)&data->unk, sizeof(int));

    Stream.read((char*)&data->szruimeshname, ((int)data->parentindex) - 32);

    //Logger::Info("Parents Read: %d\n", data->numparents);
    //Logger::Info("Faces Read: %d\n", data->numfaces);
    //Logger::Info("Verts Read: %d\n", data->numvertices);
    //Logger::Info("Pos Read: %d\n", Position());
    //
    //Logger::Info("Parents Read: %d\n", sizeof(short) * data->numparents);
    //Logger::Info("Faces Read: %d\n", 6 * data->numfaces);
    //Logger::Info("Verts Read: %d\n", 16 * data->numvertices);
    Stream.read((char*)&data->parent, sizeof(short) * data->numparents);
    Stream.read((char*)&data->vertexmap, sizeof(mstudioruivertmap_t) * data->numfaces);
    Stream.read((char*)&data->vertex, sizeof(mstudioruivert_t) * data->numvertices);
    Stream.read((char*)&data->facedata, sizeof(mstudioruimesface_t) * data->numfaces);
}

void BinaryReader::Read(mstudiostringtable_t_v53 data, studiohdr_t_v53 mdlhdr)
{
    std::vector<std::vector<char>> activites;
    std::vector<std::vector<char>> activityEvents;
    std::vector<std::vector<char>> activityModifiers;
    for (int i = 0; i < 1; i++)
    {
        int pos = Position();

        int defSize = 100;
        seek(pos);
        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.
        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                data.mdlname.push_back(value);
            }
        }
        //data->mdlname = vec;
    }


    for (int i = 0; i < 1; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                data.surfaceprop.push_back(value);
            }
        }
        //data->surfaceprop = vec;
    }

    for (int i = 0; i < mdlhdr.numbones; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.bones.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numlocalattachments; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.attachments.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numlocalanim; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.anims.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numlocalseq; i++)
    {
        std::vector<char> vec;
        std::vector<char> vec2;
        std::vector<char> vec3;
        std::vector<char> vec4;
        for (int j = 0; j < 4; j++)
        {
            int pos = Position();

            int defSize = 100;

            if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

            for (int k = 0; k < defSize; k++)
            {
                char value; Stream.read(&value, 1);
                if (&value == ".")
                {
                    break;
                }
                else
                {
                    if(k == 0) vec.push_back(value);
                    if(k == 1) vec2.push_back(value);
                    if(k == 2) vec3.push_back(value);
                    if(k == 3) vec4.push_back(value);
                }
            }
        }
        if (!ContainsString(activites, vec2)) activites.push_back(vec2);
        else
            vec2.clear();

        if (!ContainsString(activityEvents, vec3)) activityEvents.push_back(vec3);
        else
            vec3.clear();

        if (!ContainsString(activityModifiers, vec4)) activityModifiers.push_back(vec4);
        else
            vec4.clear();


        mstudioseqstring_t_v53 _seq = { vec, vec2, vec3, vec4 };
        vec.clear();
        vec2.clear();
        vec3.clear();
        vec4.clear();

        data.seqs.push_back(_seq);
    }

    for (int i = 0; i < mdlhdr.numlocalnodes; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.nodes.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numbodyparts; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.bodyparts.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.poseparams.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numikchains; i++)
    {
        int pos = Position();

        int defSize = 1000;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.ikchains.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numincludemodels; i++)
    {
        int pos = Position();

        int defSize = 1000;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.includemodel.push_back(vec);
    }

    for (int i = 0; i < mdlhdr.numtextures; i++)
    {
        int pos = Position();

        int defSize = 100;

        std::vector<char> vec;
        if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.

        for (int j = 0; j < defSize; j++)
        {
            char value; Stream.read(&value, 1);
            if (&value == ".")
            {
                break;
            }
            else
            {
                vec.push_back(value);
            }
        }
        data.textures.push_back(vec);
    }

    //for (int i = 0; i < mdlhdr.numcdtextures; i++)
    //{
    //    int pos = Position();
    //
    //    int defSize = 1000;
    //
    //    std::vector<char> vec;
    //    if (defSize > size - pos) defSize = size - pos; //Prevents overflow - Liberty.
    //
    //    for (int j = 0; j < defSize; j++)
    //    {
    //        char value; Stream.read(&value, 1);
    //        if (&value == ".")
    //        {
    //            break;
    //        }
    //        else
    //        {
    //            vec.push_back(value);
    //        }
    //    }
    //    data->cdmaterials.push_back(vec);
    //}
}


void BinaryReader::Read(mstudiostringtable_t_v52* data, studiohdr_t_v53 mdlhdr, std::vector<mstudioseqdesc_t_v53> seqs, std::vector<mstudiohitboxset_t_v53>	hitboxsets, std::vector<mstudioattachment_t_v49> attachments, std::vector< mstudionodename_t_v49> nodes, std::vector<mstudiobodyparts_t_v49> bodyparts, std::vector<mstudioikchain_t_v53> ikchains, std::vector<mstudioanimdesc_t_v53> animdescs, std::vector<mstudiotexture_t_v53> textures, std::vector<mstudiomodelgroup_t_v49> includemodels, std::vector<mstudiotexturedir_t_v49> cdmaterials, std::vector<mstudioposeparamdesc_t_v49> poseparamdescs, std::vector<mstudiosrcbonetransform_t_v49> srcbonetransforms)
{
    std::vector<std::string> stringsUsed;
    int startPos = Position();
    int eventNum = 0;
    int actModNum = 0;
    int stringTableSize = mdlhdr.length - startPos;


    data->stringtablesize = stringTableSize;
    Logger::Info("String Table Size: %d\n", data->stringtablesize);

    std::string mdlName = ReadNullTermStr(true);
    data->mdlname = mdlName;
    Logger::Info("MdlName: %s\n", mdlName.c_str());

    std::string surfacePropName = ReadNullTermStr(true);
    data->surfaceprop = surfacePropName;
    Logger::Info("SurfacePropName: %s\n", surfacePropName.c_str());
    stringsUsed.push_back(surfacePropName.c_str());
    if (mdlhdr.unkstringindex + 408 > mdlhdr.sznameindex + 407)
    {
        std::string unkString = ReadNullTermStr(true); //ReadNullTermStrTrgt(mdlsubhdr.unkstringindex + 408, false);
        data->unk = unkString;
        Logger::Info("UnkString: %s\n", unkString.c_str());
    }



    for (int i = 0; i < mdlhdr.numbones; i++)
    {
        std::string str = ReadNullTermStr(true);
        Logger::Info("BoneName: %s\n", str.c_str());
        stringsUsed.push_back(str.c_str());
        data->bones.push_back(str);
    }

    for (int i = 0; i < mdlhdr.numlocalattachments; i++)
    {
        if (!attachments[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, attachments[i].szname.c_str()))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("AttachmentName: %s\n", str.c_str());
                data->attachments.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("AttachmentReusedName: %s\n", attachments[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("AttachmentName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numhitboxsets; i++)
    {
        if (!hitboxsets[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, hitboxsets[i].szname.c_str()))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("HitboxSetName: %s\n", str.c_str());
                data->hitboxsets.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("HitboxSetReusedName: %s\n", hitboxsets[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("HitboxSetName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numlocalanim; i++)
    {
        if (!animdescs[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, animdescs[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("AnimDescName: %s\n", str.c_str());
                data->anims.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("AnimDescReusedName: %s\n", animdescs[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("AnimDescName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numlocalseq; i++)
    {
        std::vector<std::string> _activities;
        std::vector<std::string> _activityEvents;
        std::vector<std::string> _activityModifiers;

        int seqPos = (seqs[i].baseptr - seqs[i].activitymodifierindex) * -1;

        std::string szName = ReadNullTermStr(true);
        Logger::Info("SeqName: %s\n", szName.c_str());
        std::string szActivity = seqs[i].szactivityname;

        //Logger::Info("TestSeqName: %s\n", seqs[i].szlabel.c_str());
        //Logger::Info("TestSeqActivity: %s\n", seqs[i].szactivityname.c_str());
        //Logger::Info("TestSeqActivitySize: %d\n", seqs[i].szactivityname.size());
        bool test = seqs[i].szactivityname.empty();

        //if(test)Logger::Info("true\n");
        //else Logger::Info("false\n");

        if (!seqs[i].szactivityname.empty())
        {
            if (!ContainsString2(stringsUsed, szActivity))
            {
                szActivity = ReadNullTermStr(true);
                Logger::Info("SeqActivity: %s\n", szActivity.c_str());
                stringsUsed.push_back(seqs[i].szactivityname);
                _activities.push_back(seqs[i].szactivityname);
            }
            else
            {
                szActivity = "";
                Logger::Info("SeqActivityReusedName: %s\n", seqs[i].szactivityname.c_str());
            }
        }
        else
        {
            Logger::Info("SeqActivity: %s\n", "Empty");
        }

        for (int j = 0; j < seqs[i].numevents; j++)
        {
            int pos = Position();
            std::string eventName = seqs[i].szeventnames[j];
            //Logger::Info("TestSeqEvent: %s\n", seqs[i].szeventnames[j].c_str());

//            if (ContainsString2(stringsUsed, seqs[i].szeventnames[j]) || eventName == "")
//            {
//                _activityEvents.push_back("");
//            }
            if (!seqs[i].szeventnames[j].empty())
            {
                if (!ContainsString2(stringsUsed, seqs[i].szeventnames[j]))
                {
                    eventName = ReadNullTermStr(true);
                    Logger::Info("eventName: %s\n", eventName.c_str());
                    stringsUsed.push_back(seqs[i].szeventnames[j]);
                    _activityEvents.push_back(seqs[i].szeventnames[j]);
                }
                else
                {
                    _activityEvents.push_back("");
                    Logger::Info("eventNameReusedName: %s\n", seqs[i].szeventnames[j].c_str());
                }
            }
            else
            {
                _activityEvents.push_back("");
                Logger::Info("SeqActivity: %s\n", "Empty");
            }
        }

        for (int j = 0; j < seqs[i].numactivitymodifiers; j++)
        {
            int pos = Position();
            std::string actModName = seqs[i].szactivitymodifiernames[j];
            if (!seqs[i].szactivitymodifiernames[j].empty())
            {
                if (!ContainsString2(stringsUsed, seqs[i].szactivitymodifiernames[j]))
                {
                    actModName = ReadNullTermStr(true);
                    Logger::Info("actModName: %s\n", actModName.c_str());
                    stringsUsed.push_back(seqs[i].szactivitymodifiernames[j]);
                    _activityModifiers.push_back(seqs[i].szactivitymodifiernames[j]);
                }
                else
                {
                    _activityModifiers.push_back("");
                    Logger::Info("actModReusedName: %s\n", seqs[i].szactivitymodifiernames[j].c_str());
                }
            }
            else
            {
                _activityModifiers.push_back("");
                Logger::Info("actModName: %s\n", "Empty");
            }
        }
        mstudioseqstring_t_v49 _seq = { szName, szActivity, _activityEvents, _activityModifiers };

        data->seqs.push_back(_seq);
    }

    for (int i = 0; i < mdlhdr.numlocalnodes; i++)
    {
        if (!nodes[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, nodes[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("NodeName: %s\n", str.c_str());
                data->nodes.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("NodeReusedName: %s\n", nodes[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("NodeName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numbodyparts; i++)
    {
        if (!bodyparts[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, bodyparts[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("BodyPartName: %s\n", str.c_str());
                data->bodyparts.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("BodyPartReusedName: %s\n", bodyparts[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("BodyPartName: %s\n", "Empty");
        }

    }
    Logger::Info("Test:\n");
    for (int i = 0; i < mdlhdr.numhitboxsets; i++)
    {
        if (hitboxsets[i].numhitboxes > 0)
        {
            for (int j = 0; j < hitboxsets[i].numhitboxes; j++)
            {
                if (!hitboxsets[i].hitboxes[j].szname.empty())
                {
                    Logger::Info("Test:\n");
                    if (!ContainsString2(stringsUsed, hitboxsets[i].hitboxes[j].szname))
                    {
                        std::string str2 = ReadNullTermStr(true);
                        Logger::Info("HitboxName: %s\n", str2.c_str());
                        data->hitboxes.push_back(str2);
                        stringsUsed.push_back(str2);
                    }
                    else
                    {
                        Logger::Info("HitboxReusedName: %s\n", hitboxsets[i].hitboxes[j].szname.c_str());
                    }
                }
                else
                {
                    Logger::Info("HitboxName: %s\n", "Empty");
                }
            }
        }
    }

    for (int i = 0; i < mdlhdr.numlocalposeparameters; i++)
    {
        if (!poseparamdescs[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, poseparamdescs[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("PoseParamName: %s\n", str.c_str());
                data->poseparams.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("PoseParamReusedName: %s\n", poseparamdescs[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("PoseParamName: %s\n", "Empty");
        }

    }

    for (int i = 0; i < mdlhdr.numikchains; i++)
    {
        if (!ikchains[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, ikchains[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("IkChainName: %s\n", str.c_str());
                data->ikchains.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("IkChainReusedName: %s\n", ikchains[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("IkChainName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numincludemodels; i++)
    {
        if (!includemodels[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, includemodels[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("IncludeModelName: %s\n", str.c_str());
                data->includemodel.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("IncludeModelReusedName: %s\n", includemodels[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("IncludeModelName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numtextures; i++)
    {
        if (!textures[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, textures[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("TextureName: %s\n", str.c_str());
                data->textures.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("TextureReusedName: %s\n", textures[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("TextureName: %s\n", "Empty");
        }
    }

    for (int i = 0; i < mdlhdr.numcdtextures; i++)
    {
        if (!cdmaterials[i].szname.empty())
        {
            if (!ContainsString2(stringsUsed, cdmaterials[i].szname))
            {
                std::string str = ReadNullTermStr(true);
                Logger::Info("CdMaterialName: %s\n", str.c_str());
                data->cdmaterials.push_back(str);
                stringsUsed.push_back(str);
            }
            else
            {
                Logger::Info("CdMaterialReusedName: %s\n", cdmaterials[i].szname.c_str());
            }
        }
        else
        {
            Logger::Info("CdMaterialName: %s\n", "Empty");
        }
    }
}

void BinaryReader::Read(pertriheader_t* data)
{
    Stream.read((char*)&data->version, sizeof(int));
    Stream.read((char*)&data->bbmin, sizeof(Vector3));
    Stream.read((char*)&data->bbmax, sizeof(Vector3));
    Stream.read((char*)&data->unused, sizeof(int) * 8);    // negative numbers are -(leafs + 1), not nodes
}

void BinaryReader::Read(aabbnode* data)
{
    Stream.read((char*)&data->unk, sizeof(short) * 3);
    Stream.read((char*)&data->unk1, sizeof(short) * 3);
    Stream.read((char*)&data->children, sizeof(int) * 2);    // negative numbers are -(leafs + 1), not nodes
}

void BinaryReader::Read(aabbcollmaskmap* data)
{
    Stream.read((char*)&data->collmaskid, sizeof(short) * 3);
    Stream.read((char*)&data->unk1, sizeof(short)); //Contents?
}

void BinaryReader::Read(aabbleaf* data)
{
    //Logger::Info("Pos Read: %d\n", Position());
    Stream.read((char*)&data->mins, sizeof(short) * 3);
    Stream.read((char*)&data->maxs, sizeof(short) * 3);
    Stream.read((char*)&data->unk, sizeof(int));

    for (int i = 0; i < 12; i++)
    {
        Stream.read((char*)&data->map[i].collmaskid, sizeof(short) * 3);
        Stream.read((char*)&data->map[i].unk1, sizeof(short)); //Contents?
    }
}

void BinaryReader::Read(aabbcollisionmask* data, int numVerts)
{
    for (int i = 0; i < numVerts; i++)
    {
        QuaternionShort unk{};
        Stream.read((char*)&unk, sizeof(Quat48));
        data->unk.push_back(unk);
    }
}

#pragma endregion

#pragma region PHYREAD

void BinaryReader::Read(phyheader_t* data)
{
    Stream.read((char*)&data->size, sizeof(int));
    Stream.read((char*)&data->id, sizeof(int));
    Stream.read((char*)&data->numsolids, sizeof(int));
    Stream.read((char*)&data->checksum, sizeof(int));
}

void BinaryReader::Read(compactsurfaceheader_t* data)
{
    Stream.read((char*)&data->size, sizeof(int));
    Stream.read((char*)&data->id, sizeof(int));
    Stream.read((char*)&data->version, sizeof(short));
    Stream.read((char*)&data->modeltype, sizeof(short));
    Stream.read((char*)&data->surfacesize, sizeof(int));
    Stream.read((char*)&data->dragaxisareas, sizeof(Vector3));
    Stream.read((char*)&data->axismaparea, sizeof(int));
}

void BinaryReader::Read(legacysurfaceheader_t* data)
{
    Stream.read((char*)&data->mass_center, sizeof(Vector3));
    Stream.read((char*)&data->rotation_inertia, sizeof(Vector3));
    Stream.read((char*)&data->upper_limit_radius, sizeof(float));
    Stream.read((char*)&data->max_deviation, sizeof(int));
    //Stream.read((char*)&data->byte_size, sizeof(int));
    Stream.read((char*)&data->nodeindex, sizeof(int));
    Stream.read((char*)&data->pad, sizeof(int) * 2);
    Stream.read((char*)&data->id, sizeof(int));
}

void BinaryReader::Read(trianglefaceheader_t* data)
{
    Stream.read((char*)&data->vertexindex, sizeof(int));
    Stream.read((char*)&data->parent, sizeof(int));
    Stream.read((char*)&data->flags, sizeof(int));
    Stream.read((char*)&data->numfaces, sizeof(int));
}

void BinaryReader::Read(trianglevertmap_t* data)
{
    Stream.read((char*)&data->faceindex, sizeof(std::byte));
    Stream.read((char*)&data->unkdata, sizeof(std::byte) * 3);
    Stream.read((char*)&data->vertid, sizeof(short));
    Stream.read((char*)&data->unkdata1, sizeof(std::byte) * 2);
    Stream.read((char*)&data->vertid1, sizeof(short));
    Stream.read((char*)&data->unkdata2, sizeof(std::byte) * 2);
    Stream.read((char*)&data->vertid2, sizeof(short));
    Stream.read((char*)&data->unkdata3, sizeof(std::byte) * 2);
}

void BinaryReader::Read(phyvertex_t* data)
{
    Stream.read((char*)&data->pos, sizeof(Vector3));
    Stream.read((char*)&data->unk, sizeof(int));
}

void BinaryReader::Read(phynode_t* data)
{
    Stream.read((char*)&data->rightnodeindex, sizeof(int));
    Stream.read((char*)&data->convexindex, sizeof(int));
    Stream.read((char*)&data->center, sizeof(Vector3));
    Stream.read((char*)&data->radius, sizeof(int));
    Stream.read((char*)&data->bboxsize, sizeof(int));
}

void BinaryReader::Read(physection_t* data)
{
//    Logger::Info("StartPos: %d\n", Position());
    std::vector<int> usedVerts;

//    Logger::Info("Hdr1Pos: %d\n", Position());
    Read(&data->surfaceheader);
//    Logger::Info("Hdr2Pos: %d\n", Position());
    Read(&data->surfaceheader2);
//    Logger::Info("Hdr3Pos: %d\n", Position());
    Read(&data->trihdr);
    for (int i = 0; i < data->trihdr.numfaces; i++)
    {
//        Logger::Info("VertMapPos %d: %d\n", i, Position());
        trianglevertmap_t vertmap; Read(&vertmap);
//        Logger::Info("VertMapEndPos: %d\n", Position());
        if(!Contains(usedVerts, vertmap.vertid))  usedVerts.push_back(vertmap.vertid);
        if(!Contains(usedVerts, vertmap.vertid1)) usedVerts.push_back(vertmap.vertid1);
        if(!Contains(usedVerts, vertmap.vertid2)) usedVerts.push_back(vertmap.vertid2);

        data->vertmaps.push_back(vertmap);
    }

    for (int i = 0; i < usedVerts.size(); i++)
    {
//        Logger::Info("VertPos %d: %d\n", i, Position());
        phyvertex_t vertex; Read(&vertex);
//        Logger::Info("VertEndPos: %d\n", Position());
        data->vertices.push_back(vertex);
    }
//    Logger::Info("NodePos: %d\n", Position());
    Read(&data->node);
//    Logger::Info("EndPos: %d\n", Position());
}

#pragma endregion

void BinaryReader::seek(int pos)
{
    if(pos > size){
        Logger::Critical("BinaryReader tried to seek beyond the limits of the file!\n");
    }
    Stream.clear(std::_S_goodbit); //lmao
    Stream.clear(std::_S_failbit);
    Stream.clear(std::_S_eofbit);
    Stream.seekg(pos);
    if (Stream.tellg() != pos) {
        Logger::Critical("BinaryReader failed to seek the read cursor!\n");
        abort();
    }
    Stream.seekp(pos);
    if (Stream.tellp() != pos) {
        Logger::Critical("BinaryReader failed to seek the write cursor!\n");
    }
}

#pragma region WRITE_UTILITY

BinaryWriter::BinaryWriter(const char* filename)
{
  _filename = filename;
  Stream = fstream(_filename, ios::binary | ios::out);
  
  struct stat results;
  if (stat(filename, &results) == 0){
    size = results.st_size;
  }
  else{
    fprintf(stderr,"Could not get stats of input file!\n");
  }

}
BinaryWriter::~BinaryWriter()
{
  Stream.close();
  Logger::Info("BinaryWriter for file \"%s\" closed\n", _filename);
}
void BinaryWriter::write(const char* data, int size)
{
  Stream.write(data, size);
}
void BinaryWriter::Write(char* data)
{
  Stream.write((char*)&data, sizeof(char*));
}
void BinaryWriter::Write(byte data)
{
  Stream.write((char*)&data, sizeof(byte));
}
void BinaryWriter::Write(byte* data) 
{
    Stream.write((char*)&data, sizeof(byte));
}
void BinaryWriter::Write(byte data[], int size)
{
  Stream.write((char*)&data, sizeof(byte)*size);
}
void BinaryWriter::Write(char* data[])
{
  Stream.write((char*)&data, sizeof(data)/sizeof(data[0]));
}
void BinaryWriter::Write(int data)
{
  Stream.write((char*)&data, sizeof(int));
}
void BinaryWriter::Write(int data[])
{
  Stream.write((char*)&data, sizeof(int) * sizeof(data)/sizeof(data[0]));
}
void BinaryWriter::Write(float data)
{
  Stream.write((char*)&data, sizeof(float));
}
int BinaryWriter::Position()
{
  if(Stream.tellp() <= -1){
    Logger::Critical("BinaryWriter's seek position was invalid! \n");
    abort();
  }
  return Stream.tellp();
}
void BinaryWriter::Write(short data) 
{
    Stream.write((char*)&data, sizeof(short));
}
void BinaryWriter::Write(uint32_t data) 
{
    Stream.write((char*)&data, sizeof(uint32_t));
}
void BinaryWriter::Write(Vector data)
{
  Stream.write((char*)&data.x, sizeof(float));
  Stream.write((char*)&data.y, sizeof(float));
  Stream.write((char*)&data.z, sizeof(float));
}
void BinaryWriter::Write(Vector3Short data) 
{
    Stream.write((char*)&data.x, sizeof(short));
    Stream.write((char*)&data.y, sizeof(short));
    Stream.write((char*)&data.z, sizeof(short));
}
void BinaryWriter::Write(Vector3 data) 
{
    Stream.write((char*)&data.x, sizeof(float));
    Stream.write((char*)&data.y, sizeof(float));
    Stream.write((char*)&data.z, sizeof(float));
}
void BinaryWriter::Write(RadianEuler data) 
{
    Stream.write((char*)&data.x, sizeof(float));
    Stream.write((char*)&data.y, sizeof(float));
    Stream.write((char*)&data.z, sizeof(float));
}
void BinaryWriter::Write(matrix3x4_t data) 
{
    Stream.write((char*)&data.c0r0, sizeof(float));
    Stream.write((char*)&data.c1r0, sizeof(float));
    Stream.write((char*)&data.c2r0, sizeof(float));
    Stream.write((char*)&data.c3r0, sizeof(float));

    Stream.write((char*)&data.c0r1, sizeof(float));
    Stream.write((char*)&data.c1r1, sizeof(float));
    Stream.write((char*)&data.c2r1, sizeof(float));
    Stream.write((char*)&data.c3r1, sizeof(float));

    Stream.write((char*)&data.c0r2, sizeof(float));
    Stream.write((char*)&data.c1r2, sizeof(float));
    Stream.write((char*)&data.c2r2, sizeof(float));
    Stream.write((char*)&data.c3r2, sizeof(float));
}
void BinaryWriter::Write(QuaternionShort data) 
{
    Stream.write((char*)&data.one, sizeof(short));
    Stream.write((char*)&data.i, sizeof(short));
    Stream.write((char*)&data.j, sizeof(short));
    Stream.write((char*)&data.k, sizeof(short));
    //printf("read a Vector\n");
}

void BinaryWriter::Write(Quaternion data) 
{
    Stream.write((char*)&data.one, sizeof(float));
    Stream.write((char*)&data.i, sizeof(float));
    Stream.write((char*)&data.j, sizeof(float));
    Stream.write((char*)&data.k, sizeof(float));
    //printf("read a Vector\n");
}
void BinaryWriter::Write(std::string data)
{
    Stream.write((char*)&data, data.size());
}

template <typename T> void BinaryWriter::Write(std::vector<T> data)
{
    for (int i = 0; i < data.size(); i++)
    {
        T value; Stream.read((char*)&value, sizeof(T));
        data.push_back(value);
    }
}

void BinaryWriter::Padding(int count)
{
    for (int i = 0; i < count; i++)
    {
        std::byte value = (std::byte)0;
        Stream.write((char*)&value, 1);
    }
}
#pragma endregion

#pragma region V49WRITE
void BinaryWriter::Write(studiohdr_t_v49 data) 
{

    Stream.write((char*)&data.id, sizeof(int));                        
    Stream.write((char*)&data.version, sizeof(int));                   
    Stream.write((char*)&data.checksum, sizeof(int));                  
    Stream.write((char*)&data.name, 64);                               

    Stream.write((char*)&data.length, sizeof(int));                    

    Stream.write((char*)&data.eyeposition, sizeof(Vector3));	       

    Stream.write((char*)&data.illumposition, sizeof(Vector3));	       

    Stream.write((char*)&data.hull_min, sizeof(Vector3));		       
    Stream.write((char*)&data.hull_max, sizeof(Vector3));

    Stream.write((char*)&data.view_bbmin, sizeof(Vector3));		   
    Stream.write((char*)&data.view_bbmax, sizeof(Vector3));

    Stream.write((char*)&data.flags, sizeof(int));

    Stream.write((char*)&data.numbones, sizeof(int)); 
    Stream.write((char*)&data.boneindex, sizeof(int));

    Stream.write((char*)&data.numbonecontrollers, sizeof(int)); 
    Stream.write((char*)&data.bonecontrollerindex, sizeof(int));

    Stream.write((char*)&data.numhitboxsets, sizeof(int));
    Stream.write((char*)&data.hitboxsetindex, sizeof(int));

    Stream.write((char*)&data.numlocalanim, sizeof(int)); 
    Stream.write((char*)&data.localanimindex, sizeof(int));  

    Stream.write((char*)&data.numlocalseq, sizeof(int));   
    Stream.write((char*)&data.localseqindex, sizeof(int));

    Stream.write((char*)&data.activitylistversion, sizeof(int));
    Stream.write((char*)&data.eventsindexed, sizeof(int));


    Stream.write((char*)&data.numtextures, sizeof(int));
    Stream.write((char*)&data.textureindex, sizeof(int));


    Stream.write((char*)&data.numcdtextures, sizeof(int));
    Stream.write((char*)&data.cdtextureindex, sizeof(int));


    Stream.write((char*)&data.numskinref, sizeof(int));
    Stream.write((char*)&data.numskinfamilies, sizeof(int));
    Stream.write((char*)&data.skinindex, sizeof(int));

    Stream.write((char*)&data.numbodyparts, sizeof(int));
    Stream.write((char*)&data.bodypartindex, sizeof(int));

    Stream.write((char*)&data.numlocalattachments, sizeof(int));
    Stream.write((char*)&data.localattachmentindex, sizeof(int));

    Stream.write((char*)&data.numlocalnodes, sizeof(int));
    Stream.write((char*)&data.localnodeindex, sizeof(int));
    Stream.write((char*)&data.localnodenameindex, sizeof(int));

    Stream.write((char*)&data.numflexdesc, sizeof(int));
    Stream.write((char*)&data.flexdescindex, sizeof(int));

    Stream.write((char*)&data.numflexcontrollers, sizeof(int));
    Stream.write((char*)&data.flexcontrollerindex, sizeof(int));

    Stream.write((char*)&data.numflexrules, sizeof(int));
    Stream.write((char*)&data.flexruleindex, sizeof(int));

    Stream.write((char*)&data.numikchains, sizeof(int));
    Stream.write((char*)&data.ikchainindex, sizeof(int));

    Stream.write((char*)&data.nummouths, sizeof(int));
    Stream.write((char*)&data.mouthindex, sizeof(int));

    Stream.write((char*)&data.numlocalposeparameters, sizeof(int));
    Stream.write((char*)&data.localposeparamindex, sizeof(int));

    Stream.write((char*)&data.surfacepropindex, sizeof(int));

    Stream.write((char*)&data.keyvalueindex, sizeof(int));
    Stream.write((char*)&data.keyvaluesize, sizeof(int));

    Stream.write((char*)&data.numlocalikautoplaylocks, sizeof(int));
    Stream.write((char*)&data.localikautoplaylockindex, sizeof(int));


    Stream.write((char*)&data.mass, sizeof(float));
    Stream.write((char*)&data.contents, sizeof(int));


    Stream.write((char*)&data.numincludemodels, sizeof(int));
    Stream.write((char*)&data.includemodelindex, sizeof(int));

    Stream.write((char*)&data.virtualModel, sizeof(uint32_t));

    Stream.write((char*)&data.szanimblocknameindex, sizeof(int));
    Stream.write((char*)&data.numanimblocks, sizeof(int));
    Stream.write((char*)&data.animblockindex, sizeof(int));
    Stream.write((char*)&data.animblockModel, sizeof(uint32_t));

    Stream.write((char*)&data.bonetablebynameindex, sizeof(int));

    Stream.write((char*)&data.pVertexBase, sizeof(uint32_t));
    Stream.write((char*)&data.pIndexBase, sizeof(uint32_t));

    Stream.write((char*)&data.constdirectionallightdot, sizeof(std::byte));

    Stream.write((char*)&data.rootLOD, sizeof(std::byte));

    Stream.write((char*)&data.numAllowedRootLODs, sizeof(std::byte));

    Stream.write((char*)&data.unused, sizeof(std::byte));

    Stream.write((char*)&data.unused4, sizeof(int));     

    Stream.write((char*)&data.numflexcontrollerui, sizeof(int));
    Stream.write((char*)&data.flexcontrolleruiindex, sizeof(int));

    Stream.write((char*)&data.flVertAnimFixedPointScale, sizeof(float));
    Stream.write((char*)&data.surfacepropLookup, sizeof(int));

    Stream.write((char*)&data.studiohdr2index, sizeof(int));

    Stream.write((char*)&data.unused2, sizeof(int));
}

void BinaryWriter::Write(studiohdr2_t_v49 data) 
{

    Stream.write((char*)&data.numsrcbonetransform, sizeof(int));
    Stream.write((char*)&data.srcbonetransformindex, sizeof(int));

    Stream.write((char*)&data.illumpositionattachmentindex, sizeof(int));

    Stream.write((char*)&data.flMaxEyeDeflection, sizeof(float));

    Stream.write((char*)&data.linearboneindex, sizeof(int));

    Stream.write((char*)&data.sznameindex, sizeof(int));

    Stream.write((char*)&data.m_nBoneFlexDriverCount, sizeof(int));
    Stream.write((char*)&data.m_nBoneFlexDriverIndex, sizeof(int));

    Stream.write((char*)&data.reserved, sizeof(int) * 56);
}

void BinaryWriter::Write(mstudiobone_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.parent, sizeof(int));
    Stream.write((char*)&data.bonecontroller, sizeof(int) * 6);
    Stream.write((char*)&data.pos, sizeof(Vector3));
    Stream.write((char*)&data.quat, sizeof(Quaternion));
    Stream.write((char*)&data.rot, sizeof(RadianEuler));
    Stream.write((char*)&data.posscale, sizeof(Vector3));
    Stream.write((char*)&data.rotscale, sizeof(Vector3));
    Stream.write((char*)&data.poseToBone, sizeof(matrix3x4_t));
    Stream.write((char*)&data.qAlignment, sizeof(Quaternion));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.proctype, sizeof(int));
    Stream.write((char*)&data.procindex, sizeof(int));
    Stream.write((char*)&data.physicsbone, sizeof(int));
    Stream.write((char*)&data.surfacepropidx, sizeof(int));
    Stream.write((char*)&data.contents, sizeof(int));
    Stream.write((char*)&data.surfacepropLookup, sizeof(int));
    Stream.write((char*)&data.unused, sizeof(int) * 7);
    //printf("read a Vector\n");
}
void BinaryWriter::Write(mstudiojigglebone_t_v49 data) 
{
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.length, sizeof(float));
    Stream.write((char*)&data.tipMass, sizeof(float));


    Stream.write((char*)&data.yawStiffness, sizeof(float));
    Stream.write((char*)&data.yawDamping, sizeof(float));
    Stream.write((char*)&data.pitchStiffness, sizeof(float));
    Stream.write((char*)&data.pitchDamping, sizeof(float));
    Stream.write((char*)&data.alongStiffness, sizeof(float));
    Stream.write((char*)&data.alongDamping, sizeof(float));


    Stream.write((char*)&data.angleLimit, sizeof(float));


    Stream.write((char*)&data.minYaw, sizeof(float));
    Stream.write((char*)&data.maxYaw, sizeof(float));
    Stream.write((char*)&data.yawFriction, sizeof(float));
    Stream.write((char*)&data.yawBounce, sizeof(float));


    Stream.write((char*)&data.minPitch, sizeof(float));
    Stream.write((char*)&data.maxPitch, sizeof(float));
    Stream.write((char*)&data.pitchFriction, sizeof(float));
    Stream.write((char*)&data.pitchBounce, sizeof(float));


    Stream.write((char*)&data.baseMass, sizeof(float));
    Stream.write((char*)&data.baseStiffness, sizeof(float));
    Stream.write((char*)&data.baseDamping, sizeof(float));
    Stream.write((char*)&data.baseMinLeft, sizeof(float));
    Stream.write((char*)&data.baseMaxLeft, sizeof(float));
    Stream.write((char*)&data.baseLeftFriction, sizeof(float));
    Stream.write((char*)&data.baseMinUp, sizeof(float));
    Stream.write((char*)&data.baseMaxUp, sizeof(float));
    Stream.write((char*)&data.baseUpFriction, sizeof(float));
    Stream.write((char*)&data.baseMinForward, sizeof(float));
    Stream.write((char*)&data.baseMaxForward, sizeof(float));
    Stream.write((char*)&data.baseForwardFriction, sizeof(float));
}
void BinaryWriter::Write(mstudioboneflexdriver_t_v49 data) 
{
    Stream.write((char*)&data.m_nBoneIndex, sizeof(int));
    Stream.write((char*)&data.m_nControlCount, sizeof(int));
    Stream.write((char*)&data.m_nControlIndex, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 3);
}

void BinaryWriter::Write(mstudioattachment_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.localbone, sizeof(int));
    Stream.write((char*)&data.localmatrix, sizeof(matrix3x4_t));
    Stream.write((char*)&data.unused, sizeof(int) * 8);
}

void BinaryWriter::Write(mstudioikchain_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.linktype, sizeof(int));
    Stream.write((char*)&data.numlinks, sizeof(int));

    Stream.write((char*)&data.linkindex, sizeof(int));
}

void BinaryWriter::Write(mstudioiklink_t_v49 data) 
{
    Stream.write((char*)&data.bone, sizeof(int));
    Stream.write((char*)&data.kneeDir, sizeof(Vector3));
    Stream.write((char*)&data.unused0, sizeof(Vector3));
}

void BinaryWriter::Write(mstudioiklock_t_v49 data) 
{
    Stream.write((char*)&data.chain, sizeof(int));
    Stream.write((char*)&data.flPosWeight, sizeof(float));
    Stream.write((char*)&data.flLocalQWeight, sizeof(float));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.unused, sizeof(int) * 4);
}

void BinaryWriter::Write(mstudioposeparamdesc_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.start, sizeof(float));
    Stream.write((char*)&data.end, sizeof(float));
    Stream.write((char*)&data.loop, sizeof(float));
}

void BinaryWriter::Write(mstudiosrcbonetransform_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.pretransform, sizeof(matrix3x4_t));
    Stream.write((char*)&data.posttransform, sizeof(matrix3x4_t));
}

void BinaryWriter::Write(mstudiolinearbone_t_v49 data) 
{
    Stream.write((char*)&data.numbones, sizeof(int));
    Stream.write((char*)&data.flagsindex, sizeof(int));
    Stream.write((char*)&data.parentindex, sizeof(int));
    Stream.write((char*)&data.posindex, sizeof(int));
    Stream.write((char*)&data.quatindex, sizeof(int));
    Stream.write((char*)&data.rotindex, sizeof(int));
    Stream.write((char*)&data.posetoboneindex, sizeof(int));
    Stream.write((char*)&data.posscaleindex, sizeof(int));
    Stream.write((char*)&data.rotscaleindex, sizeof(int));
    Stream.write((char*)&data.qalignmentindex, sizeof(int));
    Stream.write((char*)&data.unused, sizeof(int) * 6);
}

void BinaryWriter::Write(mstudiolinearbonedata_t_v49 data, int numbones) 
{
    Stream.write((char*)&data.flags, sizeof(uint32_t) * numbones);
    Stream.write((char*)&data.parents, sizeof(uint32_t) * numbones);
    Stream.write((char*)&data.bonePos, sizeof(Vector3) * numbones);
    Stream.write((char*)&data.boneQuat, sizeof(Quaternion) * numbones);
    Stream.write((char*)&data.boneRot, sizeof(RadianEuler) * numbones);
    Stream.write((char*)&data.poseToBone, sizeof(matrix3x4_t) * numbones);
    Stream.write((char*)&data.posScale, sizeof(Vector3) * numbones);
    Stream.write((char*)&data.rotScale, sizeof(Vector3) * numbones);
    Stream.write((char*)&data.boneAlignment, sizeof(Quaternion) * numbones);
}

void BinaryWriter::Write(mstudiolinearbonedata_t_v53 data, int numbones)
{
    Stream.write((char*)&data.flags, sizeof(uint32_t) * numbones);
    Stream.write((char*)&data.parents, sizeof(uint32_t) * numbones);
    Stream.write((char*)&data.bonePos, sizeof(Vector3) * numbones);
    Stream.write((char*)&data.boneQuat, sizeof(Quaternion) * numbones);
    Stream.write((char*)&data.boneRot, sizeof(RadianEuler) * numbones);
    Stream.write((char*)&data.poseToBone, sizeof(matrix3x4_t) * numbones);
    Stream.write((char*)&data.rotScale, sizeof(Vector3) * numbones);
    Stream.write((char*)&data.boneAlignment, sizeof(Quaternion) * numbones);
}

void BinaryWriter::Write(mstudiohitboxset_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.numhitboxes, sizeof(int));
    Stream.write((char*)&data.hitboxindex, sizeof(int));
}

void BinaryWriter::Write(mstudiobbox_t_v49 data) 
{
    Stream.write((char*)&data.bone, sizeof(int));
    Stream.write((char*)&data.group, sizeof(int));
    Stream.write((char*)&data.bbmin, sizeof(Vector3));
    Stream.write((char*)&data.bbmax, sizeof(Vector3));
    Stream.write((char*)&data.szhitboxnameindex, sizeof(int));
    Stream.write((char*)&data.unused, sizeof(int) * 8);

}

void BinaryWriter::Write(mstudiobonenametable_t_v49 data) 
{

    short fill = 0;
    for (int i = 0; i < data.bones.size(); i++)
    {
        Stream.write((char*)&data.bones[i], sizeof(byte));
    }
    Stream.write((char*)&fill, sizeof(short));
}

void BinaryWriter::Write(mstudioanimdesc_t_v49 data) 
{
    Stream.write((char*)&data.baseptr, sizeof(int));
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.fps, sizeof(float));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.numframes, sizeof(int));
    Stream.write((char*)&data.nummovements, sizeof(int));
    Stream.write((char*)&data.movementindex, sizeof(int));
    Stream.write((char*)&data.ikrulezeroframeindex, sizeof(int));
    Stream.write((char*)&data.unused1, sizeof(int) * 5);
    Stream.write((char*)&data.animblock, sizeof(int));
    Stream.write((char*)&data.animindex, sizeof(int));
    Stream.write((char*)&data.numikrules, sizeof(int));
    Stream.write((char*)&data.ikruleindex, sizeof(int));
    Stream.write((char*)&data.animblockikruleindex, sizeof(int));
    Stream.write((char*)&data.numlocalhierarchy, sizeof(int));
    Stream.write((char*)&data.localhierarchyindex, sizeof(int));
    Stream.write((char*)&data.sectionindex, sizeof(int));
    Stream.write((char*)&data.sectionframes, sizeof(int));
    Stream.write((char*)&data.zeroframespan, sizeof(short));
    Stream.write((char*)&data.zeroframecount, sizeof(short));
    Stream.write((char*)&data.zeroframeindex, sizeof(int));
    Stream.write((char*)&data.zeroframestalltime, sizeof(float));
}

void BinaryWriter::Write(sectionindexesindex_t_v49 data) 
{
    Stream.write((char*)&data.anioffset, sizeof(int));
    Stream.write((char*)&data.sectionoffset, sizeof(int));
}

void BinaryWriter::Write(mstudioanim_valueptr_t_v49 data) 
{
    Stream.write((char*)&data.offset, sizeof(Vector3Short));
}

void BinaryWriter::Write(mstudioanim_t_v49 data) 
{
    Stream.write((char*)&data.bone, sizeof(byte));
    Stream.write((char*)&data.flags, sizeof(byte));
    Stream.write((char*)&data.nextoffset, sizeof(short));
    if ((int)data.flags & STUDIO_ANIM_ANIMROT) Stream.write((char*)&data.animrot, sizeof(Vector3Short));

    if ((int)data.flags & STUDIO_ANIM_ANIMPOS) Stream.write((char*)&data.animpos, sizeof(Vector3Short));

    if ((int)data.flags & STUDIO_ANIM_RAWROT) Stream.write((char*)&data.rawrot, sizeof(QuaternionShort));

    if ((int)data.flags & STUDIO_ANIM_RAWROT2) Stream.write((char*)&data.rawrot2, sizeof(QuaternionShort));

    if ((int)data.flags & STUDIO_ANIM_RAWPOS) Stream.write((char*)&data.rawpos, sizeof(Vector3Short));
}

void BinaryWriter::Write(mstudioanimdata_t_v49 data)
{
    for (int i = 0; i < data.arry.size(); i++)
    {
        Stream.write((char*)&data.arry[i], 1);
    }
}

void BinaryWriter::Write(mstudioikrule_t_v49 data) 
{
    Stream.write((char*)&data.index, sizeof(int));

    Stream.write((char*)&data.type, sizeof(int));
    Stream.write((char*)&data.chain, sizeof(int));

    Stream.write((char*)&data.bone, sizeof(int));

    Stream.write((char*)&data.slot, sizeof(int));
    Stream.write((char*)&data.height, sizeof(float));
    Stream.write((char*)&data.radius, sizeof(float));
    Stream.write((char*)&data.floor, sizeof(float));
    Stream.write((char*)&data.pos, sizeof(Vector3));
    Stream.write((char*)&data.q, sizeof(Quaternion));

    Stream.write((char*)&data.compressedikerrorindex, sizeof(int));

    Stream.write((char*)&data.unused2, sizeof(int));

    Stream.write((char*)&data.iStart, sizeof(int));
    Stream.write((char*)&data.ikerrorindex, sizeof(int));

    Stream.write((char*)&data.start, sizeof(float));
    Stream.write((char*)&data.peak, sizeof(float));
    Stream.write((char*)&data.tail, sizeof(float));
    Stream.write((char*)&data.end, sizeof(float));	

    Stream.write((char*)&data.unused3, sizeof(float));
    Stream.write((char*)&data.contact, sizeof(float));
    Stream.write((char*)&data.top, sizeof(float));

    Stream.write((char*)&data.unused6, sizeof(int));
    Stream.write((char*)&data.unused7, sizeof(int));
    Stream.write((char*)&data.unused8, sizeof(int));

    Stream.write((char*)&data.szattachmentindex, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 7);

}

void BinaryWriter::Write(mstudiocompressedikerror_t_v49 data) 
{
    Stream.write((char*)&data.scale, sizeof(float) * 6);
    Stream.write((char*)&data.index, sizeof(short) * 6);
}

void BinaryWriter::Write(mstudioikerror_t_v49 data) 
{
    Stream.write((char*)&data.pos, sizeof(Vector3));
    Stream.write((char*)&data.q, sizeof(Quaternion));
}

void BinaryWriter::Write(mstudioikrulezeroframe_t_v49 data) 
{
    Stream.write((char*)&data.chain, sizeof(short));
    Stream.write((char*)&data.slot, sizeof(short));
    Stream.write((char*)&data.start, sizeof(short));
    Stream.write((char*)&data.peak, sizeof(short));
    Stream.write((char*)&data.tail, sizeof(short));
    Stream.write((char*)&data.end, sizeof(short));

}

void BinaryWriter::Write(mstudioseqdescv49_t data) 
{

    Stream.write((char*)&data.baseptr, sizeof(int));

    Stream.write((char*)&data.szlabelindex, sizeof(int));

    Stream.write((char*)&data.szactivitynameindex, sizeof(int));

    Stream.write((char*)&data.flags, sizeof(int));

    Stream.write((char*)&data.activity, sizeof(int));
    Stream.write((char*)&data.actweight, sizeof(int));

    Stream.write((char*)&data.numevents, sizeof(int));
    Stream.write((char*)&data.eventindex, sizeof(int));

    Stream.write((char*)&data.bbmin, sizeof(Vector3));
    Stream.write((char*)&data.bbmax, sizeof(Vector3));

    Stream.write((char*)&data.numblends, sizeof(int));


    Stream.write((char*)&data.animindexindex, sizeof(int));

    Stream.write((char*)&data.movementindex, sizeof(int));		
    Stream.write((char*)&data.groupsize, sizeof(int) * 2);
    Stream.write((char*)&data.paramindex, sizeof(int) * 2);	    
    Stream.write((char*)&data.paramstart, sizeof(float) * 2);	
    Stream.write((char*)&data.paramend, sizeof(float) * 2);		
    Stream.write((char*)&data.paramparent, sizeof(int));

    Stream.write((char*)&data.fadeintime, sizeof(float));		
    Stream.write((char*)&data.fadeouttime, sizeof(float));		

    Stream.write((char*)&data.localentrynode, sizeof(int));		
    Stream.write((char*)&data.localexitnode, sizeof(int));		
    Stream.write((char*)&data.nodeflags, sizeof(int));			

    Stream.write((char*)&data.entryphase, sizeof(float));		
    Stream.write((char*)&data.exitphase, sizeof(float));		

    Stream.write((char*)&data.lastframe, sizeof(float));		

    Stream.write((char*)&data.nextseq, sizeof(int));
    Stream.write((char*)&data.pose, sizeof(int));

    Stream.write((char*)&data.numikrules, sizeof(int));

    Stream.write((char*)&data.numautolayers, sizeof(int));
    Stream.write((char*)&data.autolayerindex, sizeof(int));

    Stream.write((char*)&data.weightlistindex, sizeof(int));

    Stream.write((char*)&data.posekeyindex, sizeof(int));

    Stream.write((char*)&data.numiklocks, sizeof(int));
    Stream.write((char*)&data.iklockindex, sizeof(int));


    Stream.write((char*)&data.keyvalueindex, sizeof(int));
    Stream.write((char*)&data.keyvaluesize, sizeof(int));

    Stream.write((char*)&data.cycleposeindex, sizeof(int));

    Stream.write((char*)&data.activitymodifierindex, sizeof(int));
    Stream.write((char*)&data.numactivitymodifiers, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 5);

}

void BinaryWriter::Write(blendgroup_t_v49 data)
{
    Stream.write((char*)&data.blends, sizeof(short) * sizeof(data.blends));
}

void BinaryWriter::Write(posekey_t_v49 data, int groupSize) 
{
    if (groupSize == 3)
    {
        Stream.write((char*)&data.unk, sizeof(float) * 2);
        Stream.write((char*)&data.unk[0], sizeof(float));
    }
    else
    Stream.write((char*)&data.unk, sizeof(float) * groupSize);
}

void BinaryWriter::Write(mstudioevent_t_v49 data) 
{
    Stream.write((char*)&data.cycle, sizeof(float));
    Stream.write((char*)&data.event, sizeof(int));
    Stream.write((char*)&data.type, sizeof(int));
    Stream.write((char*)&data.options, 64);

    Stream.write((char*)&data.szeventindex, sizeof(int));

}

void BinaryWriter::Write(mstudioautolayer_t_v49 data) 
{
    Stream.write((char*)&data.iSequence, sizeof(short));
    Stream.write((char*)&data.iPose, sizeof(short));

    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.start, sizeof(float));
    Stream.write((char*)&data.peak, sizeof(float));
    Stream.write((char*)&data.tail, sizeof(float));
    Stream.write((char*)&data.end, sizeof(float));

}

void BinaryWriter::Write(mstudioactivitymodifier_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
}

void BinaryWriter::Write(seqweightlist_t_v49 data) 
{
    for (int i = 0; i < data.boneweight.size(); i++)
    {
        Stream.write((char*)&data.boneweight[i], sizeof(float));
    }
}

void BinaryWriter::Write(mstudiomodelgroup_t_v49 data) 
{
    Stream.write((char*)&data.szlabelindex, sizeof(int));
    Stream.write((char*)&data.sznameindex, sizeof(int));

}

void BinaryWriter::Write(mstudiobodyparts_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.nummodels, sizeof(int));
    Stream.write((char*)&data.base, sizeof(int));
    Stream.write((char*)&data.modelindex, sizeof(int));

}

void BinaryWriter::Write(mstudiomodel_t_v49 data) 
{
    Stream.write((char*)&data.name, 64);

    Stream.write((char*)&data.type, sizeof(int));

    Stream.write((char*)&data.boundingradius, sizeof(float));

    Stream.write((char*)&data.nummeshes, sizeof(int));
    Stream.write((char*)&data.meshindex, sizeof(int));


    Stream.write((char*)&data.numvertices, sizeof(int));
    Stream.write((char*)&data.vertexindex, sizeof(int));
    Stream.write((char*)&data.tangentsindex, sizeof(int));



    Stream.write((char*)&data.numattachments, sizeof(int));
    Stream.write((char*)&data.attachmentindex, sizeof(int));

    Stream.write((char*)&data.numeyeballs, sizeof(int));
    Stream.write((char*)&data.eyeballindex, sizeof(int));




    Stream.write((char*)&data.unused, sizeof(int) * 10);

}

void BinaryWriter::Write(mstudio_meshvertexdata_t_v49 data) 
{
    Stream.write((char*)&data.unk, sizeof(int));

    Stream.write((char*)&data.numLODVertexes, sizeof(int) * MAX_NUM_LODS);

}

void BinaryWriter::Write(mstudiomesh_t_v49 data) 
{
    Stream.write((char*)&data.material, sizeof(int));

    Stream.write((char*)&data.modelindex, sizeof(int));

    Stream.write((char*)&data.numvertices, sizeof(int));
    Stream.write((char*)&data.vertexoffset, sizeof(int));



    Stream.write((char*)&data.numflexes, sizeof(int));
    Stream.write((char*)&data.flexindex, sizeof(int));


    Stream.write((char*)&data.materialtype, sizeof(int));
    Stream.write((char*)&data.materialparam, sizeof(int));


    Stream.write((char*)&data.meshid, sizeof(int));

    Stream.write((char*)&data.center, sizeof(Vector3));

    Stream.write((char*)&data.vertexdata.unk, sizeof(int));

    Stream.write((char*)&data.vertexdata.numLODVertexes, sizeof(int) * MAX_NUM_LODS);

    Stream.write((char*)&data.unused, sizeof(int) * 8);

}

void BinaryWriter::Write(mstudioboneweight_t_v49 data) 
{
    Stream.write((char*)&data.weight, sizeof(float) * MAX_NUM_BONES_PER_VERT);
    Stream.write((char*)&data.bone, sizeof(char) * MAX_NUM_BONES_PER_VERT);
    Stream.write((char*)&data.numbones, sizeof(byte));

}

void BinaryWriter::Write(mstudiovertex_t_v49 data) 
{
    Stream.write((char*)&data.m_BoneWeights, sizeof(mstudioboneweight_t_v49));
    Stream.write((char*)&data.m_vecPosition, sizeof(Vector3));
    Stream.write((char*)&data.m_vecNormal, sizeof(Vector3));
    Stream.write((char*)&data.m_vecTexCoord, sizeof(Vector2));

}

void BinaryWriter::Write(mstudiotexturedir_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
}

void BinaryWriter::Write(mstudiotexture_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));

    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.used, sizeof(int));
    Stream.write((char*)&data.unused1, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 12);

}

void BinaryWriter::Write(mstudioskingroup_t_v49 data, int groupSize) 
{
    Stream.write((char*)&data.textureId, sizeof(short) * groupSize);
}

void BinaryWriter::Write(mstudionodedata_v49 data) 
{
    for (int i = 0; i < data.unk.size(); i++)
    {
        Stream.write((char*)&data.unk[i], 1);
    }
}

void BinaryWriter::Write(mstudionodename_t_v49 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
}

void BinaryWriter::Write(pertriheader_t_v49 data) 
{
    Stream.write((char*)&data.version, sizeof(int));



    Stream.write((char*)&data.bbmin, sizeof(Vector3));
    Stream.write((char*)&data.bbmax, sizeof(Vector3));

    Stream.write((char*)&data.unused, sizeof(int) * 8);

}

void BinaryWriter::Write(mstudiokeyvalues_t_v49 data)
{
    for (int i = 0; i < data.value.size(); i++)
    {
        Stream.write((char*)&data.value[i], sizeof(byte));
    }
}

void BinaryWriter::Write(mstudiostringtable_t_v49 data)
{
    int startPos = Position();

    for (int i = 0; i < data.mdlname.size(); i++)
    {
        Stream.write(&data.mdlname[i], 1);
    }

    for (int i = 0; i < data.surfaceprop.size(); i++)
    {
        Stream.write(&data.surfaceprop[i], 1);
    }

    for (int i = 0; i < data.bones.size(); i++)
    {
        for (int j = 0; j < data.bones[i].size(); j++)
        {
            Stream.write(&data.bones[i][j], 1);
        }
    }

    for (int i = 0; i < data.attachments.size(); i++)
    {
        for (int j = 0; j < data.attachments[i].size(); j++)
        {
            Stream.write(&data.attachments[i][j], 1);
        }
    }

    for (int i = 0; i < data.hitboxsets.size(); i++)
    {
        for (int j = 0; j < data.hitboxsets[i].size(); j++)
        {
            Stream.write(&data.hitboxsets[i][j], 1);
        }
    }

    for (int i = 0; i < data.hitboxes.size(); i++)
    {
        for (int j = 0; j < data.hitboxes[i].size(); j++)
        {
            Stream.write(&data.hitboxes[i][j], 1);
        }
    }

    for (int i = 0; i < data.anims.size(); i++)
    {
        for (int j = 0; j < data.anims[i].size(); j++)
        {
            Stream.write(&data.anims[i][j], 1);
        }
    }

    for (int i = 0; i < data.seqs.size(); i++)
    {
        for (int j = 0; j < data.seqs[i].szname.size(); j++)
        {
            Stream.write(&data.seqs[i].szname[j], 1);
        }

        for (int j = 0; j < data.seqs[i].activity.size(); j++)
        {
            Stream.write(&data.seqs[i].activity[j], 1);
        }

        for (int h = 0; h < data.seqs[i].activityevent.size(); h++)
        {
            for (int j = 0; j < data.seqs[i].activityevent[h].size(); j++)
            {
                Stream.write(&data.seqs[i].activityevent[h][j], 1);
            }
        }

        for (int h = 0; h < data.seqs[i].activitymodifier.size(); h++)
        {
            for (int j = 0; j < data.seqs[i].activitymodifier[h].size(); j++)
            {
                Stream.write(&data.seqs[i].activitymodifier[h][j], 1);
            }
        }
    }

    for (int i = 0; i < data.nodes.size(); i++)
    {
        for (int j = 0; j < data.nodes[i].size(); j++)
        {
            Stream.write(&data.nodes[i][j], 1);
        }
    }

    for (int i = 0; i < data.bodyparts.size(); i++)
    {
        for (int j = 0; j < data.bodyparts[i].size(); j++)
        {
            Stream.write(&data.bodyparts[i][j], 1);
        }
    }

    for (int i = 0; i < data.poseparams.size(); i++)
    {
        for (int j = 0; j < data.poseparams[i].size(); j++)
        {
            Stream.write(&data.poseparams[i][j], 1);
        }
    }

    for (int i = 0; i < data.ikchains.size(); i++)
    {
        for (int j = 0; j < data.ikchains[i].size(); j++)
        {
            Stream.write(&data.ikchains[i][j], 1);
        }
    }

    for (int i = 0; i < data.includemodel.size(); i++)
    {
        for (int j = 0; j < data.includemodel[i].size(); j++)
        {
            Stream.write(&data.includemodel[i][j], 1);
        }
    }

    for (int i = 0; i < data.textures.size(); i++)
    {
        for (int j = 0; j < data.textures[i].size(); j++)
        {
            Stream.write(&data.textures[i][j], 1);
        }
    }

    for (int i = 0; i < data.cdmaterials.size(); i++)
    {
        for (int j = 0; j < data.cdmaterials[i].size(); j++)
        {
            Stream.write(&data.cdmaterials[i][j], 1);
        }
    }

    int endPos = Position();

    Logger::Info("Stream Size: %d\n", data.stringtablesize);
    Logger::Info("String Table StartPos: %d\n", startPos);
    Logger::Info("String Table EndPos: %d\n", endPos);
    Logger::Info("String Table Size: %d\n", endPos - startPos);

    int stringTableSize = endPos - startPos;


    int padding = (data.stringtablesize - stringTableSize);
    Logger::Info("String Table Padding Size: %d\n", padding);
    Padding(padding);

}

#pragma endregion

#pragma region V52WRITE
void BinaryWriter::Write(mayabakery_t data)
{
    for (int i = 0; i < data.bakery.size(); i++)
    {
        Stream.write((char*)&data.bakery[i], 1);
    }
}

void BinaryWriter::Write(mstudiocompressedikerror_t_v52 data) 
{
    Stream.write((char*)&data.scale, sizeof(float) * 4);
    Stream.write((char*)&data.offset, sizeof(short) * 4);
}

void BinaryWriter::Write(mstudiostringtable_t_v52 data)
{
    int startPos = Position();

    for (int i = 0; i < data.mdlname.size(); i++)
    {
        Stream.write(&data.mdlname[i], 1);
    }

    for (int i = 0; i < data.surfaceprop.size(); i++)
    {
        Stream.write(&data.surfaceprop[i], 1);
    }

    for (int i = 0; i < data.unk.size(); i++)
    {
        Stream.write(&data.unk[i], 1);
    }

    for (int i = 0; i < data.bones.size(); i++)
    {
        for (int j = 0; j < data.bones[i].size(); j++)
        {
            Stream.write(&data.bones[i][j], 1);
        }
    }

    for (int i = 0; i < data.attachments.size(); i++)
    {
        for (int j = 0; j < data.attachments[i].size(); j++)
        {
            Stream.write(&data.attachments[i][j], 1);
        }
    }

    for (int i = 0; i < data.hitboxsets.size(); i++)
    {
        for (int j = 0; j < data.hitboxsets[i].size(); j++)
        {
            Stream.write(&data.hitboxsets[i][j], 1);
        }
    }

    for (int i = 0; i < data.hitboxes.size(); i++)
    {
        for (int j = 0; j < data.hitboxes[i].size(); j++)
        {
            Stream.write(&data.hitboxes[i][j], 1);
        }
    }

    for (int i = 0; i < data.anims.size(); i++)
    {
        for (int j = 0; j < data.anims[i].size(); j++)
        {
            Stream.write(&data.anims[i][j], 1);
        }
    }

    for (int i = 0; i < data.seqs.size(); i++)
    {
        for (int j = 0; j < data.seqs[i].szname.size(); j++)
        {
            Stream.write(&data.seqs[i].szname[j], 1);
        }

        for (int j = 0; j < data.seqs[i].activity.size(); j++)
        {
            Stream.write(&data.seqs[i].activity[j], 1);
        }

        for (int h = 0; h < data.seqs[i].activityevent.size(); h++)
        {
            for (int j = 0; j < data.seqs[i].activityevent[h].size(); j++)
            {
                Stream.write(&data.seqs[i].activityevent[h][j], 1);
            }
        }

        for (int h = 0; h < data.seqs[i].activitymodifier.size(); h++)
        {
            for (int j = 0; j < data.seqs[i].activitymodifier[h].size(); j++)
            {
                Stream.write(&data.seqs[i].activitymodifier[h][j], 1);
            }
        }
    }

    for (int i = 0; i < data.nodes.size(); i++)
    {
        for (int j = 0; j < data.nodes[i].size(); j++)
        {
            Stream.write(&data.nodes[i][j], 1);
        }
    }

    for (int i = 0; i < data.bodyparts.size(); i++)
    {
        for (int j = 0; j < data.bodyparts[i].size(); j++)
        {
            Stream.write(&data.bodyparts[i][j], 1);
        }
    }

    for (int i = 0; i < data.poseparams.size(); i++)
    {
        for (int j = 0; j < data.poseparams[i].size(); j++)
        {
            Stream.write(&data.poseparams[i][j], 1);
        }
    }

    for (int i = 0; i < data.ikchains.size(); i++)
    {
        for (int j = 0; j < data.ikchains[i].size(); j++)
        {
            Stream.write(&data.ikchains[i][j], 1);
        }
    }

    for (int i = 0; i < data.includemodel.size(); i++)
    {
        for (int j = 0; j < data.includemodel[i].size(); j++)
        {
            Stream.write(&data.includemodel[i][j], 1);
        }
    }

    for (int i = 0; i < data.animblockname.size(); i++)
    {
        Stream.write(&data.animblockname[i], 1);
    }

    for (int i = 0; i < data.textures.size(); i++)
    {
        for (int j = 0; j < data.textures[i].size(); j++)
        {
            Stream.write(&data.textures[i][j], 1);
        }
    }

    for (int i = 0; i < data.cdmaterials.size(); i++)
    {
        for (int j = 0; j < data.cdmaterials[i].size(); j++)
        {
            Stream.write(&data.cdmaterials[i][j], 1);
        }
    }

    int endPos = Position();

    Logger::Info("Stream Size: %d\n", data.stringtablesize);
    Logger::Info("String Table StartPos: %d\n", startPos);
    Logger::Info("String Table EndPos: %d\n", endPos);
    Logger::Info("String Table Size: %d\n", endPos - startPos);

    int stringTableSize = endPos - startPos;


    int padding = (data.stringtablesize - stringTableSize);
    Logger::Info("String Table Padding Size: %d\n", padding);
    Padding(padding);

}

#pragma endregion

#pragma region V53WRITE
void BinaryWriter::Write(studiohdr_t_v53 data) 
{

    Stream.write((char*)&data.id, sizeof(int));
    Stream.write((char*)&data.version, sizeof(int));
    Stream.write((char*)&data.checksum, sizeof(int));
    Stream.write((char*)&data.sznameindex, sizeof(int));
    for (int i = 0; i < data.name.size(); i++) Stream.write((char*)&data.name[i], sizeof(byte));

    Stream.write((char*)&data.length, sizeof(int));

    Stream.write((char*)&data.eyeposition, sizeof(Vector3));

    Stream.write((char*)&data.illumposition, sizeof(Vector3));

    Stream.write((char*)&data.hull_min, sizeof(Vector3));
    Stream.write((char*)&data.hull_max, sizeof(Vector3));

    Stream.write((char*)&data.view_bbmin, sizeof(Vector3));	
    Stream.write((char*)&data.view_bbmax, sizeof(Vector3));

    Stream.write((char*)&data.flags, sizeof(int));

    Stream.write((char*)&data.numbones, sizeof(int));
    Stream.write((char*)&data.boneindex, sizeof(int));

    Stream.write((char*)&data.numbonecontrollers, sizeof(int)); 
    Stream.write((char*)&data.bonecontrollerindex, sizeof(int));

    Stream.write((char*)&data.numhitboxsets, sizeof(int));
    Stream.write((char*)&data.hitboxsetindex, sizeof(int));

    Stream.write((char*)&data.numlocalanim, sizeof(int)); 
    Stream.write((char*)&data.localanimindex, sizeof(int));

    Stream.write((char*)&data.numlocalseq, sizeof(int)); 
    Stream.write((char*)&data.localseqindex, sizeof(int));

    Stream.write((char*)&data.activitylistversion, sizeof(int));
    Stream.write((char*)&data.eventsindexed, sizeof(int));


    Stream.write((char*)&data.numtextures, sizeof(int));
    Stream.write((char*)&data.textureindex, sizeof(int));


    Stream.write((char*)&data.numcdtextures, sizeof(int));
    Stream.write((char*)&data.cdtextureindex, sizeof(int));


    Stream.write((char*)&data.numskinref, sizeof(int));
    Stream.write((char*)&data.numskinfamilies, sizeof(int));
    Stream.write((char*)&data.skinindex, sizeof(int));

    Stream.write((char*)&data.numbodyparts, sizeof(int));
    Stream.write((char*)&data.bodypartindex, sizeof(int));

    Stream.write((char*)&data.numlocalattachments, sizeof(int));
    Stream.write((char*)&data.localattachmentindex, sizeof(int));

    Stream.write((char*)&data.numlocalnodes, sizeof(int));
    Stream.write((char*)&data.localnodeindex, sizeof(int));
    Stream.write((char*)&data.localnodenameindex, sizeof(int));

    Stream.write((char*)&data.numflexdesc, sizeof(int));
    Stream.write((char*)&data.flexdescindex, sizeof(int));

    Stream.write((char*)&data.numflexcontrollers, sizeof(int));
    Stream.write((char*)&data.flexcontrollerindex, sizeof(int));

    Stream.write((char*)&data.numflexrules, sizeof(int));
    Stream.write((char*)&data.flexruleindex, sizeof(int));

    Stream.write((char*)&data.numikchains, sizeof(int));
    Stream.write((char*)&data.ikchainindex, sizeof(int));

    Stream.write((char*)&data.numruimeshes, sizeof(int));
    Stream.write((char*)&data.ruimeshindex, sizeof(int));

    Stream.write((char*)&data.numlocalposeparameters, sizeof(int));
    Stream.write((char*)&data.localposeparamindex, sizeof(int));

    Stream.write((char*)&data.surfacepropindex, sizeof(int));

    Stream.write((char*)&data.keyvalueindex, sizeof(int));
    Stream.write((char*)&data.keyvaluesize, sizeof(int));

    Stream.write((char*)&data.numlocalikautoplaylocks, sizeof(int));
    Stream.write((char*)&data.localikautoplaylockindex, sizeof(int));


    Stream.write((char*)&data.mass, sizeof(float));
    Stream.write((char*)&data.contents, sizeof(int));


    Stream.write((char*)&data.numincludemodels, sizeof(int));
    Stream.write((char*)&data.includemodelindex, sizeof(int));

    Stream.write((char*)&data.virtualModel, sizeof(int));

    Stream.write((char*)&data.bonetablebynameindex, sizeof(int));

    Stream.write((char*)&data.constdirectionallightdot, sizeof(std::byte));

    Stream.write((char*)&data.rootLOD, sizeof(std::byte));

    Stream.write((char*)&data.numAllowedRootLODs, sizeof(std::byte));

    Stream.write((char*)&data.unused, sizeof(std::byte));

    Stream.write((char*)&data.fadedistance, sizeof(float));   

    Stream.write((char*)&data.numflexcontrollerui, sizeof(int));
    Stream.write((char*)&data.flexcontrolleruiindex, sizeof(int));

    Stream.write((char*)&data.pVertexBase, sizeof(int));
    Stream.write((char*)&data.pIndexBase, sizeof(int));

    Stream.write((char*)&data.mayaindex, sizeof(int));

    Stream.write((char*)&data.numsrcbonetransform, sizeof(int));
    Stream.write((char*)&data.srcbonetransformindex, sizeof(int));

    Stream.write((char*)&data.illumpositionattachmentindex, sizeof(int));

    Stream.write((char*)&data.linearboneindex, sizeof(int));

    Stream.write((char*)&data.m_nBoneFlexDriverCount, sizeof(int));
    Stream.write((char*)&data.m_nBoneFlexDriverIndex, sizeof(int));

    Stream.write((char*)&data.aabbindex, sizeof(int));
    Stream.write((char*)&data.numaabbnodes, sizeof(int));
    Stream.write((char*)&data.numaabbleaves, sizeof(int));
    Stream.write((char*)&data.numaabbverts, sizeof(int));

    Stream.write((char*)&data.unkstringindex, sizeof(int));

    Stream.write((char*)&data.vtxindex, sizeof(int)); // VTX
    Stream.write((char*)&data.vvdindex, sizeof(int)); // VVD
    Stream.write((char*)&data.vvcindex, sizeof(int)); // VVC
    Stream.write((char*)&data.vphyindex, sizeof(int)); // VP

    Stream.write((char*)&data.vtxsize, sizeof(int)); // VTX
    Stream.write((char*)&data.vvdsize, sizeof(int)); // VVD 
    Stream.write((char*)&data.vvcsize, sizeof(int)); // VVC 
    Stream.write((char*)&data.vphysize, sizeof(int)); // VPH

    Stream.write((char*)&data.unkmemberindex1, sizeof(int));
    Stream.write((char*)&data.numunkmember1, sizeof(int));

    Stream.write((char*)&data.unk, sizeof(int));

    Stream.write((char*)&data.unkindex3, sizeof(int));

    //Stream.write((char*)&data.unused1, sizeof(int) * 60);

    Padding(4 * 60);

}

void BinaryWriter::Write(mstudiobone_t_v53 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.parent, sizeof(int));
    Stream.write((char*)&data.bonecontroller, sizeof(int) * 6);
    Stream.write((char*)&data.pos, sizeof(Vector3));
    Stream.write((char*)&data.quat, sizeof(Quaternion));
    Stream.write((char*)&data.rot, sizeof(RadianEuler));
    Stream.write((char*)&data.unkvector, sizeof(Vector3));
    Stream.write((char*)&data.posscale, sizeof(Vector3));
    Stream.write((char*)&data.rotscale, sizeof(Vector3));
    Stream.write((char*)&data.unkvector1, sizeof(Vector3));
    Stream.write((char*)&data.poseToBone, sizeof(matrix3x4_t));
    Stream.write((char*)&data.qAlignment, sizeof(Quaternion));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.proctype, sizeof(int));
    Stream.write((char*)&data.procindex, sizeof(int));
    Stream.write((char*)&data.physicsbone, sizeof(int));
    Stream.write((char*)&data.surfacepropidx, sizeof(int));
    Stream.write((char*)&data.contents, sizeof(int));
    Stream.write((char*)&data.surfacepropLookup, sizeof(int));
    Stream.write((char*)&data.unkid, sizeof(short));
    Stream.write((char*)&data.unktoggle, sizeof(short));
    Stream.write((char*)&data.unused, sizeof(int) * 7);
    //printf("read a Vector\n");
}

void BinaryWriter::Write(mstudiobbox_t_v53 data) 
{
    Stream.write((char*)&data.bone, sizeof(int));
    Stream.write((char*)&data.group, sizeof(int));
    Stream.write((char*)&data.bbmin, sizeof(Vector3));
    Stream.write((char*)&data.bbmax, sizeof(Vector3));
    Stream.write((char*)&data.szhitboxnameindex, sizeof(int));
    Stream.write((char*)&data.critoverride, sizeof(int));
    Stream.write((char*)&data.keyvalueindex, sizeof(int));
    Stream.write((char*)&data.unused, sizeof(int) * 6);
}

void BinaryWriter::Write(mstudioanimdesc_t_v53 data) 
{
    Stream.write((char*)&data.baseptr, sizeof(int));
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.fps, sizeof(float));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.numframes, sizeof(int));
    Stream.write((char*)&data.nummovements, sizeof(int));
    Stream.write((char*)&data.movementindex, sizeof(int));
    Stream.write((char*)&data.compressedikerrorindex, sizeof(int));
    Stream.write((char*)&data.animindex, sizeof(int));
    Stream.write((char*)&data.numikrules, sizeof(int));
    Stream.write((char*)&data.ikruleindex, sizeof(int));
    Stream.write((char*)&data.numlocalhierarchy, sizeof(int));
    Stream.write((char*)&data.localhierarchyindex, sizeof(int));
    Stream.write((char*)&data.sectionindex, sizeof(int));
    Stream.write((char*)&data.sectionframes, sizeof(int));
    Stream.write((char*)&data.zeroframespan, sizeof(short));
    Stream.write((char*)&data.zeroframecount, sizeof(short));
    Stream.write((char*)&data.zeroframeindex, sizeof(int));
    Stream.write((char*)&data.zeroframestalltime, sizeof(float));
    Stream.write((char*)&data.unused, sizeof(int) * 5);
}

void BinaryWriter::Write(mstudioanim_t_v53 data) 
{
    Stream.write((char*)&data.posscale, sizeof(float));
    Stream.write((char*)&data.bone, sizeof(byte));
    Stream.write((char*)&data.flags, sizeof(byte));
    Stream.write((char*)&data.unk, sizeof(short));

    bool hasRawPosFlag = (int)data.flags & STUDIO_ANIM_RAWPOS_53;
    bool hasRawRotFlag = (int)data.flags & STUDIO_ANIM_RAWROT_53;
    bool hasRawScaleFlag = (int)data.flags & STUDIO_ANIM_RAWSCALE_53;

    if (!hasRawRotFlag)
    {
        Stream.write((char*)&data.animrot, sizeof(Vector3Short));
        Stream.write((char*)&data.unused, sizeof(short));
    }
    else
    {
        Stream.write((char*)&data.rawrot, sizeof(QuaternionShort));
    }

    if (!hasRawPosFlag)
    {
        Stream.write((char*)&data.animpos, sizeof(Vector3Short));
    }
    else
    {
        Stream.write((char*)&data.rawpos, sizeof(Vector3Short));
    }

    if (!hasRawScaleFlag)
    {
        Stream.write((char*)&data.animscale, sizeof(Vector3Short));
    }
    else
    {
        Stream.write((char*)&data.rawscale, sizeof(Vector3Short));
    }

    Stream.write((char*)&data.nextoffset, sizeof(int));
}

void BinaryWriter::Write(sectionindexes_t_v53 data) 
{
    Stream.write((char*)&data.sectionoffsets, sizeof(int));
}


void BinaryWriter::Write(mstudioikrule_t_v53 data) 
{
    Stream.write((char*)&data.index, sizeof(int));

    Stream.write((char*)&data.type, sizeof(int));
    Stream.write((char*)&data.chain, sizeof(int));

    Stream.write((char*)&data.bone, sizeof(int));

    Stream.write((char*)&data.slot, sizeof(int));
    Stream.write((char*)&data.height, sizeof(float));
    Stream.write((char*)&data.radius, sizeof(float));
    Stream.write((char*)&data.floor, sizeof(float));
    Stream.write((char*)&data.pos, sizeof(Vector3));
    Stream.write((char*)&data.q, sizeof(Quaternion));

    Stream.write((char*)&data.compressedikerrorindex, sizeof(int));

    Stream.write((char*)&data.iStart, sizeof(int));
    Stream.write((char*)&data.ikerrorindex, sizeof(int));

    Stream.write((char*)&data.start, sizeof(float));
    Stream.write((char*)&data.peak, sizeof(float));
    Stream.write((char*)&data.tail, sizeof(float));
    Stream.write((char*)&data.end, sizeof(float));

    Stream.write((char*)&data.contact, sizeof(float));
    Stream.write((char*)&data.drop, sizeof(float));
    Stream.write((char*)&data.top, sizeof(float));

    Stream.write((char*)&data.szattachmentindex, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 9);
}

void BinaryWriter::Write(mstudioseqdesc_t_v53 data) 
{

    Stream.write((char*)&data.baseptr, sizeof(int));

    Stream.write((char*)&data.szlabelindex, sizeof(int));

    Stream.write((char*)&data.szactivitynameindex, sizeof(int));

    Stream.write((char*)&data.flags, sizeof(int));

    Stream.write((char*)&data.activity, sizeof(int));
    Stream.write((char*)&data.actweight, sizeof(int));

    Stream.write((char*)&data.numevents, sizeof(int));
    Stream.write((char*)&data.eventindex, sizeof(int));

    Stream.write((char*)&data.bbmin, sizeof(Vector3));
    Stream.write((char*)&data.bbmax, sizeof(Vector3));

    Stream.write((char*)&data.numblends, sizeof(int));


    Stream.write((char*)&data.animindexindex, sizeof(int));

    Stream.write((char*)&data.movementindex, sizeof(int));		
    Stream.write((char*)&data.groupsize, sizeof(int) * 2);
    Stream.write((char*)&data.paramindex, sizeof(int) * 2);	    
    Stream.write((char*)&data.paramstart, sizeof(float) * 2);	
    Stream.write((char*)&data.paramend, sizeof(float) * 2);		
    Stream.write((char*)&data.paramparent, sizeof(int));

    Stream.write((char*)&data.fadeintime, sizeof(float));		
    Stream.write((char*)&data.fadeouttime, sizeof(float));		

    Stream.write((char*)&data.localentrynode, sizeof(int));		
    Stream.write((char*)&data.localexitnode, sizeof(int));		
    Stream.write((char*)&data.nodeflags, sizeof(int));			

    Stream.write((char*)&data.entryphase, sizeof(float));		
    Stream.write((char*)&data.exitphase, sizeof(float));		

    Stream.write((char*)&data.lastframe, sizeof(float));		

    Stream.write((char*)&data.nextseq, sizeof(int));			
    Stream.write((char*)&data.pose, sizeof(int));			    

    Stream.write((char*)&data.numikrules, sizeof(int));

    Stream.write((char*)&data.numautolayers, sizeof(int));
    Stream.write((char*)&data.autolayerindex, sizeof(int));

    Stream.write((char*)&data.weightlistindex, sizeof(int));

    Stream.write((char*)&data.posekeyindex, sizeof(int));

    Stream.write((char*)&data.numiklocks, sizeof(int));
    Stream.write((char*)&data.iklockindex, sizeof(int));


    Stream.write((char*)&data.keyvalueindex, sizeof(int));
    Stream.write((char*)&data.keyvaluesize, sizeof(int));

    Stream.write((char*)&data.cycleposeindex, sizeof(int));		

    Stream.write((char*)&data.activitymodifierindex, sizeof(int));
    Stream.write((char*)&data.numactivitymodifiers, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 10);

}

void BinaryWriter::Write(mstudioactivitymodifier_t_v53 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.unk, sizeof(int));
}

void BinaryWriter::Write(mstudioikchain_t_v53 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));
    Stream.write((char*)&data.linktype, sizeof(int));
    Stream.write((char*)&data.numlinks, sizeof(int));

    Stream.write((char*)&data.linkindex, sizeof(int));

    Stream.write((char*)&data.unk, sizeof(float));

    Stream.write((char*)&data.unused, sizeof(int) * 3);
}

void BinaryWriter::Write(mstudiotexture_t_v53 data) 
{
    Stream.write((char*)&data.sznameindex, sizeof(int));

    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.used, sizeof(int));
    Stream.write((char*)&data.unused1, sizeof(int));

    Stream.write((char*)&data.unused, sizeof(int) * 7);

}

void BinaryWriter::Write(mstudiorruiheader_t data)
{
    Stream.write((char*)&data.ruiunk, sizeof(int));
    Stream.write((char*)&data.ruimeshindex, sizeof(int));
}

void BinaryWriter::Write(mstudioruivertmap_t data)
{
    Stream.write((char*)&data.vertstartid, sizeof(short));
    Stream.write((char*)&data.vertendid, sizeof(short));

    Stream.write((char*)&data.vertextraid, sizeof(short));

}

void BinaryWriter::Write(mstudioruivert_t data)
{
    Stream.write((char*)&data.parent, sizeof(int));
    Stream.write((char*)&data.vertexpos, sizeof(Vector3));

}

void BinaryWriter::Write(mstudioruimesface_t data)
{
    Stream.write((char*)&data.faceuvmin, sizeof(Vector2));
    Stream.write((char*)&data.faceuvmax, sizeof(Vector2));

    Stream.write((char*)&data.facescalemin, sizeof(Vector2));
    Stream.write((char*)&data.facescalemax, sizeof(Vector2));

}

void BinaryWriter::Write(mstudioruimesh_t data)
{
    Stream.write((char*)&data.numparents, sizeof(int));
    Stream.write((char*)&data.numvertices, sizeof(int));
    Stream.write((char*)&data.numfaces, sizeof(int));

    Stream.write((char*)&data.parentindex, sizeof(int));

    Stream.write((char*)&data.vertexindex, sizeof(int));
    Stream.write((char*)&data.vertmapindex, sizeof(int));
    Stream.write((char*)&data.facedataindex, sizeof(int));

    Stream.write((char*)&data.unk, sizeof(int));

    Stream.write((char*)&data.szruimeshname, static_cast<std::streamsize>(data.parentindex) - 32);

    for (int i = 0; i < data.numparents; i++)
    {
        Stream.write((char*)&data.parent, sizeof(short) * data.numparents);
        Logger::Info("Parent Read: %d\n", Position());
    }

    for (int i = 0; i < data.numfaces; i++)
    {
        Stream.write((char*)&data.vertexmap, sizeof(mstudioruivertmap_t) * data.numfaces);
        Logger::Info("VertMap Read: %d\n", i);
    }

    for (int i = 0; i < data.numvertices; i++)
    {
        Stream.write((char*)&data.vertex, sizeof(mstudioruivert_t) * data.numvertices);
        Logger::Info("Vert Read: %d\n", i);
    }

    for (int i = 0; i < data.numfaces; i++)
    {
        Stream.write((char*)&data.facedata, sizeof(mstudioruimesface_t) * data.numfaces);
        Logger::Info("FaceData Read: %d\n", i);
    }

}

void BinaryWriter::Write(pertriheader_t data)
{
    Stream.write((char*)&data.version, sizeof(int));
    Stream.write((char*)&data.bbmin, sizeof(Vector3));
    Stream.write((char*)&data.bbmax, sizeof(Vector3));
    Stream.write((char*)&data.unused, sizeof(int) * 8);
}

void BinaryWriter::Write(aabbnode data)
{
    Stream.write((char*)&data.unk, sizeof(short) * 3);
    Stream.write((char*)&data.unk1, sizeof(short) * 3);
    Stream.write((char*)&data.children, sizeof(int) * 2);    // negative numbers are -(leafs + 1), not nodes
}

void BinaryWriter::Write(aabbcollmaskmap data)
{
    Stream.write((char*)&data.collmaskid, sizeof(short) * 3);
    Stream.write((char*)&data.unk1, sizeof(short)); //Contents?
}

void BinaryWriter::Write(aabbleaf data)
{
    Stream.write((char*)&data.mins, sizeof(short) * 3);
    Stream.write((char*)&data.maxs, sizeof(short) * 3);
    Stream.write((char*)&data.unk, sizeof(int));

    for (int i = 0; i < 12; i++)
    {
        Stream.write((char*)&data.map[i].collmaskid, sizeof(short) * 3);
        Stream.write((char*)&data.map[i].unk1, sizeof(short)); //Contents?
    }
}

void BinaryWriter::Write(aabbcollisionmask data)
{
    for (int i = 0; i < data.unk.size(); i++)
    {
        Stream.write((char*)&data.unk[i], sizeof(Quat48));
    }
}
#pragma endregion

#pragma region PHYWRITE
void BinaryWriter::Write(phyheader_t data)
{
    Stream.write((char*)&data.size, sizeof(int));
    Stream.write((char*)&data.id, sizeof(int));
    Stream.write((char*)&data.numsolids, sizeof(int));
    Stream.write((char*)&data.checksum, sizeof(int));
}

void BinaryWriter::Write(compactsurfaceheader_t data)
{
    Stream.write((char*)&data.size, sizeof(int));
    Stream.write((char*)&data.id, sizeof(int));
    Stream.write((char*)&data.version, sizeof(short));
    Stream.write((char*)&data.modeltype, sizeof(short));
    Stream.write((char*)&data.surfacesize, sizeof(int));
    Stream.write((char*)&data.dragaxisareas, sizeof(Vector3));
    Stream.write((char*)&data.axismaparea, sizeof(int));
}

void BinaryWriter::Write(legacysurfaceheader_t data)
{
    Stream.write((char*)&data.mass_center, sizeof(Vector3));
    Stream.write((char*)&data.rotation_inertia, sizeof(Vector3));
    Stream.write((char*)&data.upper_limit_radius, sizeof(float));
    Stream.write((char*)&data.max_deviation, sizeof(int));
    //Stream.write((char*)&data.byte_size, sizeof(int));
    Stream.write((char*)&data.nodeindex, sizeof(int));
    Stream.write((char*)&data.pad, sizeof(int) * 2);
    Stream.write((char*)&data.id, sizeof(int));
}

void BinaryWriter::Write(trianglefaceheader_t data)
{
    Stream.write((char*)&data.vertexindex, sizeof(int));
    Stream.write((char*)&data.parent, sizeof(int));
    Stream.write((char*)&data.flags, sizeof(int));
    Stream.write((char*)&data.numfaces, sizeof(int));
}

void BinaryWriter::Write(trianglevertmap_t data)
{
    Stream.write((char*)&data.faceindex, sizeof(std::byte));
    Stream.write((char*)&data.unkdata, sizeof(std::byte) * 3);
    Stream.write((char*)&data.vertid, sizeof(short));
    Stream.write((char*)&data.unkdata1, sizeof(std::byte) * 3);
    Stream.write((char*)&data.vertid1, sizeof(short));
    Stream.write((char*)&data.unkdata2, sizeof(std::byte) * 3);
    Stream.write((char*)&data.vertid2, sizeof(short));
    Stream.write((char*)&data.unkdata3, sizeof(std::byte) * 3);
}

void BinaryWriter::Write(phyvertex_t data)
{
    Stream.write((char*)&data.pos, sizeof(Vector3));
    Stream.write((char*)&data.unk, sizeof(int));
}

void BinaryWriter::Write(phynode_t data)
{
    Stream.write((char*)&data.rightnodeindex, sizeof(int));
    Stream.write((char*)&data.convexindex, sizeof(int));
    Stream.write((char*)&data.center, sizeof(Vector3));
    Stream.write((char*)&data.radius, sizeof(int));
    Stream.write((char*)&data.bboxsize, sizeof(int));
}

void BinaryWriter::Write(physection_t data)
{
    Write(data.surfaceheader);
    Write(data.surfaceheader2);
    Write(data.trihdr);
    for (int i = 0; i < data.trihdr.numfaces; i++)
    {
        Write(data.vertmaps[i]);
    }

    for (int i = 0; i < data.vertices.size(); i++)
    {
        Write(data.vertices);
    }
    Write(data.node);
}
#pragma endregion

void BinaryWriter::seek(int pos)
{
    if (Stream.fail())
    {
        Stream.close();
        Stream.open(_filename);
        Logger::Notice("Stream died, reopening file \"%s\"\n", _filename);
    }

    Stream.seekp(pos, std::ios::beg);
    if (Stream.tellp() != pos)
    {
        Logger::Critical("BinaryWriter failed to seek the write cursor to %d!\n", pos);
        abort();
    }
    Stream.seekg(pos, std::ios::beg);
    if (Stream.tellg() != pos)
    {
        Logger::Critical("BinaryWriter failed to seek the read cursor to %d!\n", pos);
    }
}
