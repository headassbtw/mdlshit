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
using namespace std;

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
        case 33: return 14;
        case 32: return 14;
        case 8: return 10;
        case 1: return 26;
        case 12: return 8;

        default: return 14;
    }
}

int GetAnimBoneHeaderTotal(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int hdrNum = 0;
    int* hdrNumPerAnim = new int[Initial_Header->numlocalanim];
    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int bHdrCount = 0;
        int strPos = Initial_Header->localanimindex + 100 * i;
        int animPos = strPos + 56;
        Stream->seek(animPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(strPos + animIndex);

        for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++)
        {
            int pos2 = Stream->Position();
            std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
            std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
            short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
            bHdrCount++;
            hdrNum++;
            if (nextOffset == 0)
            {
                break;
            }
            Stream->seek(pos2 + nextOffset);

        }
        if (debug) Logger::Info("AnimNum: %d, hdrNum:  %d\n", i, bHdrCount);
        hdrNumPerAnim[i] = bHdrCount;


    }
    return hdrNum;
}

std::vector<int> GetAnimBoneHeaderCount(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int hdrNum = 0;
    std::vector<int> hdrNumPerAnim;
    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int bHdrCount = 0;
        int strPos = Initial_Header->localanimindex + 100 * i;
        int animPos = strPos + 56;
        Stream->seek(animPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(strPos + animIndex);

        for (int j = 0; j < Initial_Header->numbones * Initial_Header->numlocalanim; j++)
        {
            int pos2 = Stream->Position();
            std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
            std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
            short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
            bHdrCount++;
            hdrNum++;
            if (nextOffset == 0)
            {
                break;
            }
            Stream->seek(pos2 + nextOffset);

        }
        if (debug) Logger::Info("AnimNum: %d, hdrNum:  %d\n", i, bHdrCount);
        hdrNumPerAnim.push_back(bHdrCount);


    }
    return hdrNumPerAnim;
}

int GetAnimBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    int hdrCount = GetAnimBoneHeaderTotal(Stream, Initial_Header, false);

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int strPos = Initial_Header->localanimindex + 100 * i;
        int animPos = strPos + 56;
        int secIndexPos = strPos + 80;
        Stream->seek(animPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);
        Stream->seek(strPos + animIndex);
        if (sectionIndex == 0)
        {
            for (int j = 0; j < 1000000; j++)
            {
                int pos2 = Stream->Position();
                std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                int headerSize = GetAnimHeaderSize((int)flag);
                bytesAdded += (32 - headerSize);
                if (nextOffset == 0)
                {
                    if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, bytesAdded);
                    break;
                }
                Stream->seek(pos2 + nextOffset);

            }
        }

    }
    return bytesAdded;
}


int GetAnimSectionCountTotal(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int secNum = 0;
    int* secPerAnim = new int[Initial_Header->numlocalanim];
    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);

        if (sectionIndex > 0)
        {
            int secNumber = (frames / secFrames) + 2;
            if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, secNumber);
            secPerAnim[i] = secNumber;
            secNum += secNumber;
        }
        else secPerAnim[i] = 0;



    }
    return secNum;
}

std::vector<int> GetAnimSectionBoneHeaderCount(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    int  secTotal = GetAnimSectionCountTotal(Stream, Initial_Header, false);
    int* hdrsPerSec = new int[secTotal];
    std::vector<int> secHdrsPerAnim;
    int secHdrNum = 0;
    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);

        if (sectionIndex > 0)
        {

            for (int j = 0; j < secPerAnim[i]; j++)
            {
                //secHdrNum = 0;

                Stream->seek(startPos + sectionIndex);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                for (int k = 0; k < Initial_Header->numbones * Initial_Header->numlocalanim; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    secHdrNum++;
                    if (nextOffset == 0)
                    {
                        Stream->seek(pos2 + headerSize);
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
            }
            secHdrsPerAnim.push_back(secHdrNum);

            //secHdrsPerAnim[i] = secHdrNum;
            //if (debug) Logger::Info("AnimNum: %d, secHdrNum:  %d\n", i, secHdrNum);
        }
        else secHdrsPerAnim.push_back(0);



    }
    return secHdrsPerAnim;

}

int GetAnimSectionBoneHeaderCountTotal(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    int secHdrNum = 0;
    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);

        if (sectionIndex > 0)
        {

            for (int j = 0; j < secPerAnim[i]; j++)
            {
                //secHdrNum = 0;

                Stream->seek(startPos + sectionIndex);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                for (int k = 0; k < Initial_Header->numbones * Initial_Header->numlocalanim; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    secHdrNum++;
                    if (nextOffset == 0)
                    {
                        Stream->seek(pos2 + headerSize);
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
            }
            if (debug) Logger::Info("AnimNum: %d, secHdrNum:  %d\n", i, secHdrNum);
        }



    }
    return secHdrNum;

}

std::vector<int> GetAnimSectionBoneHeaderCountPerSec(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    int  secTotal = GetAnimSectionCountTotal(Stream, Initial_Header, false);
    std::vector<int> hdrsPerSec;
    int* secHdrsPerAnim = new int[secTotal];
    int secNum = 0;
    int test = 0;
    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);

        if (sectionIndex > 0)
        {
            int secHdrNum = 0;

            for (int j = 0; j < secPerAnim[i]; j++)
            {
                secHdrNum = 0;

                Stream->seek(startPos + sectionIndex);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                for (int k = 0; k < 100000000; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    secHdrNum++;
                    if (nextOffset == 0)
                    {
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
                hdrsPerSec.push_back(secHdrNum);
                if (debug) Logger::Info("SecNum: %d, secHdrNum:  %d\n", j, secHdrNum);
                secNum++;
            }
        }
    }
    return hdrsPerSec;

}

int GetAnimSectionBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    std::vector<int> secHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, false);
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    int secNum = 0;

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);

        if (sectionIndex > 0)
        {
            for (int j = 0; j < secPerAnim[i]; j++)
            {
                int bytesAddedPer = 0;
                Stream->seek(startPos + sectionIndex + 8 * j);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                for (int k = 0; k < 1000000; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    bytesAddedPer += (32 - headerSize);
                    bytesAdded += (32 - headerSize);
                    if (nextOffset == 0)
                    {
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
                secNum++;
            }
        }
    }
    return bytesAdded;
}

std::vector<int> GetAnimSectionBytesAdded2(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    std::vector<int> secHdrsPerAnim = GetAnimSectionBoneHeaderCount(Stream, Initial_Header, false);
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    std::vector<int> bytesAddedPerAnim;
    int secNum = 0;

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);
        bytesAddedPerAnim.push_back(bytesAdded);
        if (sectionIndex > 0)
        {
            for (int j = 0; j < secPerAnim[i]; j++)
            {
                int bytesAddedPer = 0;
                Stream->seek(startPos + sectionIndex + 8 * j);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                for (int k = 0; k < secHdrsPerAnim[i]; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    bytesAddedPer += (32 - headerSize);
                    bytesAdded += (32 - headerSize);
                    if (nextOffset == 0)
                    {
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
                secNum++;
            }
        }
        //bytesAddedPerAnim.push_back(bytesAdded);
    }
    return bytesAddedPerAnim;
}

std::vector<int> GetAnimSectionBoneHeaderBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    std::vector<int> secHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(Stream, Initial_Header, false);
    std::vector<int> secBytesAddedPerAnim;
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    std::vector<int> secBytesAddedPerSec;
    int secNum = 0;
    int test = 0;

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);
        if (secNum < 1)
        {
            secBytesAddedPerAnim.push_back(0);
            if (debug) Logger::Info("AnimNum: %d, secBytesAdded:  %d\n", i, bytesAdded);
        }
        if (sectionIndex > 0)
        {
            //if (secNum == 1) secBytesAddedPerAnim.push_back(0);
            for (int j = 0; j < secPerAnim[i]; j++)
            {
                int bytesAddedPer = 0;
                Stream->seek(startPos + sectionIndex + 8 * j);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                for (int k = 0; k < secHdrsPerSec[i]; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    bytesAddedPer += (32 - headerSize);
                    bytesAdded += (32 - headerSize);
                    if (nextOffset == 0)
                    {
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
                secNum++;
            }
        }
        if (secNum > 1)
        {
            secBytesAddedPerAnim.push_back(bytesAdded);
            if (debug) Logger::Info("AnimNum: %d, secBytesAdded:  %d\n", i, bytesAdded);
        }

    }
    return secBytesAddedPerAnim;
}

std::vector<int> GetAnimBoneHeaderBytesAdded2(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    std::vector<int> hdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, false);
    std::vector<int> hdrBytesAddedPerAnim;
    int passedSec = 0;
    int test = 0;

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int bytesAddedPer = 0;
        int strPos = Initial_Header->localanimindex + 100 * i;
        int animPos = strPos + 56;
        int secIndexPos = strPos + 80;
        Stream->seek(animPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);
        Stream->seek(strPos + animIndex);
        
        hdrBytesAddedPerAnim.push_back(bytesAdded);
        if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, hdrBytesAddedPerAnim[i]);

        for (int j = 0; j < hdrsPerAnim[i]; j++)
        {
            int pos2 = Stream->Position();
            std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
            std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
            short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
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
    return hdrBytesAddedPerAnim;
}


