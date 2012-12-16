#include "Bullet.h"

using std::cout;

Bullet::Bullet(Vector3 pos, Vector3 dir, Vector2 rotation, GLuint textureID, Model bulletModel) : Entity(pos, dir, rotation, textureID){

    m_existence = 250;
    m_timestep = 2E-1;
    //m_timestep = 1E-5;


    m_colType = COLLISION_BOX;
    m_enemy = false;

    setModel("dragon");
    m_model = bulletModel;


    setColRadius(0.50f);
    setShader("psycho");

    for(int i = 0; i < 3*m_model.model->numvertices; i++)
    {

        if(i%3 == 0)
        {
            if (m_min.x > m_model.model->vertices[i])
                m_min.x = m_model.model->vertices[i];
            else if (m_max.x < m_model.model->vertices[i])
                m_max.x = m_model.model->vertices[i];

        }
        else if(i%3 == 1)
        {
            if (m_min.y > m_model.model->vertices[i])
                m_min.y = m_model.model->vertices[i];
            else if (m_max.y < m_model.model->vertices[i])
                m_max.y = m_model.model->vertices[i];

        }
        else if(i%3 == 2)
        {
            if (m_min.z > m_model.model->vertices[i])
                m_min.z = m_model.model->vertices[i];
            else if (m_max.z < m_model.model->vertices[i])
                m_max.z = m_model.model->vertices[i];
        }
    }
}

Bullet::~Bullet(){

}


void Bullet::onUpdate(){
    m_pos += m_dir * m_timestep;
    m_existence--;
    m_emitter->updateParticles();       //Move the particles
}

void Bullet::collisionLogic(){
    m_existence = -1;

}

void Bullet::onRender(){
    m_emitter->drawParticles();         //Draw the particles

    glAccum(GL_MULT, 0.9);
    glAccum(GL_ACCUM, 0.1);
    glAccum(GL_RETURN, 1);
    glFlush();

}

void Bullet::onCollisionRender(){
    if(m_colType == COLLISION_SPHERE){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3.f);
        glColor3f(1.0f, 0.5f, 0.5f);

        glPushMatrix();
        glTranslatef(m_pos.x, m_pos.y, m_pos.z);

        gluSphere(m_quadric, getColRadius(), 10, 10);

        glPopMatrix();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.f);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else if(m_colType == COLLISION_BOX){


        glColor3f(1.f, 1.f, 1.f);
        glPushMatrix();
        glTranslatef(m_pos.x, m_pos.y, m_pos.z);
        glRotatef(m_rotation.x, 0.0f, 1.0f, 0.0f);
        glRotatef(m_rotation.y, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_max.y, m_min.z);
        glVertex3f(m_min.x, m_max.y, m_min.z);
        glVertex3f(m_min.x, m_min.y, m_min.z);
        glVertex3f(m_max.x, m_min.y, m_min.z);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_min.y, m_max.z);
        glVertex3f(m_max.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_min.y, m_max.z);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_max.y, m_min.z);
        glVertex3f(m_max.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_max.y, m_min.z);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_min.y, m_max.z);
        glVertex3f(m_min.x, m_min.y, m_max.z);
        glVertex3f(m_min.x, m_min.y, m_min.z);
        glVertex3f(m_max.x, m_min.y, m_min.z);
        glEnd();

        glPopMatrix();
    }
}

bool Bullet::isDone(){
    if(m_existence <= 0){
        return true;
    }

    return false;
}

