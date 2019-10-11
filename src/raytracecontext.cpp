#include <iostream>

#include "raytracecontext.h"
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mesh.h"
#include <cstring>

using namespace RT;

RayTraceContext::RayTraceContext()
{

}

void RayTraceContext::init()
{
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertexShaderSource =
        #include "vertexshader.glsl"
    ;

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    const char* fragmentShaderSource =
        #include "fragmentshader.glsl"
    ;

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    std::cout << "shader compilation complete\n";

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    float vertices[] = {
        // first triangle
         1.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f,  1.0f, 0.0f,  // top left
        // second triangle
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);




    // Create triangle buffer
    glGenBuffers(1, &triangle_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangle_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Create Treenode buffer
    glGenBuffers(1, &treenode_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treenode_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Tree) * aabbtree.size(), aabbtree.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, treenode_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    // Set camera buffers;
    eye = glGetUniformLocation(shaderProgram, "eye");
    eye_dir = glGetUniformLocation(shaderProgram, "eye_dir");

    screen_size = glGetUniformLocation(shaderProgram, "size");



}

void RayTraceContext::updateGPUTriangles() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_ssbo);
    //GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    //memcpy(p, triangles.data(), sizeof(Triangle) * triangles.size());
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangle_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void RayTraceContext::updateGPUTreenodes() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treenode_ssbo);
    //GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    //memcpy(p, aabbtree.data(), sizeof(Tree) * aabbtree.size());
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    std::cout << "Storing " << sizeof(Tree) * aabbtree.size() << " treenode bytes\n";

    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Tree) * aabbtree.size(), aabbtree.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, treenode_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTraceContext::updateGPUTrianglesPartial(int b, int e) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_ssbo);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p, triangles.data() + b, sizeof(Triangle) * (e - b));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void RayTraceContext::updateGPUTreenodesPartial(int b, int e) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treenode_ssbo);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p, aabbtree.data() + b, sizeof(Tree) * (e - b));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

Mesh* RayTraceContext::createMesh() {
    Mesh mesh(this);
    meshes.push_back(mesh);


    return &meshes.back();
}

int RayTraceContext::addTriangles(std::vector<Triangle>* triangles) {
    this->triangles.insert(this->triangles.end(), triangles->begin(), triangles->end());
    return this->triangles.size() - triangles->size();
}

int RayTraceContext::addNode(std::vector<Tree>* nodes) {
    this->aabbtree.insert(this->aabbtree.end(), nodes->begin(), nodes->end());

    std::cout << "node 0: " << aabbtree[0].minx << ", " << aabbtree[0].miny << ", " << aabbtree[0].minz << " - " << aabbtree[0].maxx << ", " << aabbtree[0].maxy << ", " << aabbtree[0].maxz << "\n";
    updateGPUTreenodes();
    updateGPUTriangles();

    std::cout << "size of tree class: " << sizeof(Tree) << "\n";


    return this->aabbtree.size() - nodes->size();
}

void RayTraceContext::removeTriangles(int tbegin, int tend) {

}

std::vector<Triangle>* RayTraceContext::getTriangles() {
    return &this->triangles;
}

void RayTraceContext::setCameraPosition(float x, float y, float z) {
    glUniform3f(eye, x, y, z);
}

void RayTraceContext::setCameraDirection(float x, float y, float z) {
    glUniform3f(eye_dir, x, y, z);
}

void RayTraceContext::draw(Window* window) {
    int width, height;
    glfwGetWindowSize(window->glwindow, &width, &height);
    glUniform2i(screen_size, width, height);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

