#include "Entity.h"
#include "PBCurve.h"


Entity::Entity(Vector3 pos, Vector3 dir, Vector2 rotation){
    m_pos = pos;
    m_dir = dir;
    m_rotation = rotation;

    //Collision

    // Get a new quadrics obect
    m_quadric = gluNewQuadric();
    m_colRadius = 0.50f;
}


Entity::~Entity(){
    gluDeleteQuadric(m_quadric);

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

            Vector3 minone = e->getPos() + e->m_min;
            Vector3 maxone = e->getPos() + e->m_max;
            Vector3 mintwo = getPos() + m_min;
            Vector3 maxtwo = getPos() + m_max;

            if (!(minone.y > maxtwo.y || mintwo.y > maxone.y ||
                  minone.x > maxtwo.x || mintwo.x > maxone.x ||
                  minone.z > maxtwo.z || mintwo.z > maxone.z))
                collisionLogic();
        }
    }


}
