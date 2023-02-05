#ifndef BOUNDARY_H
#define BOUNDARY_H

class Boundary {
public:

    Boundary() = default;

    virtual bool outOfBounds(int ix, int iy){std::cout << "base\n";return false;}

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
        int px = 0,
        int py = 0
    )
    :   minX(mx), 
        minY(my), 
        maxX(Mx), 
        maxY(My), 
        periodicX(px), 
        periodicY(py)
    {}


    bool outOfBounds(int ix, int iy){
        if (
            ix < minX ||
            ix >= maxX ||
            iy < minY ||
            iy >= maxY
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
