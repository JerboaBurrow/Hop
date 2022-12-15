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
    double phi;

    double mass;
    double momentOfInertia;

    double fx;
    double fy;
    double omega;
};

#endif /* CPHYSICS_H */
