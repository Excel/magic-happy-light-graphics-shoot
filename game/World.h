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

    /*! shoots a bullet in the target direction */
    void fireRay(Vector3 pos, Vector3 ray, const OrbitCamera &camera, GLuint textureID);

    /*! updates the entities in the world */
    void onUpdate();
    QList<Entity* > getEntities() {return m_entities;}

    void addEntity(Entity* e) {m_entities.push_back(e);}

protected:

    //Resources
    int m_score;    // current score
    int m_cooldown; // cooldown of weapon
    int m_maxCooldown;  // max cooldown of weapon
    QList<Entity* > m_entities; // bullets, targets, special targets, whatever man
};


#endif // WORLD_H
