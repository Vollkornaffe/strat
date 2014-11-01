#include "Graphics.hh"

#include "Math.hh"
#include "Map.hh"
#include "InterpState.hh"

#include <GL/glu.h>

#include <vector>
#include <iostream>

#define checkError() printOglError(__FILE__, __LINE__)

static glm::vec3 playerColors[4] = {
    glm::vec3(0.0, 1.0, 1.0),
    glm::vec3(1.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0)
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

void printOglError(const char *file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "gl error in file " << file
                  << " @ line " << line << ": "
                  << gluErrorString(error) << std::endl;
    }
}

void RenderBuildingSystem::configure(entityx::EventManager &events) {
    events.subscribe<BuildingCreated>(*this);
}

void RenderBuildingSystem::render(entityx::EntityManager &entities) {
    GameObject::Handle gameObject;
    Building::Handle building;
    for (entityx::Entity entity:
         entities.entities_with_components(gameObject, building)) {
        glPushMatrix();
        glTranslatef(building->getPosition().x, building->getPosition().y,
                     map.point(building->getPosition().x,
                               building->getPosition().y).height );
        glScalef(building->getTypeInfo().size.x,
                 building->getTypeInfo().size.y,
                 building->getTypeInfo().size.z);
        assert(gameObject->getOwner() > 0 && gameObject->getOwner()-1 < 4);
        glm::vec3 color(playerColors[gameObject->getOwner()-1]);
        glBegin(GL_QUADS);
        glColor4f(color.x, color.y, color.z, 1.0f);
        drawCube();
        glEnd();
        glPopMatrix();
    }
}

void RenderBuildingSystem::receive(const BuildingCreated &event) {
}

void RenderResourceTransferSystem::render(entityx::EntityManager &entities) {
    ResourceTransfer::Handle r;
    for (auto entity : entities.entities_with_components(r)) {
        glm::vec3 a(r->fromPosition), b(r->toPosition);
        a.z = a.z ;
        b.z = b.z ;

        glm::vec3 m((a.x + b.x) * 0.5,
                    (a.y + b.y) * 0.5,
                    (a.z + b.z) * 0.5f + r->distance.toFloat() * 0.5f);

        float ta = r->getLastProgress().toFloat();
        float tb = r->getProgress().toFloat();
        float t = lerp<float>(ta, tb, interp.getT());

        glm::vec3 a_to_m(m - a);
        glm::vec3 da(a + a_to_m * t);
        glm::vec3 m_to_b(b - m);
        glm::vec3 dm(m + m_to_b * t);

        glm::vec3 da_to_dm(dm - da);
        glm::vec3 dda(da + da_to_dm * t);

        glPushMatrix();
        glTranslatef(dda.x, dda.y, dda.z);
        glTranslatef(-0.5f, -0.5f, 0.0f);
        glm::vec3 color(1.0, 0.0, 1.0);
        glBegin(GL_QUADS);
        glColor4f(color.x, color.y, color.z, 1.0f);
        drawCube();
        glEnd();
        glPopMatrix();
    }
}

void setupGraphics(const Config &config, const View &view) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f,
        config.screenWidth / static_cast<float>(config.screenHeight),
        1.0f, 5000.0f); 
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(view.position.x, view.position.y, view.position.z,
              view.target.x, view.target.y, view.target.z,
              0.0, 0.0, 1.0);

    //GLfloat mat_specular[] = { 0.25, 0.25, 0.25, 1.0 };
    GLfloat mat_specular[] = {0,0,0,0};
    GLfloat mat_shininess[] = { 10.0 };
    GLfloat light_position[] = { 1, 1, 1, 0.0 };
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat global_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //
}

void drawCursor(const Map &map, const View &view) {
    float tx = view.cursor.x;
    float ty = view.cursor.y;
    float tz = map.point(view.cursor).height + 0.001;

    float s = 0.5f;

    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.0f, 1.0f, 0.5f);
    glVertex3f(tx - s, ty - s, tz);
    glVertex3f(tx + s, ty - s, tz);
    glVertex3f(tx + s, ty + s, tz);
    glVertex3f(tx - s, ty + s, tz);
    glEnd();
}
