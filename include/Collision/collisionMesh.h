#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>
#include <cmath>
#include <limits>
#include <cstdint>
#include <memory>
#include <set>
#include <algorithm>

#include <Component/cTransform.h>
#include <Component/cPhysics.h>
#include <Collision/collisionPrimitive.h>
#include <Maths/distance.h>
#include <Maths/rectangle.h>

namespace Hop::System::Physics
{
    using Hop::Object::Component::cTransform;
    using Hop::Object::Component::cPhysics;

    struct MeshPoint
    {
        MeshPoint(double x, double y, double r)
        : x(x), y(y), r(r)
        {}

        MeshPoint()
        : x(0.0), y(0.0), r(0.0)
        {}

        virtual ~MeshPoint() = default;

        double x; double y; double r;

        bool operator==(const MeshPoint & rhs)
        {
            return x == rhs.x && y == rhs.y && r == rhs.r;
        }
    };

    struct MeshRectangle : public MeshPoint
    {

        MeshRectangle()
        : MeshRectangle(0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0)
        {}

        MeshRectangle
        (        
            double llx, double lly,
            double ulx, double uly,
            double urx, double ury,
            double lrx, double lry
        )
        : llx(llx), lly(lly),
          ulx(ulx), uly(uly),
          urx(urx), ury(ury),
          lrx(lrx), lry(lry)
        {
            x = (llx+ulx+urx+lrx)/4.0;
            y = (lly+uly+ury+lry)/4.0;

            double dx = llx-x;
            double dy = lly-y;

            r = std::sqrt(dx*dx+dy*dy);
        }

        bool operator==(const MeshRectangle & rhs)
        {
            return llx == rhs.llx && 
                   lly == rhs.lly &&
                   ulx == rhs.ulx &&
                   uly == rhs.uly &&
                   urx == rhs.urx &&
                   ury == rhs.ury &&
                   lrx == rhs.lrx &&
                   lry == rhs.lry;
        }

        double llx, lly, ulx, uly, urx, ury, lrx, lry;
    };

    struct CollisionMesh 
    {
        CollisionMesh()
        {}
        // construct a mesh around a model space polygon 
        //   with vertices v with each mesh vertex having 
        //   radius r in model space
        //CollisionMesh(std::vector<Vertex> v, double r = 0.01);

