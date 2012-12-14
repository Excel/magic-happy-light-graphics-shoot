#ifndef WORLD_H
#define WORLD_H

#include <QList>
#include "game/Entity.h"
#include "vector.h"
#include "camera.h"

class World {
public:

    World();

    virtual ~World();

    int getScore() {return m_score;}
    void setScore(int score){m_score = score;}

    void fireRay(Vector3 pos, Vector3 ray, const OrbitCamera &camera, GLuint textureID);

    void onUpdate();
    QList<Entity* > getEntities() {return m_entities;}

    void addEntity(Entity* e) {m_entities.push_back(e);}

protected:

    //Resources

    int m_score;
    int m_cooldown;
    int m_maxCooldown;
    QList<Entity* > m_entities;
};


#endif // WORLD_H
