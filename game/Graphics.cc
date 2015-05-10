#include "Graphics.hh"

#include "Math.hh"
#include "Map.hh"
#include "InterpState.hh"

#include <GL/glu.h>
#include <inline_variant_visitor/inline_variant.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void printOglError(const char *file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "gl error in file " << file
                  << " @ line " << line << ": "
                  << gluErrorString(error) << std::endl;
    }
}

void RenderShipSystem::render(entityx::EntityManager &entities, const InterpState &interp) {
    glDisable(GL_CULL_FACE);
    glColor4f(0.7f, 0.7f, 0.7f, 1.0f);

    for (auto &part : shipObj.parts) {
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
            //std::cout << "[" << previousPhysicsState->state.position << " => " << physicsState->position << "] @ " << interp.getT() << " / " << (1-interp.getT()) << ": " << interpPhysicsState.position << std::endl;

            //interpPhysicsState = *physicsState.get();

            glPushMatrix();
            glTranslatef(interpPhysicsState.position.x.toFloat(),
                         interpPhysicsState.position.y.toFloat(),
                         interpPhysicsState.position.z.toFloat());

            quat orientation(fixedToFloat(interpPhysicsState.orientation));
            mat4 orientationMatrix(glm::mat4_cast(orientation));
            glMultMatrixf(&orientationMatrix[0][0]);

            //glScalef(3.0f, 3.0f, 3.0f);
            //glTranslatef(1.0f, 0.0f, 0.0f);

            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

            //glScalef(physicsState->size.x.toFloat(), physicsState->size.y.toFloat(), physicsState->size.z.toFloat());
            //glColor4f(color.x, color.y, color.z, 1.0f);


            glDrawArrays(GL_TRIANGLES, 0, part.vertices->getNumElements());

            //glBegin(GL_QUADS);
            //drawCube();
            //glEnd();
        
            glPopMatrix();

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    glEnable(GL_CULL_FACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void DebugRenderPhysicsStateSystem::render(entityx::EntityManager &entities) {
    PhysicsState::Handle physicsState;

    for (entityx::Entity entity :
         entities.entities_with_components(physicsState)) {
        glPushMatrix();
        glTranslatef(physicsState->position.x.toFloat(), physicsState->position.y.toFloat(), physicsState->position.z.toFloat());

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

void setupGraphics(const Config &config, const Input::View &view) {
    checkError();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*gluPerspective(60.0f,
        config.screenWidth / static_cast<float>(config.screenHeight),
        1.0f, 5000.0f); */
    glMultMatrixf(glm::value_ptr(view.projectionMat));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*gluLookAt(view.position.x, view.position.y, view.position.z,
              view.target.x, view.target.y, view.target.z,
              0.0, 0.0, 1.0);*/
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
