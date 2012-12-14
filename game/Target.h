#ifndef TARGET_H
#define TARGET_H

#include "World.h"
#include "Entity.h"
#include "vector.h"

class Target : public Entity {
public:

    Target(Vector3 pos, Vector2 rotation, GLuint textureID);

    virtual ~Target();

    virtual void onUpdate();
    virtual void onCollide(Entity *e);
    virtual void onRender();
    virtual void onCollisionRender();

    void setWorld(World* w) {m_world = w;}

    bool isDone();

protected:
    World* m_world;
    int m_existence;
    bool m_hit;
};

#endif // TARGET_H
