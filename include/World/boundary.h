#ifndef BOUNDARY_H
#define BOUNDARY_H

class Boundary {
public:

    Boundary() = default;

    virtual bool outOfBounds(int ix, int iy) = 0;

};

class InfiniteBoundary : public Boundary {
    bool outOfBounds(int ix, int iy){return false;}
};

class FiniteBoundary : public Boundary {

public:

    FiniteBoundary(
        int mx, 
        int my, 
        int Mx, 
        int My,
        bool px,
        bool py
    )
    :   minX(mx), 
        minY(my), 
        maxX(Mx), 
        maxY(my), 
        periodicX(px), 
        periodicY(py)
    {}


    bool outOfBounds(int ix, int iy){
        if (
            ix < minX ||
            ix > maxX ||
            iy < minY ||
            iy > maxY
        ){
            return true;
        }
        return false;
    }

private:

    int minX, minY, maxX, maxY;
    bool periodicX, periodicY;
    
};

#endif /* BOUNDARY_H */
