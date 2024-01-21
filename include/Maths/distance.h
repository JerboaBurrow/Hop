#ifndef DISTANCE_H
#define DISTANCE_H

#include <Maths/rectangle.h>

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
        Rectangle r1,
        Rectangle r2,
        T axisX,
        T axisY
    )
    {
        T ll11 = r1.ll.x*axisX + r1.ll.y*axisY;
        T ll12 = r2.ll.x*axisX + r2.ll.y*axisY;

        T ul11 = r1.ul.x*axisX + r1.ul.y*axisY;
        T ul12 = r2.ul.x*axisX + r2.ul.y*axisY;

        T ur11 = r1.ur.x*axisX + r1.ur.y*axisY;
        T ur12 = r2.ur.x*axisX + r2.ur.y*axisY;

        T lr11 = r1.lr.x*axisX + r1.lr.y*axisY;
        T lr12 = r2.lr.x*axisX + r2.lr.y*axisY;

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
        Rectangle r1,
        Rectangle r2,
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
        
        s11 = axisOverlap<T>(r1,r2,r1.axis1.x,r1.axis1.y);

        if (s11 <= 0.0){ return false; }

        // assume s11 is smallest overlap
        nx = r1.axis1.x; ny = r1.axis1.y; s = s11;
    
        s12 = axisOverlap<T>(r1,r2,r1.axis2.x,r1.axis2.y);
 
        if (s12 <= 0.0){ return false; }

        if (s12 < s)
        {
            nx = r1.axis2.x; ny = r1.axis2.y; s = s12;
        }

        s21 = axisOverlap<T>(r1,r2,r2.axis1.x,r2.axis1.y);

        if (s21 <= 0.0){ return false; }

        if (s21 < s)
        {
            nx = r2.axis1.x; ny = r2.axis1.y; s = s21;
        }

        s22 = axisOverlap<T>(r1,r2,r2.axis2.x,r2.axis2.y);

        if (s22 <= 0.0){ return false; }

        if (s22 < s)
        {
            nx = r2.axis2.x; ny = r2.axis2.y; s = s22;
        }

        return true;

    }

    template <class T>
    T sdf(Rectangle r, T px, T py)
    {
        // thickness
        T thx = r.ll.x-r.lr.x;
        T thy = r.ll.y-r.lr.y;
        T th = std::sqrt(thx*thx+thy*thy);
        // line through middle
        T ax = (r.ll.x + r.lr.x)/2.0;
        T ay = (r.ll.y + r.lr.y)/2.0;
        T bx = (r.ul.x + r.ur.x)/2.0;
        T by = (r.ul.y + r.ur.y)/2.0;

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
    void shortestDistanceSquared(T px, T py, Rectangle r, T & nx, T & ny, T & d)
    {
        T d2, nxt, nyt;

        d = pointLineSegmentDistanceSquared<T>(
            px, py,
            r.ll.x, r.ll.y,
            r.ul.x, r.ul.y,
            nx, ny
        );

        // rx = r.ll.x-px;
        // ry = r.ll.y-py;

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
            r.ul.x, r.ul.y,
            r.ur.x, r.ur.y,
            nxt, nyt
        );

        if (d2 < d)
        {
            d = d2; nx = nxt; ny = nyt;
        }

        // rx = r.ulx-px;
        // ry = r.uly-py;

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
            r.ur.x, r.ur.y,
            r.lr.x, r.lr.y,
            nxt, nyt
        );

        if (d2 < d)
        {
            d = d2; nx = nxt; ny = nyt;
        }

        // rx = r.urx-px;
        // ry = r.ury-py;

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
            r.lr.x, r.lr.y,
            r.ll.x, r.ll.y,
            nxt, nyt
        );

        if (d2 < d)
        {
            d = d2; nx = nxt; ny = nyt;
        }

        // rx = r.lr.x-px;
        // ry = r.lr.y-py;

        // d2 = rx*rx+ry*ry;

        // if (d2 < d)
        // {
        //     d = d2;
        //     s = std::sqrt(d);
        //     nx = rx / s;
        //     ny = ry / s;
        // }
    }

    template <class F>
    F angleDistanceAtan2(F from, F to)
    {
        /*

            Signed angular distance from angle "from" to angle "to" given from 
              the atan2 convetion, [-pi, pi]. 
        
        */
        F d = to - from;
        d += (d>M_PI) ? -2.0*M_PI : (d<-M_PI) ? 2.0*M_PI : 0.0;
        return d;
    }

}

#endif /* DISTANCE_H */
