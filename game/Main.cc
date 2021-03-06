#include "Graphics.hh"
#include "Config.hh"
#include "Map.hh"
#include "Client.hh"
#include "InterpState.hh"
#include "Input.hh"
#include "Terrain.hh"
#include "util/Log.hh"
#include "util/Profiling.hh"

#include <entityx/entityx.h>

#include <cstdlib>

#include <GL/glew.h>
#include <IL/il.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

void errorCallback(int error, const char *description) {
    std::cerr << "GLFW error: " << description << std::endl;
}

int main(int argc, char *argv[]) {
    Log::addSink(new ConsoleLogSink);

    Config config;

    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed" << std::endl;
        return 1;
    }

    ilInit();

    glfwSetErrorCallback(errorCallback);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    GLFWwindow *window = glfwCreateWindow(config.screenWidth,
        config.screenHeight, "Strats", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (!window) {
        glfwTerminate();
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        return 1;
    }

    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return 1;
    }

    Client client("leo");
    client.connect("localhost", 1234);

    std::cout << "Waiting for the game to start" << std::endl;
    while (!client.isStarted()) {
        client.update(0.0);
    }
    std::cout << "Game started" << std::endl;

    Sim &sim(client.getSim());
    const SimState &simState(sim.getState());
    const Map &map(simState.getMap());
    const Water &water(simState.getWater());
    const InterpState &interp(client.getInterp());

    Input input(config, window, client, sim.getEvents());
    const Input::View &view(input.getView());

    opengl::TextureManager textures;
    opengl::ProgramManager programs;

    Graphics graphics(config, input, textures, programs);
    graphics.load();
    graphics.initTerrain(map, water);

    size_t frames = 0, fps = 0;
    double lastFrameTime = glfwGetTime();

    double frameStartTime = glfwGetTime();

    bool quit = false;
    while (!quit && !glfwWindowShouldClose(window)) {
        PROFILE(main);

        double dt = glfwGetTime() - frameStartTime;
        frameStartTime = glfwGetTime();


        {
            PROFILE(update);

            {
                PROFILE(client);
                client.update(dt);
            }
            {
                PROFILE(input);
                input.update(dt);
            }

            graphics.update();
        }

        {
            PROFILE(render);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            graphics.renderShadowMap(sim.getEntities(), interp);

            graphics.setup(view);
            graphics.render(sim.getEntities(), interp);

            graphics.debugRender();

            {
                PROFILE(swap);
                glfwSwapBuffers(window);
            }
        }

        glfwPollEvents();

        frames++;
        if (glfwGetTime() - lastFrameTime >= 1.0f) {
            fps = frames;
            frames = 0;
            lastFrameTime += 1.0f;
        }

        std::stringstream ss;
        ss << "Strats (" << fps << " FPS, " << "): ";

        glfwSetWindowTitle(window, ss.str().c_str());
    } 

    enet_deinitialize();
    glfwTerminate();

    return 0;
}
