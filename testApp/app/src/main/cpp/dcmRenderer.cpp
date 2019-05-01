#include "dcmRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
dcmVolumeRender::dcmVolumeRender(AAssetManager *assetManager):
        cubeRenderer(assetManager){
    new assetLoader(assetManager);
}

void dcmVolumeRender::addImage(GLubyte * img, float location) {
    images_.push_back(new dcmImage(
            img,
            location));
}

void dcmVolumeRender::assembleTexture() {
    std::sort(images_.begin(), images_.end(),
              [](const dcmImage* img1, const dcmImage* img2){return img1->location < img2->location;});
    dimensions = images_.size();

    size_t data_size = img_width * img_height * dimensions;
    auto *data = new GLubyte[data_size];
    auto each_size = img_height * img_width* UI_SIZE;
    for(int i=0; i<dimensions; i++)
        memcpy(data+i*each_size, images_[i]->data, each_size);

    glGenTextures(1, &volume_texid);
    // bind 3D texture target
    glBindTexture(GL_TEXTURE_3D, volume_texid);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    // pixel transfer happens here from client to OpenGL server
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, (int)img_width, (int)img_height, (int)dimensions, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    delete[]data;


    initGeometry_texturebased();

    initGeometry();
//
}
void dcmVolumeRender::initGeometry_Naive() {
    float vertices[] = {
            -0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,0.0f,
            0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,0.0f,
            0.5f,0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,1.0f,
            0.5f,0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,1.0f,
            -0.5f,0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,1.0f,
            -0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,0.0f,
            -0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,0.0f,
            0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,0.0f,
            0.5f,0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,1.0f,
            0.5f,0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,1.0f,
            -0.5f,0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,1.0f,
            -0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,0.0f,
            -0.5f,0.5f,0.5f,-1.0f,0.0f,0.0f,1.0f,0.0f,
            -0.5f,0.5f,-0.5f,-1.0f,0.0f,0.0f,1.0f,1.0f,
            -0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f,0.0f,1.0f,
            -0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f,0.0f,1.0f,
            -0.5f,-0.5f,0.5f,-1.0f,0.0f,0.0f,0.0f,0.0f,
            -0.5f,0.5f,0.5f,-1.0f,0.0f,0.0f,1.0f,0.0f,
            0.5f,0.5f,0.5f,1.0f,0.0f,0.0f,1.0f,0.0f,
            0.5f,0.5f,-0.5f,1.0f,0.0f,0.0f,1.0f,1.0f,
            0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,0.0f,1.0f,
            0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,0.0f,1.0f,
            0.5f,-0.5f,0.5f,1.0f,0.0f,0.0f,0.0f,0.0f,
            0.5f,0.5f,0.5f,1.0f,0.0f,0.0f,1.0f,0.0f,
            -0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
            0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,1.0f,1.0f,
            0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,0.0f,
            0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,0.0f,
            -0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,0.0f,0.0f,
            -0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
            -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f,1.0f,
            0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,1.0f,1.0f,
            0.5f,0.5f,0.5f,0.0f,1.0f,0.0f,1.0f,0.0f,
            0.5f,0.5f,0.5f,0.0f,1.0f,0.0f,1.0f,0.0f,
            -0.5f,0.5f,0.5f,0.0f,1.0f,0.0f,0.0f,0.0f,
            -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO[0]);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    mProgram = assetLoader::instance()->createGLShaderProgramFromFile("shaders/cube.vert", "shaders/cube.frag");
    if(!mProgram)
        LOGE("===Failed to create shader program===");
    glClearColor(0,0,0,0);
}
void dcmVolumeRender::onViewCreated(){
    program_ray = assetLoader::instance()->createGLShaderProgramFromFile("shaders/raycastVolume.vert", "shaders/raycastVolume.frag");
    program_texture = assetLoader::instance()->createGLShaderProgramFromFile("shaders/textureVolume.vert", "shaders/textureVolume.frag");
    if(!program_ray || !program_texture)
        LOGE("===Failed to create shader program");
}
void dcmVolumeRender::initGeometry() {
    float dx = 0.5f / img_width;
    float dy = 0.5f / img_height;
    float dz = 0.5f / dimensions;

    float x0 = 0.0f + dx;
    float x1 = 1.0f - dx;
    float y0 = 0.0f + dy;
    float y1 = 1.0f - dy;
    float z0 = 0.0f + dz;
    float z1 = 1.0f - dz;

    GLfloat Vertex[] = {//World					//Color
            -0.5f,-0.5f,0.5f,		x0, y0, z1, //0.0f,0.0f,0.0f,	//v0
            0.5f,-0.5f,0.5f,		x1,y0,z1, //1.0f,0.0f,0.0f,	//v1
            0.5f,0.5f,0.5f,			x1, y1, z1,//1.0f,1.0f,0.0f,	//v2
            -0.5f,0.5f,0.5f,		x0,y1,z1, //0.0f,1.0f,0.0f,	//v3
            -0.5f,-0.5f,-0.5f,		x0,y0,z0,//0.0f,0.0f,1.0f,	//v4
            0.5f,-0.5f,-0.5f,		x1,y0,z0,//1.0f,0.0f,1.0f,	//v5
            0.5f,0.5f,-0.5f,		x1,y1,z0, //1.0f,1.0f,1.0f,	//v6
            -0.5f,0.5f,-0.5f,		x0,y1,z0//0.0f,1.0f,1.0f,	//v7
    };

    GLuint Indices[] = { 0,1,2,0,2,3,	//front
                         4,7,6,4,6,5,	//back
                         4,0,3,4,3,7,	//left
                         1,5,6,1,6,2,	//right
                         3,2,6,3,6,7,	//top
                         4,5,1,4,1,0,	//bottom
    };

    glGenBuffers(1, VBO);
    glGenBuffers(1, &EBO);

    // bind buffer for positions and copy data into buffer
    // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // feed the buffer, and let OpenGL know that we don't plan to
    // change it (STATIC) and that it will be used for drawing (DRAW)
    glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GL_FLOAT), Vertex, GL_STATIC_DRAW);

    //Set the index array
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GL_UNSIGNED_INT), Indices, GL_STATIC_DRAW);

    //Generate the VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // bind buffer for positions and copy data into buffer
    // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Unbind the vertex array
    glBindVertexArray(0);


    //Disable Buffers and vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void dcmVolumeRender::initGeometry_texturebased() {
    m_VAOs = new GLuint[dimensions];
    float dimension_inv = 1.0f / dimensions;
    for (int i = 0; i < dimensions; i++)
    {
        float mappedZVal = (-1.0f + 2.0f * (float)i * dimension_inv)/2.0f;

        float zTex = (float)mappedZVal + 0.5f;

        float vertices[] = {
                // positions		// texture coords
                -0.5f,  -0.5f, mappedZVal,	0.0f, 0.0f, zTex,   // top right
                0.5f, -0.5f, mappedZVal,	1.0f, 0.0f, zTex,  // bottom right
                0.5f, 0.5f, mappedZVal,		1.0f, 1.0f, zTex,  // bottom left
                -0.5f,  0.5f, mappedZVal,	0.0f, 1.0f,  zTex  // top left
        };

        unsigned int VBO, EBO;
        unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &m_VAOs[i]);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray((GLuint)m_VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}
