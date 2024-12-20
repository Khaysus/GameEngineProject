﻿// Enable SIMD optimizations for glm
#define GLM_FORCE_PURE
#include "Window.h"
#include "fileformats/las.h"
#include "LuaState.h"

#include <iostream>

Window* window;

int main()
{
    LuaState();
    LuaState::open_libs();

    window = new Window();
    auto initCode = window->init();
    if (initCode != 0)
    {
        std::cerr << "Failed to initialize window" << std::endl;
        return initCode;
    }

    while (!window->should_close())
    {
        window->update();
        window->render();
    }

    delete window;

    LuaState::close();

    return 0;
}