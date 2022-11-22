#include <rendering/render.hpp>
#include <imgui.h>
#include <rendering/filewidget.hpp>
#include <binarystream.hpp>
#include <mdls.hpp>

Widgets::File* mdl;
std::optional<MDL::v49Mdl> _opt_v49;
std::optional<MDL::v53Mdl> _opt_v53;
int extract_dropdown;
const char* extract_options[] = { "RUI Structs", "Src Bone Transforms", "AABB Trees"};

void UI::SetupReadMdlWindow(){
    mdl = new Widgets::File("MDL", false, "*.mdl");
}

void UI::SetReadMdlFile(const char* path){
    strcpy(mdl->BoxBuffer, path);
}


void extract_rui(){
  std::vector<Error> rtn;
    std::ifstream test(&mdl->BoxBuffer[0]);
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
          rtn.push_back({ ErrorType::Success,std::string("Version 49") });
          BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
          MDL::v49Mdl mdl = mdl._v49Mdl(&Stream, false);
        }
        else if ((int)ver[0] == 53) {
          rtn.push_back({ErrorType::Success, std::string("Version 53")});
          BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
          MDL::v53Mdl mdl = mdl._v53Mdl(&Stream, false);
          studiohdr_t_v53 mdlhdr = mdl.mdlhdr;
          std::vector<mstudiorruiheader_t>                    ruiHdrs;
          std::vector<mstudioruimesh_t>                        ruiMeshes;
          if (mdlhdr.numruimeshes > 0)
          {
            Stream.seek(mdlhdr.ruimeshindex);
            for (int i = 0; i < mdlhdr.numruimeshes; i++)
            {
              mstudiorruiheader_t ruiHdr; Stream.Read(&ruiHdr);
              ruiHdrs.push_back(ruiHdr);
              Logger::Info("RuiHdr Read: %d\n", i);
            }
            Stream.seek(mdlhdr.ruimeshindex + ruiHdrs[0].ruimeshindex);
            for (int j = 0; j < ruiHdrs.size(); j++)
            {
              int ruiPos = Stream.Position();
              mstudioruimesh_t ruiMesh; Stream.Read(&ruiMesh);
              ruiMeshes.push_back(ruiMesh);
            }
            int end = Stream.Position();
            BinaryWriter Bing = BinaryWriter("ass.bin");
            char data[end - mdlhdr.ruimeshindex];
            Stream.read(data,end - mdlhdr.ruimeshindex);
            Stream.seek(mdlhdr.ruimeshindex);
            Bing.write(data,end - mdlhdr.ruimeshindex);
            Logger::Info("Finished writing RUI structs\n");
            Bing.Stream.close();
          }
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

void UI::RenderReadMdlWindow(int x, int y){
    mdl->UI(x-8);
    FileInfo info;
    if(ImGui::Button("Read MDL"))
    {


        std::vector<Error> rtn;
        std::ifstream test(&mdl->BoxBuffer[0]);
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
                    rtn.push_back({ ErrorType::Success,std::string("Version 49") });
                    BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
                    _opt_v49 = _opt_v49->_v49Mdl(&Stream, false);
                }
                else if ((int)ver[0] == 53)
                {
                    rtn.push_back({ ErrorType::Success,std::string("Version 53") });
                    BinaryReader Stream = BinaryReader(&mdl->BoxBuffer[0]);
                    MDL::v53Mdl mdl = mdl._v53Mdl(&Stream, false);

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
  ImGui::SameLine();
  ImGui::Combo("##ExtractDropdown", &extract_dropdown, extract_options, IM_ARRAYSIZE(extract_options));
  ImGui::SameLine();
  if(ImGui::Button("Read"))
  {
    switch(extract_dropdown){
      case 0:
        extract_rui();
        break;
    }
  }
  ImGui::Separator();
  //ok actual details n shit go here
  if(_opt_v49.has_value()){
    int treeidx = 0;
    ImGui::Text("Name: %s",_opt_v49->mdlhdr.name); 
    ImGui::Text("Version: 49");
    if (ImGui::TreeNode((void*)(intptr_t)0, "Bones (%zu)",_opt_v49->bones.size()))
    {
      for (int i = 0; i < _opt_v49->bones.size(); i++)
      {
        
        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->stringtable.bones[i].c_str()))
        {
          ImGui::Text("Flags: %08x", _opt_v49->bones[i].flags);
          ImGui::Text("Pos: (%f , %f , %f)",_opt_v49->bones[i].pos.x,_opt_v49->bones[i].pos.y,_opt_v49->bones[i].pos.z);
          ImGui::Text("Rot: (%f , %f , %f)",_opt_v49->bones[i].rot.x,_opt_v49->bones[i].rot.y,_opt_v49->bones[i].rot.z);
          
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
    
    if (ImGui::TreeNode((void*)(intptr_t)1, "Attachments (%zu)",_opt_v49->attachments.size()))
    {
      for (int i = 0; i < _opt_v49->attachments.size(); i++)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->attachments[i].szname.c_str()))
        {
          ImGui::Text("Flags: %08x", _opt_v49->attachments[i].flags);
          ImGui::Text("LocalBone: %i", _opt_v49->attachments[i].localbone);
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
    if (ImGui::TreeNode((void*)(intptr_t)2, "AnimDescs (%zu)",_opt_v49->animdescs.size()))
    {
      for (int i = 0; i < _opt_v49->animdescs.size(); i++)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->animdescs[i].szname.c_str()))
        {
          ImGui::Text("FPS: %f", _opt_v49->animdescs[i].fps);
          ImGui::Text("Frames: %i", _opt_v49->animdescs[i].numframes);
          ImGui::Text("Animindex: %i", _opt_v49->animdescs[i].animindex);
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
    if (ImGui::TreeNode((void*)(intptr_t)3, "SeqDescs (%zu)",_opt_v49->seqdescs.size()))
    {
      for (int i = 0; i < _opt_v49->seqdescs.size(); i++)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", _opt_v49->seqdescs[i].szlabel.c_str()))
        {
          
          ImGui::Text("Nodeflags: %i", _opt_v49->seqdescs[i].nodeflags);
          ImGui::Text("BBmax: (%f , %f , %f)",_opt_v49->seqdescs[i].bbmax.x,_opt_v49->seqdescs[i].bbmax.y,_opt_v49->seqdescs[i].bbmax.z);
          ImGui::Text("BBmin: (%f , %f , %f)",_opt_v49->seqdescs[i].bbmin.x,_opt_v49->seqdescs[i].bbmin.y,_opt_v49->seqdescs[i].bbmin.z);
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
    
  }

}