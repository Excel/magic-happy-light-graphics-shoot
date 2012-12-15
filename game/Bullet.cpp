#include "Bullet.h"

using std::cout;

Bullet::Bullet(Vector3 pos, Vector3 dir, Vector2 rotation, GLuint textureID) : Entity(pos, dir, rotation, textureID){

    m_existence = 150;
    m_timestep = 1E-1;
    //m_timestep = 1E-5;

    setColRadius(0.50f);

    m_colType = COLLISION_SPHERE;
    m_enemy = false;

    setModel("sphere");
    setShader("psycho");

}

Bullet::~Bullet(){

}


void Bullet::onUpdate(){
    m_pos += m_dir * m_timestep;
    m_existence--;
    //m_emitter->updateParticles();       //Move the particles
}

void Bullet::onCollide(Entity* e){
    //check for collisions here
    if(e->isEnemy() != isEnemy()){
        //assume they are spheres at the mo
        if((e->getPos() - m_pos).length() < e->getColRadius() + m_colRadius){
            m_existence = -1;
        }


    }

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
}

bool Bullet::isDone(){
    if(m_existence <= 0){
        return true;
    }

    return false;
}

