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
void drawTri(float x, float y, float width, float height, glm::vec3 color,float rotation,const unsigned int shaderID);
bool checkCollision(player p,levelObject l);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int VBO, VAO, EBO;
unsigned int TVBO, TVAO;

enum GameMode {
    CUBE = 0,
    SHIP = 1   
};
GameMode currentmode = CUBE;

enum BlockType{
    SQUARE = 0,
    SPIKE = 1
};

struct player{
    float posX = -15.0;
    float posY = 5.0;

    float velX = 6.0;
    float velY = 0.0;

    float rotation = 0;

    const float width = 1.0;
    const float height = 1.0;

    const glm::vec3 color = glm::vec3(0.145f,0.588f,0.745f); 

    bool touchingGround = false;
    bool isAlive = true;

    void update(float dt){
        if(currentmode == CUBE){
            if(!touchingGround){
                velY -= 60.0f * dt;
                rotation -= 360 * dt;//target is 180 degrees in one jump, per frame
            }
            posY += velY * dt;//increase/decrease y pos
            posX += velX * dt;//increase/decrease x pos

            if(posY <= 2.0f){//failsafe for when player hits ground
                posY = 2.0f;
                velY = 0;
                touchingGround = true;
                rotation = round(rotation / 90.0f) * 90.0f;
            }
        }
        if(currentmode == SHIP){

        }

    }
    void playerDraw(unsigned int shaderID){
        if(currentmode == CUBE){drawQuad(posX,posY,1.25,1.25,color,rotation,shaderID);}
        if(currentmode == SHIP){drawQuad(posX,posY,2.25,0.75,color,rotation,shaderID);}
    }
    void input(){
        if(currentmode == CUBE){
            if(touchingGround){
                velY = 17.0f;
                touchingGround = false;
            }
        }
        if(currentmode == SHIP){

        }
    }
    void reset(){
        currentmode = CUBE;
        posX = 2.0;
        posY = 8.0;
        velX = 3.0;
        velY = 0.0;

        touchingGround = false;
        isAlive = true;
    }
};
player p;

struct levelObject{
    float x, y, width, height;
    BlockType type;
    unsigned int textureId;
};

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
    glm::mat4 worldParameters = glm::ortho(0.0f, 16.0f, 0.0f, 9.0f,-1.0f,1.0f);//BASE WORLD PARAMETERS, DO NOT CHANGE

    // THESE ARE DEFAULT VALUES. DO NOT CHANGE!!!!!!!!!!!!!!!!!!!!!!!!!!!
    float vertices[] = {
        // positions                    
         0.5f,  0.5f, 0.0f, // top right
         0.5f,  -0.5f, 0.0f, // bottom right
         -0.5f, -0.5f, 0.0f, // bottom left
         -0.5f,  0.5f, 0.0f, // top left 
    };
    float triVertices[] = {//vertices for the triangle
        0.0f,  0.5f, 0.0f,  // top center
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f  //bottom left
    };
    unsigned int indices[] = {  //we basically making a cube out of two triangles u feel
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
    
    //TRIANGLE TVAO & TVBO
    glGenVertexArrays(1, &TVAO);
    glGenBuffers(1, &TVBO);

    glBindVertexArray(TVAO);

    glBindBuffer(GL_ARRAY_BUFFER, TVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    float cameraX = p.posX -5;
    float cameraY = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);

        //camera position logic
        cameraX = p.posX -5;
        if (p.posY > cameraY + 7.0f) {
            cameraY = p.posY - 7.0f;
        }
        if (p.posY < cameraY + 2.0f) {
            cameraY = p.posY - 2.0f;
        }

        // render
        glClearColor(0.266f, 0.184f, 0.988f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render container
        ourShader.use();

        glm::mat4 worldParameters = glm::ortho(cameraX, cameraX + 16.0f, cameraY, cameraY + 9.0f, -1.0f, 1.0f);

        unsigned int projLoc = glGetUniformLocation(ourShader.ID, "worldParameters");
        glUniformMatrix4fv(projLoc, 1,GL_FALSE, glm::value_ptr(worldParameters));

        drawQuad(0,0.3575,200000,2,glm::vec3(0.0941f,0.0235f,0.729f),0.0,ourShader.ID);//Ground
        drawTri(2,2,0.5f,1.0f,glm::vec3(0,0,0),0,ourShader.ID);

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
        p.input();
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
    model = glm::rotate(model,glm::radians(rotation), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::scale(model, glm::vec3(width,height,1.0f));
    //NOTE: model matrix is a container that holds translation and scaler data in one and then we send that off

    unsigned int modelLoc = glGetUniformLocation(shaderID,"model");//location of the model matrix, this is so shader knows where it is as it cant directly access it
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));//sending said model matrix to vertex shader

    unsigned int colorLoc = glGetUniformLocation(shaderID, "color");//location of the color vec3, this is so fragment shader knows where it is to color the pixels
    glUniform3fv(colorLoc,1,glm::value_ptr(color));//sending the color vector to the fragment shader

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawTri(float x, float y, float width, float height, glm::vec3 color,float rotation,const unsigned int shaderID){
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x,y,0.0f));
    model = glm::rotate(model,glm::radians(rotation), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::scale(model, glm::vec3(width,height,1.0f));
    //NOTE: model matrix is a container that holds translation and scaler data in one and then we send that off

    unsigned int modelLoc = glGetUniformLocation(shaderID,"model");//location of the model matrix, this is so shader knows where it is as it cant directly access it
    glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));//sending said model matrix to vertex shader

    unsigned int colorLoc = glGetUniformLocation(shaderID, "color");//location of the color vec3, this is so fragment shader knows where it is to color the pixels
    glUniform3fv(colorLoc,1,glm::value_ptr(color));//sending the color vector to the fragment shader

    glBindVertexArray(TVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

bool checkCollision(player p,levelObject l){
    //retrieve "bounds" for player, this works for ANY gamemode btw, for the future :)
    float playerleft = p.posX - (p.width / 2);
    float playerright = p.posX + (p.width / 2);
    float playerbottom = p.posY - (p.height / 2);
    float playertop = p.posY + (p.height / 2);

    //retrieve bounds for any object, also very future proof
    float objleft = l.x - (l.width / 2);
    float objright = l.x + (l.width / 2);
    float objbottom = l.y - (l.height / 2);
    float objtop = l.y + (l.height / 2);

    if(playerleft < objright && playerright > objleft && playerbottom < objtop && playertop > objbottom){
        return true;
    }
    else{
        return false;
    }
}