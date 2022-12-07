#ifndef CPHYSICS_H
#define CPHYSICS_H

/*
    Component for dynamics, i.e applying forces
     and moving.
*/
struct cPhysics {
    double x;
    double y;
    double theta;

    double lastX;
    double lastY;
    double lastTheta;

    double vx;
    double vy;

    double mass;

    double fx;
    double fy;
}

#endif /* CPHYSICS_H */
