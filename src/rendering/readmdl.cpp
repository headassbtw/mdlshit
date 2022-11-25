#include <rendering/render.hpp>
#include <imgui.h>
#include <rendering/filewidget.hpp>
#include <binarystream.hpp>
#include <mdls.hpp>

Widgets::File* _mdl;
std::optional<MDL::v49Mdl> _opt_v49;
std::optional<MDL::v53Mdl> _opt_v53;
int extract_dropdown;

void UI::SetupReadMdlWindow(){
    _mdl = new Widgets::File("MDL", false, "*.mdl");
}


void UI::SetReadMdlFile(const char* path){
    strcpy(_mdl->BoxBuffer, path);
}

//std::vector<const char*> GetExtractOptions(int version)
//{
//
//    std::vector<const char*> extract_options;
//
//    switch (version)
//    {
//        case 49:
//            return extract_options;
//            break;
//
//        case 52:
//            extract_options.push_back("Src Bone Transforms");
//            extract_options.push_back("AABB Trees");
//            return extract_options;
//            break;
//
//        case 53:
//            extract_options.push_back("RUI Structs");
//            extract_options.push_back("Src Bone Transforms");
//            extract_options.push_back("AABB Trees");
//            return extract_options;
//            break;
//
//        default:
//            return extract_options;
//            break;
//    }
//}

const char** GetExtractOptions(int version)
{

    //const char* extract_options = nullptr;
    const char* extract_options[] = {"RUI Structs", "Src Bone Transforms", "AABB Trees"};
    switch (version)
    {
    case 49:
    {
        return extract_options;
    }
        break;

    case 52:
    {
        const char* extract_options2[] = { "Src Bone Transforms", "AABB Trees" };
        return extract_options2;
    }
        break;

    case 53:
    {
        const char* extract_options3[] = { "RUI Structs", "Src Bone Transforms", "AABB Trees" };
        return extract_options3;
    }
        break;

    default:
        break;
    }
    return extract_options;
}


void extract_rui(){
  //std::vector<Error> rtn;
  //  std::ifstream test(&mdl->BoxBuffer[0]);
  //  if (!test)
  //  {
  //    rtn.push_back({ ErrorType::Blocking,std::string("File does not exist") });
  //  }
  //  else
  //  {
  //
  //    char magic[4];
  //    test.read(magic, 4);
  //    if (strcmp(magic, "IDST") >= 0) {
  //      rtn.push_back({ ErrorType::Success,std::string("Valid MDL file") });
  //
  //
  //      char ver[1];
  //      test.read(ver, 1);
  //      if ((int)ver[0] == 49)
  //      {
  //        rtn.push_back({ ErrorType::Success,std::string("Version 49") });
  //        BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
  //        MDL::v49Mdl mdl = mdl._v49Mdl(&Stream, false);
  //      }
  //      else if ((int)ver[0] == 53) {
  //        rtn.push_back({ErrorType::Success, std::string("Version 53")});
  //        BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
  //        MDL::v53Mdl mdl = mdl._v53Mdl(&Stream, false);
  //        studiohdr_t_v53 mdlhdr = mdl.mdlhdr;
  //        std::vector<mstudiorruiheader_t>                    ruiHdrs;
  //        std::vector<mstudioruimesh_t>                        ruiMeshes;
  //        if (mdlhdr.numruimeshes > 0)
  //        {
  //          Stream.seek(mdlhdr.ruimeshindex);
  //          for (int i = 0; i < mdlhdr.numruimeshes; i++)
  //          {
  //            mstudiorruiheader_t ruiHdr; Stream.Read(&ruiHdr);
  //            ruiHdrs.push_back(ruiHdr);
  //            Logger::Info("RuiHdr Read: %d\n", i);
  //          }
  //          Stream.seek(mdlhdr.ruimeshindex + ruiHdrs[0].ruimeshindex);
  //          for (int j = 0; j < ruiHdrs.size(); j++)
  //          {
  //            int ruiPos = Stream.Position();
  //            mstudioruimesh_t ruiMesh; Stream.Read(&ruiMesh);
  //            ruiMeshes.push_back(ruiMesh);
  //          }
  //          int end = Stream.Position();
  //          BinaryWriter Bing = BinaryWriter("ass.bin");
  //          char data[end - mdlhdr.ruimeshindex];
  //          Stream.read(data,end - mdlhdr.ruimeshindex);
  //          Stream.seek(mdlhdr.ruimeshindex);
  //          Bing.write(data,end - mdlhdr.ruimeshindex);
  //          Logger::Info("Finished writing RUI structs\n");
  //          Bing.Stream.close();
  //        }
  //      }
  //    }
  //    else {
  //      std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
  //      rtn.push_back({ ErrorType::Blocking,err });
  //      Logger::Critical("%s\n", err.c_str());
  //    }
  //    test.close();
  //  }
}

