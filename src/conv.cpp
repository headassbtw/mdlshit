#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include <conv.hpp>
#include <logger.hpp>
#include <structs.hpp>
#include <binarystream.hpp>
#include <structs.hpp>
#include <argtools.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <rendering/progress.hpp>
#include <vector>
#include <Source.hpp>
#include <half.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

#pragma region helper functions

/// \param reader The input file
/// \param writer The output file
/// \param add The number to add to the int that was read
/// \param count Amount of times to repeat this
void CopyAddInt32(BinaryReader* reader, BinaryWriter* writer, int add, int count){
    for(int i = 0; i < count;i++){
        int tmp;
        reader->Read(&tmp);
        writer->Write(tmp + add);
    }
}
void CopySetInt32(BinaryReader* reader, BinaryWriter* writer, int set, int count){
    for(int i = 0; i < count;i++){
        int tmp;
        reader->Read(&tmp);
        writer->Write(set);
    }
}
void CopyCheckSetInt32(BinaryReader* reader, BinaryWriter* writer, int set, int count, int check) {
    for (int i = 0; i < count; i++) {
        int tmp;
        reader->Read(&tmp);
        if (check == 0) 
            writer->Write(tmp);
        else
            writer->Write(set);
        
    }
}
void CopyAddInt32NullCheck(BinaryReader* reader, BinaryWriter* writer, int add, int count){
    for(int i = 0; i < count;i++){
        int tmp;
        reader->Read(&tmp);
        if(tmp == 0)
          writer->Write(tmp);
        else
          writer->Write(tmp + add);
    }
}

void AddShort(BinaryWriter* writer, int add, int count) {
    for (int i = 0; i < count; i++) {
        short tmp = (short)0;
        writer->Write(tmp + (short)add);
    }
}

void CopyAddShortNullCheck(BinaryReader* reader, BinaryWriter* writer, int add, int count) {
    for (int i = 0; i < count; i++) {
        short tmp;
        reader->Read(&tmp);
        if (tmp == 0)
            writer->Write(tmp);
        else
            writer->Write(tmp + add);
    }
}
void AddInt32(BinaryWriter* writer, int add, int count){
    for(int i = 0; i < count;i++){
        writer->Write(add);
    }
}
void SetInt32(BinaryWriter* writer, int set, int count){
    for(int i = 0; i < count;i++){
        writer->Write(set);
    }
}
void AddFloat32( BinaryWriter* writer, float add, int count) {
    for (int i = 0; i < count; i++) {
        float tmp;
        writer->Write(tmp + add);
    }
}
void CopyAddFloat32(BinaryReader* reader, BinaryWriter* writer, float add, int count){
    for(int i = 0; i < count;i++){
        float tmp;
        reader->Read(&tmp);
        writer->Write(tmp + add);
    }
}
void CopySetFloat32(BinaryReader* reader, BinaryWriter* writer, float set, int count){
    for(int i = 0; i < count;i++){
        float tmp;
        reader->Read(&tmp);
        writer->Write(set);
    }
}
std::string h(const char* file, const char* ext){
  std::string yo = std::string(file);
  auto dot = yo.find_last_of('.');
  yo.resize(dot);
  yo.append(".");
  yo.append(ext);
  return yo;
}
void filler(BinaryReader* reader, BinaryWriter* writer, int count){
  for(int i = 0;i < count;i++){
    std::byte tmp;
    reader->Read(&tmp);
    writer->Write(tmp);
  }
}

void fillerWrite(BinaryWriter* writer, int count) {
    for (int i = 0; i < count; i++) {
        std::byte tmp = (std::byte)0;
        writer->Write(tmp);
    }
}

void read2(BinaryReader* reader, int count) {
    for (int i = 0; i < count; i++) {
        std::byte tmp;
        reader->Read(&tmp);
    }
}
#pragma endregion

int GetR2Flag(int flag)
{

    switch (flag)
    {
        case 0: return 0;
        case 33: return 14;
        case 32: return 14;
        case 8: return 10;
        case 1: return 26;
        case 12: return 8;
        case 49: return 15;
        case 48: return 15;
        case 24: return 11;
        case 17: return 27;
        case 28: return 9;

        default: return 0;
    }
}

