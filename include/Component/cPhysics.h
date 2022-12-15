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

    double scale;

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

    cPhysics(double x, double y, double t, double s)
    : x(x),y(y),theta(t),
      lastX(x), lastY(y), lastTheta(t),
      vx(0.0),vy(0.0),mass(0.01),momentOfInertia(0.01),
      phi(0.0),fx(0.0),fy(0.0),omega(0.0),scale(s)
    {}

    cPhysics() = default;
};

#endif /* CPHYSICS_H */
