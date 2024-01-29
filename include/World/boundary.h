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
            bool hard = false,
            int px = 0,
            int py = 0
        )
        :   minX(mx), 
            minY(my), 
            maxX(Mx), 
            maxY(My), 
            periodicX(px), 
            periodicY(py)
        {
            hardOutOfBounds = hard;
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

        double getMinX() const { return minX; }
        double getMinY() const { return minY; }
        double getMaxX() const { return maxX; }
        double getMaxY() const { return maxY; }

    private:

        int minX, minY, maxX, maxY;
        bool periodicX, periodicY;
        
    };

}
#endif /* BOUNDARY_H */