int FlagTest(int flag)
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
        //Logger::Info("Has Flag: %d\n", STUDIO_ANIM_DELTA);
        test += STUDIO_ANIM_DELTA_53;
    }

    if (flagNum == 1)
    {
        if (flag & STUDIO_ANIM_ANIMROT )
        {
            //Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMROT);
            test += STUDIO_ANIM_RAWPOS_53;
        }
        if (flag & STUDIO_ANIM_ANIMPOS )
        {
            //Logger::Info("Has Flag: %d\n", STUDIO_ANIM_ANIMPOS);
            test += STUDIO_ANIM_RAWROT_53;
        }

        if (flag & STUDIO_ANIM_RAWROT )
        {
            //Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT);
            test += STUDIO_ANIM_RAWPOS_53;
        }
        if (flag & STUDIO_ANIM_RAWROT2 )
        {
            //Logger::Info("Has Flag: %d\n", STUDIO_ANIM_RAWROT2);
            test += STUDIO_ANIM_RAWPOS_53;
        }
        if (flag & STUDIO_ANIM_RAWPOS )
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


v49_Header* ReadV49Header(BinaryReader* Stream, v49_Header* Initial_Header)
{
    Stream->seek(0);
    Stream->Read(&Initial_Header->id);
    Stream->Read(&Initial_Header->version);
    Stream->Read(&Initial_Header->checksum);
    Stream->read(Initial_Header->name, 64);
    Stream->Read(&Initial_Header->fileSize);
    Stream->Read(&Initial_Header->eyeposition);
    Stream->Read(&Initial_Header->illumposition);
    Stream->Read(&Initial_Header->hull_min);
    Stream->Read(&Initial_Header->hull_max);
    Stream->Read(&Initial_Header->view_bbmin);
    Stream->Read(&Initial_Header->view_bbmax);
    Stream->Read(&Initial_Header->flags);
    Stream->Read(&Initial_Header->numbones);
    Stream->Read(&Initial_Header->boneindex);
    Stream->Read(&Initial_Header->numbonecontrollers);
    Stream->Read(&Initial_Header->bonecontrollerindex);
    Stream->Read(&Initial_Header->numhitboxsets);
    Stream->Read(&Initial_Header->hitboxsetindex);
    Stream->Read(&Initial_Header->numlocalanim);
    Stream->Read(&Initial_Header->localanimindex);

    Stream->Read(&Initial_Header->numlocalseq);
    Stream->Read(&Initial_Header->localseqindex);

    Stream->Read(&Initial_Header->activitylistversion);
    Stream->Read(&Initial_Header->eventsindexed);

    Stream->Read(&Initial_Header->numtextures);
    Stream->Read(&Initial_Header->textureindex);

    Stream->Read(&Initial_Header->numcdtextures);
    Stream->Read(&Initial_Header->cdtextureindex);

    Stream->Read(&Initial_Header->numskinref);
    Stream->Read(&Initial_Header->numskinfamilies);
    Stream->Read(&Initial_Header->skinindex);

    Stream->Read(&Initial_Header->numbodyparts);
    Stream->Read(&Initial_Header->bodypartindex);

    Stream->Read(&Initial_Header->numlocalattachments);
    Stream->Read(&Initial_Header->localattachmentindex);
    Stream->Read(&Initial_Header->numlocalnodes);
    Stream->Read(&Initial_Header->localnodeindex);
    Stream->Read(&Initial_Header->localnodenameindex);
    Stream->Read(&Initial_Header->numflexdesc);
    Stream->Read(&Initial_Header->flexdescindex);
    Stream->Read(&Initial_Header->numflexcontrollers);
    Stream->Read(&Initial_Header->flexcontrollerindex);
    Stream->Read(&Initial_Header->numflexrules);
    Stream->Read(&Initial_Header->flexruleindex);
    Stream->Read(&Initial_Header->numikchains);
    Stream->Read(&Initial_Header->ikchainindex);
    Stream->Read(&Initial_Header->nummouths);
    Stream->Read(&Initial_Header->mouthindex);
    Stream->Read(&Initial_Header->numlocalposeparameters);
    Stream->Read(&Initial_Header->localposeparamindex);
    Stream->Read(&Initial_Header->surfacepropindex);
    Stream->Read(&Initial_Header->keyvalueindex);
    Stream->Read(&Initial_Header->keyvaluesize);
    Stream->Read(&Initial_Header->numlocalikautoplaylocks);
    Stream->Read(&Initial_Header->localikautoplaylockindex);
    Stream->Read(&Initial_Header->mass);
    Stream->Read(&Initial_Header->contents);
    Stream->Read(&Initial_Header->numincludemodels);
    Stream->Read(&Initial_Header->includemodelindex);
    Stream->Read(&Initial_Header->virtualModel);
    Stream->Read(&Initial_Header->szanimblocknameindex);
    Stream->Read(&Initial_Header->numanimblocks);
    Stream->Read(&Initial_Header->animblockindex);
    Stream->Read(&Initial_Header->animblockModel);
    Stream->Read(&Initial_Header->bonetablebynameindex);
    Stream->Read(&Initial_Header->pVertexBase);
    Stream->Read(&Initial_Header->pIndexBase);
    Stream->Read(&Initial_Header->constdirectionallightdot);
    Stream->Read(&Initial_Header->rootLOD);
    Stream->Read(&Initial_Header->numAllowedRootLODs);
    Stream->Read(&Initial_Header->unused[0]);
    Stream->Read(&Initial_Header->unused4);
    Stream->Read(&Initial_Header->numflexcontrollerui);
    Stream->Read(&Initial_Header->flexcontrolleruiindex);
    Stream->Read(&Initial_Header->flVertAnimFixedPointScale);
    Stream->Read(&Initial_Header->surfacepropLookup);
    Stream->Read(&Initial_Header->studiohdr2index);
    Stream->Read(&Initial_Header->unused2[0]);
    return Initial_Header;
}

studiohdr2_t* ReadV49SubHeader(BinaryReader* Stream, int studiohdr2index, studiohdr2_t* Initial_Header_Part2)
{
    Stream->seek(studiohdr2index);

    Stream->Read(&Initial_Header_Part2->numsrcbonetransform);
    Stream->Read(&Initial_Header_Part2->srcbonetransformindex);
    Stream->Read(&Initial_Header_Part2->illumpositionattachmentindex);
    Stream->Read(&Initial_Header_Part2->flMaxEyeDeflection);
    Stream->Read(&Initial_Header_Part2->linearboneindex);
    Stream->Read(&Initial_Header_Part2->sznameindex);
    Stream->Read(&Initial_Header_Part2->m_nBoneFlexDriverCount);
    Stream->Read(&Initial_Header_Part2->m_nBoneFlexDriverIndex);
    Stream->Read(&Initial_Header_Part2->reserved[0]);
    return Initial_Header_Part2;
}

v53_Header* ConvertHeader(BinaryReader* Stream, BinaryWriter* OutStream, v53_Header* Dest_Header, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int animBytes, SequenceData* sequenceData, int ruiBytes, int numOfRui)
{

    int bytesAddedToHeader = 52;
    int bytesAddedToBones = Initial_Header->numbones * 28;
    int bytesAddedToAnims = -8 * Initial_Header->numlocalanim;
    int bytesAddedToAnimData = Initial_Header->numlocalanim > 0 ? animBytes : 0;
    int bytesAddedToSeqs = 20 * Initial_Header->numlocalseq;
    int bytesAddedToTextures = -20 * Initial_Header->numtextures;
    int bytesAddedToIkChains = 16 * Initial_Header->numikchains;
    int bytesAddedToActMods = 4 * sequenceData->numOfActMods;
    int textureFiller = 0;
    int strFiller = 60;
    int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + ruiBytes;

    Dest_Header->id = Initial_Header->id;
    Dest_Header->version = Initial_Header->version;
    Dest_Header->checksum = Initial_Header->checksum;
    Dest_Header->nameCopyOffset = Initial_Header_Part2->sznameindex + allBytesAdded + 408 + strFiller;
    Dest_Header->name[63] = Initial_Header->name[63];
    Dest_Header->fileSize = Initial_Header->fileSize;
    Dest_Header->eyeposition = Initial_Header->eyeposition;
    Dest_Header->illumposition = Initial_Header->illumposition;
    Dest_Header->hull_min = Initial_Header->hull_min;
    Dest_Header->hull_max = Initial_Header->hull_max;
    Dest_Header->view_bbmin = Initial_Header->view_bbmin;
    Dest_Header->view_bbmax = Initial_Header->view_bbmax;
    Dest_Header->flags = Initial_Header->flags;
    Dest_Header->numbones = Initial_Header->numbones;
    Dest_Header->boneindex = Initial_Header->boneindex + bytesAddedToHeader;
    Dest_Header->numbonecontrollers = Initial_Header->numbonecontrollers;
    Dest_Header->bonecontrollerindex = Initial_Header->bonecontrollerindex + bytesAddedToHeader + bytesAddedToBones;
    Dest_Header->numhitboxsets = Initial_Header->numhitboxsets;
    Dest_Header->hitboxsetindex = Initial_Header->hitboxsetindex + bytesAddedToHeader + bytesAddedToBones;
    Dest_Header->numlocalanim = Initial_Header->numlocalanim;
    Dest_Header->localanimindex = Initial_Header->localanimindex + bytesAddedToHeader + bytesAddedToBones;

    Dest_Header->numlocalseq = Initial_Header->numlocalseq;
    Dest_Header->localseqindex = Initial_Header->localseqindex + bytesAddedToAnims + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;

    Dest_Header->activitylistversion = Initial_Header->activitylistversion;
    Dest_Header->eventsindexed = Initial_Header->eventsindexed;

    Dest_Header->numtextures = Initial_Header->numtextures;
    Dest_Header->textureindex = Initial_Header->textureindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + ruiBytes;

    Dest_Header->numcdtextures = Initial_Header->numcdtextures;
    Dest_Header->cdtextureindex = Initial_Header->cdtextureindex + allBytesAdded;

    Dest_Header->numskinref = Initial_Header->numskinref;
    Dest_Header->numskinfamilies = Initial_Header->numskinfamilies;
    Dest_Header->skinindex = Initial_Header->skinindex + allBytesAdded;

    Dest_Header->numbodyparts = Initial_Header->numbodyparts;
    Dest_Header->bodypartindex = Initial_Header->bodypartindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;

    Dest_Header->numlocalattachments = Initial_Header->numlocalattachments;
    Dest_Header->localattachmentindex = Initial_Header->localattachmentindex + bytesAddedToHeader + bytesAddedToBones;
    Dest_Header->numlocalnodes = Initial_Header->numlocalnodes;
    Dest_Header->localnodeindex = Initial_Header->localnodeindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->localnodenameindex = Initial_Header->localnodenameindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->numflexdesc = Initial_Header->numflexdesc;
    Dest_Header->flexdescindex = Initial_Header->flexdescindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->numflexcontrollers = Initial_Header->numflexcontrollers;
    Dest_Header->flexcontrollerindex = Initial_Header->flexcontrollerindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->numflexrules = Initial_Header->numflexrules;
    Dest_Header->flexruleindex = Initial_Header->flexruleindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->numikchains = Initial_Header->numikchains;
    Dest_Header->ikchainindex = Initial_Header->ikchainindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->nummouths = numOfRui;
    Dest_Header->mouthindex = Initial_Header->includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + 8 * Initial_Header->numincludemodels;
    Dest_Header->numlocalposeparameters = Initial_Header->numlocalposeparameters;
    Dest_Header->localposeparamindex = Initial_Header->localposeparamindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->surfacepropindex = Initial_Header->surfacepropindex + allBytesAdded + strFiller;
    Dest_Header->keyvalueindex = Initial_Header->keyvalueindex + allBytesAdded;
    Dest_Header->keyvaluesize = Initial_Header->keyvaluesize;
    Dest_Header->numlocalikautoplaylocks = Initial_Header->numlocalikautoplaylocks;
    Dest_Header->localikautoplaylockindex = Initial_Header->localikautoplaylockindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->mass = Initial_Header->mass;
    Dest_Header->contents = Initial_Header->contents;
    Dest_Header->numincludemodels = Initial_Header->numincludemodels;
    Dest_Header->includemodelindex = Initial_Header->includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->virtualModel = Initial_Header->virtualModel;
    Dest_Header->bonetablebynameindex = Initial_Header->bonetablebynameindex + bytesAddedToHeader + bytesAddedToBones;
    Dest_Header->constdirectionallightdot = Initial_Header->constdirectionallightdot;
    Dest_Header->rootLOD = Initial_Header->rootLOD;
    Dest_Header->numAllowedRootLODs = Initial_Header->numAllowedRootLODs;
    Dest_Header->unused[0] = Initial_Header->unused[0];
    Dest_Header->unused4 = -1;
    Dest_Header->numflexcontrollerui = Initial_Header->numflexcontrollerui;
    Dest_Header->flexcontrolleruiindex = Initial_Header->flexcontrolleruiindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods;
    Dest_Header->pVertexBase = Initial_Header->pVertexBase;
    Dest_Header->pIndexBase = Initial_Header->pIndexBase;
    Dest_Header->unused2[0] = Initial_Header->unused2[0];

    Stream->seek(0);
    OutStream->seek(0);
    OutStream->Write(Dest_Header->id);
    OutStream->Write(53);
    OutStream->Write(Dest_Header->checksum);
    OutStream->Write(Dest_Header->nameCopyOffset);
    OutStream->write(Initial_Header->name, 64);
    OutStream->Write(Dest_Header->fileSize);
    OutStream->Write(Dest_Header->eyeposition);
    OutStream->Write(Dest_Header->illumposition);
    OutStream->Write(Dest_Header->hull_min);
    OutStream->Write(Dest_Header->hull_max);
    OutStream->Write(Dest_Header->view_bbmin);
    OutStream->Write(Dest_Header->view_bbmax);
    OutStream->Write(Dest_Header->flags);
    OutStream->Write(Dest_Header->numbones);
    OutStream->Write(Dest_Header->boneindex);
    OutStream->Write(Dest_Header->numbonecontrollers);
    OutStream->Write(Dest_Header->bonecontrollerindex);
    OutStream->Write(Dest_Header->numhitboxsets);
    OutStream->Write(Dest_Header->hitboxsetindex);
    OutStream->Write(Dest_Header->numlocalanim);
    OutStream->Write(Dest_Header->localanimindex);

    OutStream->Write(Dest_Header->numlocalseq);
    OutStream->Write(Dest_Header->localseqindex);

    OutStream->Write(Dest_Header->activitylistversion);
    OutStream->Write(Dest_Header->eventsindexed);

    OutStream->Write(Dest_Header->numtextures);
    OutStream->Write(Dest_Header->textureindex);

    OutStream->Write(Dest_Header->numcdtextures);
    OutStream->Write(Dest_Header->cdtextureindex);

    OutStream->Write(Dest_Header->numskinref);
    OutStream->Write(Dest_Header->numskinfamilies);
    OutStream->Write(Dest_Header->skinindex);

    OutStream->Write(Dest_Header->numbodyparts);
    OutStream->Write(Dest_Header->bodypartindex);

    OutStream->Write(Dest_Header->numlocalattachments);
    OutStream->Write(Dest_Header->localattachmentindex);
    OutStream->Write(Dest_Header->numlocalnodes);
    OutStream->Write(Dest_Header->localnodeindex);
    OutStream->Write(Dest_Header->localnodenameindex);
    OutStream->Write(Dest_Header->numflexdesc);
    OutStream->Write(Dest_Header->flexdescindex);
    OutStream->Write(Dest_Header->numflexcontrollers);
    OutStream->Write(Dest_Header->flexcontrollerindex);
    OutStream->Write(Dest_Header->numflexrules);
    OutStream->Write(Dest_Header->flexruleindex);
    OutStream->Write(Dest_Header->numikchains);
    OutStream->Write(Dest_Header->ikchainindex);
    OutStream->Write(Dest_Header->nummouths);
    OutStream->Write(Dest_Header->mouthindex);
    OutStream->Write(Dest_Header->numlocalposeparameters);
    OutStream->Write(Dest_Header->localposeparamindex);
    OutStream->Write(Dest_Header->surfacepropindex);
    OutStream->Write(Dest_Header->keyvalueindex);
    OutStream->Write(Dest_Header->keyvaluesize);
    OutStream->Write(Dest_Header->numlocalikautoplaylocks);
    OutStream->Write(Dest_Header->localikautoplaylockindex);
    OutStream->Write(Dest_Header->mass);
    OutStream->Write(Dest_Header->contents);
    OutStream->Write(Dest_Header->numincludemodels);
    OutStream->Write(Dest_Header->includemodelindex);
    OutStream->Write(Dest_Header->virtualModel);
    OutStream->Write(Dest_Header->bonetablebynameindex);
    OutStream->Write(Dest_Header->constdirectionallightdot);
    OutStream->Write(Dest_Header->rootLOD);
    OutStream->Write(Dest_Header->numAllowedRootLODs);
    OutStream->Write(Dest_Header->unused[0]);
    OutStream->Write(Dest_Header->unused4);
    OutStream->Write(Dest_Header->numflexcontrollerui);
    OutStream->Write(Dest_Header->flexcontrolleruiindex);
    OutStream->Write(Dest_Header->pVertexBase);
    OutStream->Write(Dest_Header->pIndexBase);
    OutStream->Write(Dest_Header->unused2[0]);


    return Dest_Header;

}

v53_Header_Part2* ConvertSubHeader(BinaryReader* Stream, BinaryWriter* OutStream, v53_Header_Part2* Dest_Header_Part2, v53_Header* Dest_Header, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, FileInfo info, int animBytes, SequenceData* sequenceData, int ruiBytes)
{
    int bytesAddedToHeader = 52;
    int bytesAddedToBones = Initial_Header->numbones * 28;
    int bytesAddedToAnims = -8 * Initial_Header->numlocalanim;
    int bytesAddedToAnimData = Initial_Header->numlocalanim > 0 ? animBytes : 0;
    int bytesAddedToSeqs = 20 * Initial_Header->numlocalseq;
    int bytesAddedToTextures = -20 * Initial_Header->numtextures;
    int bytesAddedToIkChains = 16 * Initial_Header->numikchains;
    int bytesAddedToActMods = 4 * sequenceData->numOfActMods;
    int textureFiller = 0;
    int strFiller = 60;
    int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + ruiBytes;
    int mdlSize = 0;
    int phySize = 0;
    int phyBSize = 0;
    int vtxSize = 0;
    int vvdSize = 0;


    if (info.mdl.has_value())
    {
        BinaryReader MdlStream = BinaryReader(info.mdl.value().c_str());
        mdlSize = MdlStream.size;
    }
    if (info.phy.has_value())
    {
        BinaryReader PhyStream = BinaryReader(info.phy.value().c_str());
        phySize = PhyStream.size;
    }
    if (info.pfb.has_value())
    {
        BinaryReader PhyBStream = BinaryReader(info.pfb.value().c_str());
        phyBSize = PhyBStream.size;
    }
    if (info.vtx.has_value())
    {
        BinaryReader VtxStream = BinaryReader(info.vtx.value().c_str());
        vtxSize = VtxStream.size;
    }
    if (info.vvd.has_value())
    {
        BinaryReader VvdStream = BinaryReader(info.vvd.value().c_str());
        vvdSize = VvdStream.size;
    }

    Dest_Header_Part2->numsrcbonetransform = Initial_Header_Part2->numsrcbonetransform;
    Dest_Header_Part2->srcbonetransformindex = Initial_Header_Part2->srcbonetransformindex + allBytesAdded;
    Dest_Header_Part2->illumpositionattachmentindex = Initial_Header_Part2->illumpositionattachmentindex;
    Dest_Header_Part2->linearboneindex = Initial_Header_Part2->linearboneindex + allBytesAdded + 408;
    Dest_Header_Part2->m_nBoneFlexDriverCount = Initial_Header_Part2->m_nBoneFlexDriverCount;
    Dest_Header_Part2->m_nBoneFlexDriverIndex = Initial_Header_Part2->m_nBoneFlexDriverIndex;
    Dest_Header_Part2->unkindex = Initial_Header->szanimblocknameindex + allBytesAdded;
    Dest_Header_Part2->numunk;
    Dest_Header_Part2->unkindex1 = Initial_Header->szanimblocknameindex + allBytesAdded + strFiller;
    Dest_Header_Part2->vtxindex = vtxSize > 0 ? mdlSize + allBytesAdded + phySize + phyBSize + strFiller : 0;
    Dest_Header_Part2->vvdindex = vvdSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize + strFiller : 0;
    Dest_Header_Part2->vvcindex = 0;
    Dest_Header_Part2->vphyindex = phySize > 0 ? mdlSize + allBytesAdded + strFiller : 0;
    Dest_Header_Part2->vtxsize = vtxSize;
    Dest_Header_Part2->vvdsize = vvdSize;
    Dest_Header_Part2->vvcsize = 0;
    Dest_Header_Part2->vphysize = phySize;
    Dest_Header_Part2->phybindex = mdlSize + allBytesAdded + phySize + strFiller;
    Dest_Header_Part2->numofphyb = phyBSize > 0 ? 0 : 0;
    Dest_Header_Part2->unk = 0;
    Dest_Header_Part2->unkindex3 = mdlSize + allBytesAdded + phySize + strFiller;




    OutStream->Write(Initial_Header_Part2->numsrcbonetransform);
    OutStream->Write(Dest_Header_Part2->srcbonetransformindex);
    OutStream->Write(Dest_Header_Part2->illumpositionattachmentindex);
    OutStream->Write(Dest_Header_Part2->linearboneindex);
    OutStream->Write(Initial_Header_Part2->m_nBoneFlexDriverCount);
    OutStream->Write(Initial_Header_Part2->m_nBoneFlexDriverIndex);
    OutStream->Write(Dest_Header_Part2->unkindex);
    AddInt32(OutStream, 0, 3);
    OutStream->Write(Initial_Header->szanimblocknameindex + allBytesAdded + strFiller);
    OutStream->Write(Dest_Header_Part2->vtxindex);
    OutStream->Write(Dest_Header_Part2->vvdindex);
    OutStream->Write(Dest_Header_Part2->vvcindex);
    OutStream->Write(Dest_Header_Part2->vphyindex);
    OutStream->Write(Dest_Header_Part2->vtxsize);
    OutStream->Write(Dest_Header_Part2->vvdsize);
    OutStream->Write(Dest_Header_Part2->vvcsize);
    OutStream->Write(Dest_Header_Part2->vphysize);
    OutStream->Write(Dest_Header_Part2->phybindex);
    OutStream->Write(Dest_Header_Part2->numofphyb);
    OutStream->Write(Dest_Header_Part2->unk);
    OutStream->Write(Dest_Header_Part2->unkindex3);
    AddInt32(OutStream, 0, 60);
    return Dest_Header_Part2;

}

const int compact_range = 20000;

short compactFloat(double input) {
    return round(input * compact_range / 1000);
}
double expandToFloat(short input) {
    return ((double)input) * 1000 / compact_range;
}

int Conversion::ReadHeader(FileInfo info) {
  Logger::Debug("Conversion started\n");
  UI::Progress.MainTask.Begin("Converting");

  BinaryReader Stream = BinaryReader(info.mdl.value().c_str());
  BinaryWriter OutStream = BinaryWriter(info.out.value().c_str());
  v49_Header* Initial_Header = new v49_Header();
  studiohdr2_t* Initial_Header_Part2 = new studiohdr2_t();
  v53_Header* Dest_Header = new v53_Header();
  v53_Header_Part2* Dest_Header_Part2 = new v53_Header_Part2();

  Stream.seek(0);

  Initial_Header = ReadV49Header(&Stream, Initial_Header);
  Initial_Header_Part2 = ReadV49SubHeader(&Stream, Initial_Header->studiohdr2index, Initial_Header_Part2);

  AnimData* animData = GetAnimData(&Stream, Initial_Header, Initial_Header_Part2, false);
  HitboxData* hitboxData = GetHitboxData(&Stream, Initial_Header, false);
  BoneData* boneData = GetBoneData(&Stream, Initial_Header, false);
  AttachmentData* attachmentData = GetAttachmentData(&Stream, Initial_Header, false);
  SequenceData* sequenceData = GetSequenceData(&Stream, Initial_Header, false);

  Stream.seek(Initial_Header->localanimindex);

  int animByteAddedTotal = GetAnimBytesAdded(&Stream, Initial_Header, false);
  int animSecByteAddedTotal = GetAnimSectionBytesAdded(&Stream, Initial_Header, false);

  std::vector<int> hdrBytesAnimDescAdd = GetAnimBoneHeaderBytesAdded(&Stream, Initial_Header, false);
  std::vector<int> secHdrBytesAnimDescAdd = GetAnimSectionBytesAdded2(&Stream, Initial_Header, false);
  std::vector<int> secHdrBytesSecAdd = GetAnimSectionBoneHeaderBytesAddedPerSec(&Stream, Initial_Header, false);
  std::vector<int> numOfBoneHdrsPerAnim = GetAnimBoneHeaderCount(&Stream, Initial_Header, false);
  std::vector<int> numOfSecBoneHdrsPerAnim = GetAnimSectionBoneHeaderCount(&Stream, Initial_Header, false);
  std::vector<int> numOfSecBoneHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(&Stream, Initial_Header, false);
  std::vector<int> ikChainBones = GetIkChainBones(&Stream, Initial_Header, false);

  int animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
  int bytesAddedToRuiMesh = 0;
  std::vector<int> bytesAddedPerRuiMesh;
  std::vector<mstudioruimesh_t> ruiMeshes;

  Dest_Header = ConvertHeader(&Stream, &OutStream, Dest_Header, Initial_Header, Initial_Header_Part2, animByteAddedTotal + animSecByteAddedTotal, sequenceData, bytesAddedToRuiMesh, ruiMeshes.size());
  Dest_Header_Part2 = ConvertSubHeader(&Stream, &OutStream, Dest_Header_Part2, Dest_Header, Initial_Header, Initial_Header_Part2, info, animByteAddedTotal + animSecByteAddedTotal, sequenceData, bytesAddedToRuiMesh);
 

  int bytesAddedToIkRules = 0;// -12 * animData->numOfIkRules;
  int bytesAddedToHeader = 52;
  int bytesAddedToBones = Initial_Header->numbones * 28;
  int bytesAddedToAnims = -8 * Initial_Header->numlocalanim;
  int bytesAddedToAnimData = Initial_Header->numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal + bytesAddedToIkRules : 0;
  int bytesAddedToSeqs = 20 * Initial_Header->numlocalseq;
  int bytesAddedToTextures = -20 * Initial_Header->numtextures;
  int bytesAddedToIkChains = 16 * Initial_Header->numikchains;
  int bytesAddedToActMods = 4 * sequenceData->numOfActMods;
  int textureFiller = 0;
  int strFiller = 60;
  int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + bytesAddedToIkRules + textureFiller + bytesAddedToRuiMesh;
  int numOfLinks = 0;

  Stream.seek(Initial_Header->boneindex);
  OutStream.seek(Dest_Header->boneindex);
  int bonecount = Initial_Header->numbones;
  UI::Progress.SubTask.Begin("Converting Bones");
  int* ikChainChildren = new int[10];

  for (int i = 0; i < Initial_Header->numbones; i++) {
      if (info.disable_bones) {
          char info[216];
          Stream.Stream.read(info, 108);
          OutStream.Stream.write(info, 108);
      }
      else
      {
          int stairs = bytesAddedToAnims + bytesAddedToSeqs + (28 * bonecount) - (28 * i) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
          mstudiobone_t_v49 v49Bone; Stream.Read(&v49Bone);

          for (int j = 0; j < ikChainBones.size(); j++)
          {
              if (i == ikChainBones[j]) v49Bone.flags += 0x20;
          }

          mstudiobone_t_v53 bone = BoneConversion(v49Bone, stairs);
          bone.unkid = -1;
          OutStream.Write(bone);

          UI::Progress.SubTask.Update((i + 1.0f) / (float)bonecount);
      }
  }
  delete boneData;
  UI::Progress.SubTask.End();
  int bone_filler_dest = Initial_Header->localattachmentindex - ( Initial_Header->boneindex + Initial_Header->numbones * 216 );
  filler(&Stream, &OutStream, bone_filler_dest);

  UI::Progress.SubTask.Begin("Converting Attachments");
  for (int i = 0; i < (info.disable_attachments ? 0 : Dest_Header->numlocalattachments); i++) {
      mstudioattachment_t_v49 attachment = attachmentData->attachments[i];
      int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
      attachment.sznameindex += stairs;
      OutStream.Write(attachment);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalattachments);
  }
  UI::Progress.SubTask.End();

  //hitboxsets

  int numOfHitboxes = 0;

  Stream.seek(Initial_Header->hitboxsetindex);
  OutStream.seek(Dest_Header->hitboxsetindex);
  UI::Progress.SubTask.Begin("Updating Hitbox Sets");
  for (int i = 0; i < Initial_Header->numhitboxsets; i++) {
      if (info.disable_hitboxsets) {
          char pog[44];
          mstudiohitboxset_t_v49 v49HitboxSet; Stream.Read(&v49HitboxSet);
          OutStream.Write(v49HitboxSet);
      }
      else {
          mstudiohitboxset_t_v49 v49HitboxSet; Stream.Read(&v49HitboxSet);
          int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
          if (v49HitboxSet.sznameindex > 0) v49HitboxSet.sznameindex += stairs;
          OutStream.Write(v49HitboxSet);
      }
      Logger::Notice("Updated hitbox set %d of %d\n", i + 1, Initial_Header->numhitboxsets);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numhitboxsets);
  }
  UI::Progress.SubTask.End();

  if (hitboxData->numOfHitboxes > 0)
  {
      UI::Progress.SubTask.Begin("Updating Hitboxes");
      for (int j = 0; j < hitboxData->numOfHitboxes; j++)
      {
          mstudiobbox_t_v49 v49Hitbox; Stream.Read(&v49Hitbox);
          int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
          mstudiobbox_tv53 hitbox = HitboxConversion(v49Hitbox, stairs);
          OutStream.Write(hitbox);
          Logger::Notice("Updated hitboxs %d of %d\n", j + 1, numOfHitboxes);
      }
      UI::Progress.SubTask.End();
  }

  Stream.seek(Initial_Header->bonetablebynameindex);
  OutStream.seek(Dest_Header->bonetablebynameindex);
  UI::Progress.SubTask.Begin("Converting BoneTable");
  for (int i = 0; i < Initial_Header->numbones; i++) {
      if (info.disable_bones) {

      }
      else
      {
          filler(&Stream, &OutStream, 1);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)bonecount);
      }
  }
  UI::Progress.SubTask.End();


  int anim_filler_dest = Initial_Header->localanimindex - Stream.Position();
  filler(&Stream, &OutStream, anim_filler_dest);

  Stream.seek(Initial_Header->localanimindex);
  OutStream.seek(Dest_Header->localanimindex);
  UI::Progress.SubTask.Begin("Converting Animations");
  int ikRuleNum = 0;
  std::vector<int> ikRuleStairsPerAnim;
  for (int i = 0; i < Initial_Header->numlocalanim; i++) 
  {
      if (info.disable_animations) {
          char ßtruct[100];
          Stream.Stream.read(ßtruct, 100);
          OutStream.Stream.write(ßtruct, 100);
      }
      else {
          int outPos = Dest_Header->localanimindex + 92 * i;
          int PISS = (-((8 * (Initial_Header->numlocalanim - i))) + bytesAddedToSeqs) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;
          int PISS2 = -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];
          int ikStairs = -12 * (ikRuleNum);
          mstudioanimdesc_t_v49 v49AnimDesc; Stream.Read(&v49AnimDesc);

          ikRuleStairsPerAnim.push_back(ikStairs);
          if (v49AnimDesc.numikrules > 0)
          {
              int stairs = GetAnimBoneHeaderBytesAddedIndv(&Stream, Initial_Header, i, false);
              v49AnimDesc.ikruleindex += stairs;
              ikRuleNum += v49AnimDesc.numikrules;
          }
          mstudioanimdesc_tv53 animDesc = AnimDescConversion(v49AnimDesc, PISS2, PISS, ikStairs, -outPos);
          animDesc.animindex -= 4;
          if (animDesc.sectionindex > 0) animDesc.sectionindex -= 4;
          OutStream.Write(animDesc);
      }
      ikRuleNum = 0;
      Logger::Notice("Converted animation %d of %d\n", i + 1, Initial_Header->numlocalanim);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalanim);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished animations\n");
  
  if (info.disable_animations) {
      char ßtruct[100];
      Stream.Stream.read(ßtruct, 100);
      OutStream.Stream.write(ßtruct, 100);
  }
  else
  { 
      int boneHdrNum = 0;
      int secBoneHdrNum = 0;
      int secIdxNum = 0;
      int secNumber = 0;

      BoneHeaderv53* boneHeaders = new BoneHeaderv53[GetAnimBoneHeaderTotal(&Stream, Initial_Header, false)];
      SectionBoneHeaderv53* secBoneHeaders = new SectionBoneHeaderv53[GetAnimSectionBoneHeaderCountTotal(&Stream, Initial_Header, false)];
      for (int i = 0; i < Initial_Header->numlocalanim; i++)
      {
          mstudioanimdescv53_t animDesc = animData->animDescs[i];
          int startPos = Dest_Header->localanimindex + 92 * i;
          int strStartPos = Initial_Header->localanimindex + 100 * i;
          int PISS2 = -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i] + ikRuleStairsPerAnim[i] - 4;
          int PISS3 = -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] - 4;
          OutStream.seek(startPos + animDesc.animindex + PISS2);


          int strPos = Initial_Header->localanimindex + 100 * i;
          int animIndexPos = strPos + 56;
          int secIndexPos = strPos + 80;
          int framePos = strPos + 16;
          int secFramPos = strPos + 84;

          Stream.seek(framePos);
          int frames; Stream.Read(&frames);
          Stream.seek(secFramPos);
          int secFrames; Stream.Read(&secFrames);

          Stream.seek(animIndexPos);
          int animIndex; Stream.Read(&animIndex);
          Stream.seek(secIndexPos);
          int sectionIndex; Stream.Read(&sectionIndex);
          if (sectionIndex == 0)
          {
              OutStream.seek(startPos + animDesc.animindex + PISS2);
              Stream.seek(strPos + animIndex);

              for (int j = 0; j < 100000; j++)
              {
                  int hdrStartPos = Stream.Position();
                  std::byte bone; Stream.Read(&bone);
                  std::byte flag; Stream.Read(&flag);
                  short nextOffset; Stream.Read(&nextOffset);
                  int headerSize = GetAnimHeaderSize((int)flag);
                  int dataSize = nextOffset > 0 ? nextOffset - headerSize : 0;
                  FlagTest((int)flag);

                  Stream.seek(hdrStartPos);
                  ReadBoneHeader(&Stream, boneHeaders, Initial_Header, Initial_Header_Part2, boneHdrNum, dataSize, false);
                  int pos = OutStream.Position();
                  BoneHeaderv53 boneHeader = boneHeaders[boneHdrNum];
                  OutStream.Write((float)boneHeader.translationScale);
                  OutStream.Write(boneHeader.bone);
                  OutStream.Write((std::byte)FlagTest((int)boneHeader.r2Flag));
                  OutStream.Write(boneHeader.flags);
                  OutStream.Write(boneHeader.packedRotation.one);
                  OutStream.Write(boneHeader.packedRotation.i);
                  OutStream.Write(boneHeader.packedRotation.j);
                  OutStream.Write(boneHeader.packedRotation.k);
                  OutStream.Write(boneHeader.rawPos.x);
                  OutStream.Write(boneHeader.rawPos.y);
                  OutStream.Write(boneHeader.rawPos.z);
                  OutStream.Write(boneHeader.rawScale.x);
                  OutStream.Write(boneHeader.rawScale.y);
                  OutStream.Write(boneHeader.rawScale.z);
                  OutStream.Write(boneHeader.nextOffset);
                  
                  Stream.seek(boneHeader.rawStrPos);
                  
                  boneHdrNum++;
                  if (boneHeader.nextOffset == 0)
                  {
                      Stream.seek(hdrStartPos + headerSize);
                     if(animData->animDescs[i].numikrules > 0) filler(&Stream, &OutStream, strStartPos + animData->animDescs[i].ikruleindex - boneHeader.rawStrPos);
                     if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0 && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
                     if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex == 0 && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].animindex - boneHeader.rawStrPos);
                     if (i + 1 > Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex == 0 && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, Initial_Header->localseqindex - boneHeader.rawStrPos);
                      break;
                  }
                  filler(&Stream, &OutStream, dataSize);

                  Stream.seek(hdrStartPos + nextOffset);
              }
          }

          if (sectionIndex > 0)
          {
              Stream.seek(strPos + animDesc.sectionindex);
              OutStream.seek(startPos + animDesc.sectionindex + PISS2);

              secHdrBytesSecAdd[0] = 0;
              int num = (animDesc.numframes / animDesc.sectionframes) + 2;

             for (int j = 0; j < num; j++)
             {

                 Stream.seek(strPos + animDesc.sectionindex + 8 * j);
                 int animBlock; Stream.Read(&animBlock);
                 int animOffset; Stream.Read(&animOffset);
                 OutStream.Write(animOffset + PISS3 + secHdrBytesSecAdd[secIdxNum]);
                 secIdxNum++;
             }

             for (int j = 0; j < num; j++)
             {
                 Stream.seek(strPos + sectionIndex + 8 * j);
                 int animBlock; Stream.Read(&animBlock);
                 int animOffset; Stream.Read(&animOffset);
                 int nextAnimBlock = 0;
                 int nextAnimOffset = 0;

                 if (j + 1 < num)
                 {
                     Stream.Read(&nextAnimBlock);
                     Stream.Read(&nextAnimOffset);
                 }


                 Stream.seek(strPos + animOffset);
                 OutStream.seek(startPos + animOffset + PISS3 + secHdrBytesSecAdd[secNumber]);
                 for (int k = 0; k < 10000; k++)
                 {
                     int hdrStartPos = Stream.Position();
                     int pos = OutStream.Position();
                     std::byte bone; Stream.Read(&bone);
                     std::byte flag; Stream.Read(&flag);
                     short nextOffset; Stream.Read(&nextOffset);
                     int headerSize = GetAnimHeaderSize((int)flag);
                     int dataSize = nextOffset > 0 ? nextOffset - headerSize : 0;

                     Stream.seek(hdrStartPos);
                     ReadSecBoneHeader(&Stream, secBoneHeaders, Initial_Header, Initial_Header_Part2, secBoneHdrNum, dataSize, false);
                     SectionBoneHeaderv53 secboneHeader = secBoneHeaders[secBoneHdrNum];
                     OutStream.Write((float)secboneHeader.translationScale);
                     OutStream.Write(secboneHeader.bone);
                     OutStream.Write((std::byte)FlagTest((int)secboneHeader.r2Flag));
                     OutStream.Write(secboneHeader.flags);
                     OutStream.Write(secboneHeader.packedRotation.one);
                     OutStream.Write(secboneHeader.packedRotation.i);
                     OutStream.Write(secboneHeader.packedRotation.j);
                     OutStream.Write(secboneHeader.packedRotation.k);
                     OutStream.Write(secboneHeader.rawPos.x);
                     OutStream.Write(secboneHeader.rawPos.y);
                     OutStream.Write(secboneHeader.rawPos.z);
                     OutStream.Write(secboneHeader.rawScale.x);
                     OutStream.Write(secboneHeader.rawScale.y);
                     OutStream.Write(secboneHeader.rawScale.z);
                     OutStream.Write(secboneHeader.nextOffset);

                     Stream.seek(secboneHeader.rawStrPos);

                     secBoneHdrNum++;
                     if (secboneHeader.nextOffset == 0)
                     {
                         Stream.seek(hdrStartPos + headerSize);
                         if (animData->animDescs[i].numikrules > 0) filler(&Stream, &OutStream, strStartPos + animData->animDescs[i].ikruleindex - secboneHeader.rawStrPos);
                         if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0 && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].sectionindex - secboneHeader.rawStrPos);  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
                         if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex == 0 && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].animindex - secboneHeader.rawStrPos);
                         if (j + 1 < num && i + 1 > Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0 && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, strStartPos + nextAnimOffset - secboneHeader.rawStrPos);
                         if (j + 1 > num && i + 1 > Initial_Header->numlocalanim && animData->animDescs[i].numikrules == 0) filler(&Stream, &OutStream, Initial_Header->localseqindex - secboneHeader.rawStrPos);
                         break;
                     }
                     filler(&Stream, &OutStream, dataSize);

                     Stream.seek(hdrStartPos + nextOffset);
                 }
                 secNumber++;
             }

          }

          if (animDesc.numikrules > 0)
          {
              int numOfCompressedIkErrors = 0;
              int numOfIkErrors = 0;

              for (int j = 0; j < animDesc.numikrules; j++)
              {
                  int ikStairs = -12 * (animDesc.numikrules - j);
                  mstudioikrule_t_v49 v49IkRule; Stream.Read(&v49IkRule);
                  mstudioikrule_tv53 ikRule = IkRuleConversion(v49IkRule, ikStairs);
                  OutStream.Write(ikRule);
                  if (ikRule.compressedikerrorindex > 0) numOfCompressedIkErrors++;
                  if (ikRule.ikerrorindex > 0) numOfIkErrors++;

                  ikRuleNum++;
              }

              if (numOfCompressedIkErrors > 0)
              {
                  for (int j = 0; j < numOfCompressedIkErrors; j++)
                  {
                      mstudiocompressedikerror_t_v49 v49CompressedIkError; Stream.Read(&v49CompressedIkError);
                      OutStream.Write(v49CompressedIkError);
                      filler(&Stream, &OutStream, 24);
                  }
              }

          }
      }
  }


  std::vector<std::pair<int, int>> events_change;
  ////sequences
  int seq_filler_dest = Initial_Header->localseqindex - Stream.Position();
  filler(&Stream, &OutStream, seq_filler_dest);
  Stream.seek(Initial_Header->localseqindex);
  OutStream.seek(Dest_Header->localseqindex);
  UI::Progress.SubTask.Begin("Converting Sequences");
  int numOfActMods2 = 0;
  int numOfActMods3 = 0;
  for (int i = 0; i < Initial_Header->numlocalseq; i++) {
      if (info.disable_sequences) {
          char sstruct[212];
          Stream.Stream.read(sstruct, 212);
          OutStream.Stream.write(sstruct, 212);
      }
      else {
          mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];
          int strPos = Initial_Header->localseqindex + 212 * i;
          int outPos = Dest_Header->localseqindex + 232 * i;
          if (i > 0 && strPos + sequence.weightlistindex != strPos - 212 + sequenceData->sequences[i - 1].weightlistindex) numOfActMods3 = numOfActMods2;
          int stairs = 20 * (Initial_Header->numlocalseq - i) + 4 * numOfActMods2;
          int stairs1 = strPos + sequence.weightlistindex == Initial_Header->localseqindex + sequenceData->sequences[0].weightlistindex ? 20 * (Initial_Header->numlocalseq - i) : 20 * (Initial_Header->numlocalseq - i) + 4 * numOfActMods3;
          int stairs2 = 20 * (Initial_Header->numlocalseq - i);
          int updateOffset = bytesAddedToTextures + bytesAddedToIkChains + stairs2 + bytesAddedToActMods + textureFiller + strFiller + bytesAddedToRuiMesh;

          OutStream.Write(-OutStream.Position());

          OutStream.Write(sequence.szlabelindex + updateOffset);

          OutStream.Write(sequence.szactivitynameindex + updateOffset);

          OutStream.Write(sequence.flags); // looping/non-looping flags

          OutStream.Write(sequence.activity); // initialized at loadtime to game DLL values
          OutStream.Write(sequence.actweight);

          OutStream.Write(sequence.numevents);
          sequence.eventindex > 0 ? OutStream.Write(sequence.eventindex + stairs) : OutStream.Write(sequence.eventindex);

          OutStream.Write(sequence.bbmin); // per sequence bounding box
          OutStream.Write(sequence.bbmax);

          OutStream.Write(sequence.numblends);

          // Index into array of shorts which is groupsize[0] x groupsize[1] in length
          sequence.animindexindex > 0 ? OutStream.Write(sequence.animindexindex + stairs) : OutStream.Write(sequence.animindexindex);

          sequence.movementindex > 0 ? OutStream.Write(sequence.movementindex + stairs) : OutStream.Write(sequence.movementindex); // [blend] float array for blended movement
          OutStream.Write(sequence.groupsize0);
          OutStream.Write(sequence.groupsize1);
          OutStream.Write(sequence.paramindex0); // X, Y, Z, XR, YR, ZR
          OutStream.Write(sequence.paramindex1); // X, Y, Z, XR, YR, ZR
          OutStream.Write(sequence.paramstart0); // local (0..1) starting value
          OutStream.Write(sequence.paramstart1); // local (0..1) starting value
          OutStream.Write(sequence.paramend0); // local (0..1) ending value
          OutStream.Write(sequence.paramend1); // local (0..1) ending value
          OutStream.Write(sequence.paramparent);

          OutStream.Write(sequence.fadeintime); // ideal cross fate in time (0.2 default)
          OutStream.Write(sequence.fadeouttime); // ideal cross fade out time (0.2 default)

          OutStream.Write(sequence.localentrynode); // transition node at entry
          OutStream.Write(sequence.localexitnode); // transition node at exit
          OutStream.Write(sequence.nodeflags); // transition rules

          OutStream.Write(sequence.entryphase); // used to match entry gait
          OutStream.Write(sequence.exitphase); // used to match exit gait

          OutStream.Write(sequence.lastframe); // frame that should generation EndOfSequence

          OutStream.Write(sequence.nextseq); // auto advancing sequences
          OutStream.Write(sequence.pose); // index of delta animation between end and nextseq

          OutStream.Write(sequence.numikrules);

          OutStream.Write(sequence.numautolayers);
          sequence.autolayerindex > 0 ? OutStream.Write(sequence.autolayerindex + stairs) : OutStream.Write(sequence.autolayerindex);

          sequence.weightlistindex > 0 ? OutStream.Write(sequence.weightlistindex + stairs1) : OutStream.Write(sequence.weightlistindex);

          sequence.posekeyindex > 0 ? OutStream.Write(sequence.posekeyindex + stairs) : OutStream.Write(sequence.posekeyindex);

          OutStream.Write(sequence.numiklocks);
          sequence.iklockindex > 0 ?OutStream.Write(sequence.iklockindex + stairs) : OutStream.Write(sequence.iklockindex);


          sequence.keyvalueindex > 0 ? OutStream.Write(sequence.keyvalueindex + stairs) : OutStream.Write(sequence.keyvalueindex);
          OutStream.Write(sequence.keyvaluesize);

          sequence.cycleposeindex > 0 ? OutStream.Write(sequence.cycleposeindex + stairs) : OutStream.Write(sequence.cycleposeindex); // index of pose parameter to use as cycle index

          sequence.activitymodifierindex > 0 ? OutStream.Write(sequence.activitymodifierindex + stairs) : OutStream.Write(sequence.activitymodifierindex);
          OutStream.Write(sequence.numactivitymodifiers);
          
          AddInt32(&OutStream, 0, 10);
          numOfActMods2 += sequence.numactivitymodifiers;
      }
      Logger::Notice("Converted sequence %d of %d\n", i + 1, Initial_Header->numlocalseq);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalseq);
  }
  Stream.seek(Initial_Header->localseqindex + 212 * Initial_Header->numlocalseq);
  OutStream.seek(Dest_Header->localseqindex + 232 * Initial_Header->numlocalseq);
  filler(&Stream, &OutStream, 4 * Initial_Header->numbones);
  int numOfActMods = 0;
  int numOfActMods4 = 0;
  for (int i = 0; i < Initial_Header->numlocalseq; i++)
  {
      mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];
      int strPos = Initial_Header->localseqindex + 212 * i;
      int outPos = Dest_Header->localseqindex + 232 * i;
      if (i > 0 && strPos + sequence.weightlistindex != strPos - 212 + sequenceData->sequences[i - 1].weightlistindex) numOfActMods4 = numOfActMods;
      int actModStart = numOfActMods == 0 && sequence.numactivitymodifiers > 0 ? 4 * sequence.numactivitymodifiers : 0;
      int stairs1 = strPos + sequence.weightlistindex == Initial_Header->localseqindex + sequenceData->sequences[0].weightlistindex ? 20 * (Initial_Header->numlocalseq - i) : 20 * (Initial_Header->numlocalseq - i) + 4 * numOfActMods4;
      int stairs = 20 * (Initial_Header->numlocalseq - i);

      Stream.seek(strPos);
      OutStream.seek(outPos);

      if (sequence.numblends > 0)
      {
          
          Stream.seek(strPos + sequence.animindexindex);
          OutStream.seek(outPos + sequence.animindexindex + stairs + 4 * numOfActMods);
          filler(&Stream, &OutStream, 2 * sequence.numblends);
      }

      if (sequence.posekeyindex > 0)
      {
          Stream.seek(strPos + sequence.posekeyindex);
          OutStream.seek(outPos + sequence.posekeyindex + stairs + 4 * numOfActMods);

          filler(&Stream, &OutStream, 4 * (sequence.groupsize0 + sequence.groupsize1 - 1));
          int lastFloat; Stream.Read(&lastFloat); OutStream.Write(0);
      }

      if (sequence.numautolayers > 0)
      {
          Stream.seek(strPos + sequence.autolayerindex);
          OutStream.seek(outPos + sequence.autolayerindex + stairs + 4 * numOfActMods);
          filler(&Stream, &OutStream, 4);
          int flag; Stream.Read(&flag); flag == 16384 ? OutStream.Write(49152) : OutStream.Write(49152);
          filler(&Stream, &OutStream, 16);

      }

      if (sequence.numevents > 0)
      {
          Stream.seek(strPos + sequence.eventindex);
          OutStream.seek(outPos + sequence.eventindex + stairs + 4 * numOfActMods);
          filler(&Stream, &OutStream, 80 * sequence.numevents);
      }

      if (sequence.numactivitymodifiers > 0)
      {
          Stream.seek(strPos + sequence.activitymodifierindex);
          OutStream.seek(outPos + sequence.activitymodifierindex + stairs + 4 * numOfActMods);
          for (int j = 0; j < sequence.numactivitymodifiers; j++)
          {
              Stream.seek(strPos + sequence.activitymodifierindex + 4 * j);
              OutStream.seek(outPos + sequence.activitymodifierindex + stairs + (4 * numOfActMods) + 8 * j);
              filler(&Stream, &OutStream, 4);
          }
      }

      if (sequence.weightlistindex > 0)
      {
          Stream.seek(strPos + sequence.weightlistindex);
          OutStream.seek(outPos + sequence.weightlistindex + stairs1);
          filler(&Stream, &OutStream, 4 * Initial_Header->numbones);
      }
      numOfActMods += sequence.numactivitymodifiers;

  }
  Stream.seek(Initial_Header->localnodeindex);
  OutStream.seek(Dest_Header->localnodeindex);

  UI::Progress.SubTask.End();
  Logger::Info("Finished sequences\n");
  int numOfModels = 0;
  int numOfMeshes = 0;
  int numOfIkLinks = 0;
  int meshStart = 0;

  if (Initial_Header->numbodyparts > 0)
  {
      int bpart_filler_dest = Initial_Header->bodypartindex - Stream.Position();
      UI::Progress.SubTask.Begin("Converting Body Parts");
      filler(&Stream, &OutStream, bpart_filler_dest);
      Stream.seek(Initial_Header->bodypartindex);
      OutStream.seek(Dest_Header->bodypartindex);

      for (int i = 0; i < Initial_Header->numbodyparts; i++) {
          //16 bytes
          if (info.disable_bodyparts) {
              char ßtruct[16];
              Stream.Stream.read(ßtruct, 16);
              OutStream.Stream.write(ßtruct, 16);
          }
          else {
              int stairs = bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller + bytesAddedToRuiMesh;
              mstudiobodyparts_t_v49 v49BodyPart; Stream.Read(&v49BodyPart);
              if (v49BodyPart.sznameindex > 0) v49BodyPart.sznameindex += stairs;
              if (v49BodyPart.nummodels > 0) numOfModels += v49BodyPart.nummodels;
              OutStream.Write(v49BodyPart);
          }
          Logger::Notice("Converted body part %d of %d\n", i + 1, Initial_Header->numbodyparts);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numbodyparts);
      }
      UI::Progress.SubTask.End();
      Logger::Info("Finished body parts\n");
  }

  
  if (numOfModels > 0)
  {
      int model_filler_dest = Initial_Header->bodypartindex + 16 * Initial_Header->numbodyparts - Stream.Position();
      UI::Progress.SubTask.Begin("Updating Model Structs");
      filler(&Stream, &OutStream, model_filler_dest);
      for (int i = 0; i < numOfModels; i++)
      {
          mstudiomodel_t_v49 _model; Stream.Read(&_model);
          if (_model.eyeballindex > 0) _model.eyeballindex = 0;
          if (_model.meshindex > 0) _model.meshindex += bytesAddedToIkChains;
          if (_model.attachmentindex > 0) _model.attachmentindex += bytesAddedToIkChains;
          if (_model.nummeshes > 0) numOfMeshes += _model.nummeshes;
          OutStream.Write(_model);
          Logger::Notice("Updated model struct %d of %d\n", i + 1, numOfModels);
      }
      UI::Progress.SubTask.End();
      Logger::Info("Finished model structs\n");
  }



  if (Initial_Header->numikchains > 0)
  {
      UI::Progress.SubTask.Begin("Converting IkChains");
      for (int i = 0; i < Initial_Header->numikchains; i++)
      {
          int pos = Stream.Position();
          int offset = bytesAddedToTextures + textureFiller + strFiller + (16 * Initial_Header->numikchains) - (16 * i) + bytesAddedToRuiMesh;
          int offset2 = (16 * Initial_Header->numikchains) - (16 * i);
          Vector3 kneeDir = {0,0,0};
          mstudioikchain_t_v49 v49IkChain; Stream.Read(&v49IkChain);
          if (v49IkChain.sznameindex > 0) v49IkChain.sznameindex += offset;
          if (v49IkChain.linkindex > 0) v49IkChain.linkindex += offset2;
          if (v49IkChain.numlinks > 0) numOfIkLinks += v49IkChain.numlinks;

          OutStream.Write(v49IkChain);
          OutStream.Write((float)0.707);
          OutStream.Write(kneeDir);
          Logger::Notice("Converting IkChain %d of %d\n", i + 1, Initial_Header->numikchains);
      }

      for (int i = 0; i < numOfIkLinks; i++)
      {
          mstudioiklink_t_v49 v49IkLink; Stream.Read(&v49IkLink);
          OutStream.Write(v49IkLink);
      }



      UI::Progress.SubTask.End();
      Logger::Info("Finished Converting IkChains\n");
  }

  if (numOfMeshes > 0)
  {
      UI::Progress.SubTask.Begin("Updating Mesh Structs");
      for (int i = 0; i < numOfMeshes; i++)
      {
          mstudiomesh_t_v49 _mesh; Stream.Read(&_mesh);
          if(Initial_Header->numikchains > 0)_mesh.modelindex -= bytesAddedToIkChains;
          OutStream.Write(_mesh);
          Logger::Notice("Updated mesh struct %d of %d\n", i + 1, numOfMeshes);
      }

      UI::Progress.SubTask.End();
      Logger::Info("Finished mesh structs\n");
  }

  if (Initial_Header->numlocalposeparameters > 0)
  {
      int pose_filler_dest = Initial_Header->localposeparamindex - Stream.Position();
      filler(&Stream, &OutStream, pose_filler_dest);
      UI::Progress.SubTask.Begin("Updating PoseParameters");
      for (int i = 0; i < Initial_Header->numlocalposeparameters; i++)
      {
          CopyAddInt32(&Stream, &OutStream, bytesAddedToTextures + textureFiller + strFiller + bytesAddedToRuiMesh, 1);                           //nameIndex
          CopyAddInt32(&Stream, &OutStream, 0, 1);                                              //flags
          CopyAddFloat32(&Stream, &OutStream, 0, 1);                                            //start
          CopyAddFloat32(&Stream, &OutStream, 0, 1);                                            //end
          CopyAddFloat32(&Stream, &OutStream, 0, 1);                                            //loop
          Logger::Notice("Updating PoseParameter %d of %d\n", i + 1, Initial_Header->numlocalposeparameters);
      }
      UI::Progress.SubTask.End();
      Logger::Info("Finished Updating PoseParameters\n");
  }
  
  UI::Progress.SubTask.Begin("Converting Included Models");
  
  Stream.seek(Initial_Header->includemodelindex);
  OutStream.seek(Dest_Header->includemodelindex);
  for (int i = 0; i < Initial_Header->numincludemodels; i++) {
      //8 bytes
      if (info.disable_includemodels) {
          char ßtruct[8];
          Stream.Stream.read(ßtruct, 8);
          OutStream.Stream.write(ßtruct, 8);
      }
      else {
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //material
          CopyAddInt32(&Stream, &OutStream, bytesAddedToTextures + textureFiller + strFiller + bytesAddedToRuiMesh, 1);  //material
      }

      Logger::Notice("Converted included model %d of %d\n", i + 1, Initial_Header->numincludemodels);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numincludemodels);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished included models\n");


  //if (info.aabb.has_value()) {
  //    BinaryReader RUIStream = BinaryReader(info.aabb.value().c_str());
  //    if (!RUIStream.Stream.good()) {
  //        Logger::Error("Model's phy file does not exist, please ensure %s exists, and is located in the same directory as the file\n", info.aabb.value().c_str());
  //        return 1;
  //    }
  //    printf("writing rui at [%i]...\n", OutStream.Position());
  //    UI::Progress.SubTask.Begin("Copying Physics Data");
  //    for (int i = 0; i < RUIStream.size; i++) {
  //        byte tmp;
  //        RUIStream.Read(&tmp);
  //        OutStream.Write(tmp);
  //        UI::Progress.SubTask.Update((i + 1.0f) / (float)RUIStream.size);
  //    }
  //    UI::Progress.SubTask.End();
  //    Logger::Info("done\n");
  //}
  //
  //if (ruiMeshes.size() > 0 && !info.aabb.has_value() && info.str.has_value())
  //{
  //    for (int i = 0; i < ruiMeshes.size(); i++)
  //    {
  //        OutStream.Write(0);
  //        OutStream.Write( (int)( 8 * ( ruiMeshes.size() - i ) + bytesAddedPerRuiMesh[i]));
  //        double sizeCheck = GetRuiBytesAddedIdv(ruiMeshes, i) / 16.0;
  //        int idk = 0;
  //        Logger::Info("sizeCheck: %d, rui: %d\n", GetRuiBytesAddedIdv(ruiMeshes, i), i);
  //        if (!IsWhole(sizeCheck))
  //        {
  //            Logger::Info("sizeCheck: %d, rui: %d\n", sizeCheck, i);
  //            double sizeCheck2 = 16 * ( (int)sizeCheck + 1);
  //            idk = sizeCheck2 - GetRuiBytesAddedIdv(ruiMeshes, i);
  //        }
  //        
  //        int parentIdxAdd = 2 * ruiMeshes[i].numparents;
  //        int facesIdxAdd = 32 * ruiMeshes[i].numfaces;
  //        int verticesIdxAdd = 16 * ruiMeshes[i].numvertices;
  //        int verticesIdxAdd2 = 6 * ruiMeshes[i].numfaces;
  //        int meshNameIdxAdd = ruiMeshes[i].szruimeshname.size() + idk;
  //        int unk = 0;
  //        std::byte null = (std::byte)0;
  //        Logger::Info("idk: %d, rui: %d\n", idk, i);
  //        OutStream.Write(ruiMeshes[i].numparents);
  //        OutStream.Write(ruiMeshes[i].numvertices);
  //        OutStream.Write(ruiMeshes[i].numfaces);
  //        OutStream.Write((int)(32 + meshNameIdxAdd));
  //        OutStream.Write((int)(32 + meshNameIdxAdd + verticesIdxAdd2 + parentIdxAdd));
  //        OutStream.Write((int)(32 + meshNameIdxAdd + parentIdxAdd));
  //        OutStream.Write((int)(32 + meshNameIdxAdd + parentIdxAdd + verticesIdxAdd + verticesIdxAdd2));
  //        OutStream.Write(unk);
  //        OutStream.write(ruiMeshes[i].szruimeshname.c_str(), ruiMeshes[i].szruimeshname.size());
  //        for (int j = 0; j < idk; j++) OutStream.Write(null);
  //
  //        for (int j = 0; j < ruiMeshes[i].numparents; j++)
  //        {
  //            OutStream.Write(ruiMeshes[i].parent[j]);
  //        }
  //
  //        for (int j = 0; j < ruiMeshes[i].numfaces; j++)
  //        {
  //            OutStream.Write(ruiMeshes[i].vertexmap[j].vertstartid);
  //            OutStream.Write(ruiMeshes[i].vertexmap[j].vertendid);
  //            OutStream.Write(ruiMeshes[i].vertexmap[j].vertextraid);
  //        }
  //
  //        for (int j = 0; j < ruiMeshes[i].numvertices; j++)
  //        {
  //            OutStream.Write(ruiMeshes[i].vertex[j].parent);
  //            OutStream.Write(ruiMeshes[i].vertex[j].vertexpos);
  //        }
  //
  //        for (int j = 0; j < ruiMeshes[i].numfaces; j++)
  //        {
  //            OutStream.Write(ruiMeshes[i].facedata[j].faceuvmin.x);
  //            OutStream.Write(ruiMeshes[i].facedata[j].faceuvmin.y);
  //            OutStream.Write(ruiMeshes[i].facedata[j].faceuvmax.x);
  //            OutStream.Write(ruiMeshes[i].facedata[j].faceuvmax.y);
  //            OutStream.Write(ruiMeshes[i].facedata[j].facescalemin.x);
  //            OutStream.Write(ruiMeshes[i].facedata[j].facescalemin.y);
  //            OutStream.Write(ruiMeshes[i].facedata[j].facescalemax.x);
  //            OutStream.Write(ruiMeshes[i].facedata[j].facescalemax.y);
  //        }
  //    }
  //}