std::vector<int> GetAnimBoneHeaderBytesAdded(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    std::vector<int> hdrsPerAnim = GetAnimBoneHeaderCount(Stream, Initial_Header, false);
    std::vector<int> hdrBytesAddedPerAnim;
    int passedSec = 0;
    int test = 0;

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int bytesAddedPer = 0;
        int strPos = Initial_Header->localanimindex + 100 * i;
        int animPos = strPos + 56;
        int secIndexPos = strPos + 80;
        Stream->seek(animPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);
        Stream->seek(strPos + animIndex);
        hdrBytesAddedPerAnim.push_back(bytesAdded);
        if (debug) Logger::Info("AnimNum: %d, bytesAdded:  %d\n", i, hdrBytesAddedPerAnim[i]);

        if (sectionIndex == 0)
        {
            for (int j = 0; j < hdrsPerAnim[i]; j++)
            {
                int pos2 = Stream->Position();
                std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
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
        else hdrBytesAddedPerAnim[i] = bytesAdded;

    }
    return hdrBytesAddedPerAnim;
}


std::vector<int> GetAnimSectionBoneHeaderBytesAddedPerSec(BinaryReader* Stream, v49_Header* Initial_Header, bool debug)
{
    int bytesAdded = 0;
    int* secBytesAddedPerAnim = new int[Initial_Header->numlocalanim];
    int* secPerAnim = GetAnimSectionCount(Stream, Initial_Header, false);
    std::vector<int> secBytesAddedPerSec;
    std::vector<int> secHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(Stream, Initial_Header, false);
    int secNum = 0;
    int test = 0;

    for (int i = 0; i < Initial_Header->numlocalanim; i++)
    {
        int startPos = Initial_Header->localanimindex + 100 * i;
        int animIndexPos = startPos + 56;
        int secIndexPos = startPos + 80;
        int framePos = startPos + 16;
        int secFramPos = startPos + 84;

        Stream->seek(framePos);
        int frames; Stream->Read(&frames);
        Stream->seek(secFramPos);
        int secFrames; Stream->Read(&secFrames);

        Stream->seek(animIndexPos);
        int animIndex; Stream->Read(&animIndex);
        Stream->seek(secIndexPos);
        int sectionIndex; Stream->Read(&sectionIndex);

        if (sectionIndex > 0)
        {
            int num = (frames / secFrames) + 2;
            for (int j = 0; j < num; j++)
            {
                int bytesAddedPer = 0;
                Stream->seek(startPos + sectionIndex + 8 * j);
                int animBlock; Stream->Read(&animBlock);
                int animOffset; Stream->Read(&animOffset);
                Stream->seek(startPos + animOffset);
                secBytesAddedPerSec.push_back(bytesAdded);
                for (int k = 0; k < 10000000; k++)
                {
                    int pos2 = Stream->Position();
                    std::byte bone; Stream->Read(&bone); //Logger::Info("Bone: %d, Pos:  %d\n", bone, Stream->Position() - 1);
                    std::byte flag; Stream->Read(&flag); //Logger::Info("Flag: %d, Pos:  %d\n", flag, Stream->Position() - 1);
                    short nextOffset; Stream->Read(&nextOffset); //Logger::Info("Next Offset: %d, Pos:  %d\n", nextOffset, Stream->Position() - 2);
                    int headerSize = GetAnimHeaderSize((int)flag);
                    bytesAddedPer += (32 - headerSize);
                    bytesAdded += (32 - headerSize);
                    if (nextOffset == 0)
                    {
                        Stream->seek(pos2 + headerSize);
                        //if (debug) Logger::Info("SecNum: %d, secBytesAddedTtl:  %d\n", secNum, bytesAdded);
                        //if (debug) Logger::Info("SecNum: %d, secBytesAddePerd:  %d\n", secNum, bytesAddedPer);
                        break;
                    }
                    Stream->seek(pos2 + nextOffset);
                }
                secNum++;
            }
        }
    }
    return secBytesAddedPerSec;
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

v53_Header* ConvertHeader(BinaryReader* Stream, BinaryWriter* OutStream, v53_Header* Dest_Header, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, int animBytes)
{

    int bytesAddedToHeader = 52;
    int bytesAddedToBones = Initial_Header->numbones * 28;
    int bytesAddedToAnims = -8 * Initial_Header->numlocalanim;
    int bytesAddedToAnimData = Initial_Header->numlocalanim > 0 ? animBytes : 0;
    int bytesAddedToSeqs = 20 * Initial_Header->numlocalseq;
    int bytesAddedToTextures = -20 * Initial_Header->numtextures;
    int bytesAddedToIkChains = 16 * Initial_Header->numikchains;
    int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData;

    Dest_Header->id = Initial_Header->id;
    Dest_Header->version = Initial_Header->version;
    Dest_Header->checksum = Initial_Header->checksum;
    Dest_Header->nameCopyOffset = Initial_Header_Part2->sznameindex + allBytesAdded + 408;
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
    Dest_Header->textureindex = Initial_Header->textureindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData;

    Dest_Header->numcdtextures = Initial_Header->numcdtextures;
    Dest_Header->cdtextureindex = Initial_Header->cdtextureindex + allBytesAdded;

    Dest_Header->numskinref = Initial_Header->numskinref;
    Dest_Header->numskinfamilies = Initial_Header->numskinfamilies;
    Dest_Header->skinindex = Initial_Header->skinindex + allBytesAdded;

    Dest_Header->numbodyparts = Initial_Header->numbodyparts;
    Dest_Header->bodypartindex = Initial_Header->bodypartindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;

    Dest_Header->numlocalattachments = Initial_Header->numlocalattachments;
    Dest_Header->localattachmentindex = Initial_Header->localattachmentindex + bytesAddedToHeader + bytesAddedToBones;
    Dest_Header->numlocalnodes = Initial_Header->numlocalnodes;
    Dest_Header->localnodeindex = Initial_Header->localnodeindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->localnodenameindex = Initial_Header->localnodenameindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->numflexdesc = Initial_Header->numflexdesc;
    Dest_Header->flexdescindex = Initial_Header->flexdescindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->numflexcontrollers = Initial_Header->numflexcontrollers;
    Dest_Header->flexcontrollerindex = Initial_Header->flexcontrollerindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->numflexrules = Initial_Header->numflexrules;
    Dest_Header->flexruleindex = Initial_Header->flexruleindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->numikchains = Initial_Header->numikchains;
    Dest_Header->ikchainindex = Initial_Header->ikchainindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->nummouths = Initial_Header->nummouths;
    Dest_Header->mouthindex = Initial_Header->mouthindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData;
    Dest_Header->numlocalposeparameters = Initial_Header->numlocalposeparameters;
    Dest_Header->localposeparamindex = Initial_Header->localposeparamindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData;
    Dest_Header->surfacepropindex = Initial_Header->surfacepropindex + allBytesAdded;
    Dest_Header->keyvalueindex = Initial_Header->keyvalueindex + allBytesAdded;
    Dest_Header->keyvaluesize = Initial_Header->keyvaluesize;
    Dest_Header->numlocalikautoplaylocks = Initial_Header->numlocalikautoplaylocks;
    Dest_Header->localikautoplaylockindex = Initial_Header->localikautoplaylockindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
    Dest_Header->mass = Initial_Header->mass;
    Dest_Header->contents = Initial_Header->contents;
    Dest_Header->numincludemodels = Initial_Header->numincludemodels;
    Dest_Header->includemodelindex = Initial_Header->includemodelindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToIkChains + bytesAddedToAnimData;
    Dest_Header->virtualModel = Initial_Header->virtualModel;
    Dest_Header->bonetablebynameindex = Initial_Header->bonetablebynameindex + bytesAddedToHeader + bytesAddedToBones;
    Dest_Header->constdirectionallightdot = Initial_Header->constdirectionallightdot;
    Dest_Header->rootLOD = Initial_Header->rootLOD;
    Dest_Header->numAllowedRootLODs = Initial_Header->numAllowedRootLODs;
    Dest_Header->unused[0] = Initial_Header->unused[0];
    Dest_Header->unused4 = -1;
    Dest_Header->numflexcontrollerui = Initial_Header->numflexcontrollerui;
    Dest_Header->flexcontrolleruiindex = Initial_Header->flexcontrolleruiindex + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnimData;
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

v53_Header_Part2* ConvertSubHeader(BinaryReader* Stream, BinaryWriter* OutStream, v53_Header_Part2* Dest_Header_Part2, v53_Header* Dest_Header, v49_Header* Initial_Header, studiohdr2_t* Initial_Header_Part2, FileInfo info, int animBytes)
{
    int bytesAddedToHeader = 52;
    int bytesAddedToBones = Initial_Header->numbones * 28;
    int bytesAddedToAnims = -8 * Initial_Header->numlocalanim;
    int bytesAddedToAnimData = Initial_Header->numlocalanim > 0 ? animBytes : 0;
    int bytesAddedToSeqs = 20 * Initial_Header->numlocalseq;
    int bytesAddedToTextures = -20 * Initial_Header->numtextures;
    int bytesAddedToIkChains = 16 * Initial_Header->numikchains;
    int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData;
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
    Dest_Header_Part2->unkindex = 0;
    Dest_Header_Part2->numunk;
    Dest_Header_Part2->unkindex1 = Initial_Header->szanimblocknameindex;
    Dest_Header_Part2->vtxindex = vtxSize > 0 ? mdlSize + allBytesAdded + phySize + phyBSize : 0;
    Dest_Header_Part2->vvdindex = vvdSize > 0 ? mdlSize + allBytesAdded + phySize + vtxSize + phyBSize : 0;
    Dest_Header_Part2->vvcindex = 0;
    Dest_Header_Part2->vphyindex = phySize > 0 ? mdlSize + allBytesAdded : 0;
    Dest_Header_Part2->vtxsize = vtxSize;
    Dest_Header_Part2->vvdsize = vvdSize;
    Dest_Header_Part2->vvcsize = 0;
    Dest_Header_Part2->vphysize = phySize;
    Dest_Header_Part2->phybindex = mdlSize + allBytesAdded + phySize;
    Dest_Header_Part2->numofphyb = phyBSize > 0 ? 1 : 0;
    Dest_Header_Part2->unk = 0;
    Dest_Header_Part2->unkindex3 = 0;




    OutStream->Write(Initial_Header_Part2->numsrcbonetransform);
    OutStream->Write(Dest_Header_Part2->srcbonetransformindex);
    OutStream->Write(Dest_Header_Part2->illumpositionattachmentindex);
    OutStream->Write(Dest_Header_Part2->linearboneindex);
    OutStream->Write(Initial_Header_Part2->m_nBoneFlexDriverCount);
    OutStream->Write(Initial_Header_Part2->m_nBoneFlexDriverIndex);
    OutStream->Write(Dest_Header_Part2->unkindex);
    AddInt32(OutStream, 0, 3);
    OutStream->Write(Initial_Header->szanimblocknameindex + allBytesAdded);
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
  //BoneHeaderData* boneHeaderData = animData->boneHeaderData;
  SequenceData* sequenceData = GetSequenceData(&Stream, Initial_Header, false);

  Stream.seek(Initial_Header->localanimindex);

  int animByteAddedTotal = GetAnimBytesAdded(&Stream, Initial_Header, false);
  int animSecByteAddedTotal = GetAnimSectionBytesAdded(&Stream, Initial_Header, false);

  std::vector<int> hdrBytesAnimDescAdd = GetAnimBoneHeaderBytesAdded(&Stream, Initial_Header, true);
  std::vector<int> secHdrBytesAnimDescAdd = GetAnimSectionBytesAdded2(&Stream, Initial_Header, true);
  std::vector<int> secHdrBytesSecAdd = GetAnimSectionBoneHeaderBytesAddedPerSec(&Stream, Initial_Header, true);
  std::vector<int> numOfBoneHdrsPerAnim = GetAnimBoneHeaderCount(&Stream, Initial_Header, true);
  std::vector<int> numOfSecBoneHdrsPerAnim = GetAnimSectionBoneHeaderCount(&Stream, Initial_Header, true);
  std::vector<int> numOfSecBoneHdrsPerSec = GetAnimSectionBoneHeaderCountPerSec(&Stream, Initial_Header, true);

  int animBytesAdded = animByteAddedTotal + animSecByteAddedTotal;
  //for (int i = 0; i < Initial_Header->numlocalanim; i++)
  //{
  //    animBytesAdded += hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];
  //}
  Logger::Info("stairs: %d %d\n", animSecByteAddedTotal);

  Dest_Header = ConvertHeader(&Stream, &OutStream, Dest_Header, Initial_Header, Initial_Header_Part2, animByteAddedTotal + animSecByteAddedTotal);
  Dest_Header_Part2 = ConvertSubHeader(&Stream, &OutStream, Dest_Header_Part2, Dest_Header, Initial_Header, Initial_Header_Part2, info, animByteAddedTotal + animSecByteAddedTotal);

  int bytesAddedToHeader = 52;
  int bytesAddedToBones = Initial_Header->numbones * 28;
  int bytesAddedToAnims = -8 * Initial_Header->numlocalanim;
  int bytesAddedToAnimData = Initial_Header->numlocalanim > 0 ? animByteAddedTotal + animSecByteAddedTotal : 0;
  int bytesAddedToSeqs = 20 * Initial_Header->numlocalseq;
  int bytesAddedToTextures = -20 * Initial_Header->numtextures;
  int bytesAddedToIkChains = 16 * Initial_Header->numikchains;
  int bytesAddedToAnimSections;
  int allBytesAdded = bytesAddedToHeader + bytesAddedToBones + bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData;
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
          LinearBoneData* linearBoneData = GetLinearBoneData(&Stream, Initial_Header, Initial_Header_Part2, i);
          int stairs = bytesAddedToAnims + bytesAddedToSeqs + (28 * bonecount) - (28 * i) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData;
          mstudiobonev53_t bone = boneData->bones[i];
          OutStream.Write(bone.sznameindex + stairs);
          OutStream.Write(bone.parent);
          AddInt32(&OutStream, -1, 6);
          OutStream.Write(bone.pos);
          OutStream.Write(bone.quat);
          OutStream.Write(bone.rot);
          OutStream.Write(bone.posscale);
          OutStream.Write(bone.unkvector);
          
          OutStream.Write(bone.rotscale);
          SetInt32(&OutStream, 973078528, 3);
          OutStream.Write(bone.poseToBone);
          OutStream.Write(bone.qAlignment.one);
          OutStream.Write(bone.qAlignment.i);
          OutStream.Write(bone.qAlignment.j);
          OutStream.Write(bone.qAlignment.k);
          OutStream.Write(bone.flags);
          OutStream.Write(bone.proctype);
          bone.procindex > 0 ? OutStream.Write(bone.procindex + stairs) : OutStream.Write(bone.procindex);
          OutStream.Write(bone.physicsbone);
          bone.surfacepropidx > 0 ? OutStream.Write(bone.surfacepropidx + stairs) : OutStream.Write(bone.surfacepropidx);
          OutStream.Write(bone.contents);
          OutStream.Write((int)0);
          OutStream.Write(bone.unkindex);
          AddInt32(&OutStream, 0, 7);

          UI::Progress.SubTask.Update((i + 1.0f) / (float)bonecount);
      }
  }
  delete boneData;
  UI::Progress.SubTask.End();
  int bone_filler_dest = Initial_Header->localattachmentindex - ( Initial_Header->boneindex + Initial_Header->numbones * 216 );
  filler(&Stream, &OutStream, bone_filler_dest);

  UI::Progress.SubTask.Begin("Converting Attachments");
  for (int i = 0; i < (info.disable_attachments ? 0 : Dest_Header->numlocalattachments); i++) {
      mstudioattachment_t attachment = attachmentData->attachments[i];
      int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData;
      OutStream.Write(attachment.sznameindex + stairs);
      OutStream.Write(attachment.flags);
      OutStream.Write(attachment.localbone);
      OutStream.Write(attachment.localmatrix);
      AddInt32(&OutStream, 0, 8);
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
          Stream.Stream.read(pog, 44);
          OutStream.Stream.write(pog, 44);
      }
      else {
          v53_HitboxSet hitboxset = hitboxData->hitboxSets[i];
          int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData;
          OutStream.Write(hitboxset.sznameindex + stairs);
          OutStream.Write(hitboxset.numOfHitboxes);
          OutStream.Write(hitboxset.hitboxIdx);
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
          mstudiobboxv53_t hitbox = hitboxData->hitboxes[j];
          int stairs = bytesAddedToAnims + bytesAddedToSeqs + bytesAddedToIkChains + bytesAddedToTextures + bytesAddedToAnimData;
          OutStream.Write(hitbox.bone);
          OutStream.Write(hitbox.group);
          OutStream.Write(hitbox.bbmin);
          OutStream.Write(hitbox.bbmax);
          OutStream.Write(hitbox.szhitboxnameindex + stairs);
          OutStream.Write(hitbox.unk);
          OutStream.Write(hitbox.keyvalueindex);
          AddInt32(&OutStream, 0, 6);
          Logger::Notice("Updated hitboxs %d of %d\n", j + 1, numOfHitboxes);
      }
      UI::Progress.SubTask.End();
  }
  //int hbox_filler_dest = Initial_Header->bonetablebynameindex - Stream.Position();
  //filler(&Stream, &OutStream, hbox_filler_dest);

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

  for (int i = 0; i < Initial_Header->numlocalanim; i++) 
  {
      if (info.disable_animations) {
          char ßtruct[100];
          Stream.Stream.read(ßtruct, 100);
          OutStream.Stream.write(ßtruct, 100);
      }
      else {
          mstudioanimdescv53_t animDesc = animData->animDescs[i];

          int PISS = (-((8 * (Initial_Header->numlocalanim - i))) + bytesAddedToSeqs) + bytesAddedToTextures + bytesAddedToIkChains + bytesAddedToAnimData;
          int PISS2 = -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];//Contains(animsWithSections, i, Initial_Header->numlocalanim) ? -((8 * (Initial_Header->numlocalanim - i))) + animHdrBytesAdd[i] + boneHeaderData->secHdrBytesAnimDescAdd[i] : -((8 * (Initial_Header->numlocalanim - i))) + animHdrBytesAdd[i]; //boneHeaderData->secHdrBytesAnimDescAdd[i];
          Logger::Info("animSecBytesAdd: %d %d\n", secHdrBytesAnimDescAdd[i]);
          Logger::Info("animBytesAdd: %d %d\n", hdrBytesAnimDescAdd[i]);
          Logger::Info("stairs: %d %d\n", -((8 * (Initial_Header->numlocalanim - i))));
          int outPos = Dest_Header->localanimindex + 92 * i;
          OutStream.Write(-outPos);
          OutStream.Write(animDesc.sznameindex + PISS);
          OutStream.Write((float)animDesc.fps);
          OutStream.Write(animDesc.flags);
          OutStream.Write(animDesc.numframes);
          OutStream.Write(animDesc.nummovements);
          OutStream.Write(animDesc.movementindex);
          OutStream.Write(animDesc.animblock);
          OutStream.Write(animDesc.animindex + -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i]);// +4 * animData->secPerAnim[i]);//animDesc.sectionindex > 0 ? OutStream.Write(animDesc.animindex + PISS2) : OutStream.Write(animDesc.animindex + PISS2 + 4 * animData->secPerAnim[i]);
          OutStream.Write(animDesc.numikrules);
          OutStream.Write(animDesc.ikruleindex);
          OutStream.Write(animDesc.numlocalhierarchy);
          OutStream.Write(animDesc.localhierarchyindex);
          animDesc.sectionindex > 0 ? OutStream.Write(animDesc.sectionindex + -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i]) : OutStream.Write(animDesc.sectionindex);
          OutStream.Write(animDesc.sectionframes);
          OutStream.Write(animDesc.zeroframespan);
          OutStream.Write(animDesc.zeroframecount);
          OutStream.Write(animDesc.zeroframeindex);
          OutStream.Write(animDesc.zeroframestalltime);
          AddInt32(&OutStream, 0, 5);
          
      }
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
          int PISS2 = -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesAnimDescAdd[i];
          int PISS3 = -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i];
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
          //hdrBytesAddedToAnimDesc[i] = bytesAdded;
          //Logger::Info("AnimNum: %d,BytesAdded: %d\n", i, bytesAdded);
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

                  Stream.seek(hdrStartPos);
                  ReadBoneHeader(&Stream, boneHeaders, Initial_Header, Initial_Header_Part2, boneHdrNum, dataSize, false);
                  int pos = OutStream.Position();
                  BoneHeaderv53 boneHeader = boneHeaders[boneHdrNum];
                  OutStream.Write((float)boneHeader.translationScale);
                  OutStream.Write(boneHeader.bone);
                  OutStream.Write((std::byte)GetR2Flag((int)boneHeader.r2Flag));
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
                     if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
                     if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex == 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].animindex - boneHeader.rawStrPos);
                     //if (i + 1 > Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0) filler(&Stream, &OutStream, strStartPos + 100 + boneHeaderData->secOffsets[secIdxNum] - boneHeader.rawStrPos);
                     if (i + 1 > Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex == 0) filler(&Stream, &OutStream, Initial_Header->localseqindex - boneHeader.rawStrPos);
                      break;
                  }
                  //OutStream.seek(pos + boneHeader.nextOffset);
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
                 //Logger::Info("SecOffset: %d\n", animOffset);
                 Logger::Info("BytesAddedToSec: %d\n", secHdrBytesSecAdd[secIdxNum]);
                 Logger::Info("SecOffsetAfter: %d\n", startPos + animOffset + PISS3 + secHdrBytesSecAdd[secIdxNum]);
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


                 Logger::Info("SecOutPos: %d\n", startPos + animOffset + -((8 * (Initial_Header->numlocalanim - i))) + hdrBytesAnimDescAdd[i] + secHdrBytesSecAdd[secNumber]);
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
                     OutStream.Write((std::byte)GetR2Flag((int)secboneHeader.r2Flag));
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

                         if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].sectionindex - secboneHeader.rawStrPos);  //Logger::Info("Distance from sectionIdx: %d\n", strStartPos + 100 + animData->animDescs[i + 1].sectionindex - boneHeader.rawStrPos);
                         if (i + 1 < Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex == 0) filler(&Stream, &OutStream, strStartPos + 100 + animData->animDescs[i + 1].animindex - secboneHeader.rawStrPos);
                         if (j + 1 < num && i + 1 > Initial_Header->numlocalanim && animData->animDescs[i + 1].sectionindex > 0) filler(&Stream, &OutStream, strStartPos + nextAnimOffset - secboneHeader.rawStrPos);
                         if (j + 1 > num && i + 1 > Initial_Header->numlocalanim ) filler(&Stream, &OutStream, Initial_Header->localseqindex - secboneHeader.rawStrPos);
                         break;
                     }
                     //Stream.seek(hdrStartPos + headerSize);
                     //OutStream.seek(pos + secboneHeader.nextOffset);
                     filler(&Stream, &OutStream, dataSize);

                     Stream.seek(hdrStartPos + nextOffset);
                 }
                 secNumber++;
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

  for (int i = 0; i < Initial_Header->numlocalseq; i++) {
      if (info.disable_sequences) {
          char sstruct[212];
          Stream.Stream.read(sstruct, 212);
          OutStream.Stream.write(sstruct, 212);
      }
      else {
          int stairs = 20 * (Initial_Header->numlocalseq - i);
          int updateOffset = bytesAddedToTextures + bytesAddedToIkChains + stairs;
          mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];

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

          sequence.weightlistindex > 0 ? OutStream.Write(sequence.weightlistindex + stairs) : OutStream.Write(sequence.weightlistindex);

          sequence.posekeyindex > 0 ? OutStream.Write(sequence.posekeyindex + stairs) : OutStream.Write(sequence.posekeyindex);

          OutStream.Write(sequence.numiklocks);
          sequence.iklockindex > 0 ?OutStream.Write(sequence.iklockindex + stairs) : OutStream.Write(sequence.iklockindex);


          sequence.keyvalueindex > 0 ? OutStream.Write(sequence.keyvalueindex + stairs) : OutStream.Write(sequence.keyvalueindex);
          OutStream.Write(sequence.keyvaluesize);

          sequence.cycleposeindex > 0 ? OutStream.Write(sequence.cycleposeindex + stairs) : OutStream.Write(sequence.cycleposeindex); // index of pose parameter to use as cycle index

          sequence.activitymodifierindex > 0 ?OutStream.Write(sequence.activitymodifierindex + stairs) : OutStream.Write(sequence.activitymodifierindex);
          OutStream.Write(sequence.numactivitymodifiers);
          
          AddInt32(&OutStream, 0, 10);

          //int PISS = (20 * (Initial_Header->numlocalseq - i));
          //int PISS2 = (20 * Initial_Header->numlocalseq ) - (20 * i) + bytesAddedToTextures + bytesAddedToIkChains;
          //int seq_start = OutStream.Position();
          //Logger::Info("Sequence %i has %i events\n", i + 1, PISS2);
          //
          //int basePtr; Stream.Read(&basePtr); OutStream.Write(-OutStream.Position());//CopyAddInt32(&Stream, &OutStream, 0, 1); //baseptr
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS2, 1); //szlabelindex
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS2, 1); //szactivitynameindex
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //flags
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //activity
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //activityWeight
          //
          //int eventcount; Stream.Read(&eventcount); OutStream.Write(eventcount);
          //Logger::Info("Sequence %i has %i events\n", i + 1, eventcount);
          ////Logger::Info("PISS: %i, PISS2: %i\n", PISS, PISS2);
          //int eventindex; Stream.Read(&eventindex); OutStream.Write(eventindex + PISS); //Logger::Info("Event Index: %i %i\n", eventindex + PISS);
          //int eventindex_real = seq_start + eventindex + PISS;
          //events_change.push_back(std::pair<int, int>(eventindex, eventcount));
          //
          //CopyAddInt32(&Stream, &OutStream, 0, 3);   //bbmin
          //CopyAddInt32(&Stream, &OutStream, 0, 3);   //bbmax
          //CopyAddInt32(&Stream, &OutStream, 0, 1);   //numblends
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //animindexindex
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //movementindex
          //CopyAddInt32(&Stream, &OutStream, 0, 2); //groupsize
          //CopyAddInt32NullCheck(&Stream, &OutStream, 0, 2); //paramindex
          //CopyAddInt32(&Stream, &OutStream, 0, 2); //paramstart
          //CopyAddInt32(&Stream, &OutStream, 0, 2); //paramend
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //paramparent
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //fadeintime
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //fadeouttime
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //localentrynode
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //localexitnode
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //nodeflags
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //entryphase
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //exitphase
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //lastframe
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //nextseq
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //pose
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //numikrules
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //numautolayers
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //autolayerindex
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //weightlistindex
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //posekeyindex
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //pPoseKey
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //PoseKey
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //numiklocks
          //CopyAddInt32(&Stream, &OutStream, 0, 1); ///keyvaluesize/iklockindex
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //cycleposeindex//keyvalueindex
          //CopyAddInt32NullCheck(&Stream, &OutStream, PISS, 1); //keyvaluesize
          //CopyAddInt32(&Stream, &OutStream, 0, 1); //cycleposeindex
          //CopyAddInt32(&Stream, &OutStream, 0, 1);//int activityModifier; Stream.Read(&activityModifier); OutStream.Write(activityModifier);
          //CopyAddInt32(&Stream, &OutStream, 0, 1);//int activityModifierCount; Stream.Read(&activityModifierCount); OutStream.Write(activityModifierCount);
          ////CopyAddInt32(&Stream, &OutStream, 0, 5);
          ////AddInt32(&OutStream, 0, 10);
          ////Stream.seek(Stream.Position() + 40);
          ////filler(&Stream, &OutStream, 4);
          ////Logger::Info("Sequence OutPos: %i Sequence StrPos: %i\n",OutStream.Position(), Stream.Position());
          //int unused[5];
          //Stream.read((char*)&unused, 12);
          //////OutStream.Write(0); //activityModifierOffset
          //////OutStream.Write(0); //activityModifierCount
          //OutStream.write((char*)&unused, 12);
          //AddInt32(&OutStream, 0, 5);
          ////OutStream.Write(0);
          ////OutStream.Write(0);
          ////OutStream.Write(0);
          ////int return_idx = OutStream.Position();
      }
      Logger::Notice("Converted sequence %d of %d\n", i + 1, Initial_Header->numlocalseq);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalseq);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished sequences\n");
  int numOfModels = 0;
  int numOfMeshes = 0;
  int numOfIkLinks = 0;
  int meshStart = 0;

  if (Initial_Header->numbodyparts > 0)
  {
      Stream.seek(Initial_Header->localseqindex + 212 * Initial_Header->numlocalseq);
      OutStream.seek(Dest_Header->localseqindex + 232 * Initial_Header->numlocalseq);
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
              int idx;
              int startPos = Stream.Position();
              Stream.Read(&idx);
              int off = (idx)+bytesAddedToTextures + bytesAddedToIkChains;
              OutStream.Write(off);
              int numberOfModels; Stream.Read(&numberOfModels); numOfModels += numberOfModels; OutStream.Write(numberOfModels);
              int base; Stream.Read(&base); OutStream.Write(base);
              int modelIndex; Stream.Read(&modelIndex); OutStream.Write(modelIndex);

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
          int modelStart = Stream.Position();
          filler(&Stream, &OutStream, 64);
          CopyAddInt32(&Stream, &OutStream, 0, 1); //type
          CopyAddInt32(&Stream, &OutStream, 0, 1); //bounding radius
          int numberOfMeshes; Stream.Read(&numberOfMeshes); numOfMeshes += numberOfMeshes; OutStream.Write(numberOfMeshes);
          int meshIndex; Stream.Read(&meshIndex); OutStream.Write(meshIndex + bytesAddedToIkChains); //meshIndex
          CopyAddInt32(&Stream, &OutStream, 0, 1); //num vertices
          CopyAddInt32NullCheck(&Stream, &OutStream, 0, 1); //vertex index
          CopyAddInt32NullCheck(&Stream, &OutStream, 0, 1); //tangents index
          CopyAddInt32(&Stream, &OutStream, 0, 1); //number of attachments
          CopyAddInt32NullCheck(&Stream, &OutStream, bytesAddedToIkChains, 1); //attachmentindex
          CopyAddInt32(&Stream, &OutStream, 0, 1); //numeyeballs
          CopyAddInt32NullCheck(&Stream, &OutStream, bytesAddedToIkChains, 1); //eyeballindex
          CopyAddInt32(&Stream, &OutStream, 0, 4);
          CopyAddInt32(&Stream, &OutStream, 0, 1);
          CopyAddInt32(&Stream, &OutStream, 0, 1);
          CopyAddInt32(&Stream, &OutStream, 0, 4);
          if (i == 1) meshStart = modelStart + meshIndex;
          Logger::Notice("Updated model struct %d of %d\n", i + 1, numOfModels);
      }
      UI::Progress.SubTask.End();
      Logger::Info("Finished model structs\n");
  }

  if (Initial_Header->numikchains > 0)
  {
      UI::Progress.SubTask.Begin("Converting IkChains");
      int mesh_filler_dest = Initial_Header->ikchainindex - Stream.Position();
      filler(&Stream, &OutStream, mesh_filler_dest);
      for (int i = 0; i < Initial_Header->numikchains; i++)
      {
          int offset = bytesAddedToTextures + (16 * Initial_Header->numikchains) - (16 * i);
          int offset2 = (16 * Initial_Header->numikchains) - (16 * i);

          CopyAddInt32(&Stream, &OutStream, offset, 1); //nameIndex
          CopyAddInt32(&Stream, &OutStream, 0, 1);      //linkType
          int numberOfIkLinks; Stream.Read(&numberOfIkLinks); numOfIkLinks += numberOfIkLinks; OutStream.Write(numberOfIkLinks); //numOfIkLinks
          CopyAddInt32(&Stream, &OutStream, offset2, 1); //linkIndex

          AddInt32(&OutStream, 0, 3);                   //kneeDir

          AddInt32(&OutStream, 0, 1);                   //unk
          Logger::Notice("Converting IkChain %d of %d\n", i + 1, Initial_Header->numikchains);
      }
      UI::Progress.SubTask.End();
      Logger::Info("Finished Converting IkChains\n");
  }
  
  if (numOfMeshes > 0)
  {
      int mesh_filler_dest = meshStart - Stream.Position();
      UI::Progress.SubTask.Begin("Updating Model Structs");
      filler(&Stream, &OutStream, mesh_filler_dest);
      UI::Progress.SubTask.Begin("Updating Mesh Structs");
      for (int i = 0; i < numOfMeshes; i++)
      {
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //material
          CopyAddInt32(&Stream, &OutStream, -bytesAddedToIkChains, 1);  //modelIndex
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //numOfVertices
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //vertexOffset
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //numOfFlexes
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //flexIndex
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //materialType
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //materialParam
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //meshId
          CopyAddInt32(&Stream, &OutStream, 0, 3);  //center
          CopyAddInt32(&Stream, &OutStream, 0, 1);  //unk
          CopyAddInt32(&Stream, &OutStream, 0, 8);  //numOfLodVertexes
          CopyAddInt32(&Stream, &OutStream, 0, 8);  //unused
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
          CopyAddInt32(&Stream, &OutStream, bytesAddedToTextures, 1);                           //nameIndex
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
          CopyAddInt32(&Stream, &OutStream, bytesAddedToTextures, 1);  //material
      }

      Logger::Notice("Converted included model %d of %d\n", i + 1, Initial_Header->numincludemodels);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numincludemodels);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished included models\n");

