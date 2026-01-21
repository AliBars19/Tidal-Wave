#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../assets/shader_s.h"

#include <iostream>

//functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drawQuad(float x, float y, float width, float height, glm::vec3 color,float rotation,const unsigned int shaderID);
void releaseQuad();
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int VBO, VAO, EBO;

struct player{
    float posX = 2.0;
    float posY = 8.0;

    float velX = 3.0;
    float velY = 0.0;

    const float width = 1.0;
    const float height = 1.0;

    const glm::vec3 color = glm::vec3(0.145f,0.588f,0.745f); 

    bool touchingGround = false;
    bool isAlive = true;

    void update(float dt){
        if(!touchingGround){
            velY -= 20.0f * dt;
        }
        posY += velY * dt;
        posX += velX * dt;
        if(posY <= 2.0f){
            posY = 2.0f;
            velY = 0;
            touchingGround = true;
        }
    }
    void playerDraw(unsigned int shaderID){
        drawQuad(posX,posY,1.25,1.25,color,0.0,shaderID);
    }
    void jump(){
        if(touchingGround){
            velY = 7.5f;
            touchingGround = false;
        }
    }
    void reset(){
        posX = 2.0;
        posY = 8.0;
        velX = 3.0;
        velY = 0.0;

        bool touchingGround = false;
        bool isAlive = true;
    }
};
player p;

int main()
{
    std::cout << "The rest of the program has compiled and main is running" <<std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Stereo Madness", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("../assets/vertex.vs", "../assets/fragment.fs"); 

    //                                     left right bottom top
    glm::mat4 worldParameters = glm::ortho(0.0f, 16.0f, 0.0f, 9.0f,-1.0f,1.0f);

    // THESE ARE DEFAULT VALUES. DO NOT CHANGE!!!!!!!!!!!!!!!!!!!!!!!!!!!
    float vertices[] = {
        // positions                    
         1.0f,  1.0f, 0.0f, // top right
         1.0f,  0.0f, 0.0f, // bottom right
         0.0f,  0.0f, 0.0f, // bottom left
         0.0f,  1.0f, 0.0f, // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute

    /*// load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     IMAGE IMPORT LOGIC
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  

    unsigned char *data = stbi_load(("../assets/container.jpg"), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
*/

    

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);
        float cameraX = p.posX -5;
        float cameraY = p.posY -2.5;

        // render
        glClearColor(0.266f, 0.184f, 0.988f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        //glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        ourShader.use();

        glm::mat4 worldParameters = glm::ortho(cameraX, cameraX + 16.0f, cameraY, cameraY + 9.0f, -1.0f, 1.0f);

        unsigned int projLoc = glGetUniformLocation(ourShader.ID, "worldParameters");
        glUniformMatrix4fv(projLoc, 1,GL_FALSE, glm::value_ptr(worldParameters));

        drawQuad(0,0,200000,2,glm::vec3(0.0941f,0.0235f,0.729f),0.0,ourShader.ID);//Ground
        p.playerDraw(ourShader.ID);
        p.update(deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //de-alloc
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || 
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS         ||
        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS            || 
        glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)              {
        p.jump();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void drawQuad(float x, float y, float width, float height, glm::vec3 color,float rotation,const unsigned int shaderID){
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x,y,0.0f));
    model = glm::scale(model, glm::vec3(width,height,1.0f));
    model = glm::rotate(model,glm::radians(rotation), glm::vec3(0.0f,0.0f,1.0f));
    //NOTE: model matrix is a container that holds translation and scaler data in one and then we send that off

    unsigned int modelLoc = glGetUniformLocation(shaderID,"model");//location of the model matrix, this is so shader knows where it is as it cant directly access it
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));//sending said model matrix to vertex shader

    unsigned int colorLoc = glGetUniformLocation(shaderID, "color");//location of the color vec3, this is so fragment shader knows where it is to color the pixels
    glUniform3fv(colorLoc,1,glm::value_ptr(color));//sending the color vector to the fragment shader

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}