#ifndef STRAT_GAME_INPUT_HH
#define STRAT_GAME_INPUT_HH

#include "util/Math.hh"
#include "Map.hh"
#include "common/Defs.hh"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
#include <boost/variant.hpp>

#include <algorithm>

struct Config;
struct Client;
struct Sim;
struct TerrainMesh;

// Handles user input, including:
// - scrolling, rotating and zooming the camera,
// - finding the currently selected grid point on the map,
// - selecting buildings,
// - selecting a rectangle part of the map,
// - sending orders such as BUILD and ATTACK.
//
// All the resulting information that is relevant for drawing
// is stored in a View.
struct Input {
    struct DefaultMode {
    };

    typedef boost::variant<DefaultMode> Mode;

    // Camera information
    struct View {
        glm::vec3 position, target;
        float angle, distance;

        glm::mat4 projectionMat, cameraMat;

        View();
    };

    Input(const Config &, GLFWwindow *, Client &, entityx::EventManager &);
    ~Input();

    const View &getView() const;
    const Mode &getMode() const;

    void update(double dt); 

private:
    const Config &config;
    GLFWwindow *window;

    Client &client;
    Sim &sim;
    const Map &map;

    Mode mode;

    View view;
    Ray mouseRay;
    float scrollSpeed;

    void scrollView(double dt);

    // Scroll as far as possible on the map using delta
    void tryScroll(const glm::vec2 &delta);

    // Intersect view ray with selectable objects,
    // returns the closest hit or an invalid entity handle if there is none
    entityx::Entity pickEntity();

    // Callbacks for GLFW
    static void setCallbacks(GLFWwindow *);

    static void onMouseButton(GLFWwindow *, int, int, int);
    static void onKey(GLFWwindow *, int, int, int, int);

    // disallow:
    Input(const Input &);
    void operator=(const Input &);
};

#endif
