#ifndef ENTITY_H
#define ENTITY_H

#include "vector.h"
#include "particleemitter.h"
#include "glm.h"

#include "resourceloader.h"

class Entity{
public:
    //! Enumeration for collision types.
    enum CollisionType {
       COLLISION_SPHERE, COLLISION_BOX
    };

    /*!
      pos - position of entity
      ray - direction of entity
      rotation - rotation of entity
      textureID - texture for particles
      */
    Entity(Vector3 pos, Vector3 ray, Vector2 rotation, GLuint textureID);

    virtual ~Entity();

    /*! generally used for moving and existing */
    virtual void onUpdate() = 0;
    /*! logic to determine if this entity collides with another entity */
    void onCollide(Entity* e);
    /*! logic to determine what happens in a collision */
    virtual void collisionLogic() = 0;
    /*! how to draw the entity */
    virtual void onRender() = 0;
    /*! how to draw the collision shape of the entity */
    virtual void onCollisionRender() = 0;

    /*! whether to remove this after onUpdate() finishes for World */
    virtual bool isDone() = 0;

    Vector3 getPos() {return m_pos;}
    Vector3 getDir() {return m_dir;}
    Vector2 getRotation() {return m_rotation;}

    /*! getShader() returns a string that'll hash into the shader hashmap in glwidget */
    QString getShader() {return m_shader;}
    void setShader(QString shader) {m_shader = shader;}

    /*! differentiate between bullets (false) and targets/specials (true) */
    bool isEnemy() {return m_enemy;}

    CollisionType getCollisionType() {return m_colType;}
    float getColRadius() {return m_colRadius;}
    Vector3 getColMin() {return m_colMin;}
    Vector3 getColMax() {return m_colMax;}
    void setColRadius(float r) {m_colRadius = r;}
    QString getModel() {return m_model;}
    void setModel(QString model) {m_model = model;}
    ParticleEmitter * m_emitter;

    Vector3 m_max; //defines bounding box
    Vector3 m_min;

protected:
    //Physics
    Vector3 m_pos;
    Vector3 m_dir;
    /*! x = rotate by theta, y = rotate by phi*/
    Vector2 m_rotation;


    //Rendering
    QString m_model;
    QString m_shader;

    //Collision
    GLUquadric *m_quadric;
    GLUtesselator *m_tess;
    CollisionType m_colType;
    float m_colRadius;
    Vector3 m_colMin, m_colMax;
    bool m_enemy;


};


#endif // ENTITY_H
