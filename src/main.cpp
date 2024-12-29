#include <iostream>

#include "Backend/Application/Application.hpp"
#include "SystemsMain/SystemsMain.hpp"
#include "Util/Weights/Weights.hpp"
#include "Util/VirtualCockpit/VirtualCockpit.hpp"


#include "Assets/Images/overhead.h"
#include "Assets/Images/pedestal.h"

bool g_ApplicationRunning = true;

namespace Ouroboros {
    class RenderLayer final : public Layer {
    public:
        void OnUIRender() override {

            if (VirtualCockpit::GetInstance().IsInitialized()) {
                VirtualCockpit::GetInstance().RenderImGui();
            }
            Weights::GetInstance().RenderImGui();
        }

        void OnAttach() override {
            std::thread([] { SystemMain(); }).detach();

            {
                uint32_t w, h;
                void *data = Image::Decode(g_OverheadPanel, sizeof(g_OverheadPanel), w, h);
                auto image_ptr = std::make_shared<Image>(w, h, ImageFormat::RGBA, data);
                ImageStorage::Instance().AddImage("overhead", image_ptr);
                free(data);
            }
            {
                uint32_t w, h;
                void *data = Image::Decode(g_Pedestal, sizeof(g_Pedestal), w, h);
                auto image_ptr = std::make_shared<Image>(w, h, ImageFormat::RGBA, data);
                ImageStorage::Instance().AddImage("pedestal", image_ptr);
                free(data);
            }
        }
    };

    Application *CreateApplication(int argc, char **argv) {
        const ApplicationSpecifications specifications = {"Ouroboros E170 Systems", 1440, 1026, 2560, 1440, 1440, 1026,
                                                          true, true};
        const auto app = new Application(specifications);
        app->PushLayer<RenderLayer>();

        return app;
    }

    int Main(const int argc, char **argv) {
        while (g_ApplicationRunning) {
            const auto app = CreateApplication(argc, argv);
            app->Run();
            delete app;
            g_ApplicationRunning = false;
        }
        return 0;
    }
}

int main(const int argc, char **argv) {
    Ouroboros::Main(argc, argv);
}

