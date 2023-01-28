#ifndef FIELDSOURCE_H
#define FIELDSOURCE_H

class FieldSource {
public:

    FieldSource(){}

    virtual ~FieldSource(){}

    virtual void getAtCoordinate(int i, int j, bool & value) = 0;

private:

};

#endif /* FIELDSOURCE_H */
