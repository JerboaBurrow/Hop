#ifndef CPHYSICS_H
#define CPHYSICS_H

namespace Hop::Object::Component
{
  /*
      Component for dynamics, i.e applying forces
      and moving.
  */
  const double PARTICLE_MASS = 1.0;
  const double EFFECTIVE_MASS = 1.0 / (2.0/PARTICLE_MASS); // mass defined as 1
  
  const double DEFAULT_MASS = 1.0;
  const double DEFAULT_INTERTIA = 0.1;
  const double DEFAULT_TRANSLATIONAL_DRAG = 0.0;
  const double DEFAULT_ROTATIONAL_DRAG = 0.0;

  struct cPhysics 
  {
      double x, y;
      double lastX;
      double lastY;
      double lastTheta;

      double vx;
      double vy;
      double phi;

      double momentOfInertia;
      double mass;

      double fx;
      double fy;

      double omega, tau;

      double translationalDrag;
      double rotationalDrag;

      bool isMoveable;

      cPhysics(double x, double y, double t)
      : x(x), y(y), lastX(x), lastY(y), lastTheta(t),
        vx(0.0),vy(0.0),phi(0.0),momentOfInertia(DEFAULT_INTERTIA),
        mass(DEFAULT_MASS),
        fx(0.0),fy(0.0), omega(0.0), tau(0.0),
        translationalDrag(DEFAULT_TRANSLATIONAL_DRAG), 
        rotationalDrag(DEFAULT_ROTATIONAL_DRAG),
        isMoveable(true)
      {}

      cPhysics
      (
        double x, double y, double t, 
        double td, double rd, double J, double m
      )
      : x(x), y(y), lastX(x), lastY(y), lastTheta(t),
        vx(0.0),vy(0.0),phi(0.0),momentOfInertia(J),
        mass(m),
        fx(0.0),fy(0.0), omega(0.0), tau(0.0),
        translationalDrag(td), rotationalDrag(rd),
        isMoveable(true)
      {}

      cPhysics() = default;
  };
}

#endif /* CPHYSICS_H */