#pragma region texture conversion
  AddInt32(&OutStream, 0, 3);  //filler
  Stream.seek(Initial_Header->textureindex);
  OutStream.seek(Dest_Header->textureindex);
  UI::Progress.SubTask.Begin("Converting Textures");
  for (int i = 0; i < Initial_Header->numtextures; i++) {
      if (info.disable_textures) {
          char ßtruct[64];
          Stream.Stream.read(ßtruct, 64);
          OutStream.Stream.write(ßtruct, 64);
      }
      else {
          CopyAddInt32(&Stream, &OutStream, -((20) * (Initial_Header->numtextures - i)) + strFiller, 1);
          CopyAddInt32(&Stream, &OutStream, 0, 10);  //filler
          Stream.seek(Stream.Position() + 20);
      }
      Logger::Notice("Converted texture %d of %d\n", i + 1, Initial_Header->numtextures);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numtextures);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished textures\n");
#pragma endregion

  int cdt_filler_dest = Initial_Header->cdtextureindex - Stream.Position();
  UI::Progress.SubTask.Begin("Converting Cd Textures");
  filler(&Stream, &OutStream, cdt_filler_dest);
  for (int i = 0; i < Initial_Header->numcdtextures; i++) {
      if (info.disable_textures) {
          char ßtruct[4];
          Stream.Stream.read(ßtruct, 4);
          OutStream.Stream.write(ßtruct, 4);
      }
      else 
      {
          CopyAddInt32(&Stream, &OutStream, allBytesAdded + strFiller, 1);
      }
      Logger::Notice("Converted texture %d of %d\n", i + 1, Initial_Header->numcdtextures);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numcdtextures);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished cd textures\n");

  int lnrB_filler_dest = Initial_Header_Part2->linearboneindex + 408 - Stream.Position();
  filler(&Stream, &OutStream, lnrB_filler_dest);
  
  Stream.seek(Initial_Header_Part2->linearboneindex + 408);
  mstudiolinearbone_t_v49 linearBone; Stream.Read(&linearBone);
  mstudiolinearbonedata_t_v49 linearBoneData; Stream.Read(&linearBoneData, Initial_Header->numbones);
  linearBone.posscaleindex = 0;
  for (int i = 0; i < Initial_Header->numbones; i++) 
  {
      if (info.disable_bones) {
          char info[216];
          Stream.Stream.read(info, 108);
          OutStream.Stream.write(info, 108);
      }
      else
      {
          for (int j = 0; j < ikChainBones.size(); j++)
          {
              if (i == ikChainBones[j])
              {
                  linearBoneData.flags[i] += 0x20;
              }
          }
          linearBoneData.posScale[i] = { 0,0,0 };
      }
  }
  OutStream.Write(linearBone);
  OutStream.Write(linearBoneData, Initial_Header->numbones);
  AddInt32(&OutStream, 0, 15);  //filler

