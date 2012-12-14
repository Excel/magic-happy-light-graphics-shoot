#include "Entity.h"


Entity::Entity(Vector3 pos, Vector3 dir, Vector2 rotation, GLuint textureID){
    m_pos = pos;
    m_dir = dir;
    m_rotation = rotation;
    m_emitter = new ParticleEmitter(textureID);

    //Collision

    // Get a new quadrics obect
    m_quadric = gluNewQuadric();
    m_colRadius = 0.50f;


}

Entity::~Entity(){
    delete m_emitter;
    gluDeleteQuadric(m_quadric);

}

