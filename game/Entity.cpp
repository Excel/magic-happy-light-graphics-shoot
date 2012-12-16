#include "Entity.h"


Entity::Entity(Vector3 pos, Vector3 dir, Vector2 rotation, GLuint textureID){
    m_pos = pos;
    m_dir = dir;
    m_rotation = rotation;
    m_emitter = new ParticleEmitter(textureID);

    //Collision

    // Get a new quadrics obect
    m_quadric = gluNewQuadric();
    m_tess = gluNewTess();
    m_colRadius = 0.50f;


}


Entity::~Entity(){
    delete m_emitter;
    gluDeleteQuadric(m_quadric);
    gluDeleteTess(m_tess);

}


void Entity::onCollide(Entity *e){
    //check for collisions here
    if(e->isEnemy() != isEnemy()){
        //assume they are spheres at the mo
        if(e->getCollisionType() == COLLISION_SPHERE && getCollisionType() == COLLISION_SPHERE){
            if((e->getPos() - getPos()).length() < e->getColRadius() + getColRadius()){
                collisionLogic();
            }
        }
        else if(e->getCollisionType() == COLLISION_BOX && getCollisionType() == COLLISION_BOX){

        }


    }


}
