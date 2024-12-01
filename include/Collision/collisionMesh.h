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

    /**
     * @brief A point in a collision mesh.
     * 
     */
    struct MeshPoint
    {
        /**
         * @brief Construct a new Mesh Point from a position and radius.
         * 
         * @param x x coordinate.
         * @param y y coordinate.
         * @param r radius.
         */
        MeshPoint(double x, double y, double r)
        : x(x), y(y), r(r)
        {}

        /**
         * @brief Construct a new Mesh Point at (0,0) with radius 0.
         * 
         */
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

    /**
     * @brief A rectangular mesh point.
     * 
     */
    struct MeshRectangle : public MeshPoint
    {

        /**
         * @brief Construct a new Mesh Rectangle with points at 0.0.
         * 
         */
        MeshRectangle()
        : MeshRectangle(0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0)
        {}

        /**
         * @brief Construct a new Mesh Rectangle from vertices.
         * 
         * @param llx lower left x.
         * @param lly lower left y.
         * @param ulx upper left x.
         * @param uly upper left y.
         * @param urx upper right x.
         * @param ury upper right y.
         * @param lrx lower right x.
         * @param lry lower right y.
         */
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

        double height()
        {
            double u = llx-ulx;
            double v = lly-uly;

            return std::sqrt(u*u+v*v);
        }

        double width()
        {
            double u = llx-lrx;
            double v = lly-lry;

            return std::sqrt(u*u+v*v);
        }

        double llx, lly, ulx, uly, urx, ury, lrx, lry;
    };

    /**
     * @brief A mesh of collideable primitives.
     * 
     */
    struct CollisionMesh
    {
        CollisionMesh()
        {}

        /**
         * @brief Construct a new Collision Mesh from primitives.
         * 
         * @param v list of collideable primitives.
         * @param x x world coordinate of mesh.
         * @param y y world coordinate of mesh.
         * @param theta orientation of mesh.
         * @param scale scale of mesh.
         */
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

        /**
         * @brief Construct a new Collision Mesh from a primitives list.
         * 
         * @param v list of collideable primitives.
         */
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

        /**
         * @brief Construct a new Collision Mesh from another.
         * 
         * @param m the mesh to copy.
         */
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

        /**
         * @brief Construct a new Collision Mesh from model and world vertices.
         * 
         * @param model model positions of mesh primitives.
         * @param world world positions of mesh primitives.
         * @param tags tags of mesh primitives for sub-meshing.
         */
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

        /**
         * @brief Obtain all primitives with the given tag.
         * 
         * @param t tag to select.
         * @return (sub) CollisionMesh selected by tag.
         */
        CollisionMesh getSubMesh(uint64_t t)
        {
            auto model = getModelByTag(t);
            auto world = getByTag(t);
            std::set<uint64_t> tags = {t};
            return CollisionMesh(model, world, tags);
        }

        /**
         * @brief Flag mesh as needing an initial update.
         * 
         */
        void reinitialise() { needsInit = true; }

        /**
         * @brief Set the transform of the mesh.
         * 
         * @param t new transform of the mesh.
         */
        void transform(cTransform t)
        {
            needsInit = true;
            cPhysics phys(t.x,t.y,t.theta);
            updateWorldMesh(t, phys, 0.0);
        }

        /**
         * @brief Add a primitive to the mesh.
         * 
         * @param c new primitive.
         */
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

        /**
         * @brief Remove primitive at index i.
         * 
         * @param i index of primitive to remove.
         */
        void remove(size_t i)
        {
            vertices.erase(vertices.begin()+i);
            worldVertices.erase(worldVertices.begin()+i);
            needsInit = true;

            calculateIsRigid();
            calculateTotalEffectiveMass();
            updateTags();
        }

        /**
         * @brief Check if (x,y) hits a primitive.
         * 
         * @param x x coordinate of test point.
         * @param y y coordinate of test point.
         * @return int index of the first primitive click (or -1 if not clicked).
         */
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

        /**
         * @brief Get a model space primitive.
         * 
         * @param i index of primitive
         * @return std::shared_ptr<MeshPoint> model space primitive.
         */
        std::shared_ptr<MeshPoint> getModelVertex(size_t i)
        {
            return vertices[i];
        }

        /**
         * @brief Get a world space primitive.
         * 
         * @param i index of primitive.
         * @return std::shared_ptr<CollisionPrimitive> world space primitive.
         */
        std::shared_ptr<CollisionPrimitive> getMeshVertex(size_t i)
        {
            return worldVertices[i];
        }

        /**
         * @brief Get a world space primitive.
         * 
         * @param i index of primitive.
         * @return std::shared_ptr<CollisionPrimitive> world space primitive.
         */
        std::shared_ptr<CollisionPrimitive> operator[](size_t i)
        {
            return worldVertices[i];
        }

        /**
         * @brief Update the mesh to a new transform and physics component.
         * 
         * @remark Will integrate mesh points (i.e. handle soft meshes).
         * @param transform new transform for the mesh.
         * @param physics new physics component for the mesh.
         * @param dt timestep.
         */
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

        /**
         * @brief Updates the mesh if it is rigid.
         * 
         * @param transform new transform.
         * @param dt timestep.
         */
        void updateWorldMeshRigid(
            const cTransform & transform,
            double dt
        );

        /**
         * @brief Update the world mesh if it is soft.
         * 
         * @remark Applies internal soft-body forces.
         * @param transform new transform.
         * @param physics mesh physics components.
         * @param dt timestep.
         */
        void updateWorldMeshSoft(
            cTransform & transform,
            cPhysics & physics,
            double dt
        );

        /**
         * @brief The optimal angle to represent the mesh in world space.
         * 
         * @param x x coordinate of the mesh.
         * @param y y coordinate of the mesh.
         * @param scaleX scale of mesh in x.
         * @param scaleY scale of mesh in y.
         * @return double optimal orientation.
         */
        double bestAngle(double x, double y, double scaleX, double scaleY);
        
        void centerOfMassWorld(double & cx, double & cy);
        
        /**
         * @brief Rebase coordinates to centre of mess.
         * 
         */
        void modelToCenterOfMassFrame();

        double momentOfInertia(double x, double y, double mass);
        
        void computeRadius();
        
        double getRadius(){return radius;}

        bool getIsRigid(){ return isRigid; }

        /**
         * @brief Determine if all mesh points are rigid.
         * 
         */
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

        /**
         * @brief Get all world space primitives by tag.
         * 
         * @param t tag to match.
         * @return std::vector<std::shared_ptr<CollisionPrimitive>> world primitives matching the tag.
         */
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

        /**
         * @brief Get all model space primitives by tag.
         * 
         * @param t tag to select.
         * @return std::vector<std::shared_ptr<MeshPoint>> model primitives matching tag.
         */
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

        /**
         * @brief Remove all primitives matching tag.
         * 
         * @param t tag to match.
         */
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

        /**
         * @brief Get the world-space bounding box.
         * 
         * @return Hop::Maths::BoundingBox 
         */
        Hop::Maths::BoundingBox getBoundingBox() const
        {
            return getBoundingBox(worldVertices);
        }

        /**
         * @brief Get the bounding box of a list of primitives.
         * 
         * @param c primitives to bound.
         * @return Hop::Maths::BoundingBox 
         */
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

        /**
         * @brief Get the bounding box of all primitives with tag.
         * 
         * @param tag tag to match.
         * @return Hop::Maths::BoundingBox 
         */
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