void dcmVolumeRender::onDraw() {
//
    if(swithcer_render_texture)
        onTexturebasedDraw();
    else
        onRaycastDraw();
}
void dcmVolumeRender::onNaiveDraw() {
//    glClear(GL_COLOR_BUFFER_BIT);
//    glUseProgram(mProgram);
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,VERTEX);
//    glEnableVertexAttribArray(0);
//    glDrawArrays(GL_TRIANGLES,0,3);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(mProgram);
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uModelMat"), 1, GL_FALSE, &_modelMat[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
void dcmVolumeRender::onTexturebasedDraw(){
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < dimensions; i++) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, volume_texid);

        glUseProgram(program_texture);
        glUniformMatrix4fv(glGetUniformLocation(program_texture, "uProjMat"), 1, GL_FALSE,
                           &(_camera->getProjMat()[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(program_texture, "uViewMat"), 1, GL_FALSE,
                           &(_camera->getViewMat()[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(program_texture, "uModelMat"), 1, GL_FALSE,
                           &_modelMat[0][0]);

        glUniform1i(glGetUniformLocation(program_texture, "uSampler_tex"), 0);
        glBindVertexArray(m_VAOs[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}
void dcmVolumeRender::onRaycastDraw(){
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volume_texid);

    glUseProgram(program_ray);

    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));

    glm::mat4 sliceModel;
    //sliceModel = glm::translate(sliceModel, glm::vec3(0.0f, 0.0f, 0.0f));
    //sliceModel = glm::rotate(sliceModel, (float)glfwGetTime() * 30, glm::vec3(0.0f, 1.0f, 0.0f));
    sliceModel = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 0.5f));
    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uModelMat"), 1, GL_FALSE, &_modelMat[0][0]);

    glUniform1i(glGetUniformLocation(program_ray, "uSampler_tex"), 0);
    glUniform3fv(glGetUniformLocation(program_ray, "uEyePos"), 1, &(_camera->getCameraPosition()[0]));
    glUniform1f(glGetUniformLocation(program_ray, "sample_step_inverse"), adjustParam[0]);
    glUniform1f(glGetUniformLocation(program_ray, "val_threshold"),adjustParam[1]);
    glUniform1f(glGetUniformLocation(program_ray, "brightness"), adjustParam[2]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}