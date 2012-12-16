#ifndef PBCURVE_H
#define PBCURVE_H

#include <QList>
#include "vector.h"

class PBCurve {
public:

    PBCurve();

    virtual ~PBCurve();

    void addPoint(Vector3 point);

    QList<Vector3> getPoints() {return m_points;}

    Vector3 getPathPoint(float time);

protected:

    QList<Vector3> m_points; //what points are in the thing
    int m_curves; //how many group of points are there
};


#endif // PBCURVE_H
