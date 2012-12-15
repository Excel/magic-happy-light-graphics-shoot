#ifndef BULLET_H
#define BULLET_H

#include "Entity.h"
#include "vector.h"

class Bullet : public Entity {
public:

    Bullet(Vector3 pos, Vector3 ray, Vector2 rotation, GLuint textureID);

    virtual ~Bullet();

    virtual void onUpdate();
    virtual void onCollide(Entity *e);
    virtual void onRender();
    virtual void onCollisionRender();

    bool isDone();

protected:
    int m_existence;

    /*! how fast this thing goes with respect to the direction */
    float m_timestep;



};


#endif // BULLET_H