#pragma region rest of the file
  int pos = Stream.Position();
  UI::Progress.SubTask.Begin("Copying Misc Data");
  for (int i = 0; i < Stream.size - pos; i++) {
      byte tmp;
      Stream.Read(&tmp);
      OutStream.Write(tmp);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)(Stream.size - pos));
  }
  UI::Progress.SubTask.End();
  if (OutStream.Position() < Stream.size + allBytesAdded) {
      Logger::Critical("Not enough of the file was written\n");
      Logger::Critical("(%d vs %d)\n", OutStream.Position(), Stream.size + allBytesAdded);
      return 1;
  }

  Logger::Notice("Wrote Rest of File\n");
  if (OutStream.Position() < Stream.Position() + allBytesAdded) {
      Logger::Critical("Relative cursors are desynced\n");
      Logger::Critical("(%d vs %d)\n", OutStream.Position(), Stream.Position() + allBytesAdded);
      return 1;
  }
  
  if (OutStream.Position() < Stream.Position() + allBytesAdded) {
      Logger::Critical("Relative cursors are desynced\n");
      Logger::Critical("(%d vs %d)\n", OutStream.Position(), Stream.Position() + allBytesAdded);
      return 1;
  }


  if (info.phy.has_value()) {
      BinaryReader PhyStream = BinaryReader(info.phy.value().c_str());
      if (!PhyStream.Stream.good()) {
          Logger::Error("Model's phy file does not exist, please ensure %s exists, and is located in the same directory as the file\n", info.phy.value().c_str());
          return 1;
      }
      printf("writing phy at [%i]...\n", OutStream.Position());
      Dest_Header_Part2->vphyindex = OutStream.Position();
      UI::Progress.SubTask.Begin("Copying Physics Data");
      for (int i = 0; i < PhyStream.size; i++) {
          byte tmp;
          PhyStream.Read(&tmp);
          OutStream.Write(tmp);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)PhyStream.size);
      }
      UI::Progress.SubTask.End();
      Logger::Info("done\n");
  }

  if (info.pfb.has_value()) {
      BinaryReader PhyBStream = BinaryReader(info.pfb.value().c_str());
      if (!PhyBStream.Stream.good()) {
          Logger::Error("Model's rphy file does not exist, please ensure %s exists, and is located in the same directory as the file\n", info.pfb.value().c_str());
          return 1;
      }
      printf("writing rphy at [%i]...\n", OutStream.Position());
      Dest_Header_Part2->phybindex = OutStream.Position();
      UI::Progress.SubTask.Begin("Copying Respawn Physics Data");
      for (int i = 0; i < PhyBStream.size; i++) {
          byte tmp;
          PhyBStream.Read(&tmp);
          OutStream.Write(tmp);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)PhyBStream.size);
      }
      UI::Progress.SubTask.End();
      Logger::Info("done\n");
  }


  if (info.vtx.has_value()) {
      BinaryReader VtxStream = BinaryReader(info.vtx.value().c_str());
      if (!VtxStream.Stream.good()) {
          Logger::Error("Model's vtx file does not exist, please ensure %s exists, and is located in the same directory as the file\n", info.vtx.value().c_str());
          return 1;
      }
      Logger::Info("writing vtx at [%d]...\n", OutStream.Position());
      Dest_Header_Part2->vtxindex = OutStream.Position();
      Dest_Header_Part2->unkindex3 = OutStream.Position();
      Dest_Header_Part2->phybindex = OutStream.Position();
      UI::Progress.SubTask.Begin("Copying Mesh Data");
      for (int i = 0; i < VtxStream.size; i++) {
          byte tmp;
          VtxStream.Read(&tmp);
          OutStream.Write(tmp);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)VtxStream.size);
      }
      UI::Progress.SubTask.End();
      Logger::Info("done\n");
  }

  if (info.vvd.has_value()) {
      BinaryReader VvdStream = BinaryReader(info.vvd.value().c_str());
      if (!VvdStream.Stream.good()) {
          Logger::Error("Model's vvd file does not exist, please ensure %s exists, and is located in the same directory as the file\n", info.vvd.value().c_str());
          return 1;
      }
      Logger::Info("writing vvd at [%d]...\n", OutStream.Position());
      Dest_Header_Part2->vvdindex = OutStream.Position();
      UI::Progress.SubTask.Begin("Copying Vertex Data");
      for (int i = 0; i < VvdStream.size; i++) {
          byte tmp;
          VvdStream.Read(&tmp);
          OutStream.Write(tmp);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)VvdStream.size);
      }
      UI::Progress.SubTask.End();
      Logger::Info("done\n");
  }
  if (sequenceData->numOfEvents > 0)
  {
      int numActMods = 0;
      for (int i = 0; i < Initial_Header->numlocalseq; i++)
      {
          mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];
          int PISS = 20 * (Initial_Header->numlocalseq - i) + 4 * numActMods;
          int startOutPos = Dest_Header->localseqindex + 232 * i;
          int startStrPos = Initial_Header->localseqindex + 212 * i;
          Stream.seek(startStrPos + sequence.eventindex);
          OutStream.seek(startOutPos + sequence.eventindex + PISS);
  
          for (int j = 0; j < sequence.numevents; j++)
          {
              Stream.seek(startStrPos + sequence.eventindex + 76 + (80 * j));
              OutStream.seek(startOutPos + sequence.eventindex + PISS + 76 + (80 * j));
              int idx; Stream.Read(&idx); OutStream.Write(idx + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller + (4 * (sequenceData->numOfActMods - numActMods)));
          }
          numActMods += sequence.numactivitymodifiers;
      }
  }
  
  if (sequenceData->numOfActMods > 0)
  {
      int numActMods = 0;
      for (int i = 0; i < Initial_Header->numlocalseq; i++)
      {
          mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];
          int PISS = (20 * Initial_Header->numlocalseq) - (20 * i) + 4 * numActMods;
          int startOutPos = Dest_Header->localseqindex + 232 * i;
          int startStrPos = Initial_Header->localseqindex + 212 * i;
          Stream.seek(startStrPos + sequenceData->actModIdxs[i]);
          OutStream.seek(startOutPos + sequenceData->actModIdxs[i] + PISS);
          if (sequence.numactivitymodifiers > 0)
          {
              int idx; Stream.Read(&idx); OutStream.Write(idx + bytesAddedToRuiMesh + bytesAddedToTextures + bytesAddedToIkChains + textureFiller + strFiller + (4 * (sequenceData->numOfActMods - numActMods)) );
              OutStream.seek(OutStream.Position() + 4);
          }
          numActMods += sequence.numactivitymodifiers;
      }
  }
  
  if (Initial_Header->numlocalnodes > 0)
  {
      UI::Progress.SubTask.Begin("Updating Local Node Names");
      Stream.seek(Initial_Header->localnodenameindex);
      OutStream.seek(Dest_Header->localnodenameindex);
      for (int i = 0; i < Initial_Header->numlocalnodes; i++)
      {
          CopyAddInt32(&Stream, &OutStream, allBytesAdded + strFiller, 1);
          Logger::Notice("Converted body part %d of %d\n", i + 1, Initial_Header->numlocalnodes);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalnodes);
      }
  }

  if (Initial_Header->numlocalposeparameters > 0)
  {
      UI::Progress.SubTask.Begin("Updating Pose Param Names");
      for (int i = 0; i < Initial_Header->numlocalposeparameters; i++)
      {
          Stream.seek(Initial_Header->localposeparamindex + 20 * i);
          OutStream.seek(Dest_Header->localposeparamindex + 20 * i);
          CopyAddInt32(&Stream, &OutStream, bytesAddedToTextures + textureFiller + strFiller + bytesAddedToRuiMesh, 1);


          Logger::Notice("Updated Pose Param Name %d of %d\n", i + 1, Initial_Header->numlocalposeparameters);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalposeparameters);
      }
  }


  OutStream.Stream.close();
  Stream.Stream.close();

  Logger::Info("Finished!\n");
  UI::Progress.MainTask.End();
  return 0;
}
#pragma clang diagnostic pop