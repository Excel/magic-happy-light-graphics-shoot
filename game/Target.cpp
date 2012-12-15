#include "Target.h"


Target::Target(Vector3 pos, Vector2 rotation, GLuint textureID) : Entity(pos, Vector3(0,0,0), rotation, textureID){
    m_existence = 1;
    m_hit = false;
    setColRadius(0.50f);

    m_colType = COLLISION_SPHERE;
    m_enemy = true;

    setModel("dragon");
    setShader("refract");
}

Target::~Target(){

}


void Target::onUpdate(){
    if(m_hit){
        //m_emitter->updateParticles();       //Move the particles
        m_existence--;

    }
}

void Target::onCollide(Entity* e){
    //check for collisions here
    if(e->isEnemy() != isEnemy()){
        //assume they are spheres at the mo
        if((e->getPos() - m_pos).length() < e->getColRadius() + m_colRadius){
            m_hit = true;
            m_world->setScore(m_world->getScore()+1);
        }


    }

}

void Target::onRender(){
    if(m_hit){
        m_emitter->drawParticles();         //Draw the particles
    }
    glAccum(GL_MULT, 0.9);
    glAccum(GL_ACCUM, 0.1);
    glAccum(GL_RETURN, 1);
    glFlush();

}

void Target::onCollisionRender(){
    if(m_colType == COLLISION_SPHERE){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3.f);
        glColor3f(0.5f, 0.5f, 1.0f);

        glPushMatrix();
        glTranslatef(m_pos.x, m_pos.y, m_pos.z);

        gluSphere(m_quadric, m_colRadius, 10, 10);

        glPopMatrix();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.f);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

bool Target::isDone(){
    if(m_existence <= 0 && m_hit){
        return true;
    }

    return false;
}

