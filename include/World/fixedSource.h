#ifndef FIXEDSOURCE_H
#define FIXEDSOURCE_H

class FixedSource : public FieldSource {
public:

    FixedSource(){}

    void getAtCoordinate(int i, int j, bool & value){}

    void save(std::string filename){}
    void load(std::string filename){}

private:
};

#endif /* FIXEDSOURCE_H */