void UI::RenderReadMdlWindow(int x, int y){
    _mdl->UI(x-8);
    FileInfo info;
    if(ImGui::Button("Read MDL"))
    {


        std::vector<Error> rtn;
        std::ifstream test(&_mdl->BoxBuffer[0]);
        if (!test)
        {
            rtn.push_back({ ErrorType::Blocking,std::string("File does not exist") });
        }
        else 
        {

            char magic[4];
            test.read(magic, 4);
            if (strcmp(magic, "IDST") >= 0) {
                rtn.push_back({ ErrorType::Success,std::string("Valid MDL file") });


                char ver[1];
                test.read(ver, 1);


                if ((int)ver[0] == 49)
                {
                    Logger::Debug("MDL Version: %d\n", (int)ver[0]);
                    rtn.push_back({ ErrorType::Success,std::string("Version 49") });
                    BinaryReader Stream = BinaryReader(&_mdl->BoxBuffer[0]);
                    _opt_v49 = _opt_v49->_v49Mdl(&Stream, false);
                }
                else if ((int)ver[0] == 53)
                {
                    Logger::Debug("MDL Version: %d\n", (int)ver[0]);
                    rtn.push_back({ ErrorType::Success,std::string("Version 53") });
                    BinaryReader Stream = BinaryReader(&_mdl->BoxBuffer[0]);
                    MDL::v53Mdl mdl = mdl._v53Mdl(&Stream, false);
                    _opt_v53 = mdl;

                    //mdl.v53ExtractAABBTree(&Stream);
                    //mdl.v53ExtractSrcBoneTransforms(&Stream);
                }
                else if ((int)ver[0] == 52)
                {
                    Logger::Debug("MDL Version: %d\n", (int)ver[0]);
                    rtn.push_back({ ErrorType::Success,std::string("Version 52") });
                    BinaryReader Stream = BinaryReader(&_mdl->BoxBuffer[0]);
                    MDL::v52Mdl mdl = mdl._v52Mdl(&Stream, false);
                    //mdl.v52ExtractAABBTree(&Stream);
                    //mdl.v52ExtractSrcBoneTransforms(&Stream);
                }
            }
            else {
                std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
                rtn.push_back({ ErrorType::Blocking,err });
                Logger::Critical("%s\n", err.c_str());
            }
            test.close();
        }
    }
 //ImGui::SameLine();
 //ImGui::Combo("##ExtractDropdown", &extract_dropdown, extract_options, IM_ARRAYSIZE(extract_options));
 //ImGui::SameLine();
 //if(ImGui::Button("Extract"))
 //{
 //  switch(extract_dropdown){
 //    case 0:
 //      extract_rui();
 //      break;
 //  }
 //}
  //ImGui::Separator();
  std::vector<Error> rtn;
  std::ifstream test(&_mdl->BoxBuffer[0]);
  if (!test)
  {
      rtn.push_back({ ErrorType::Blocking,std::string("File does not exist") });
  }
  else
  {

      char magic[4];
      test.read(magic, 4);
      if (strcmp(magic, "IDST") >= 0) {
          rtn.push_back({ ErrorType::Success,std::string("Valid MDL file") });


          char ver[1];
          test.read(ver, 1);
          //const char* extract_options[] = { GetExtractOptions((int)ver[0])[0],GetExtractOptions((int)ver[0])[1] };


          //ImGui::SameLine();
          //ImGui::Combo("##ExtractDropdown", &extract_dropdown, extract_options, IM_ARRAYSIZE(extract_options));
          //ImGui::SameLine();
          //if (ImGui::Button("Extract"))
          //{
          //    switch (extract_dropdown) {
          //    case 0:
          //    {
          //        extract_rui();
          //        break;
          //    }
          //    }
          //    ImGui::Separator();
          //}


          if ((int)ver[0] == 49)
          {
              //Logger::Debug("MDL Version: %d\n", (int)ver[0]);
              //rtn.push_back({ ErrorType::Success,std::string("Version 49") });
              //BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
              //                    _opt_v49 = _opt_v49->_v49Mdl(&Stream, false);
          }
          else if ((int)ver[0] == 53 && _opt_v53.has_value())
          {
              //Logger::Debug("MDL Version: %d\n", (int)ver[0]);
              //rtn.push_back({ ErrorType::Success,std::string("Version 53") });
              //BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
              const char* extract_options[] = {"RUI Structs", "Src Bone Transforms", "AABB Trees"};
              std::optional<MDL::v53Mdl> mdl = _opt_v53;

              ImGui::SameLine();
              ImGui::Combo("##ExtractDropdown", &extract_dropdown, extract_options, IM_ARRAYSIZE(extract_options));
              ImGui::SameLine();
              if (ImGui::Button("Extract"))
              {
                  switch (extract_dropdown) 
                  {
                    case 0:
                    {
                        mdl->v53ExtractRUIMesh(&BinaryReader(&_mdl->BoxBuffer[0]));
                        break;
                    }

                    case 1:
                    {
                        mdl->v53ExtractSrcBoneTransforms(&BinaryReader(&_mdl->BoxBuffer[0]));
                        break;
                    }

                    case 2:
                    {
                        mdl->v53ExtractAABBTree(&BinaryReader(&_mdl->BoxBuffer[0]));
                        break;
                    }
                  }
              }

                  int treeidx = 0;
                  ImGui::Text("Name: %s", mdl->stringtable.mdlname.c_str());
                  ImGui::Text("Version: %d", (int)ver[0]);
                  if (ImGui::TreeNode((void*)(intptr_t)0, "Bones (%zu)", mdl->bones.size()))
                  {
                      for (int i = 0; i < mdl->bones.size(); i++)
                      {

                          if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->stringtable.bones[i].c_str()))
                          {
                              ImGui::Text("Flags: %08x", mdl->bones[i].flags);
                              ImGui::Text("Pos: (%f , %f , %f)", mdl->bones[i].pos.x, mdl->bones[i].pos.y, mdl->bones[i].pos.z);
                              ImGui::Text("Rot: (%f , %f , %f)", mdl->bones[i].rot.x, mdl->bones[i].rot.y, mdl->bones[i].rot.z);

                              ImGui::TreePop();
                          }
                      }
                      ImGui::TreePop();
                  }

                  if (ImGui::TreeNode((void*)(intptr_t)1, "Attachments (%zu)", mdl->attachments.size()))
                  {
                      for (int i = 0; i < mdl->attachments.size(); i++)
                      {
                          if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->attachments[i].szname.c_str()))
                          {
                              ImGui::Text("Flags: %08x", mdl->attachments[i].flags);
                              ImGui::Text("LocalBone: %i", mdl->attachments[i].localbone);
                              ImGui::TreePop();
                          }
                      }
                      ImGui::TreePop();
                  }
                  if (ImGui::TreeNode((void*)(intptr_t)2, "AnimDescs (%zu)", mdl->animdescs.size()))
                  {
                      for (int i = 0; i < mdl->animdescs.size(); i++)
                      {
                          if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->animdescs[i].szname.c_str()))
                          {
                              ImGui::Text("FPS: %f", mdl->animdescs[i].fps);
                              ImGui::Text("Frames: %i", mdl->animdescs[i].numframes);
                              ImGui::Text("Animindex: %i", mdl->animdescs[i].animindex);
                              ImGui::TreePop();
                          }
                      }
                      ImGui::TreePop();
                  }

                  if (ImGui::TreeNode((void*)(intptr_t)3, "SeqDescs (%zu)", mdl->seqdescs.size()))
                  {
                      for (int i = 0; i < mdl->seqdescs.size(); i++)
                      {
                          if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->seqdescs[i].szlabel.c_str()))
                          {

                              //ImGui::Text("Nodeflags: %i", mdl->seqdescs[i].nodeflags);
                              //ImGui::Text("BBmax: (%f , %f , %f)", mdl->seqdescs[i].bbmax.x, mdl->seqdescs[i].bbmax.y, mdl->seqdescs[i].bbmax.z);
                              //ImGui::Text("BBmin: (%f , %f , %f)", mdl->seqdescs[i].bbmin.x, mdl->seqdescs[i].bbmin.y, mdl->seqdescs[i].bbmin.z);

                              if (mdl->seqdescs[i].numblends > 0 && ImGui::TreeNode((void*)(intptr_t)4, "Anims (%zu)", mdl->seqdescs[i].numblends))
                              {
                                  if (mdl->seqdescs[i].numblends > 0)
                                  {
                                      for (int j = 0; j < mdl->seqdescs[i].numblends; j++)
                                      {
                                          int animBlend = mdl->seqdescs[i].blends.blends[j];
                                          ImGui::Text("Anim: %s : %d", mdl->animdescs[animBlend].szname.c_str(), j);
                                      }
                                  }
                                  ImGui::TreePop();
                              }

                              if (mdl->seqdescs[i].numactivitymodifiers > 0 && ImGui::TreeNode((void*)(intptr_t)5, "ActMods (%zu)", mdl->seqdescs[i].numactivitymodifiers))
                              {
                                  if (mdl->seqdescs[i].szactivitymodifiernames.size() > 0)
                                  {
                                      for (int j = 0; j < mdl->seqdescs[i].szactivitymodifiernames.size(); j++)
                                      {
                                          ImGui::Text("ActMod: %s : %d", mdl->seqdescs[i].szactivitymodifiernames[j].c_str(), j);
                                      }
                                  }
                                  ImGui::TreePop();
                              }
                              ImGui::TreePop();
                          }
                      }
                      ImGui::TreePop();
                  }
                  //mdl->v53ExtractAABBTree(&Stream);
                  //mdl->v53ExtractSrcBoneTransforms(&Stream);
              }
              else if ((int)ver[0] == 52)
              {
                  //Logger::Debug("MDL Version: %d\n", (int)ver[0]);
                  //rtn.push_back({ ErrorType::Success,std::string("Version 52") });
                  //BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
                  //MDL::v52Mdl mdl = mdl._v52Mdl(&Stream, false);
                  //mdl.v52ExtractAABBTree(&Stream);
                  //mdl.v52ExtractSrcBoneTransforms(&Stream);
              }
          }
          else 
          {
              std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
              rtn.push_back({ ErrorType::Blocking,err });
              Logger::Critical("%s\n", err.c_str());
          }
          test.close();
          //ok actual details n shit go here
          //if(_opt_v49.has_value()){
          //  int treeidx = 0;
          //  ImGui::Text("Name: %s",_opt_v49->mdlhdr.name); 
          //  ImGui::Text("Version: 49");
          //  if (ImGui::TreeNode((void*)(intptr_t)0, "Bones (%zu)",_opt_v49->bones.size()))
          //  {
          //    for (int i = 0; i < _opt_v49->bones.size(); i++)
          //    {
          //      
          //      if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->stringtable.bones[i].c_str()))
          //      {
          //        ImGui::Text("Flags: %08x", _opt_v49->bones[i].flags);
          //        ImGui::Text("Pos: (%f , %f , %f)",_opt_v49->bones[i].pos.x,_opt_v49->bones[i].pos.y,_opt_v49->bones[i].pos.z);
          //        ImGui::Text("Rot: (%f , %f , %f)",_opt_v49->bones[i].rot.x,_opt_v49->bones[i].rot.y,_opt_v49->bones[i].rot.z);
          //        
          //        ImGui::TreePop();
          //      }
          //    }
          //    ImGui::TreePop();
          //  }
          //  
          //  if (ImGui::TreeNode((void*)(intptr_t)1, "Attachments (%zu)",_opt_v49->attachments.size()))
          //  {
          //    for (int i = 0; i < _opt_v49->attachments.size(); i++)
          //    {
          //      if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->attachments[i].szname.c_str()))
          //      {
          //        ImGui::Text("Flags: %08x", _opt_v49->attachments[i].flags);
          //        ImGui::Text("LocalBone: %i", _opt_v49->attachments[i].localbone);
          //        ImGui::TreePop();
          //      }
          //    }
          //    ImGui::TreePop();
          //  }
          //  if (ImGui::TreeNode((void*)(intptr_t)2, "AnimDescs (%zu)",_opt_v49->animdescs.size()))
          //  {
          //    for (int i = 0; i < _opt_v49->animdescs.size(); i++)
          //    {
          //      if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->animdescs[i].szname.c_str()))
          //      {
          //        ImGui::Text("FPS: %f", _opt_v49->animdescs[i].fps);
          //        ImGui::Text("Frames: %i", _opt_v49->animdescs[i].numframes);
          //        ImGui::Text("Animindex: %i", _opt_v49->animdescs[i].animindex);
          //        ImGui::TreePop();
          //      }
          //    }
          //    ImGui::TreePop();
          //  }
          //  if (ImGui::TreeNode((void*)(intptr_t)3, "SeqDescs (%zu)",_opt_v49->seqdescs.size()))
          //  {
          //    for (int i = 0; i < _opt_v49->seqdescs.size(); i++)
          //    {
          //      if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->seqdescs[i].szlabel.c_str()))
          //      {
          //        
          //        ImGui::Text("Nodeflags: %i", _opt_v49->seqdescs[i].nodeflags);
          //        ImGui::Text("BBmax: (%f , %f , %f)",_opt_v49->seqdescs[i].bbmax.x,_opt_v49->seqdescs[i].bbmax.y,_opt_v49->seqdescs[i].bbmax.z);
          //        ImGui::Text("BBmin: (%f , %f , %f)",_opt_v49->seqdescs[i].bbmin.x,_opt_v49->seqdescs[i].bbmin.y,_opt_v49->seqdescs[i].bbmin.z);
          //        ImGui::TreePop();
          //      }
          //    }
          //    ImGui::TreePop();
          //  }
          //  
          //}
  }
}