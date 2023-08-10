#ifndef DISTANCE_H
#define DISTANCE_H

#include <Collision/collisionMesh.h>

using Hop::System::Physics::Rectangle;

namespace Hop::Maths
{

    template <class T>
    T pointLineSegmentDistanceSquared(
        T px, T py,
        T ax, T ay,
        T bx, T by,
        T & cx, T & cy
    )
    {

        T rx = bx-ax; T ry = by-ay;

        T length2 = rx*rx+ry*ry;

        T pMINUSaDOTrOVERlength2 = ((px-ax)*rx + (py-ay)*ry)/length2;

        pMINUSaDOTrOVERlength2 = std::max(static_cast<T>(0.0),std::min(static_cast<T>(1.0),pMINUSaDOTrOVERlength2));

        // contact point
        cx = ax + pMINUSaDOTrOVERlength2 * rx;
        cy = ay + pMINUSaDOTrOVERlength2 * ry;

        T dx = px-cx;
        T dy = py-cy;

        return dx*dx+dy*dy;
    }

    template <class T>
    T pointLineSegmentDistanceSquared(
        T px, T py,
        T ax, T ay,
        T bx, T by
    )
    {
        T cx, cy;
        return pointLineSegmentDistanceSquared(px,py,ax,ay,bx,by,cx,cy);
    }

    template <class T>
    T axisOverlap
    (
        Rectangle * r1,
        Rectangle * r2,
        T axisX,
        T axisY
    )
    {
        T ll11 = r1->llx*axisX + r1->lly*axisY;
        T ll12 = r2->llx*axisX + r2->lly*axisY;

        T ul11 = r1->ulx*axisX + r1->uly*axisY;
        T ul12 = r2->ulx*axisX + r2->uly*axisY;

        T ur11 = r1->urx*axisX + r1->ury*axisY;
        T ur12 = r2->urx*axisX + r2->ury*axisY;

        T lr11 = r1->lrx*axisX + r1->lry*axisY;
        T lr12 = r2->lrx*axisX + r2->lry*axisY;

        T m1 = std::min( std::min(ll11, ul11), std::min(ur11, lr11) );
        T M1 = std::max( std::max(ll11, ul11), std::max(ur11, lr11) );
        T m2 = std::min( std::min(ll12, ul12), std::min(ur12, lr12) );
        T M2 = std::max( std::max(ll12, ul12), std::max(ur12, lr12) );

        if (m1 <= M2 && M1 >= m2)
        {
            // intersection on this axis
            return std::min(M2,M1)-std::max(m1,m2);
        }
        else
        {
            return 0.0;
        }
    }

    template <class T>
    bool rectangleRectangleCollided
    (
        Rectangle * r1,
        Rectangle * r2,
        T & nx,
        T & ny,
        T & s
    )
    {

        nx = 0.0;
        ny = 0.0;
        s = 0.0;

        T s11 = 0.0;
        T s12 = 0.0;
        T s21 = 0.0;
        T s22 = 0.0;
        
        s11 = axisOverlap<T>(r1,r2,r1->axis1x,r1->axis1y);

        if (s11 <= 0.0){ return false; }

        // assume s11 is smallest overlap
        nx = r1->axis1x; ny = r1->axis1y; s = s11;
    
        s12 = axisOverlap<T>(r1,r2,r1->axis2x,r1->axis2y);
 
        if (s12 <= 0.0){ return false; }

        if (s12 < s)
        {
            nx = r1->axis2x; ny = r1->axis2y; s = s12;
        }

        s21 = axisOverlap<T>(r1,r2,r2->axis1x,r2->axis1y);

        if (s21 <= 0.0){ return false; }

        if (s21 < s)
        {
            nx = r2->axis1x; ny = r2->axis1y; s = s21;
        }

        s22 = axisOverlap<T>(r1,r2,r2->axis2x,r2->axis2y);

        if (s22 <= 0.0){ return false; }

        if (s22 < s)
        {
            nx = r2->axis2x; ny = r2->axis2y; s = s22;
        }

        return true;

    }

    template <class T>
    T sdf(Rectangle * r, T px, T py)
    {
        // thickness
        T thx = r->llx-r->lrx;
        T thy = r->lly-r->lry;
        T th = std::sqrt(thx*thx+thy*thy);
        // line through middle
        T ax = (r->llx + r->lrx)/2.0;
        T ay = (r->lly + r->lry)/2.0;
        T bx = (r->ulx + r->urx)/2.0;
        T by = (r->uly + r->ury)/2.0;

        T lx = bx-ax;
        T ly = by-ay;
        T l = std::sqrt(lx*lx+ly*ly);

        lx /= l;
        ly /= l;

        // midpoint to point p
        T qx = px-(ax+bx)*0.5;
        T qy = py-(ay+by)*0.5;

        T qxt = lx*qx - ly*qy;
        qy = ly*qx + lx*qy;
        qx = qxt;

        qx = std::abs(qx)-l*0.5;
        qy = std::abs(qy)-th*0.5;

        qxt = std::max(qx,0.0);
        T qyt = std::max(qy,0.0);

        T d = std::sqrt(qxt*qxt+qyt*qyt);

        return d + std::min(std::max(qx,qy),0.0); 

    }

    template <class T>
    void shortestDistanceSquared(T px, T py, Rectangle * r, T & nx, T & ny, T & d)
    {
        T d2, nxt, nyt;

        d = pointLineSegmentDistanceSquared<T>(
            px, py,
            r->llx, r->lly,
            r->ulx, r->uly,
            nx, ny
        );

        // rx = r->llx-px;
        // ry = r->lly-py;

        // d2 = rx*rx+ry*ry;

        // if (d2 < d)
        // {
        //     d = d2;
        //     s = std::sqrt(d);
        //     nx = rx / s;
        //     ny = ry / s;
        // }

        d2 = pointLineSegmentDistanceSquared<T>(
            px, py,
            r->ulx, r->uly,
            r->urx, r->ury,
            nxt, nyt
        );

        if (d2 < d)
        {
            d = d2; nx = nxt; ny = nyt;
        }

        // rx = r->ulx-px;
        // ry = r->uly-py;

        // d2 = rx*rx+ry*ry;

        // if (d2 < d)
        // {
        //     d = d2;
        //     s = std::sqrt(d);
        //     nx = rx / s;
        //     ny = ry / s;
        // }

        d2 = pointLineSegmentDistanceSquared<T>(
            px, py,
            r->urx, r->ury,
            r->lrx, r->lry,
            nxt, nyt
        );

        if (d2 < d)
        {
            d = d2; nx = nxt; ny = nyt;
        }

        // rx = r->urx-px;
        // ry = r->ury-py;

        // d2 = rx*rx+ry*ry;

        // if (d2 < d)
        // {
        //     d = d2;
        //     s = std::sqrt(d);
        //     nx = rx / s;
        //     ny = ry / s;
        // }

        d2 = pointLineSegmentDistanceSquared<T>(
            px, py,
            r->lrx, r->lry,
            r->llx, r->lly,
            nxt, nyt
        );

        if (d2 < d)
        {
            d = d2; nx = nxt; ny = nyt;
        }

        // rx = r->lrx-px;
        // ry = r->lry-py;

        // d2 = rx*rx+ry*ry;

        // if (d2 < d)
        // {
        //     d = d2;
        //     s = std::sqrt(d);
        //     nx = rx / s;
        //     ny = ry / s;
        // }
    }

}

#endif /* DISTANCE_H */
