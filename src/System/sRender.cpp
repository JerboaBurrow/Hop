#include <System/sRender.h>


namespace Hop::System::Rendering
{

    void sRender::processThreaded(ObjectManager * m, Shaders * s, size_t threadId)
    {
        for (auto it = threadJobs[threadId].begin(); it != threadJobs[threadId].end(); it++)
        {
            cRenderable & dataR = m->getComponent<cRenderable>(*it);
            cTransform & dataT = m->getComponent<cTransform>(*it);

            std::string handle = idToIndex[*it].first;
            std::size_t start = idToIndex[*it].second;

            size_t offset = 0;
            
            offsets[handle].second[start*4] = dataT.x;
            offsets[handle].second[start*4+1] = dataT.y;
            offsets[handle].second[start*4+2] = dataT.theta;
            offsets[handle].second[start*4+3] = dataT.scale*2.0;
        }
    }

    void sRender::updateThreaded(ObjectManager * m, Shaders * s)
    {


        for (int j = 0; j < threadJobs.size(); j++)
        {
            m->postJob(
                std::bind(
                    &sRender::processThreaded,
                    this,
                    m,
                    s,
                    j
                )
            );
        }
        
        m->waitForJobs();

        for (auto it = shaderBufferObjects.begin(); it != shaderBufferObjects.end(); it++)
        {
            glBindVertexArray(it->second.first);

            std::shared_ptr<Shader> shader = s->get(it->first);
            shader->use();

            updateOffsets(it->first);
        }
    }


    void sRender::update(ObjectManager * m, Shaders * s, bool refresh)
    {
        if (m->isThreaded() && !refresh)
        {
            updateThreaded(m,s);
            return;
        }

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
                glError("add new shader");

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
            start = idToIndex[*it].second*4;
            offset = 0;
            
            offsets[handle].second[start] = dataT.x;
            offsets[handle].second[start+1] = dataT.y;
            offsets[handle].second[start+2] = dataT.theta;
            offsets[handle].second[start+3] = dataT.scale*2.0;

            if (newData)
            {
                offset = 4*MAX_OBJECTS_PER_SHADER;
                offsets[handle].second[start+offset] = dataR.r;
                offsets[handle].second[start+1+offset] = dataR.g;
                offsets[handle].second[start+2+offset] = dataR.b;
                offsets[handle].second[start+3+offset] = dataR.a;

                offset = 2*4*MAX_OBJECTS_PER_SHADER;
                offsets[handle].second[start+offset] = dataR.ux;
                offsets[handle].second[start+1+offset] = dataR.uy;
                offsets[handle].second[start+2+offset] = dataR.vx;
                offsets[handle].second[start+3+offset] = dataR.vy;
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
            }
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
            &offsets[handle].second[2*4*MAX_OBJECTS_PER_SHADER]
        );
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void sRender::addNewShader(std::string handle)
    {
        offsets[handle] = std::pair(-1,std::vector<float>());
        offsets[handle].second.reserve(3*MAX_OBJECTS_PER_SHADER*4);

        for (int i = 0; i < 3*MAX_OBJECTS_PER_SHADER*4; i++)
        {
            offsets[handle].second.push_back(0.0);
        }

        GLuint vboO, vboC, vboT, vao;
        glGenVertexArrays(1,&vao);
        glGenBuffers(1,&vboO);
        glGenBuffers(1,&vboC);
        glGenBuffers(1,&vboT);
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
        // colour
        glBindBuffer(GL_ARRAY_BUFFER,vboC);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*MAX_OBJECTS_PER_SHADER*sizeof(float),
            &offsets[handle].second[4*MAX_OBJECTS_PER_SHADER],
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
        // texOffset
        glBindBuffer(GL_ARRAY_BUFFER,vboT);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*MAX_OBJECTS_PER_SHADER*sizeof(float),
            &offsets[handle].second[2*4*MAX_OBJECTS_PER_SHADER],
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
        glBindVertexArray(0);

        shaderBufferObjects[handle] = std::pair(
            vao,
            std::vector<GLuint>{vboO,vboC,vboT}
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