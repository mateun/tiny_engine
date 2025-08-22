

#include "engine.h"
#include <iostream>




int main(int argc, char** args) 
{
    std::cout << "game starting" << std::endl;
    auto window = tiny_engine::createWindow(800, 600, false);
    auto graphics = tiny_engine::initGraphics("dx11", window);

    auto heroTexture = tiny_engine::createTextureFromFile(graphics, "../sample_assets/hero.png");



    bool runGame = true;
    while (runGame) {

        
        auto events = tiny_engine::pollWindowMessages(window);
        for (auto& e : events)
        {
            if (e.type == "quit") runGame = false;
        }

        tiny_engine::bindBackBuffer(graphics, 0, 0, 800, 600);
        tiny_engine::clearBackBuffer(graphics, 1, 0, 0, 1);


        tiny_engine::drawTexture(graphics, heroTexture, 100, 100);


        tiny_engine::presentBackBuffer(graphics);

    }

    


    
}

