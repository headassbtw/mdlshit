 

#include "glm/ext/vector_float3.hpp"
#include "structs.hpp"
#include <rendering/render.hpp>
#include <imgui.h>
#include <rendering/filewidget.hpp>
#include <binarystream.hpp>
#include <mdls.hpp>
#include <MLUtil.h>
#include <GL/glew.h>
#include <rendering/shaders.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <thread>

Widgets::File* _mdl;
std::optional<MDL::v49Mdl> _opt_v49;
std::optional<MDL::v53Mdl> _opt_v53;
std::optional<MDL::v52Mdl> _opt_v52;
int extract_dropdown;
bool isReading = false;

#pragma region sub-rendering
int subRendererResolution;
glm::vec3 pos;
float fov = 90;
float pos_gui[3] = {4,4,-4};
float lightpos_gui[3] = {-3,3,-3};
int SelectedMesh = 0;
GLuint Texture;
GLuint Framebuffer;
GLuint FramebufferDepth;
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint ShaderSelected;
GLuint LightID;

std::vector<vec3> vertices;
std::vector<vec2> uvs;
std::vector<vec3> normals;
std::vector<vec3> colors;
std::vector<float> faceids;

GLuint vbo[5] = {0,0,0,0,0};
GLuint vao = 0;
GLuint Shader;

GLuint SetupRuiMeshRenderPipeline(){
  GLFWmonitor* primary = glfwGetPrimaryMonitor();
  int m_x, m_y;
  glfwGetMonitorWorkarea(primary, nullptr,nullptr, &m_x, &m_y);
  auto style = ImGui::GetStyle();
  m_x -= (400+(style.WindowPadding.x*2) + style.ItemSpacing.x);
  m_y -= (190 + (style.ItemSpacing.y*4));
  subRendererResolution = std::min(m_x,m_y); //i'd do max but i doubt you'd actually be able to make space for it, 
                                                  //unless you had a square monitor which then defeats the purpose
  Logger::Info("Initializing RUI Mesh preview texture at %ipx\n",subRendererResolution);

  glGenRenderbuffers(1, &FramebufferDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, FramebufferDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, subRendererResolution, subRendererResolution);
  glGenFramebuffers(1,&Framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER,Framebuffer);
  glGenTextures(1, &Texture);
  glBindTexture(GL_TEXTURE_2D, Texture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,subRendererResolution,subRendererResolution,0,GL_RGBA,GL_FLOAT,0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, FramebufferDepth
  );

  glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,Texture,0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    
    Logger::Critical("Framebuffer was not ok!\n");
    abort();
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  Shader = Resources::Shaders::CompileShaders(Resources::Shaders::VertexShader, Resources::Shaders::FragmentShader);
  glBindFramebuffer(GL_FRAMEBUFFER,0);



  return Texture;
}
void DestroyRuiMeshRenderPipeline(){
  glDeleteBuffers(5,vbo);
  glDeleteVertexArrays(1,&vao);
  glDeleteTextures(1,&Texture);
  glDeleteRenderbuffers(1,&FramebufferDepth);
  glDeleteFramebuffers(1,&Framebuffer);
  glDeleteProgram(Shader);
}
glm::vec3 mltoglm(Vector3 vec){
  return vec3(vec.x,vec.y,vec.z);
}


