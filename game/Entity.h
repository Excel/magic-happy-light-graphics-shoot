#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"
#include "particleemitter.h"

class Entity{
public:
    //! Enumeration for collision types.
    enum CollisionType {
       COLLISION_SPHERE, COLLISION_BOX
    };

    Entity(Vector3 pos, Vector3 ray, Vector2 rotation, GLuint textureID);

    virtual ~Entity();

    virtual void onUpdate() = 0;
    virtual void onCollide(Entity* e) = 0;
    virtual void onRender() = 0;
    virtual void onCollisionRender() = 0;

    virtual bool isDone() = 0;

    Vector3 getPos() {return m_pos;}
    Vector3 getDir() {return m_dir;}
    Vector2 getRotation() {return m_rotation;}
    bool isEnemy() {return m_enemy;}

    float getColRadius() {return m_colRadius;}
    void setColRadius(float r) {m_colRadius = r;}
    //void setWorld(World* w) {m_world = w;}
    //void setModel(Model* model) {m_model = model;}
    ParticleEmitter * m_emitter;

protected:
    Vector3 m_pos;
    Vector3 m_dir;
    /*! x = rotate by theta, y = rotate by phi*/
    Vector2 m_rotation;


    //Model *m_model;
    //Collision
    GLUquadric *m_quadric;
    CollisionType m_colType;
    float m_colRadius;
    bool m_enemy;


};


#endif // ENTITY_H
