#ifndef BOUNDARY_H
#define BOUNDARY_H

namespace Hop::World 
{

    template <class D>
    class Boundary 
    {

    public:

        Boundary() = default;

        virtual bool outOfBounds(D x, D y){return false;}
        virtual const bool isHard() const { return false; }

    protected:

        bool hardOutOfBounds = false;

    };

    template <class D>
    class InfiniteBoundary : public Boundary<D> 
    {
        bool outOfBounds(D x, D y){return false;}
    };

    template <class D>
    class FiniteBoundary : public Boundary<D> 
    {

    public:

        FiniteBoundary(
            D mx, 
            D my, 
            D Mx, 
            D My,
            bool hardBottom = false,
            bool hardTop = false,
            bool hardLeft = false,
            bool hardRight = false
        )
        :   minX(mx), 
            minY(my), 
            maxX(Mx), 
            maxY(My), 
            hardTop(hardTop),
            hardBottom(hardBottom),
            hardLeft(hardLeft),
            hardRight(hardRight)
        {
            hardOutOfBounds = hardBottom || hardTop || hardLeft || hardRight;
        }


        bool outOfBounds(int ix, int iy)
        {
            if (
                ix < minX ||
                ix >= maxX ||
                iy < minY ||
                iy >= maxY
            )
            {
                return true;
            }
            return false;
        }

        const bool isHard() const { return hardOutOfBounds; }
        const bool isHardTop() const { return hardTop; }
        const bool isHardBottom() const { return hardBottom; }
        const bool isHardLeft() const { return hardLeft; }
        const bool isHardRight() const { return hardRight; }

        D getMinX() const { return minX; }
        D getMinY() const { return minY; }
        D getMaxX() const { return maxX; }
        D getMaxY() const { return maxY; }

    private:

        D minX, minY, maxX, maxY;
        bool hardTop, hardBottom, hardLeft, hardRight;
        bool hardOutOfBounds;
        
    };

}
#endif /* BOUNDARY_H */