void PushRuiMeshData(std::vector<mstudioruimesh_t> meshes){
  


  glUseProgram(Shader);
  MatrixID = glGetUniformLocation(Shader, "MVP");
  ViewMatrixID = glGetUniformLocation(Shader, "V");
	ModelMatrixID = glGetUniformLocation(Shader, "M");
  ShaderSelected = glGetUniformLocation(Shader, "Selected");
  LightID = glGetUniformLocation(Shader, "LightPosition_worldspace");

  for(int i = 0; i < meshes.size();i++){
    for(int j = 0; j < meshes[i].numvertices;j+=3){
      
      vertices.push_back(mltoglm(meshes[i].vertex[j].vertexpos));
      vertices.push_back(mltoglm(meshes[i].vertex[j+1].vertexpos));
      vertices.push_back(mltoglm(meshes[i].vertex[j+2].vertexpos));
      
      vec3 nml = normalize(cross(mltoglm(meshes[i].vertex[j+1].vertexpos) - mltoglm(meshes[i].vertex[j].vertexpos),
      mltoglm(meshes[i].vertex[j+2].vertexpos) - mltoglm(meshes[i].vertex[j].vertexpos)));
      
      colors.push_back(vec3(1.0f,0.0f,0.0f));
      colors.push_back(vec3(0.0f,1.0f,0.0f));
      colors.push_back(vec3(0.0f,0.0f,1.0f));

      for(int k = 0; k < 3; k++){
        faceids.push_back(i+1);
        normals.push_back(nml);
        uvs.push_back(vec2(0,0));
      }
    }
  }





  glGenBuffers(5, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 3 * vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 2 * uvs.size()      * sizeof(float), uvs.data(),      GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, 3 * normals.size()  * sizeof(float), normals.data(),  GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
  glBufferData(GL_ARRAY_BUFFER, 3 * normals.size()  * sizeof(float), colors.data(),   GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
  glBufferData(GL_ARRAY_BUFFER, 1 * faceids.size()  * sizeof(float), faceids.data(),   GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, NULL);

}

GLuint RenderRuiMeshes(){
  
  glDisable(GL_CULL_FACE);
  glCullFace(GL_NONE);

  pos = vec3(pos_gui[0],pos_gui[1],pos_gui[2]);
  glm::vec3 lightPos = vec3(lightpos_gui[0],lightpos_gui[1],lightpos_gui[2]);
  glm::mat4 Projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 15000.0f);
  glm::mat4 View = glm::lookAt(
    glm::vec3(pos.x,pos.y,pos.z),                               //position
    glm::vec3(0,0,0),    //point at a point
    glm::vec3(0,1,0)
  );
  glm::mat4 Model = glm::mat4(1.0f);
  glm::mat4 mvp = Projection * View * Model;
  glm::mat4 ViewMatrix = mvp;
  glm::mat4 ModelMatrix = glm::mat4(1.0);

  glUseProgram(Shader);

  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    
  glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

  glBindFramebuffer(GL_FRAMEBUFFER,Framebuffer);
  glViewport(0, 0, subRendererResolution, subRendererResolution);
  glClearColor(0.0, 0.0, 0.0, 0.5);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  

  glBindVertexArray(vao);
  glUniform1f(ShaderSelected,SelectedMesh);
  glDrawArrays(GL_TRIANGLES,0,vertices.size());


  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  return Texture;
}
bool renderRuiMesh = false;
#pragma endregion


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
            if (ImGui::TreeNode((void*)(intptr_t)4, "RUI Meshes (%zu)", mdl->ruimeshes.size())) {
              SelectedMesh = 0;
              for (int i = 0; i < mdl->ruimeshes.size(); i++) {
                if (ImGui::TreeNode((void *) (intptr_t) i, "%s", mdl->ruimeshes[i].szruimeshname)) {
                  if (ImGui::IsItemHovered()) {
                    SelectedMesh = i + 1;
                  }

                  if (ImGui::TreeNode((void *) (intptr_t) (i + 1), "Faces (%zu)", mdl->ruimeshes[i].numfaces)) {
                    for (int m = 0; m < mdl->ruimeshes[i].numfaces; m++) {
                      ImGui::Text("UV: [Min: (%f, %f) Max: (%f, %f)]", mdl->ruimeshes[i].facedata[m].faceuvmin.x,
                                  mdl->ruimeshes[i].facedata[m].faceuvmin.y, mdl->ruimeshes[i].facedata[m].faceuvmax.x,
                                  mdl->ruimeshes[i].facedata[m].faceuvmax.y);
                    }
                    ImGui::TreePop();
                  }
                  if (ImGui::TreeNode((void *) (intptr_t) (i + 2), "Vertices (%zu)", mdl->ruimeshes[i].numvertices)) {
                    for (int m = 0; m < mdl->ruimeshes[i].numvertices; m++) {
                      ImGui::Text("Pos: (%f, %f, %f) Parent: %i", mdl->ruimeshes[i].vertex[m].vertexpos.x,
                                  mdl->ruimeshes[i].vertex[m].vertexpos.y, mdl->ruimeshes[i].vertex[m].vertexpos.z,
                                  mdl->ruimeshes[i].vertex[m].parent);
                    }
                    ImGui::TreePop();
                  }
                  ImGui::TreePop();
                } else {
                  if (ImGui::IsItemHovered()) {
                    SelectedMesh = i + 1;
                  }
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
                    auto reader = BinaryReader(&_mdl->BoxBuffer[0]);
                    mdl->v49ExtractSrcBoneTransforms(&reader);
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
                    //auto reader = BinaryReader(&);
                    std::thread extract_rui_thread([&mdl]{
                      mdl->v53ExtractRUIMesh(&_mdl->BoxBuffer[0]);
                    });
                    extract_rui_thread.detach();
                    break;
                }

                case 1:
                {
                    auto reader = BinaryReader(&_mdl->BoxBuffer[0]);
                    mdl->v53ExtractSrcBoneTransforms(&reader);
                    break;
                }

                case 2:
                {
                    auto reader = BinaryReader(&_mdl->BoxBuffer[0]);
                    mdl->v53ExtractAABBTree(&reader);
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
                    auto reader = BinaryReader(&_mdl->BoxBuffer[0]);
                    mdl->v52ExtractSrcBoneTransforms(&reader);
                    break;
                }

                case 1:
                {
                    auto reader = BinaryReader(&_mdl->BoxBuffer[0]);
                    mdl->v52ExtractAABBTree(&reader);
                    break;
                }
                }
            }
        }
}


