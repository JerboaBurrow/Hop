#include <System/sRender.h>


namespace Hop::System::Rendering
{

    void sRender::update(EntityComponentSystem * m, Shaders * s, bool refresh)
    {
        
        bool newData = false;
        bool staleData = false;
        std::string handle;
        std::size_t start, offset;
        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            cRenderable & dataR = m->getComponent<cRenderable>(*it);
            cTransform & dataT = m->getComponent<cTransform>(*it);

            if(refresh){
                if (offsets.find(dataR.shaderHandle) == offsets.end())
                {
                    // new shader
                    addNewShader(dataR.shaderHandle);
                    newData = true;
                }

                if (idToIndex.find(*it) == idToIndex.end())
                {
                    // new object
                    addNewObject(*it,dataR.shaderHandle);
                    newData = true;           
                }
                glError("add new object");

                // handle shader change
                if (idToIndex[*it].first != dataR.shaderHandle)
                {
                    moveOffsets(*it,idToIndex[*it].first,dataR.shaderHandle);
                    newData = true;
                }
                glError("move offsets");
            }

            handle = dataR.shaderHandle;
            start = idToIndex[*it].second*OFFSET_COMPONENTS;
            offset = 0;
            
            offsets[handle].second[start] = dataT.x;
            offsets[handle].second[start+1] = dataT.y;
            offsets[handle].second[start+2] = dataT.theta;
            offsets[handle].second[start+3] = dataT.scale*2.0;

            if (newData)
            {
                offset = OFFSET_COMPONENTS*MAX_OBJECTS_PER_SHADER;
                offsets[handle].second[start+offset] = dataR.r;
                offsets[handle].second[start+1+offset] = dataR.g;
                offsets[handle].second[start+2+offset] = dataR.b;
                offsets[handle].second[start+3+offset] = dataR.a;

                offset = 2*OFFSET_COMPONENTS*MAX_OBJECTS_PER_SHADER;
                offsets[handle].second[start+offset] = dataR.ux;
                offsets[handle].second[start+1+offset] = dataR.uy;
                offsets[handle].second[start+2+offset] = dataR.vx;
                offsets[handle].second[start+3+offset] = dataR.vy;


                offset = 3*OFFSET_COMPONENTS*MAX_OBJECTS_PER_SHADER;
                offsets[handle].second[start+offset] = dataR.uA;
                offsets[handle].second[start+1+offset] = dataR.uB;
                offsets[handle].second[start+2+offset] = dataR.uC;
                offsets[handle].second[start+3+offset] = dataR.uD;
            }
        }

