// #include <texturedQuad.h>

// TexturedQuad::TexturedQuad(uint64_t s, std::unique_ptr<float[]> image, glm::mat4 p)
// : size(s), projection(p)
// {
//     data = new float [s*s*3];
//     for (int i = 0; i < s*s*3; i++){
//         data[i] = image[i];
//     }

//     glGenTextures(1,&tex);
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D,tex);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

//     glTexImage2D(
//         GL_TEXTURE_2D,
//         0,
//         GL_RGB32F,
//         size,
//         size,
//         0,
//         GL_RGB,
//         GL_FLOAT,
//         data
//     );
//     glBindTexture(GL_TEXTURE_2D,0);
//     glError();
    
//     glGenVertexArrays(1,&VAO);
//     glGenBuffers(1,&VBO);
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER,sizeof(float)*6*4,vertices,GL_STATIC_DRAW);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float),0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);

//     shader = glCreateProgram();
//     compileShader(shader,texturedQuadVertexShader,texturedQuadFragmentShader);
//     glUseProgram(shader);

//     glUniformMatrix4fv(
//         glGetUniformLocation(shader,"proj"),
//         1,
//         GL_FALSE,
//         &projection[0][0]
//     );

//     glUniform1i(
//         glGetUniformLocation(shader,"tex"),
//         0
//     );
// }

// void TexturedQuad::draw(float scale, float x, float y){
//         glActiveTexture(GL_TEXTURE0);
//         glBindTexture(GL_TEXTURE_2D,tex);
//         glBindVertexArray(VAO);

//         glUseProgram(shader);
//         glUniform1f(
//           glGetUniformLocation(shader,"scale"),
//           scale
//         );
//         glUniform2f(
//           glGetUniformLocation(shader,"offset"),
//           x,y
//         );

//         glDrawArrays(GL_TRIANGLES,0,6);
//     }
