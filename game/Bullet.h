#ifndef BULLET_H
#define BULLET_H

#include "Entity.h"
#include "vector.h"

class Bullet : public Entity {
public:

    Bullet(Vector3 pos, Vector3 ray, Vector2 rotation, Model bulletModel);

    virtual ~Bullet();

    virtual void onUpdate();
    virtual void collisionLogic();
    virtual void onCollisionRender();

    bool isDone();

    Model m_model; // bullet model


protected:
    int m_existence;

    /*! how fast this thing goes with respect to the direction */
    float m_timestep;



};


#endif // BULLET_H
