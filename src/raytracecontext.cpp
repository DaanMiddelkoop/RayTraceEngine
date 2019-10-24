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
    // Claim first node of the aabb tree so we can manage objects.
    aabbtree.push_back(Tree());


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

    // Create Material buffer
    glGenBuffers(1, &materials_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materials_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material) * materials.size(), materials.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materials_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &textures_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textures_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Texture) * textures.size(), textures.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, textures_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &pixels_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixels_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * pixels.size(), pixels.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, pixels_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &transforms_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix4x4) * transforms.size(), transforms.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, transforms_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    // Set camera buffers;
    eye = glGetUniformLocation(shaderProgram, "eye");
    eye_dir = glGetUniformLocation(shaderProgram, "eye_dir");

    screen_size = glGetUniformLocation(shaderProgram, "size");

    debug_info = glGetUniformLocation(shaderProgram, "debug");

    scene_root = glGetUniformLocation(shaderProgram, "scene_root");


}

void RayTraceContext::updateGPUTriangles() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangle_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void RayTraceContext::updateGPUTreenodes() {
    std::cout << "Updating gpu treenodes " << std::endl;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treenode_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Tree) * aabbtree.size(), aabbtree.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, treenode_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTraceContext::updateGPUMaterials() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materials_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material) * materials.size(), materials.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, materials_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTraceContext::updateGPUTextures() {

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, textures_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Texture) * textures.size(), textures.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, textures_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTraceContext::updateGPUPixels() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pixels_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * pixels.size(), pixels.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, pixels_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTraceContext::updateGPUTransforms() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Matrix4x4) * transforms.size(), transforms.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, transforms_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTraceContext::updateGPUTransform(int transform_id) {

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, transforms_ssbo);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p + (transform_id * sizeof(Matrix4x4)), ((void*)transforms.data()) + (transform_id * sizeof(Matrix4x4)), sizeof(Matrix4x4));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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

void RayTraceContext::updateGPUTreenodesPath(int node) {
    return;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treenode_ssbo);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(p + node * sizeof(Tree), &aabbtree[node], sizeof(Tree));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    if (aabbtree[node].parent != -1) {
        updateGPUTreenodesPath(aabbtree[node].parent);
    }
}

Mesh* RayTraceContext::createMesh() {
    Mesh* mesh = new Mesh(this);
    meshes.push_back(mesh);

    return meshes.back();
}

MaterialHandle RayTraceContext::createMaterial() {
    materials.push_back(Material());

    return MaterialHandle(this, materials.size() - 1);
}

Material* RayTraceContext::getMaterial(int material_id) {
    return &(materials[material_id]);
}

TextureHandle RayTraceContext::createTexture() {
    textures.push_back(Texture());

    return TextureHandle(this, textures.size() - 1);
}

Texture* RayTraceContext::getTexture(int texture_id) {
    std::cout << "Retrieving texture id: " << texture_id;

    return &(textures[texture_id]);
}

std::vector<unsigned int>* RayTraceContext::getPixels() {
    return &pixels;
}

int RayTraceContext::addTriangles(std::vector<Triangle>* triangles) {
    this->triangles.insert(this->triangles.end(), triangles->begin(), triangles->end());
    return this->triangles.size() - triangles->size();
}

int RayTraceContext::addNode(std::vector<Tree>* nodes) {

    this->aabbtree.reserve(this->aabbtree.size() + nodes->size() + 1);

    int oldRoot = recoverSceneRoot();

    this->aabbtree[oldRoot].insertNode(&aabbtree, oldRoot, (*nodes)[0]);

    int currentNodeSize = this->aabbtree.size() - 1;
    for (int i = 1; i < nodes->size(); i++) {
        (*nodes)[i].parent += currentNodeSize;

        if (!(*nodes)[i].leaf) {
            (*nodes)[i].node1 += currentNodeSize;
            (*nodes)[i].node2 += currentNodeSize;
        }
    }

    int objectRoot = this->aabbtree.size() - 1;
    this->aabbtree.at(objectRoot).node1 += currentNodeSize;
    this->aabbtree.at(objectRoot).node2 += currentNodeSize;


    this->aabbtree.insert(this->aabbtree.end(), nodes->begin() + 1, nodes->end());


    this->aabbtree.at(objectRoot).setDepths(&this->aabbtree);


    updateGPUTreenodes();
    updateGPUTriangles();

    int sceneroot = 0;
    while (this->aabbtree.at(sceneroot).parent != -1) {
        sceneroot = this->aabbtree.at(sceneroot).parent;
    }

    setSceneRoot(sceneroot);






    return objectRoot;
}

std::vector<Tree>* RayTraceContext::getNodes() {
    return &this->aabbtree;
}

void RayTraceContext::removeTriangles(int tbegin, int tend) {

}

std::vector<Triangle>* RayTraceContext::getTriangles() {
    return &this->triangles;
}

int RayTraceContext::addTransform(Matrix4x4* transform) {
    this->transforms.push_back(*transform);
    return this->transforms.size() - 1;
}

std::vector<Matrix4x4>* RayTraceContext::getTransforms() {
    return &this->transforms;
}

void RayTraceContext::setCameraPosition(float x, float y, float z) {
    glUniform3f(eye, x, y, z);
}

void RayTraceContext::setCameraDirection(float x, float y, float z) {
    glUniform3f(eye_dir, x, y, z);
}

void RayTraceContext::setDebugInfo(float x, float y, float z) {
    glUniform3f(debug_info, x, y, z);
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

void RayTraceContext::setSceneRoot(int root) {
    glUniform1i(scene_root, root);
}

int RayTraceContext::recoverSceneRoot() {
    int sceneroot = 0;
    while (this->aabbtree.at(sceneroot).parent != -1) {
        sceneroot = this->aabbtree.at(sceneroot).parent;
    }
    setSceneRoot(sceneroot);
    return sceneroot;
}

