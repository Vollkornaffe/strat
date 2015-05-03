#include "Input.hh"

#include "Sim.hh"
#include "Map.hh"
#include "Client.hh"
#include "Terrain.hh"
#include "Config.hh"
#include "util/Profiling.hh"

#define GLM_FORCE_RADIANS
#include <GL/glu.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <inline_variant_visitor/inline_variant.hpp>

#define DOUBLE_CLICK_S 0.4f

using namespace glm;

// GLFW does not allow setting a context for callbacks,
// so here we use a global variable for the current input instance
static Input *g_input = NULL;

Input::View::View()
    : angle(0),
      distance(40.0f) {
}

// Calculates a ray going from the camera position in the direction the mouse is pointing at
static Ray calculateViewRay(double mx, double my, int width, int height,
                            const Input::View &view) {
    dmat4 modelview(view.cameraMat),
          projection(view.projectionMat);
    GLint viewport[4] = { 0, 0, width, height };
    dvec3 nearP, farP;

    gluUnProject(mx, viewport[3] - my, 0.0,
                 value_ptr(modelview), value_ptr(projection), viewport,
                 &nearP.x, &nearP.y, &nearP.z);
    gluUnProject(mx, viewport[3] - my, 0.1,
                 value_ptr(modelview), value_ptr(projection), viewport,
                 &farP.x, &farP.y, &farP.z);

    return Ray(view.position, vec3(normalize(farP - nearP)));
}

Input::Input(const Config &config, GLFWwindow *window, Client &client,
             entityx::EventManager &events, const TerrainMesh &terrain)
    : config(config), window(window),
      client(client), sim(client.getSim()),
      terrain(terrain), map(sim.getState().getMap()),
      mode(DefaultMode()),
      scrollSpeed(5.0f) {
    /*view.target.x = map.getSizeX() / 2;
    view.target.y = map.getSizeY() / 2;*/
    //auto mainEnt = findMainBuilding();
    view.target = vec3(50, 50, 0); //mainEnt.component<Building>()->getPosition();
    const GridPoint &gp(map.point(view.target.x, view.target.y));
    view.target.z = gp.height + sim.getState().getWater().point(view.target.x, view.target.y).height.toFloat();

    vec3 origin_camera(0, -40.0f, view.distance);
    view.position = view.target + rotateZ(origin_camera, view.angle);

    assert(!g_input);
    g_input = this;

    setCallbacks(window);
}

Input::~Input() {
    assert(g_input == this);
    g_input = NULL;
}

const Input::View &Input::getView() const {
    return view;
}

const Input::Mode &Input::getMode() const {
    return mode;
}

void Input::update(double dt) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    {
        PROFILE(mouse_ray);

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        mouseRay = calculateViewRay(mx, my, width, height, view);
    }

    scrollView(dt);
    
    {
        view.projectionMat = perspective<float>(M_PI / 2.0f, width / (float)height,
                                                1.0f, 5000.0f);                                          
        view.cameraMat = lookAt(view.position, view.target, vec3(0, 0, 1));
    }
}

void Input::scrollView(double dt) {
    vec2 mapDirection(view.target - view.position);
    vec2 orthDirection(cross(vec3(0, 0, 1), vec3(mapDirection, 0)));

    float moveDelta = scrollSpeed * dt;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS
        && view.target.y < map.getSizeY()) {
        tryScroll(mapDirection * moveDelta);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
        && view.target.x > 0) {
        tryScroll(orthDirection * moveDelta);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
        && view.target.y > 0) {
        tryScroll(-mapDirection * moveDelta);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS
        && view.target.x < map.getSizeX()) {
        tryScroll(-orthDirection * moveDelta);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS
        && view.distance > 3.0f) {
        view.distance -= dt * 35.0f;
        if (view.distance < 3.0f)
            view.distance = 3.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        view.distance += dt * 35.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        view.angle += dt * 2.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        view.angle -= dt * 2.0f;
    }

    view.position.x = view.target.x;
    view.position.y = view.target.y - 10.0f;
    view.position.z = view.distance;

    if (view.position.y > 0)
        view.position.z += map.point(view.position.x, view.position.y).height;
    else
        view.position.z += map.point(view.target.x, view.target.y).height;

    vec3 origin_camera(0, -40.0f, view.distance);
    view.position = view.target + rotateZ(origin_camera, view.angle);
}

void Input::tryScroll(const vec2 &delta) {
    view.target += vec3(delta, 0);

    if (view.target.x < 0) view.target.x = 0;
    if (view.target.x >= map.getSizeX()) view.target.x = map.getSizeX() - 1;
    if (view.target.y < 0) view.target.y = 0;
    if (view.target.y >= map.getSizeY()) view.target.y = map.getSizeY() - 1;

    const GridPoint &gp(map.point(view.target.x, view.target.y));
    view.target.z = gp.height + sim.getState().getWater().point(view.target.x, view.target.y).height.toFloat();
}

entityx::Entity Input::pickEntity() {
    entityx::Entity minEntity;
    /*float minD = 0.0f;

    GameObject::Handle gameObject;
    Building::Handle building;
    for (auto entity : sim.getEntities().entities_with_components(gameObject, building)) {
        AABB aabb(vec3(building->getPosition()), //- vec3(0.5, 0.5, 0),
                  vec3(building->getPosition() + building->getTypeInfo().size));
        float d;

        if (aabb.intersectWithRay(mouseRay, 1.0f, 5000.0f, &d)
            && (!minEntity || d < minD)) {
            minEntity = entity;
        }
    }*/

    return minEntity;
}

void Input::setCallbacks(GLFWwindow *window) {
    glfwSetMouseButtonCallback(window, Input::onMouseButton);
    glfwSetKeyCallback(window, Input::onKey);
}

void Input::onMouseButton(GLFWwindow *window,
                          int button, int action, int mods) {
    if (Input *self = g_input) {
        match(self->mode,
            [&](const DefaultMode &) {
            });
    }
}

void Input::onKey(GLFWwindow *window, int key, int, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_P)
        ProfilingData::dump();

    if (Input *self = g_input) {
        match(self->mode,
            [&](const DefaultMode &) {
                if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
                    Order order(Order::ACCELERATE);
                    order.accelerate.direction = DIRECTION_FORWARD;

                    self->client.order(order);
                }
                if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
                    Order order(Order::ACCELERATE);
                    order.accelerate.direction = DIRECTION_BACKWARD;

                    self->client.order(order);
                }
                if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
                    Order order(Order::ACCELERATE);
                    order.accelerate.direction = DIRECTION_LEFT;

                    self->client.order(order);
                }
                if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
                    Order order(Order::ACCELERATE);
                    order.accelerate.direction = DIRECTION_RIGHT;

                    self->client.order(order);
                }
            });
    }
}