        // construct a mesh from given points
        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v,
            double x,
            double y, 
            double theta, 
            double scale
        )
        : CollisionMesh(std::move(v))
        {
            cTransform transform(x,y,theta,scale);
            cPhysics phys(x,y,theta);
            updateWorldMesh(transform, phys, 0.0);
        }

        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v
        )
        : totalEffectiveMass(1.0), gx(0.0), gy(0.0)
        {
            worldVertices.clear();
            for (unsigned i = 0; i < v.size(); i++)
            {
                add(v[i]);
            }
            computeRadius();
        }

        CollisionMesh
        (
            CollisionMesh & m
        )
        {
            vertices = m.vertices;
            worldVertices = m.worldVertices;
            tags = m.tags;

            totalEffectiveMass = m.totalEffectiveMass;

            radius = m.radius;
            gx = m.gx;
            gy = m.gy;
            kineticEnergy = m.kineticEnergy;
            isRigid = m.isRigid;
            needsInit = m.needsInit;
            someRectangles = m.someRectangles;
        }

        CollisionMesh
        (
            std::vector<std::shared_ptr<MeshPoint>> model,
            std::vector<std::shared_ptr<CollisionPrimitive>> world,
            std::set<uint64_t> tags
        )
        : vertices(model), worldVertices(world), tags(tags),
          totalEffectiveMass(0.0), radius(0.0), gx(0.0), gy(0.0),
          kineticEnergy(0.0), isRigid(true), needsInit(true),
          someRectangles(false)
        {
            calculateIsRigid();
            calculateTotalEffectiveMass();
            updateTags();
        }

        CollisionMesh getSubMesh(uint64_t t)
        {
            auto model = getModelByTag(t);
            auto world = getByTag(t);
            std::set<uint64_t> tags = {t};
            return CollisionMesh(model, world, tags);
        }

        void reinitialise() { needsInit = true; }

        void transform(cTransform t)
        {
            needsInit = true;
            cPhysics phys(t.x,t.y,t.theta);
            updateWorldMesh(t, phys, 0.0);
        }
        
        void add(std::shared_ptr<CollisionPrimitive> c)
        {

            auto circ = std::make_shared<MeshPoint>
            (
                c->x, c->y, c->r
            );

            for (auto c : vertices)
            {
                if (*c.get() == *circ.get())
                {
                    return;
                }
            }

            RectanglePrimitive * l = dynamic_cast<RectanglePrimitive*>(c.get());

            std::shared_ptr<CollisionPrimitive> p;

            if (l != nullptr)
            {
                vertices.push_back
                (
                    std::move
                    (
                        std::make_shared<MeshRectangle>
                        (
                            l->llx, l->lly,
                            l->ulx, l->uly,
                            l->urx, l->ury,
                            l->lrx, l->lry
                        )
                    )
                );

                p = std::make_shared<RectanglePrimitive>
                (
                    RectanglePrimitive
                    (
                        l->llx, l->lly,
                        l->ulx, l->uly,
                        l->urx, l->ury,
                        l->lrx, l->lry,
                        l->tag,
                        l->stiffness
                    )
                );

                someRectangles = true;

            }
            else
            {
                vertices.push_back
                (
                    std::move
                    (
                        circ
                    )
                );
                p = std::make_shared<CollisionPrimitive>
                (
                    CollisionPrimitive
                    (
                        c->x,
                        c->y,
                        c->r, 
                        c->tag,
                        c->stiffness, 
                        c->damping, 
                        c->effectiveMass
                    )
                );
            }

            worldVertices.push_back(std::move(p));

            calculateIsRigid();
            calculateTotalEffectiveMass();
            updateTags();

            needsInit = true;
        }

        void remove(size_t i)
        {
            vertices.erase(vertices.begin()+i);
            worldVertices.erase(worldVertices.begin()+i);
            needsInit = true;

            calculateIsRigid();
            calculateTotalEffectiveMass();
            updateTags();
        }

        int clicked(float x, float y)
        {

            for (int j = 0; j < int(worldVertices.size()); j++)
            {
                double rx = worldVertices[j]->x - x;
                double ry = worldVertices[j]->y - y;
                double d2 = rx*rx+ry*ry;

                if (d2 < worldVertices[j]->r*worldVertices[j]->r)
                {
                    return j;
                }
            }

            return -1;
        }

        size_t size(){return vertices.size();}

        std::shared_ptr<MeshPoint> getModelVertex(size_t i)
        {
            return vertices[i];
        }

        std::shared_ptr<CollisionPrimitive> getMeshVertex(size_t i)
        {
            return worldVertices[i];
        }

        std::shared_ptr<CollisionPrimitive> operator[](size_t i) 
        {
            return worldVertices[i];
        }

        void updateWorldMesh(
            cTransform & transform,
            cPhysics & physics,
            double dt
        )
        {
            kineticEnergy = 0.0;
            if (isRigid)
            {
                return updateWorldMeshRigid(transform, dt);
            }
            else 
            {
                return updateWorldMeshSoft(transform, physics, dt);
            }
        }

        void updateWorldMeshRigid(
            const cTransform & transform,
            double dt
        );

        void updateWorldMeshSoft(
            cTransform & transform,
            cPhysics & physics,
            double dt
        );

        double bestAngle(double x, double y, double scale);
        void centerOfMassWorld(double & cx, double & cy);
        void modelToCenterOfMassFrame();

        double momentOfInertia(double x, double y, double mass);
        void computeRadius();
        double getRadius(){return radius;}

        bool getIsRigid(){ return isRigid; }

        void calculateIsRigid() 
        {
            for (auto v : worldVertices)
            {
                if (v->stiffness < CollisionPrimitive::RIGID)
                {
                    isRigid = false;
                    return;
                }
            }
            isRigid = true;
        }

        void applyForce(double fx, double fy, bool global = false)
        {
            if (global)
            {
                gx += fx;
                gy += fy;
            }
            else 
            {
                for (auto w : worldVertices)
                {
                    w->applyForce(fx, fy);
                }
            }

        }

        double getEffectiveMass() const { return totalEffectiveMass; }

        void calculateTotalEffectiveMass() 
        {
            totalEffectiveMass = 0.0;
            for (auto v : worldVertices)
            {
                totalEffectiveMass += v->effectiveMass;
            }

            if (totalEffectiveMass <= 0.0)
            {
                totalEffectiveMass = 1.0;
            }
        }

        double energy()
        {
            return kineticEnergy;
        }
        
        bool areSomeRectangles() const { return someRectangles; }

        void updateTags()
        {
            tags.clear();
            for (const std::shared_ptr<CollisionPrimitive> & c : worldVertices)
            {
                tags.insert(c->tag);
            }
        }

        std::vector<std::shared_ptr<CollisionPrimitive>> getByTag(uint64_t t)
        {
            std::vector<std::shared_ptr<CollisionPrimitive>> v;
            if (tags.find(t) == tags.cend())
            {
                return v;
            }

            for (std::shared_ptr<CollisionPrimitive> & c : worldVertices)
            {
                if (c->tag == t)
                {
                    v.push_back(c);
                }
            }

            return v;
        }

        std::vector<std::shared_ptr<MeshPoint>> getModelByTag(uint64_t t)
        {
            std::vector<std::shared_ptr<MeshPoint>> v;
            if (tags.find(t) == tags.cend())
            {
                return v;
            }

            for (unsigned i = 0; i < size(); i++)
            {
                if (worldVertices[i]->tag == t)
                {
                    v.push_back(vertices[i]);
                }
            }

            return v;
        }

        void removeByTag(uint64_t t)
        {
            std::vector<std::shared_ptr<CollisionPrimitive>> v;

            if (tags.find(t) == tags.cend())
            {
                return;
            }

            unsigned i = 0;
            while (i < worldVertices.size())
            {
                if (worldVertices[i]->tag == t)
                {
                    remove(i);
                }
                else
                {
                    i++;
                }
            }
        }

        Hop::Maths::BoundingBox getBoundingBox() const
        {
            return getBoundingBox(worldVertices);
        }

        Hop::Maths::BoundingBox getBoundingBox
        (
            const std::vector<std::shared_ptr<CollisionPrimitive>> & c
        ) const
        {
            std::vector<Vertex> v(c.size());
            std::transform
            (
                c.begin(), 
                c.end(), 
                v.begin(),
                [](std::shared_ptr<CollisionPrimitive> c)
                {
                    return Vertex(c->x, c->y);
                }
            );

            double r = 0.0;

            for (const std::shared_ptr<CollisionPrimitive> & c : worldVertices)
            {
                r = std::max(r, c->r);
            }
            
            return Hop::Maths::boundingBox(v, r);
        }

        Hop::Maths::BoundingBox getBoundingBox(uint64_t tag)
        {
            std::vector<std::shared_ptr<CollisionPrimitive>> c = getByTag(tag);
            return getBoundingBox(c);
        }

        std::set<uint64_t> getTags() const { return tags; }
        
    private:

        std::vector<std::shared_ptr<MeshPoint>> vertices;
        std::vector<std::shared_ptr<CollisionPrimitive>> worldVertices;
        std::set<uint64_t> tags;

        double totalEffectiveMass;

        double radius;

        double gx, gy, kineticEnergy;

        bool isRigid = true;
        bool needsInit = false;
        bool someRectangles = false;
    };

}

#endif /* COLLISIONMESH_H */
