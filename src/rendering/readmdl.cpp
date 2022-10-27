#include <rendering/render.hpp>
#include <imgui.h>
#include <rendering/filewidget.hpp>
#include <binarystream.hpp>
#include <mdls.hpp>

Widgets::File* mdl;

void UI::SetupReadMdlWindow(){
    mdl = new Widgets::File("MDL", false, "*.mdl");
}

void UI::RenderReadMdlWindow(int x, int y){
    mdl->UI(x-8);
    FileInfo info;
    if(ImGui::Button("Read"))
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
                    MDL::v49Mdl mdl = mdl._v49Mdl(&Stream, false);
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

        //READ SHIT!
    }
}