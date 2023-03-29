#ifndef OBJECT_H
#define OBJECT_H

#include <Object/id.h>
namespace Hop::Object
{

    class Object 
    {

    public:

        Object()
        : id(Id::gen())
        {}

        const Id id;
    };

}
#endif /* OBJECT_H */
