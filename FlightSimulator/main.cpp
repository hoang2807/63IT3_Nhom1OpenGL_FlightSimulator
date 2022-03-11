#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include "shader_m.h"
#include "model.h"


#include "camera.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float distanceX = 0.0f, distanceY = 1.5f, distanceZ = 3.5f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//cac bien dieu khien va vi tri, scale may bay
float mov, mov2, mov3 = 0;
bool rol[4] = { 0,0,0,0 };
glm::vec3 planePos(0.0f, 0.0f, 0.0f);
glm::vec3 planeScale(0.0025f);
float speed = 0.08;

//cac bien map
glm::vec3 mapPos(-12.5f, 0.14f, -66.0f);
float mapScale = 1.0f;

bool play = 0; //bien dieu khien choi
bool show_edit = 0; //bien dieu khien hien thi chinh sua do hoa: chinh sua vi tri model, scale
bool show_menu = 1; //bien dieu khien hien thi menu
bool show_help = 0;
int win = 0;
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    if(play==1)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    //map
    Shader mapShader("model_loading.vs", "model_loading.fs");
    //model airplane
    Shader planeShader("model_loading.vs", "model_loading.fs");

    //skybox
    Shader skyboxShader("skybox.vs", "skybox.fs");
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------
    vector<std::string> faces
    {
        "skybox/left1.jpg",
        "skybox/right1.jpg",
        "skybox/top1.jpg",
        "skybox/bottom1.jpg",
        "skybox/front1.jpg",
        "skybox/back1.jpg",
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    //end of skybox 


    // load models
    // -----------
    Model planeModel("resources/objects/plane.obj");
    //Model mapModel("resources/objects/map2.obj");
    Model mapModel("resources/objects/map/map7.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //imgui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        if (play == 1)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // per-frame time logic
        // --------------------
        camera.updateFly(planePos,distanceX,distanceY,distanceZ);
        float currentFrame = static_cast<float>(glfwGetTime());
        
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        if (show_edit) {
            ImGui::SetNextWindowPos(ImVec2(700,500), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            ImGui::Begin("Edit", NULL, window_flags);
            ImGui::SliderFloat3("Map PositionX",&mapPos.x,-80.0f,20.0f);
            ImGui::SliderFloat3("MapScale", &mapScale,0.1, 2.2);

            ImGui::SliderFloat3("Plane PositionX", &planePos.x, -80.0f, 80.0f);

            if (ImGui::Button("Menu")) {
                show_menu = 1;
                show_edit = 0;
                play = 0;
            }
            ImGui::End();
        }
        if (show_menu) {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
            ImGuiWindowFlags window_flags2 = 0;
            window_flags2 |= ImGuiWindowFlags_NoCollapse;
            ImGui::Begin("Menu", NULL, window_flags2);
            if (win == 1) {
                ImGui::Text("Ban da hoan  tot chuyen bay");
            }
            if (win == -1) {
                ImGui::Text("Ban da that bai");
            }
            if (ImGui::Button("New Game")) {
                show_menu = 0;
                show_edit = 0;
                play = 0;
                show_help = 1;
                planePos=glm::vec3(0.0f, 0.0f, 0.0f);
                win = 0;
            }
            if(win==0){if (ImGui::Button("Resume")) {
                show_menu = 0;
                show_edit = 0;
                play = 0;
                show_help = 1;
            }}
            if (ImGui::Button("Quit")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();
        }
        if (show_help) {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkPos.x / 2, main_viewport->WorkPos.y / 2), ImGuiCond_FirstUseEver);
            ImGuiWindowFlags window_flags3 = 0;
            window_flags3 |= ImGuiWindowFlags_NoTitleBar;
            window_flags3 |= ImGuiWindowFlags_NoCollapse;
            window_flags3 |= ImGuiWindowFlags_NoBackground;
            ImGui::Begin("instruc", NULL, window_flags3);
            ImGui::Text("An Enter de bat dau");
            ImGui::Text("Su dung phim mui ten de di chuyen may bay");
            ImGui::Text("ESC de tro ve menu");
            ImGui::Text("WASD de dieu chinh vi tri camera");
            ImGui::Text("Su dung chuot de dieu chinh huong camera");
            ImGui::End();
        }
        
        //Map
        mapShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        mapShader.setMat4("projection", projection);
        mapShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 mapmodel = glm::mat4(1.0f);
        mapmodel = glm::translate(mapmodel, mapPos); // position
        mapmodel = glm::scale(mapmodel, glm::vec3(mapScale));	// scale model
        mapmodel = glm::rotate(mapmodel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mapShader.setMat4("model", mapmodel);
        mapModel.Draw(mapShader);


        //AIRPLANE
        // don't forget to enable shader before setting uniforms
        planeShader.use();
        // view/projection transformations
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);

        //update di chuyen cua may bay
        if (play == 1) {
            planePos = planePos + glm::vec3(mov2, mov, -speed);
        }
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, planePos); // translate it down so it's at the center of the scene
        model = glm::scale(model, planeScale);	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

        //thuc hien chuyen dong xoay don gian cua may bay theo huong di chuyen
        if (rol[0] == 1) {
            model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
            model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rol[0] = 0;
        }
        if (rol[1] == 1) {
            model = glm::rotate(model, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            rol[1] = 0;
        }
        if (rol[2] == 1) {
            model = glm::rotate(model, glm::radians(5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
            model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rol[2] = 0;
        }
        if (rol[3] == 1) {
            model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            rol[3] = 0;
        }

        planeShader.setMat4("model", model);
        planeModel.Draw(planeShader);

        //SKYBOX
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        //skybox
        if (play == 1) {
            if (planePos.z <= -325.0f && planePos.z >= -342.0f) {
                if (planePos.y <= 1.6f && planePos.y >= 0.0f && planePos.x <= 2.0f && planePos.x >= -2.0f) {
                    win = 1;
                }
                else win = -1;
                play = 0;
                show_menu = 1;
            }
        }
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        play = 0;
        show_edit = 1;
        show_menu = 0;
    }
        
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        play = 0;
        show_edit = 0;
        show_menu = 1;
        show_help = 0;
        //glfwSetWindowShouldClose(window, true);
    }
    //dieu khien camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        distanceY = distanceY + 0.1;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        distanceY = distanceY - 0.1;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        distanceX = distanceX - 0.1;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        distanceX = distanceX + 0.1;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        distanceZ = distanceZ - 0.1;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        distanceZ = distanceZ + 0.1;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        distanceX = 0.0f, distanceY = 1.5f, distanceZ = 3.5f;
    }
    //dieu khien di chuyen may bay
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (show_menu == 0 && show_edit == 0) {
            play = 1;
        }
    }
    if (play == 1) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) == !GLFW_PRESS)
        {
            if (rol[0] == 0) {
                rol[0] = 1;
            }
            mov2 = -0.05;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT) == !GLFW_PRESS)
        {
            if (rol[2] == 0) {
                rol[2] = 1;
            }
            mov2 = 0.08;
        }
        else mov2 = 0;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == !GLFW_PRESS)
        {
            if (rol[1] == 0) {
                rol[1] = 1;
            }
            mov = 0.08;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == !GLFW_PRESS)
        {
            if (rol[3] == 0) {
                rol[3] = 1;
            }
            mov = -0.08;
        }
        else mov = 0;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            speed = speed + 0.02;
            if (speed >= 2.0f) {
                speed = 1.8f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            speed = speed - 0.02;
            if (speed <= 0) {
                speed = 0.2;
            }
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (play == 1) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}



// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}