bool needsReset = false; //for the rui mesh renderer
void UI::RenderReadMdlWindow(int x, int y)
{
  //if(needsReset) ImGui::Text("Needs Reset");
  _mdl->UI(x-8);
  std::ifstream test(&_mdl->BoxBuffer[0]);
  int version = 0;


  if (!test)
  {
    _opt_v49.reset();
    _opt_v53.reset();
    _opt_v52.reset();
    if(needsReset){
      DestroyRuiMeshRenderPipeline();
      needsReset = false;
      renderRuiMesh = false;
    }
  }
  else
  {
    if (!isReading && ImGui::Button("Read MDL"))
    {
      isReading = true;
      _opt_v49.reset();
      _opt_v53.reset();
      _opt_v52.reset();
      DestroyRuiMeshRenderPipeline();
      std::thread mdl_read_thread([]{
        isReading = true;
        ReadMdl();
        isReading = false;
      });
      mdl_read_thread.detach();
      
      needsReset = true;
    }
    AddExtractionOptions();
    ImGui::SameLine();
    if(_opt_v53.has_value())
    if(!renderRuiMesh){
      if(ImGui::Button("Render RUI Mesh [FUCKED]")){
        SetupRuiMeshRenderPipeline();
        PushRuiMeshData(_opt_v53->ruimeshes);
        renderRuiMesh = true;
      }
    }
    else{
      if(ImGui::Button("Unload RUI Mesh")){
        DestroyRuiMeshRenderPipeline();
        renderRuiMesh = false;
      }
    }
    ImGui::Separator();
    if(!isReading){
      ImGui::BeginChildFrame((intptr_t)32, {400,0});
      AddMainMdlTrees();
      ImGui::EndChildFrame();
    }
    
    if(renderRuiMesh){
      RenderRuiMeshes();
      ImGui::SameLine();
      auto av = ImGui::GetContentRegionAvail();
      
      ImGui::BeginGroup();
      ImGui::SetNextItemWidth(av.x-100);
      ImGui::SliderFloat3("Camera Position", pos_gui,-10,10);
      ImGui::SetNextItemWidth(av.x-100);
      ImGui::SliderFloat3("Light Position", lightpos_gui,-12,12);
      ImGui::SetNextItemWidth(av.x-100);
      ImGui::SliderFloat("FoV",&fov,20.0f,150.0f);
      auto av2 = ImGui::GetContentRegionAvail();
      float texSize = std::min(av2.x,av2.y);
      ImGui::Image((void*)(intptr_t)Texture,{texSize,texSize},{0,1},{1,0});
      ImGui::EndGroup();
    }
  }
}

void UI::DestroyReadMdlWindow(){
    if(!renderRuiMesh){
        DestroyRuiMeshRenderPipeline();
    }
}