        for (auto it = shaderBufferObjects.begin(); it != shaderBufferObjects.end(); it++)
        {
            glBindVertexArray(it->second.first);

            std::shared_ptr<Shader> shader = s->get(it->first);
            shader->use();

            updateOffsets(it->first);
            if (newData){
                updateColours(it->first);
                updateTexOffsets(it->first);
                updateUtil(it->first);
            }

            glError("updateOffsets");
            glBufferStatus("updateOffsets");
        }


    }

    void sRender::updateOffsets(std::string handle)
    {
        size_t cnt = offsets[handle].first+1;
        GLuint oBuffer = shaderBufferObjects[handle].second[0];
        glBindBuffer(GL_ARRAY_BUFFER,oBuffer);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            4*cnt*sizeof(float),
            &offsets[handle].second[0]
        );
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void sRender::updateColours(std::string handle)
    {
        size_t cnt = offsets[handle].first+1;
        GLuint cBuffer = shaderBufferObjects[handle].second[1];
        glBindBuffer(GL_ARRAY_BUFFER,cBuffer);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            4*cnt*sizeof(float),
            &offsets[handle].second[4*MAX_OBJECTS_PER_SHADER]
        );
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void sRender::updateTexOffsets(std::string handle)
    {
        size_t cnt = offsets[handle].first+1;
        GLuint tBuffer = shaderBufferObjects[handle].second[2];
        glBindBuffer(GL_ARRAY_BUFFER,tBuffer);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            4*cnt*sizeof(float),
            &offsets[handle].second[4*MAX_OBJECTS_PER_SHADER*2]
        );
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void sRender::updateUtil(std::string handle)
    {
        size_t cnt = offsets[handle].first+1;
        GLuint tBuffer = shaderBufferObjects[handle].second[3];
        glBindBuffer(GL_ARRAY_BUFFER,tBuffer);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            4*cnt*sizeof(float),
            &offsets[handle].second[4*MAX_OBJECTS_PER_SHADER*3]
        );
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void sRender::addNewShader(std::string handle)
    {
        offsets[handle] = std::pair(-1,std::vector<float>());
        offsets[handle].second.reserve(4*MAX_OBJECTS_PER_SHADER*OFFSET_COMPONENTS);

        for (int i = 0; i < 4*MAX_OBJECTS_PER_SHADER*OFFSET_COMPONENTS; i++)
        {
            offsets[handle].second.push_back(0.0);
        }

        GLuint vboO, vboC, vboT, vboU, vao;
        glGenVertexArrays(1,&vao);
        glGenBuffers(1,&vboO);
        glGenBuffers(1,&vboC);
        glGenBuffers(1,&vboT);
        glGenBuffers(1,&vboU);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*6*sizeof(float),
            &quad[0],
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(0,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glError("add new shader quadVBO");
        // offsets
        glBindBuffer(GL_ARRAY_BUFFER,vboO);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*MAX_OBJECTS_PER_SHADER*sizeof(float),
            &offsets[handle].second[0],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(1,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glError("add new shader offsets VBO");
        // colour
        glBindBuffer(GL_ARRAY_BUFFER,vboC);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*MAX_OBJECTS_PER_SHADER*sizeof(float),
            &offsets[handle].second[MAX_OBJECTS_PER_SHADER*4],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(2,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glError("add new shader colour VBO");
        // texOffset
        glBindBuffer(GL_ARRAY_BUFFER,vboT);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*MAX_OBJECTS_PER_SHADER*sizeof(float),
            &offsets[handle].second[2*MAX_OBJECTS_PER_SHADER*4],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(3,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glError("add new shader tex VBO");
        // util
        glBindBuffer(GL_ARRAY_BUFFER,vboU);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*MAX_OBJECTS_PER_SHADER*sizeof(float),
            &offsets[handle].second[3*MAX_OBJECTS_PER_SHADER*4],
            GL_DYNAMIC_DRAW
        );
        glError("add new shader util data");
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(4,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glError("add new shader util VBO");
        glBindVertexArray(0);

        shaderBufferObjects[handle] = std::pair(
            vao,
            std::vector<GLuint>{vboO,vboC,vboT,vboU}
        );

        glError("add new shader");
    }

    void sRender::addNewObject(Id i,std::string handle)
    {
        std::size_t lastComponent = offsets[handle].first;
        idToIndex[i] = std::pair(handle,lastComponent+1);
        indexToId[idToIndex[i]] = i;
        offsets[handle].first++;
    }

    void sRender::moveOffsets(Id i, std::string oldShader, std::string newShader)
    {
        
        std::size_t start = idToIndex[i].second;
        std::size_t lastComponentOld = offsets[oldShader].first;

        if (start != lastComponentOld)
        {
            // not at end of array replace data
            // with last component
            for (int k = 0; k < 4; k++)
            {
                offsets[oldShader].second[start*4+k] = offsets[oldShader].second[lastComponentOld*4+k];
            }
            Id movedId = indexToId[std::pair(oldShader,lastComponentOld)];
            idToIndex[movedId] = std::pair(oldShader,start);
            indexToId.erase(std::pair(oldShader,lastComponentOld));
            indexToId[std::pair(oldShader,start)] = movedId;
        }
        else
        {
            // at end simply erase
            indexToId.erase(std::pair(oldShader,start));
        }

        offsets[oldShader].first--;

        std::size_t lastComponentNew = offsets[newShader].first;
        idToIndex[i] = std::pair(newShader,lastComponentNew+1);
        indexToId[std::pair(newShader,lastComponentNew+1)] = i;

    }

    void sRender::initialise()
    {
        glGenBuffers(1,&quadVBO);
    }

    sRender::~sRender()
    {
        glDeleteBuffers(1,&quadVBO);
        for (auto it = shaderBufferObjects.begin(); it != shaderBufferObjects.end(); it++)
        {
            glDeleteVertexArrays(1,&(it->second.first));
            for (auto jt = it->second.second.begin(); jt != it->second.second.end(); jt++)
            {
                glDeleteBuffers(1,&(*jt));
            }
        }
    }

    void sRender::draw(Shaders * s, bool debug){
        for (auto it = shaderBufferObjects.begin(); it != shaderBufferObjects.end(); it++)
        {
            glBindVertexArray(it->second.first);
            //glError("sRender draw vao");
            std::shared_ptr<Shader> shader = s->get(it->first);
            shader->use();

            size_t count = offsets[it->first].first+1;
            
            glDrawArraysInstanced(GL_TRIANGLES,0,6,count);
            
            glError("sRender draw");
        }
    }

}