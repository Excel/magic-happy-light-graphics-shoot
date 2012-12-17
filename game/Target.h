#ifndef TARGET_H
#define TARGET_H

#include "World.h"
#include "Entity.h"
#include "vector.h"

class Target : public Entity {
public:

    Target(Vector3 pos, Vector2 rotation, Model targetModel, bool friendly);

    virtual ~Target();

    virtual void onUpdate();
    virtual void collisionLogic();
    virtual void onCollisionRender();

    void setWorld(World* w) {m_world = w;}

    bool isDone();

    void createPath();
    Vector3 getPathPoint();

protected:
    World* m_world;
    int m_existence;
    bool m_friendly;

    /*! did this thing get hit by a bullet */
    bool m_hit;


    Model m_model;

    PBCurve *m_path;
    int m_curvesToAdd;

    int m_range;
    float m_t;
    float m_dt;
};

#endif // TARGET_H
