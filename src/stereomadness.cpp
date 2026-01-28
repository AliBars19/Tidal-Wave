#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../assets/shader_s.h"

#include <fstream>
#include <sstream>

#include <vector>
#include <iostream>

//functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drawQuad(float x, float y, float width, float height, glm::vec3 color,float rotation,const unsigned int shaderID);
void drawTri(float x, float y, float width, float height, glm::vec3 color,float rotation,const unsigned int shaderID);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const float GROUND = 2.0f;

unsigned int VBO, VAO, EBO;
unsigned int TVBO, TVAO;    

enum GameMode {
    CUBE = 0,
    SHIP = 1   
};
enum BlockType{
    SQUARE = 0,
    SPIKE = 1
};

GameMode currentmode = CUBE;

struct levelObject{
    float x, y, width, height, rotation;
    BlockType type;
    glm::vec3 color;
    unsigned int textureId;
};
levelObject currentPlatform;

struct player{
    float posX = -5.0;
    float posY = 2.0;

    float velX = 10.385;
    float velY = 0.0;

    float rotation = 0;

    const float width = 1.0;
    const float height = 1.0;

    const glm::vec3 color = glm::vec3(0.145f,0.588f,0.745f); 

    bool touchingGround = false;
    bool onPlatform = false;

    void update(float dt){
        if(currentmode == CUBE){
            if(!touchingGround){
                velY -= 75.0f * dt;
                rotation -= 360 * dt;//target is 180 degrees in one jump, per frame
            }
            posY += velY * dt;//increase/decrease y pos
            posX += velX * dt;//increase/decrease x pos

            if(posY - (height/2) <= GROUND){//failsafe for when player hits ground
                posY = GROUND  + (height/2);
                velY = 0;
                touchingGround = true;
                rotation = round(rotation / 90.0f) * 90.0f;
            }
            if(onPlatform){
                float platformLeft = currentPlatform.x - (currentPlatform.width / 2);
                float platformRight = currentPlatform.x + (currentPlatform.width / 2);

                float playerleft = posX - (width / 2);
                float playerright = posX + (width / 2);

                if(posX > platformRight){
                    onPlatform = false;
                    touchingGround = false;
                }
            }
        }
        if(currentmode == SHIP){}
    }
    void playerDraw(unsigned int shaderID){
        if(currentmode == CUBE){drawQuad(posX,posY,width,height,color,rotation,shaderID);}
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
        posX = -5.0;
        posY = 2.0;
        velX = 10.385;
        velY = 0.0;

        touchingGround = true;
    }
};
player p;

std::vector<levelObject> level;

// vvv functions that depend on levelobject being in their scope.
bool checkCollision(player p,levelObject l);
void drawLevelObject(levelObject obj, unsigned int shaderID);
bool squareCollision(levelObject obj);

//pre-defined spikes
levelObject normalSpikeNo();
levelObject flatSpikeNo();
levelObject seaOfSpikeNo();

//pre-defined spikes
levelObject o1x1();
levelObject o1x05();

//create a series of the same object 
levelObject drawSeries(levelObject obj);

int main()
{
    std::cout << "The rest of the program has compiled and main is running" <<std::endl;

    //level.push_back({2.0f,2.0f,1.0f,1.0f,0.0f,SPIKE,glm::vec3(0.0,0.0,0.0),0});
    level.push_back({20.f,2.25f,1.0f,0.5f,0.0f,SQUARE, glm::vec3(0,0,0),0});

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

        //                                     left      right            bottom         top
        glm::mat4 worldParameters = glm::ortho(cameraX, cameraX + 16.0f, cameraY, cameraY + 9.0f, -1.0f, 1.0f);
        unsigned int projLoc = glGetUniformLocation(ourShader.ID, "worldParameters");
        glUniformMatrix4fv(projLoc, 1,GL_FALSE, glm::value_ptr(worldParameters));

        drawQuad(0,1.0f,200000,2,glm::vec3(0.0941f,0.0235f,0.729f),0.0,ourShader.ID);//Ground
        
        p.update(deltaTime);

        for(auto& obj : level){//COLLISION CHECKER
            if(obj.x > p.posX - 6 && obj.x < p.posX + 6){//CHECK IF OBJECT IS WITHIN REACH
                if(checkCollision(p,obj)){
                    if(obj.type == SPIKE){
                        p.reset();
                        cameraY = 0.0;
                    }
                    if(obj.type == SQUARE){
                        if(!squareCollision(obj)){
                            p.reset();
                            cameraY = 0.0;
                        }else{
                            currentPlatform = obj;
                        }
                    }
                }
            }
        }
        p.playerDraw(ourShader.ID);
        for(auto& obj : level){//DRAW EVERY OBJECT IN LEVEL VECTOR
            if(obj.x > cameraX - 4 && obj.x < cameraX + 20){//CHECK IF OBJECT IS IN CAMERA VIEW
                drawLevelObject(obj, ourShader.ID);
            }
        }
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
//process input
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
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        p.reset();
    }
}
//vvvvvvvvvvvvv simple glfw window setup
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

// vvv this function draws the object on the screen. we will call this in the render loop, we dont need to draw it in the vector yk. uses enum
void drawLevelObject(levelObject obj, unsigned int shaderID){
    if(obj.type == SPIKE){
        drawTri(obj.x,obj.y,obj.width,obj.height,obj.color,obj.rotation,shaderID);
    }
    if(obj.type == SQUARE){
        drawQuad(obj.x,obj.y,obj.width,obj.height,obj.color,obj.rotation,shaderID);
    }
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

bool squareCollision(levelObject obj){
    float objtop = obj.y + (obj.height / 2);
    float playerbottom = p.posY - (p.height / 2);

    if(p.velY < 0 && playerbottom >= objtop - 0.1f){
        p.velY = 0.0;
        p.touchingGround = true;
        p.onPlatform = true;
        currentPlatform = obj;
        p.posY = objtop + (p.height/2);
        p.rotation = round(p.rotation / 90.0f) * 90.0f;
        return true;
    }
    else{
        return false;
    }
}