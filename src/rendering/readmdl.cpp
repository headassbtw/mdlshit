#include <rendering/render.hpp>
#include <imgui.h>
#include <rendering/filewidget.hpp>
#include <binarystream.hpp>
#include <mdls.hpp>
#include <MLUtil.h>

Widgets::File* _mdl;
std::optional<MDL::v49Mdl> _opt_v49;
std::optional<MDL::v53Mdl> _opt_v53;
std::optional<MDL::v52Mdl> _opt_v52;
int extract_dropdown;
bool isReading = false;

void UI::SetupReadMdlWindow()
{
    _mdl = new Widgets::File("MDL", false, "*.mdl");
}


void UI::SetReadMdlFile(const char* path)
{
    strcpy(_mdl->BoxBuffer, path);
}

void ReadMdl()
{
    BinaryReader Stream = BinaryReader(&_mdl->BoxBuffer[0]);
    char magic[4];
    Stream.read(magic, 4);
    if (strcmp(magic, "IDST") >= 0)
    {
        int version;
        Stream.Read(&version);
        Stream.seek(0);

        if (version == 49)
        {
            Logger::Debug("MDL Version: %d\n", version);
            MDL::v49Mdl mdl = mdl._v49Mdl(&Stream, false);
            mdl.ikchainbones = mdl.GetIKBones();
            _opt_v49 = mdl;
        }
        else if (version == 53)
        {
            Logger::Debug("MDL Version: %d\n", version);
            MDL::v53Mdl mdl = mdl._v53Mdl(&Stream, false);
            _opt_v53 = mdl;
        }
        else if (version == 52)
        {
            Logger::Debug("MDL Version: %d\n", version);
            MDL::v52Mdl mdl = mdl._v52Mdl(&Stream, false);
            _opt_v52 = mdl;

        }
    }
    else
    {
        std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
        Logger::Critical("%s\n", err.c_str());
    }
}
void AddMainMdlTrees()
{
#pragma region MDLV49
        if (_opt_v49.has_value())
        {
            std::optional<MDL::v49Mdl> mdl = _opt_v49;
            int treeidx = 0;
            std::vector<int> jiggleBones;
            //for (int i = 0; i < mdl->bones.size(); i++)
            //{
            //        if (mdl->bones[i].procindex > 0)
            //        {
            //            int jiggleBoneIdx = Utility::func::GetAttachedJiggleBone(version, mdl->mdlhdr.boneindex, i, mdl->bones[i].procindex, mdl->bones.size());
            //            jiggleBones.push_back(i);
            //        }
            //}


            ImGui::Text("Name: %s", mdl->stringtable.mdlname.c_str());
            ImGui::Text("Version: %d", mdl->mdlhdr.version);
            if (ImGui::TreeNode((void*)(intptr_t)0, "Bones (%zu)", mdl->bones.size()))
            {
                ImGui::Text("IkBones: %d", mdl->ikchainbones.size());
                for (int i = 0; i < mdl->bones.size(); i++)
                {

                    if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->stringtable.bones[i].c_str()))
                    {
                        ImGui::Text("Flags: %08x", mdl->bones[i].flags);
                        ImGui::Text("Pos: (%f , %f , %f)", mdl->bones[i].pos.x, mdl->bones[i].pos.y, mdl->bones[i].pos.z);
                        ImGui::Text("Rot: (%f , %f , %f)", mdl->bones[i].rot.x, mdl->bones[i].rot.y, mdl->bones[i].rot.z);
                        //if (mdl->bones[i].procindex > 0)
                        //{
                        //    int jiggleBoneIdx = -1;
                        //    if (jiggleBoneIdx == -1) Utility::func::GetAttachedJiggleBone(version, mdl->mdlhdr.boneindex, i, mdl->bones[i].procindex, mdl->bones.size());
                        //    ImGui::Text("JiggleBone: %d", jiggleBoneIdx);
                        //}

                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }

            //if (ImGui::TreeNode((void*)(intptr_t)1, "JiggleBones (%zu)", mdl->jigglebones.size()))
            //{
            //    for (int i = 0; i < mdl->jigglebones.size(); i++)
            //    {
            //        int bone = jiggleBones[i];
            //        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->stringtable.bones[bone].c_str()))
            //        {
            //            if (mdl->bones[i].procindex > 0)
            //            {
            //                int jiggleBoneIdx = Utility::func::GetAttachedJiggleBone(version, mdl->mdlhdr.boneindex, i, mdl->bones[i].procindex, mdl->bones.size());
            //                ImGui::Text("JiggleBone: %d", jiggleBoneIdx);
            //            }
            //
            //            ImGui::TreePop();
            //        }
            //    }
            //    ImGui::TreePop();
            //}

            if (ImGui::TreeNode((void*)(intptr_t)2, "Attachments (%zu)", mdl->attachments.size()))
            {
                for (int i = 0; i < mdl->attachments.size(); i++)
                {
                    if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->attachments[i].szname.c_str()))
                    {
                        int localBone = mdl->attachments[i].localbone;

                        ImGui::Text("Flags: %08x", mdl->attachments[i].flags);
                        ImGui::Text("LocalBone: %s", mdl->stringtable.bones[localBone].c_str());
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode((void*)(intptr_t)3, "AnimDescs (%zu)", mdl->animdescs.size()))
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

            if (ImGui::TreeNode((void*)(intptr_t)4, "SeqDescs (%zu)", mdl->seqdescs.size()))
            {
                for (int i = 0; i < mdl->seqdescs.size(); i++)
                {
                    if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->seqdescs[i].szlabel.c_str()))
                    {

                        //ImGui::Text("Nodeflags: %i", mdl->seqdescs[i].nodeflags);
                        //ImGui::Text("BBmax: (%f , %f , %f)", mdl->seqdescs[i].bbmax.x, mdl->seqdescs[i].bbmax.y, mdl->seqdescs[i].bbmax.z);
                        //ImGui::Text("BBmin: (%f , %f , %f)", mdl->seqdescs[i].bbmin.x, mdl->seqdescs[i].bbmin.y, mdl->seqdescs[i].bbmin.z);

                        if (mdl->seqdescs[i].numblends > 0 && ImGui::TreeNode((void*)(intptr_t)5, "Anims (%zu)", mdl->seqdescs[i].numblends))
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

                        if (mdl->seqdescs[i].numactivitymodifiers > 0 && ImGui::TreeNode((void*)(intptr_t)6, "Events (%zu)", mdl->seqdescs[i].numevents))
                        {
                            if (mdl->seqdescs[i].events.size() > 0)
                            {
                                for (int j = 0; j < mdl->seqdescs[i].events.size(); j++)
                                {
                                    ImGui::Text("Event: %s : %d", mdl->seqdescs[i].szeventnames[j].c_str(), j);
                                }
                            }
                            ImGui::TreePop();
                        }

                        if (mdl->seqdescs[i].numactivitymodifiers > 0 && ImGui::TreeNode((void*)(intptr_t)7, "ActMods (%zu)", mdl->seqdescs[i].numactivitymodifiers))
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
        }
#pragma endregion
#pragma region MDLV52
        if (_opt_v52.has_value())
        {
            std::optional<MDL::v52Mdl> mdl = _opt_v52;
            int treeidx = 0;
            ImGui::Text("Name: %s", mdl->stringtable.mdlname.c_str());
            ImGui::Text("Version: %d", mdl->mdlhdr.version);
            if (ImGui::TreeNode((void*)(intptr_t)0, "Bones (%zu)", mdl->bones.size()))
            {
                for (int i = 0; i < mdl->bones.size(); i++)
                {

                    if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->stringtable.bones[i].c_str()))
                    {
                        ImGui::Text("Flags: %08x", mdl->bones[i].flags);
                        ImGui::Text("Pos: (%f , %f , %f)", mdl->bones[i].pos.x, mdl->bones[i].pos.y, mdl->bones[i].pos.z);
                        ImGui::Text("Rot: (%f , %f , %f)", mdl->bones[i].rot.x, mdl->bones[i].rot.y, mdl->bones[i].rot.z);
                        if (mdl->bones[i].procindex > 0)
                        {
                            int jiggleBoneIdx = Utility::func::GetAttachedJiggleBone(mdl->mdlhdr.version, mdl->mdlhdr.boneindex, i, mdl->bones[i].procindex, mdl->bones.size());
                            ImGui::Text("JiggleBone: %d", jiggleBoneIdx);
                        }
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
                        int localBone = mdl->attachments[i].localbone;
                        ImGui::Text("Flags: %08x", mdl->attachments[i].flags);
                        ImGui::Text("LocalBone: %s", mdl->stringtable.bones[localBone].c_str());
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
        }
#pragma endregion
#pragma region MDLV53
        if (_opt_v53.has_value())
        {
            std::optional<MDL::v53Mdl> mdl = _opt_v53;
            int treeidx = 0;
            ImGui::Text("Name: %s", mdl->stringtable.mdlname.c_str());
            ImGui::Text("Version: %d", mdl->mdlhdr.version);
            if (ImGui::TreeNode((void*)(intptr_t)0, "Bones (%zu)", mdl->bones.size()))
            {
                for (int i = 0; i < mdl->bones.size(); i++)
                {

                    if (ImGui::TreeNode((void*)(intptr_t)i, "%s", mdl->stringtable.bones[i].c_str()))
                    {
                        if (mdl->bones[i].flags & BONE_USED_BY_IKCHAIN) ImGui::Text("IKBone");
                        ImGui::Text("Flags: %08x", mdl->bones[i].flags);
                        ImGui::Text("Pos: (%f , %f , %f)", mdl->bones[i].pos.x, mdl->bones[i].pos.y, mdl->bones[i].pos.z);
                        ImGui::Text("Rot: (%f , %f , %f)", mdl->bones[i].rot.x, mdl->bones[i].rot.y, mdl->bones[i].rot.z);
                        if (mdl->bones[i].procindex > 0)
                        {
                            int jiggleBoneIdx = Utility::func::GetAttachedJiggleBone(mdl->mdlhdr.version, mdl->mdlhdr.boneindex, i, mdl->bones[i].procindex, mdl->bones.size());
                            ImGui::Text("JiggleBone: %d", jiggleBoneIdx);
                        }
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
                        int localBone = mdl->attachments[i].localbone;
                        ImGui::Text("Flags: %08x", mdl->attachments[i].flags);
                        ImGui::Text("LocalBone: %s", mdl->stringtable.bones[localBone].c_str());
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
                                    int actModNum = mdl->seqdescs[i].actmods[j];
                                    ImGui::Text("ActMod: %s : %d | Negated: %d", mdl->seqdescs[i].szactivitymodifiernames[j].c_str(), j, mdl->activitymodifiers[actModNum].unk);
                                }
                            }
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
#pragma endregion
}

void AddExtractionOptions()
{
        if (_opt_v49.has_value())
        {
            const char* extract_options = { "Src Bone Transforms" };
            std::optional<MDL::v49Mdl> mdl = _opt_v49;
            ImGui::SameLine();
            ImGui::Combo("##ExtractDropdown", &extract_dropdown, extract_options, IM_ARRAYSIZE(extract_options));
            ImGui::SameLine();
            if (ImGui::Button("Extract"))
            {
                switch (extract_dropdown)
                {
                case 0:
                {
                    mdl->v49ExtractSrcBoneTransforms(&BinaryReader(&_mdl->BoxBuffer[0]));
                    break;
                }
                }
            }
        }
        else if (_opt_v53.has_value())
        {
            const char* extract_options[] = { "RUI Structs", "Src Bone Transforms", "AABB Trees" };
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
        }
        else if (_opt_v52.has_value())
        {
            const char* extract_options[] = { "Src Bone Transforms", "AABB Trees" };
            std::optional<MDL::v52Mdl> mdl = _opt_v52;

            ImGui::SameLine();
            ImGui::Combo("##ExtractDropdown", &extract_dropdown, extract_options, IM_ARRAYSIZE(extract_options));
            ImGui::SameLine();
            if (ImGui::Button("Extract"))
            {
                switch (extract_dropdown)
                {
                case 0:
                {
                    mdl->v52ExtractSrcBoneTransforms(&BinaryReader(&_mdl->BoxBuffer[0]));
                    break;
                }

                case 1:
                {
                    mdl->v52ExtractAABBTree(&BinaryReader(&_mdl->BoxBuffer[0]));
                    break;
                }
                }
            }
        }
}

//void UI::RenderReadMdlWindow(int x, int y)
//{
//    _mdl->UI(x - 8);
//    std::vector<Error> rtn;
//    std::ifstream test(&_mdl->BoxBuffer[0]);
//    if (ImGui::Button("Read MDL"))
//    {
//        if (!test)
//        {
//            rtn.push_back({ ErrorType::Blocking,std::string("File does not exist") });
//        }
//        else
//        {
//            char magic[4];
//            test.read(magic, 4);
//            if (strcmp(magic, "IDST") >= 0) 
//            {
//                rtn.push_back({ ErrorType::Success,std::string("Valid MDL file") });
//                char ver[1];
//                test.read(ver, 1);
//
//                if ((int)ver[0] == 49)
//                {
//                    isReading = true;
//                    Logger::Debug("MDL Version: %d\n", (int)ver[0]);
//                    rtn.push_back({ ErrorType::Success,std::string("Version 49") });
//                    BinaryReader Stream = BinaryReader(&_mdl->BoxBuffer[0]);
//                    MDL::v49Mdl mdl = {};
//                    mdl = mdl._v49Mdl(&Stream, false);
//                    Logger::Debug("MDL READ: %d\n", (int)ver[0]);
//                    //_opt_v49 = _opt_v49->_v49Mdl(&Stream, false);
//                }
//            }
//            else {
//                std::string err = std::string("Invalid MDL file; magic was \"") + std::string(magic) + "\"";
//                rtn.push_back({ ErrorType::Blocking,err });
//                Logger::Critical("%s\n", err.c_str());
//            }
//        }
//        test.close();
//    }
//}

void UI::RenderReadMdlWindow(int x, int y)
{
    _mdl->UI(x-8);
    std::ifstream test(&_mdl->BoxBuffer[0]);
    int version = 0;


    if (!test)
    {
        _opt_v49.reset();
        _opt_v53.reset();
        _opt_v52.reset();
    }
    else
    {
        if (!isReading && ImGui::Button("Read MDL"))
        {
            isReading = true;
            _opt_v49.reset();
            _opt_v53.reset();
            _opt_v52.reset();

            ReadMdl();
            isReading = false;
        }
        AddExtractionOptions();
        ImGui::Separator();
        AddMainMdlTrees();
    }
}