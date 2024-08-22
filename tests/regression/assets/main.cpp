#include "main.h"
#include <iostream>

int main(int argc, char ** argv)
{

    jGL::DesktopDisplay::Config conf;
    conf.VULKAN = false;
    conf.COCOA_RETINA = false;

    jGL::DesktopDisplay display(glm::ivec2(1,1),"Test TextureAssetStore", conf);
    glewInit();
    std::shared_ptr<jGL::jGLInstance> jGLInstance = std::move(std::make_shared<jGL::GL::OpenGLInstance>(display.getRes()));
    Hop::Util::Assets::TextureAssetStore textureStore(std::filesystem::path("resource"), jGLInstance);
    textureStore.scan();
    for (auto tex : textureStore)
    {
        std::cout << tex.first << " ";
    }

    return 0;
}
