#include "PBCurve.h"

using std::cout;

PBCurve::PBCurve(){

}

PBCurve::~PBCurve(){

}

void PBCurve::addPoint(Vector3 point){
    m_points.push_back(point);

    if(m_points.size() % 3 == 0){
        m_curves = m_points.size() / 3 - 1;
    }
    else{
        m_curves = m_points.size() / 3;
    }

    cout<<m_curves<<" "<<m_points.size()<<"\n";

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

    cout<<index*3<<" "<<time<<" "<<point<<"\n";
    return point;
}
