#ifndef BOUNDARY_H
#define BOUNDARY_H

namespace Hop::World 
{

    class Boundary 
    {

    public:

        Boundary() = default;

        virtual bool outOfBounds(int ix, int iy){return false;}
        virtual const bool isHard() const { return false; }

    protected:

        bool hardOutOfBounds = false;

    };

    class InfiniteBoundary : public Boundary 
    {
        bool outOfBounds(int ix, int iy){return false;}
    };

    class FiniteBoundary : public Boundary 
    {

    public:

        FiniteBoundary(
            int mx, 
            int my, 
            int Mx, 
            int My,
            bool hardBottom = false,
            bool hardTop = false,
            bool hardLeft = false,
            bool hardRight = false,
            int px = 0,
            int py = 0
        )
        :   minX(mx), 
            minY(my), 
            maxX(Mx), 
            maxY(My), 
            periodicX(px), 
            periodicY(py),
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

        double getMinX() const { return minX; }
        double getMinY() const { return minY; }
        double getMaxX() const { return maxX; }
        double getMaxY() const { return maxY; }

    private:

        int minX, minY, maxX, maxY;
        bool periodicX, periodicY;
        bool hardTop, hardBottom, hardLeft, hardRight;
        
    };

}
#endif /* BOUNDARY_H */
