#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <learnopengl/shader.h>

#include <iostream>
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_action_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

void add_points();
void delete_points();

void set_color();
void draw_points();
void draw_bezier_curve();

bool isDrag();
void move_point();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// mouse position
double lastX = 0.0;
double lastY = 0.0;

// color
ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
bool canChangeColor = false;

// vertices
float vertices[] = {
    // positions       // color
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f
};
int points_num = 0;
int draged_point = 0;
bool dragging = false;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier curve", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_position_callback);
    // glfwSetMouseButtonCallback(window, mouse_action_callback);

    // // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile shaders
    // -------------------------
    // Shader shader("bezier_curve.vs", "bezier_curve.fs");

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();

    // unsigned int VAO, VBO;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfwGL3_NewFrame();
        if (canChangeColor) {
            ImGui::Text("Here to change the curve color.");
            ImGui::ColorEdit3("Color", (float*)&color);
            ImGui::Separator();
            ImGui::Text("Press X to switch.");
        } else {
            ImGui::Text("Now you can to edit the curve:");
            ImGui::Text(" > click Mouse's left button to add a new point");
            ImGui::Text(" > click Mouse's right button to delete the last point");
            ImGui::Separator();
            ImGui::Text("Press X to switch.");
        }

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_position_callback);
        glfwSetMouseButtonCallback(window, mouse_action_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(243.0f/255.0f, 243.0f/255.0f, 243.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (canChangeColor) set_color();
        if (points_num == 4) {
            draw_bezier_curve();
        }
        draw_points();


        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
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
void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // std::cout << xpos << std::endl;
    lastX = xpos;
    lastY = ypos;
    if (dragging) {
        move_point();
    }
}


// glfw: whenever the mouse clicks, this callback is called
// -------------------------------------------------------
void mouse_action_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        glfwGetCursorPos(window, &lastX, &lastY);
        if (points_num < 4) {
            add_points();
        } else if (points_num == 4 && isDrag()) {
            dragging = true;
        }
    } else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
        delete_points();
    } else if (action == GLFW_RELEASE) {
        std::cout << "stop" << std::endl;
        dragging = false;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) 
    {
        canChangeColor = !canChangeColor;
    }
}

void add_points() {
    if (canChangeColor) return;

    if (points_num < 4) {
        int offset = points_num * 6;
        vertices[offset] = (float)(lastX - SCR_WIDTH / 2) / (float)(SCR_WIDTH / 2);
        vertices[offset + 1] = (float)(SCR_HEIGHT / 2 - lastY) / (float)(SCR_HEIGHT / 2);
        std::cout << "add: " << lastX << ", " << lastY << " -> " << "x: " << vertices[offset] << " y: " << vertices[offset + 1] << std::endl;
        points_num++;
    }
}

void delete_points() {
    if (canChangeColor) return;

    if (points_num > 0) {
        std::cout << "delete" << std::endl;
        points_num--;
    }
}

void set_color() {
    for (int i = 0; i < points_num; i++) {
        int offset = i * 6 + 3;
        vertices[offset] = color.x;
        vertices[offset + 1] = color.y;
        vertices[offset + 2] = color.z;
    }
}

void draw_points() {
    Shader shader("bezier_curve.vs", "bezier_curve.fs");
    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    shader.use();
    glBindVertexArray(VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, points_num);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void draw_bezier_curve() {
    float xP0 = vertices[0], yP0 = vertices[1],
          xP1 = vertices[6], yP1 = vertices[7],
          xP2 = vertices[12], yP2 = vertices[13],
          xP3 = vertices[18], yP3 = vertices[19];

    Shader shader("bezier_curve.vs", "bezier_curve.fs");
    
    std::vector<float> curve;
    int count = 0;

    for (float t = 0.0; t <= 1.0; t += 0.00001) {
        curve.push_back(xP0*pow(1.0f-t, 3) + 3*xP1*t*pow(1.0f-t, 2) + 3*xP2*t*t*(1.0f-t) + xP3*pow(t, 3));
        curve.push_back(yP0*pow(1.0f-t, 3) + 3*yP1*t*pow(1.0f-t, 2) + 3*yP2*t*t*(1.0f-t) + yP3*pow(t, 3));
        curve.push_back(0.0f);

        // color
        curve.push_back(color.x); curve.push_back(color.y); curve.push_back(color.z);
        count++;
    }
    
    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, curve.size() * sizeof(float), &curve[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    shader.use();
    glBindVertexArray(VAO);
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, 0, count);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}


bool isDrag() {
    float target_x = (float)(lastX - SCR_WIDTH / 2) / (float)(SCR_WIDTH / 2),
        target_y = (float)(SCR_HEIGHT / 2 - lastY) / (float)(SCR_HEIGHT / 2);

    std::cout << "targetx: " << target_x << "     targety:" << target_y << std::endl;

    for (int i = 0; i < 4; i++) {
        float offset_x = target_x - vertices[i*6], offset_y = target_y - vertices[i*6+1];
        std::cout << "offsetx: " << offset_x << "     offsety: " << offset_y << std::endl;
        if ((offset_x < 0.005 && offset_x > -0.005) && (offset_y < 0.005 && offset_y > -0.005)) {
            draged_point = i;
            std::cout << i << std::endl;
            return true;
        }
    }

    return false;
}

void move_point() {
    int offset = draged_point * 6;
    vertices[offset] = (float)(lastX - SCR_WIDTH / 2) / (float)(SCR_WIDTH / 2);
    vertices[offset + 1] = (float)(SCR_HEIGHT / 2 - lastY) / (float)(SCR_HEIGHT / 2);
}