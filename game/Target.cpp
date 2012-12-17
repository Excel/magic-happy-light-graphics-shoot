#include "Target.h"


Target::Target(Vector3 pos, Vector2 rotation, GLuint textureID, Model targetModel, bool friendly) : Entity(pos, Vector3(0,0,0), rotation, textureID){
    m_existence = 1;
    m_hit = false;
    setColRadius(0.50f);

    m_colType = COLLISION_BOX;
    m_enemy = true;
    m_friendly = friendly;

    if(!friendly)
    {
        setModel("dragon");
        setShader("refract");
    }
    else
    {
        setModel("gargoyle");
        setShader("reflect");
    }
    m_model = targetModel;


    for(int i = 0; i < 3*m_model.model->numvertices; i++)
    {

        if(i%3 == 0)
        {
            if (m_min.x > m_model.model->vertices[i])
                m_min.x = m_model.model->vertices[i];
            else if (m_max.x < m_model.model->vertices[i])
                m_max.x = m_model.model->vertices[i];

        }
        else if(i%3 == 1)
        {
            if (m_min.y > m_model.model->vertices[i])
                m_min.y = m_model.model->vertices[i];
            else if (m_max.y < m_model.model->vertices[i])
                m_max.y = m_model.model->vertices[i];

        }
        else if(i%3 == 2)
        {
            if (m_min.z > m_model.model->vertices[i])
                m_min.z = m_model.model->vertices[i];
            else if (m_max.z < m_model.model->vertices[i])
                m_max.z = m_model.model->vertices[i];
        }
    }

    m_range = 25;
    m_path = new PBCurve();
    m_curvesToAdd = 3;
    createPath();

    m_t = 0;
    m_dt = 1E-3;
}

Target::~Target(){

    delete m_path;
}


void Target::onUpdate(){
    if(m_hit){
        //m_emitter->updateParticles();       //Move the particles
        m_existence--;

    }

    m_t += m_dt;
    m_pos = getPathPoint();
}

void Target::collisionLogic(){
    m_hit = true;
    if(!m_friendly)
    m_world->setScore(m_world->getScore()+1);
    else
    m_world->setScore(m_world->getScore()-5);


}

void Target::onRender(){
    if(m_hit){
        m_emitter->drawParticles();         //Draw the particles
    }
    glAccum(GL_MULT, 0.9);
    glAccum(GL_ACCUM, 0.1);
    glAccum(GL_RETURN, 1);
    glFlush();

}

void Target::onCollisionRender(){
    if(m_colType == COLLISION_SPHERE){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3.f);
        glColor3f(0.5f, 0.5f, 1.0f);

        glPushMatrix();
        glTranslatef(m_pos.x, m_pos.y, m_pos.z);

        gluSphere(m_quadric, m_colRadius, 10, 10);

        glPopMatrix();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.f);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else if(m_colType == COLLISION_BOX){


        glColor3f(1.f, 1.f, 1.f);
        glPushMatrix();
        glTranslatef(m_pos.x, m_pos.y, m_pos.z);
        glRotatef(m_rotation.x, 0.0f, 1.0f, 0.0f);
        glRotatef(m_rotation.y, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_max.y, m_min.z);
        glVertex3f(m_min.x, m_max.y, m_min.z);
        glVertex3f(m_min.x, m_min.y, m_min.z);
        glVertex3f(m_max.x, m_min.y, m_min.z);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_min.y, m_max.z);
        glVertex3f(m_max.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_min.y, m_max.z);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_max.y, m_min.z);
        glVertex3f(m_max.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_max.y, m_max.z);
        glVertex3f(m_min.x, m_max.y, m_min.z);
        glEnd();

        glBegin(GL_LINE_LOOP);
        glVertex3f(m_max.x, m_min.y, m_max.z);
        glVertex3f(m_min.x, m_min.y, m_max.z);
        glVertex3f(m_min.x, m_min.y, m_min.z);
        glVertex3f(m_max.x, m_min.y, m_min.z);
        glEnd();

        glPopMatrix();
    }
}

bool Target::isDone(){
    if(m_existence <= 0 && m_hit){
        m_world->setNumTargets(m_world->getNumTargets() - 1);
        return true;
    }

    return false;
}


void Target::createPath(){
    if(m_path->getPoints().size() > 0){
        Vector3 prevpoint = m_path->getPoints().at(m_path->getPoints().size() - 2);
        m_path->clear();
        m_path->addPoint(m_pos);
        Vector3 endPoint = -1 * prevpoint;
        m_path->addPoint(endPoint);
    }
    else{
        m_path->clear();
        m_path->addPoint(m_pos);
    }


    while(m_path->getPoints().size() <= 3 * m_curvesToAdd - 2){
        m_path->addPoint(m_pos + Vector3(rand()%m_range - m_range/2, rand()%m_range - m_range/2, rand()%m_range - m_range/2));
    }

    m_path->completeCircuit();
}

Vector3 Target::getPathPoint()
{
    return m_path->getPathPoint(m_t);
}



