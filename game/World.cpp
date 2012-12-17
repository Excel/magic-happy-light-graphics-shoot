#include "World.h"
#include "game/Bullet.h"
#include "camera.h"
using std::cout;

World::World(){

    m_score = 0;
    m_cooldown = 0;
    m_maxCooldown = 20;

    m_t = 0;
    m_dt = 1.75E-3;

    m_targets = 0;
    m_maxTargets = 20;

    m_path = new PBCurve();

    m_range = 110;
    m_curvesToAdd = 60;

    createPath();

}

World::~World(){

    for(int i = 0; i < m_entities.size(); i++){
        delete m_entities.at(i);
    }
    m_entities.clear();
    delete m_path;
}

void World::fireRay(Vector3 pos, Vector3 ray, const OrbitCamera &camera, Model bulletModel){

    if(m_cooldown <= 0){
        m_cooldown = m_maxCooldown;

        Vector2 rotation (-camera.theta * 180/M_PI + 180, -camera.phi * 180/M_PI);
        Bullet* b = new Bullet(pos, ray, rotation, bulletModel);
        m_entities.push_back(b);
    }

}

void World::addTarget(Entity *e){
    if(m_targets < m_maxTargets){
        m_entities.push_back(e);
        m_targets++;
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


    m_t += m_dt;

    //determine whether or not to construct a new bezier path
    int index = (int)(floor(m_t));
    index = (index) % m_curvesToAdd;
    float t = fmod(m_t, 1);

    if(index == 0 && t < m_dt){
        m_t = 0;
        createPath();
    }
}

void World::createPath(){
    if(m_path->getPoints().size() > 0){
        Vector3 prevpoint = m_path->getPoints().at(m_path->getPoints().size() - 2);
        m_path->clear();
        m_path->addPoint(Vector3(0, 0, 0));
        Vector3 endPoint = -1 * prevpoint;
        m_path->addPoint(endPoint);
    }
    else{
        m_path->clear();
        m_path->addPoint(Vector3(0, 0, 0));
    }


    while(m_path->getPoints().size() <= 3 * m_curvesToAdd - 2){
        m_path->addPoint(Vector3(rand()%m_range - m_range/2, rand()%m_range - m_range/2, rand()%m_range - m_range/2));
    }

    m_path->completeCircuit();
}

Vector3 World::getPathPoint()
{
    return m_path->getPathPoint(m_t);
}