#pragma region texture conversion
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
          CopyAddInt32(&Stream, &OutStream, -((20) * (Initial_Header->numtextures - i)), 1);
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
          CopyAddInt32(&Stream, &OutStream, allBytesAdded, 1);
      }
      Logger::Notice("Converted texture %d of %d\n", i + 1, Initial_Header->numcdtextures);
      UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numcdtextures);
  }
  UI::Progress.SubTask.End();
  Logger::Info("Finished cd textures\n");

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
  //int _events_change_size = events_change.size();
  //if (events_change.size() > 0)
  //{
  //        for (int i = 0; i < _events_change_size; i++)
  //        {
  //            int PISS = (20 * Initial_Header->numlocalseq) - (20 * i);
  //            for (int j = 0; j < events_change[i].second; j++)
  //            {
  //                int evntIdxStr = Initial_Header->localseqindex + (212 * i) + events_change[i].first + 76 + (80 * j);
  //                Stream.seek(evntIdxStr);
  //                int evntNameIdx; Stream.Read(&evntNameIdx);
  //                int outPos = ( Dest_Header->localseqindex + (232 * i) ) + (events_change[i].first + PISS) + 76 + (80 * j);
  //                OutStream.seek(outPos);
  //                OutStream.Write(evntNameIdx + bytesAddedToIkChains + bytesAddedToTextures);
  //            }
  //        }
  //}
  if (sequenceData->numOfEvents > 0)
  {
      for (int i = 0; i < Initial_Header->numlocalseq; i++)
      {
          mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];
          int PISS = 20 * (Initial_Header->numlocalseq - i);
          int startOutPos = Dest_Header->localseqindex + 232 * i;
          int startStrPos = Initial_Header->localseqindex + 212 * i;
          Stream.seek(startStrPos + sequence.eventindex);
          OutStream.seek(startOutPos + sequence.eventindex + PISS);
  
          for (int j = 0; j < sequence.numevents; j++)
          {
              Stream.seek(startStrPos + sequence.eventindex + 76 + (80 * j));
              OutStream.seek(startOutPos + sequence.eventindex + PISS + 76 + (80 * j));
              int idx; Stream.Read(&idx); OutStream.Write(idx + bytesAddedToTextures + bytesAddedToIkChains);
              //CopyAddInt32(&Stream, &OutStream, bytesAddedToTextures + bytesAddedToIkChains, 1);
          }
      }
  }
  
  if (sequenceData->numOfActMods > 0)
  {
      for (int i = 0; i < Initial_Header->numlocalseq; i++)
      {
          mstudioseqdesc_t_v53 sequence = sequenceData->sequences[i];
          int PISS = (20 * Initial_Header->numlocalseq) - (20 * i);
          int startOutPos = Dest_Header->localseqindex + 232 * i;
          int startStrPos = Initial_Header->localseqindex + 212 * i;
          Stream.seek(startStrPos + sequenceData->actModIdxs[i]);
          OutStream.seek(startOutPos + sequenceData->actModIdxs[i] + PISS);
  
          for (int j = 0; j < sequenceData->numOfActModsPerSeq[i]; j++)
          {
              Stream.seek(startStrPos + sequenceData->actModIdxs[i] + 4 * j);
              OutStream.seek(OutStream.Position() + 4 * j );
              int idx; Stream.Read(&idx); OutStream.Write(idx + bytesAddedToTextures + bytesAddedToIkChains);
          }
      }
  }
  
  if (Initial_Header->numlocalnodes > 0)
  {
      UI::Progress.SubTask.Begin("Updating Local Node Names");
      for (int i = 0; i < Initial_Header->numlocalnodes; i++)
      {
          Stream.seek(Initial_Header->localnodenameindex);
          OutStream.seek(Dest_Header->localnodenameindex);
          CopyAddInt32(&Stream, &OutStream, bytesAddedToIkChains + bytesAddedToTextures, 1);
  
  
          Logger::Notice("Converted body part %d of %d\n", i + 1, Initial_Header->numlocalnodes);
          UI::Progress.SubTask.Update((i + 1.0f) / (float)Initial_Header->numlocalnodes);
      }
  }


  OutStream.Stream.close();
  Stream.Stream.close();

  Logger::Info("Finished!\n");
  UI::Progress.MainTask.End();
  return 0;
}
#pragma clang diagnostic pop