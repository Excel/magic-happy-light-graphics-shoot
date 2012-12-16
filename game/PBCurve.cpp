#include "PBCurve.h"

using std::cout;

PBCurve::PBCurve(){

}

PBCurve::~PBCurve(){

}

void PBCurve::addPoint(Vector3 point){
    m_points.push_back(point);

    if((m_points.size() - 1) % 3 == 0 && (m_points.size() - 1) != 0){
        //add an extra smoothing point
        Vector3 point2 = m_points.at(m_points.size() - 2);
        Vector3 midpoint = (point + point2)/2;
        m_points.push_back(midpoint);
        m_points.swap(m_points.size() - 1, m_points.size() - 2);
    }

    if(m_points.size() <= 4){
        m_curves = 1;
    }
    else if(m_points.size() % 3 == 0){
        m_curves = m_points.size() / 3 - 1;
    }
    else{
        m_curves = m_points.size() / 3;
    }
}


void PBCurve::completeCircuit(){
    //this should add two points to make a collinear beginning/end thing
    if(m_points.size() >= 2 && (m_points.size() + 1) % 3 == 0 ){
        Vector3 midpoint = m_points.at(0);
        Vector3 endpoint = m_points.at(1);
        Vector3 beginpoint = 2 * midpoint - endpoint;
        m_points.push_back(beginpoint);
        m_points.push_back(midpoint);
    }

    if(m_points.size() <= 4){
        m_curves = 1;
    }
    else if((m_points.size() % 3 - 1) == 0){
        m_curves = m_points.size() / 3;
    }
    else{
        m_curves = m_points.size() / 3;
    }


 //   cout<<m_points.size()<<"curves \n";
}

void PBCurve::clear(){
    m_points.clear();

    m_curves = 0;
}

Vector3 PBCurve::getPathPoint(float time){
    if(m_points.size() < 4){
        return Vector3(0, time, 0); //this shouldn't happen in the game
    }


    int index = (int)(floor(time));
    index = (index) % m_curves;
    float t = fmod(time, 1);

    Vector3 point = Vector3(0, 0, 0);

    Vector3 p0 = m_points.at(index*3);
    Vector3 p1 = m_points.at(index*3 + 1);
    Vector3 p2 = m_points.at(index*3 + 2);
    Vector3 p3 = m_points.at(index*3 + 3);
    point += pow(1 - t, 3) * p0;
    point += 3 * (1-t)*(1-t)*t * p1;
    point += 3 * (1-t)*t*t * p2;
    point += t*t*t * p3;

    //cout<<index*3<<" "<<time<<" "<<point<<"\n";
    return point;
}
