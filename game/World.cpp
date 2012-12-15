#include "World.h"
#include "game/Bullet.h"
#include "camera.h"
using std::cout;

World::World(){

    m_score = 0;
    m_cooldown = 0;
    m_maxCooldown = 20;

}

World::~World(){

    for(int i = 0; i < m_entities.size(); i++){
        delete m_entities.at(i);
    }
    m_entities.clear();
}

void World::fireRay(Vector3 pos, Vector3 ray, const OrbitCamera &camera, GLuint textureID){

    if(m_cooldown <= 0){
        m_cooldown = m_maxCooldown;

        Vector2 rotation (-camera.theta * 180/M_PI + 180, -camera.phi * 180/M_PI);
        Bullet* b = new Bullet(pos, ray, rotation, textureID);
        m_entities.push_back(b);
    }

}


void World::onUpdate(){
    m_cooldown--;

    for(int i = 0; i < m_entities.size(); i++){
        for(int j = 0; j < m_entities.size(); j++){
            if(i != j){
                m_entities.at(i)->onCollide(m_entities.at(j));
            }

        }
    }

    for(int i = 0; i < m_entities.size(); i++){
        m_entities.at(i)->onUpdate();
        if(m_entities.at(i)->isDone()){
            if(m_entities.at(i) != NULL){
                delete m_entities.at(i);
                m_entities.removeAt(i);
                i--;
            }
        }
    }
}


