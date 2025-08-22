

#include "engine.h"

int main(int argc, char** args) 
{
    std::cout << "game starting" << std::endl;
    auto window = tiny_engine::createWindow(800, 600, false);
    auto graphics = tiny_engine::initGraphics("dx11", window);
    assert(graphics != nullptr);

    auto heroTexture = tiny_engine::createTextureFromFile(graphics, "sample_assets/hero.png");
    if (!heroTexture.has_value()) {
        heroTexture = tiny_engine::createTextureFromFile(graphics, "../sample_assets/hero.png");
        assert(heroTexture.has_value());
    }

    std::cout << "hero texture loaded" << std::endl;

    bool runGame = true;
    while (runGame) {

        
        auto events = tiny_engine::pollWindowMessages(window);
        for (auto& e : events)
        {
            if (e.type == "quit") runGame = false;
        }

        tiny_engine::bindBackBuffer(graphics, 0, 0, 800, 600);
        tiny_engine::clearBackBuffer(graphics, 1, 0, 0, 1);

        tiny_engine::drawTexture(graphics, heroTexture.value(), 100, 100);
        
        tiny_engine::presentBackBuffer(graphics);

    }

    


    
}

