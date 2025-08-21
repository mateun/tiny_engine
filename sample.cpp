

#include "engine.h"
#include <iostream>




int main(int argc, char** args) 
{
    std::cout << "game starting" << std::endl;
    auto window = tiny_engine::createWindow(800, 600, false);
    auto ctx3d = tiny_engine::init3D("dx11", window);



    bool runGame = true;
    while (runGame) {

        
        auto events = tiny_engine::pollWindowMessages(window);
        for (auto& e : events)
        {
            if (e.type == "quit") runGame = false;
        }

        tiny_engine::clearBackBuffer(ctx3d, 1, 0, 0, 1);
        tiny_engine::presentBackBuffer(ctx3d);

    }

    


    
}

