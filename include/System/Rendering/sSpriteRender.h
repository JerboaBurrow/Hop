#ifndef SSPRITERENDER
#define SSPRITERENDER

#include <vector>
#include <unordered_map>
#include <map>
#include <chrono>

#include <Shader/shaders.h>
#include <Component/cRenderable.h>
#include <Object/entityComponentSystem.h>
#include <gl.h>

namespace Hop::Object
{
    class EntityComponentSystem;
}

namespace Hop::System::Rendering
{
    using Hop::Object::EntityComponentSystem;
    
    using Hop::Object::Component::cRenderable;
    using Hop::Object::Component::cTransform;
    using Hop::Object::Component::cPolygon;
    using Hop::System::Physics::CollisionPrimitive;
    using Hop::System::Physics::Rectangle;
    
    const int MAX_OBJECTS_PER_SHADER = 100000; // 0.4 megabytes per shader
    /*

        Renders objects grouped by shaders

            Assumptions:
                - Each object is a centred quad with centroid x,y, orientation t, and scale s
                    as defined in an objects cTransform component
                - An objects cRenderable component contains it's rgba colour values
                    and/or optionally texture atlas coordinates
                - An objects shader handles these components correctly, conforming to
                    a data model of 

                        "layout(location=0) in vec4 a_position;\n"
                        "layout(location=1) in vec4 a_offset;\n"
                        "layout(location=2) in vec4 a_colour;\n"
                        "layout(location=3) in vec4 a_texOffset;\n
                        "uniform mat4 proj;\n"
                        "uniform float atlasN;\n"

                    a_position is constant quad data (instanced)
                    a_offset is x,y,t,s
                    a_colour, rgba colour
                    a_texOffset is (xy) atlas coord, (zw) spare
                    proj is projection matrix
                    altasN is number of atlas textures along each
                        dimension (square grid)

    */
    class sSpriteRender : public System 
    {
        
    public:

        sSpriteRender()
        {
            initialise();
        }

        ~sSpriteRender();

        // update float verts, loop over all objects
        //  with matching signature
        void update(EntityComponentSystem * m, Shaders * s, bool refresh);
        void draw(Shaders * s);
        void updateAndDraw(EntityComponentSystem * m, Shaders * s, bool refresh);

        void initialise();

    private:

        void addNewShader(std::string handle);
        void addNewObject(Id i, std::string handle);
        void moveOffsets(Id i, std::string oldShader, std::string newShader);
        void updateOffsets(std::string handle);
        void updateColours(std::string handle);
        void updateTexOffsets(std::string handle);
        void updateUtil(std::string handle);
        
        /*
            Keep offsets in contiguous memory per shader
            record also a mapping from an object to a its
            offsets by marking shader name and index
        */
        std::unordered_map<
            std::string,
            std::pair<std::size_t,std::vector<float>>
        > offsets;

        const unsigned OFFSET_COMPONENTS = 4;

        std::unordered_map<
            Id,
            std::pair<std::string,std::size_t>
        > idToIndex;

        // an unordered map will require a 
        //  custom hash function for a pair key
        std::map<
            std::pair<std::string,std::size_t>,
            Id
        > indexToId;

        std::unordered_map<
            std::string,
            std::pair<GLuint,std::vector<GLuint>>
        > shaderBufferObjects;

        GLuint quadVBO;

        float quad[6*4] = 
        {
            // positions  / texture coords
            0.5f,  0.5f, 1.0f, 1.0f,   // top right
            0.5f,  -0.5f, 1.0f, 0.0f,   // bottom right
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f, 1.0f,    // top left 
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            0.5f,  0.5f, 1.0f, 1.0f  // top right
        };

    };

}

#endif /* SSPRITERENDER */
