#ifndef CPHYSICS_H
#define CPHYSICS_H

namespace Hop::Object::Component
{
  /*
      Component for dynamics, i.e applying forces
      and moving.
  */
  const double PARTICLE_MASS = 1.0;

  struct cPhysics 
  {
      double lastX;
      double lastY;
      double lastTheta;

      double vx;
      double vy;
      double phi;

      // double mass = 1.0;         mass defined as one
      double momentOfInertia;

      double fx;
      double fy;
      double omega;

      double translationalDrag;
      double rotationalDrag;

      cPhysics(double x, double y, double t)
      : lastX(x), lastY(y), lastTheta(t),
        vx(0.0),vy(0.0),momentOfInertia(0.01),
        phi(0.0),fx(0.0),fy(0.0),omega(0.0),
        translationalDrag(1.0), rotationalDrag(0.01)
      {}

      cPhysics() = default;
  };
}

#endif /* CPHYSICS_H */
