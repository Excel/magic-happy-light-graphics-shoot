#ifndef TARGET_H
#define TARGET_H

#include "World.h"
#include "Entity.h"
#include "vector.h"

class Target : public Entity {
public:

    Target(Vector3 pos, Vector2 rotation, GLuint textureID, Model targetModel, bool friendly);

    virtual ~Target();

    virtual void onUpdate();
    virtual void collisionLogic();
    virtual void onRender();
    virtual void onCollisionRender();

    void setWorld(World* w) {m_world = w;}

    bool isDone();

    Model m_model;

protected:
    World* m_world;
    int m_existence;
    bool m_friendly;

    /*! did this thing get hit by a bullet */
    bool m_hit;
};

#endif // TARGET_H
