#include "Graphics.hh"

#include "util/Math.hh"
#include "util/Profiling.hh"
#include "Map.hh"
#include "InterpState.hh"
#include "Terrain.hh"

#include <GL/glu.h>
#include <inline_variant_visitor/inline_variant.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <iostream>

using namespace glm;

//#define checkError() printOglError(__FILE__, __LINE__)
#define checkError()

static vec3 playerColors[4] = {
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
};

static void drawCube();
static vec3 bezier(const vec3 &a, const vec3 &b,
                   float height, float t);
static void printOglError(const char *file, int line);

void RenderShipSystem::load() {
    shipObj.reset(new opengl::OBJ("data/stupidShip/stupidShip3.obj",
                                  textures));
}

void RenderShipSystem::render(entityx::EntityManager &entities,
                              const InterpState &interp) {
    glDisable(GL_CULL_FACE);
    glColor4f(0.7f, 0.7f, 0.7f, 1.0f);

    for (auto &part : shipObj->parts) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        part.vertices->bind();
        glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*3, nullptr);

        part.normals->bind();
        glNormalPointer(GL_FLOAT, sizeof(GLfloat)*3, nullptr);

        GameObject::Handle gameObject;
        PreviousPhysicsState::Handle previousPhysicsState;
        PhysicsState::Handle physicsState;
        Ship::Handle ship;

        for (entityx::Entity entity :
             entities.entities_with_components(gameObject, previousPhysicsState, physicsState, ship)) {
            assert(gameObject->getOwner() > 0 && gameObject->getOwner()-1 < 4);
            vec3 color(playerColors[gameObject->getOwner()-1]); //TODO

            PhysicsState interpPhysicsState(
                    PhysicsState::interpolate(previousPhysicsState->state,
                                              *physicsState.get(),
                                              fixed::fromFloat(interp.getT())));
            glPushMatrix();
            glTranslatef(interpPhysicsState.position.x.toFloat(),
                         interpPhysicsState.position.y.toFloat(),
                         interpPhysicsState.position.z.toFloat());

            quat orientation(fixedToFloat(interpPhysicsState.orientation));
            mat4 orientationMatrix(glm::mat4_cast(orientation));
            glMultMatrixf(&orientationMatrix[0][0]);

            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

            glDrawArrays(GL_TRIANGLES, 0, part.vertices->getNumElements());
            glPopMatrix();

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    glEnable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void DebugRenderPhysicsStateSystem::load() {
}

void DebugRenderPhysicsStateSystem::render(entityx::EntityManager &entities,
                                           const InterpState &interp) {
    PreviousPhysicsState::Handle previousPhysicsState;
    PhysicsState::Handle physicsState;

    for (entityx::Entity entity :
         entities.entities_with_components(previousPhysicsState, physicsState)) {
        PhysicsState interpPhysicsState(
                PhysicsState::interpolate(previousPhysicsState->state,
                                          *physicsState.get(),
                                          fixed::fromFloat(interp.getT())));

        glPushMatrix();

        glTranslatef(interpPhysicsState.position.x.toFloat(),
                     interpPhysicsState.position.y.toFloat(),
                     interpPhysicsState.position.z.toFloat());

        quat orientation(fixedToFloat(physicsState->orientation));
        mat4 orientationMatrix(glm::mat4_cast(orientation));
        glMultMatrixf(&orientationMatrix[0][0]);

        glScalef(physicsState->size.x.toFloat(), physicsState->size.y.toFloat(), physicsState->size.z.toFloat());

        glTranslatef(-0.5f, -0.5f, -0.5f);

        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

        glBegin(GL_LINES);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        drawCube();
        glEnd();

        glPopMatrix();
    }
}

Graphics::Graphics(const Config &config,
                   const Input &input,
                   opengl::TextureManager &textures,
                   opengl::ProgramManager &programs)
    : config(config), input(input),
      textures(textures), programs(programs),
      renderShipSystem(input, textures) {
}

void Graphics::load() {
    renderShipSystem.load();
    debugRenderPhysicsStateSystem.load();

    shadowMapProgram = programs.load("shaders/shadowmap-vertex.glsl", "shaders/shadowmap-pixel.glsl");
    shadowMap.reset(new opengl::Framebuffer(opengl::Framebuffer::DEPTH_TEXTURE, ivec2(1024, 1024)));
}

void Graphics::initTerrain(const Map &map, const Water &water) {
    terrain.reset(new TerrainMesh(map, water, Map::Pos(32,32), programs));
}

void Graphics::renderShadowMap(entityx::EntityManager &entities,
                               const InterpState &interp) {
    PROFILE(renderShadowMap);

    vec3 lightInvDir(0.5f, 2.0f, 2.0f);

    mat4 projection(ortho<float>(-10,10,-10,10,-10,20)),
         view(lookAt(lightInvDir, vec3(0,0,0), vec3(0,1,0)));

    shadowMap->renderInto([&] {
        shadowMapProgram->bind();
        shadowMapProgram->setUniform(shadowMapProgram->getUniformLocation("depthMVP"), projection * view);

        renderShipSystem.render(entities, interp);
        debugRenderPhysicsStateSystem.render(entities, interp);

        shadowMapProgram->unbind();
    }, opengl::Framebuffer::CLEAR);
}

void Graphics::setup(const Input::View &view) {
    checkError();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(view.projectionMat));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(view.cameraMat));

    //GLfloat mat_specular[] = { 0.25, 0.25, 0.25, 1.0 };
    GLfloat mat_specular[] = {0,0,0,0};
    GLfloat mat_shininess[] = { 100.0 };
    GLfloat mat_diffuse[] = { 0.75, 0.75, 0.75, 1 };
    GLfloat light_position[] = { 3, 3, 5, 0.0 };
    GLfloat light_diffuse[] = { 1, 1, 1, 1 };
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

    GLfloat global_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);

    /*glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);*/

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Graphics::render(entityx::EntityManager &entities,
                      const InterpState &interp) {
    PROFILE(render);

    {
        PROFILE(terrain);
        terrain->draw();
    }

    {
        PROFILE(water);
        terrain->drawWater(interp);
    }

    {
        PROFILE(objects);
        renderShipSystem.render(entities, interp);
        debugRenderPhysicsStateSystem.render(entities, interp);
    }
}

void Graphics::debugRender() {
    mat4 projection(ortho<float>(0, config.screenWidth, config.screenHeight, 0));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(config.screenWidth, config.screenHeight);
    glEnd();
}

void Graphics::update() {
    PROFILE(terrain);
    terrain->update();
}

static void drawCube() {
    // front
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // back
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    // right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    // left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // top
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);
    // bottom
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
}

static vec3 bezier(const vec3 &a, const vec3 &b,
                   float height, float t) {
    // Bezier interpolation
    vec3 m((a.x + b.x) * 0.5,
           (a.y + b.y) * 0.5,
           (a.z + b.z) * 0.5f + height);

    vec3 a_to_m(m - a);
    vec3 da(a + a_to_m * t);
    vec3 m_to_b(b - m);
    vec3 dm(m + m_to_b * t);

    vec3 da_to_dm(dm - da);
    vec3 dda(da + da_to_dm * t);

    return dda;
}

static void printOglError(const char *file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "gl error in file " << file
                  << " @ line " << line << ": "
                  << gluErrorString(error) << std::endl;
    }
}

