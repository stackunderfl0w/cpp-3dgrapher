#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <array>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "shaderloader.h"
#include "camera.h"
#include "shaders.h"
#include "shader.h"
#include "tinyexpr.h"
Camera camera(glm::vec3(-5.0f, 10.0f, 5.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              11.0,
              -40);

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
using namespace std;
float f;
float fl;
float flo;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int gridlines=30;
int gridlines_old=30;

float limit_max=2;
float limit_min=-2;
float limit_max_old=2;
float limit_min_old=-2;
int sdl_interval_mode=1;
static char equation[128] = "sqrt(x*x+y*y)*sin(x*y)";
static char equation_old[128] = "sqrt(x*x+y*y)*sin(x*y)";
bool geometry_update_needed=true;

double x_var=5.0;
double y_var=5.0;
#define systime chrono::duration_cast<std::chrono::microseconds>(chrono::high_resolution_clock::now()-starttime).count()




int main(int argc, char *argv[]){
    auto starttime = chrono::high_resolution_clock::now();
    te_variable vars[] = {{"x", &x_var}, {"y", &y_var}};
    te_expr *expr = te_compile(equation, vars, 2, nullptr);


    long long last_fps_update = systime;
    long long last_frame_render = systime;
    long long next_frame=0;
    long long last_frame[60];
    int max_fps=60;
    bool captureinput=false;
    glm::vec3 lightpos(1,5,12);

    SDL_Window *window=SDL_CreateWindow("Stackunderfl0w opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    SDL_GL_SetSwapInterval(sdl_interval_mode);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 330");
    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);


    /*static const GLfloat vertex_data[] = {
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            -1.0f,-1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f,-1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f,-1.0f,
            1.0f,-1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f,-1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f,-1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f,-1.0f, 1.0f
    };*/


    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> quads;
    std::vector<glm::vec3> normals;


    static const GLfloat vertex_data[] = {
            -1.0f,-1.0f,1.0f,
            -1.0f,1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f

    };
    static std::array<std::array<double, 450>, 450> z_cords;

    float scale=(limit_max-limit_min)/gridlines;


    GLuint programID = LoadShaders_text(simplevertextransform,singlecolorfragment);
    Shader lightingShader(basiclightingvertextransform,basiclightingfragment);

    glUseProgram(programID);
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);
    GLuint vertexquadsbuffer;
    glGenBuffers(1, &vertexquadsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexquadsbuffer);
    glBufferData(GL_ARRAY_BUFFER, quads.size() * sizeof(glm::vec3), &quads[0], GL_DYNAMIC_DRAW);
    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);

    SDL_Event windowEvent;
    unsigned long long deltatime;
    bool running = true;
    double fps;
    while(running){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        SDL_GetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        glViewport( 0.f, 0.f, SCR_WIDTH, SCR_HEIGHT );
        //cout<<width;
        long long framestart = systime;
        deltatime = (systime - last_frame[0]);
        float currentFrame = SDL_GetTicks()/100.0;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //calc fps
        if(true){
            //next_frame = systime + 1000000 / (max_fps);
            fps = (1000000.0 / (systime - last_frame[59]) * 60);

            if (systime - last_fps_update > 500000) {
                last_fps_update = systime;
                SDL_SetWindowTitle(window, to_string(fps).c_str());
                //cout<<to_string(fps).c_str()<<endl;
            }
            for (int i = 59; i > 0; --i) {
                last_frame[i] = last_frame[i - 1];
            }
            last_frame[0] = systime;
        }
        //handle inputs
        SDL_PumpEvents();
        const Uint8 *keysArray = const_cast <Uint8*> (SDL_GetKeyboardState(NULL));
        // Move forward
        if(captureinput){
            if (keysArray[SDL_SCANCODE_UP]|keysArray[SDL_SCANCODE_W])
                camera.ProcessKeyboard(FORWARD, deltaTime);

            if (keysArray[SDL_SCANCODE_DOWN]|keysArray[SDL_SCANCODE_S])
                camera.ProcessKeyboard(BACKWARD, deltaTime);

            if (keysArray[SDL_SCANCODE_LEFT]|keysArray[SDL_SCANCODE_A])
                camera.ProcessKeyboard(LEFT, deltaTime);

            if (keysArray[SDL_SCANCODE_RIGHT]|keysArray[SDL_SCANCODE_D])
                camera.ProcessKeyboard(RIGHT, deltaTime);

            if (keysArray[SDL_SCANCODE_RIGHT]|keysArray[SDL_SCANCODE_SPACE])
                camera.ProcessKeyboard(UP, deltaTime);

            if (keysArray[SDL_SCANCODE_RIGHT]|keysArray[SDL_SCANCODE_LSHIFT])
                camera.ProcessKeyboard(DOWN, deltaTime);
        }



        while(SDL_PollEvent(&windowEvent)){
            ImGui_ImplSDL2_ProcessEvent(&windowEvent);
            if (windowEvent.type == SDL_QUIT){
                running=false;
            }
            if (windowEvent.type == SDL_KEYUP &&
                windowEvent.key.keysym.sym == SDLK_ESCAPE){
                running=false;
            }
            if (windowEvent.type == SDL_KEYDOWN &&
                windowEvent.key.keysym.sym == SDLK_F6){
                sdl_interval_mode++;
                if (sdl_interval_mode>1)
                    sdl_interval_mode=-1;
                SDL_GL_SetSwapInterval(sdl_interval_mode);
                //cout<<sdl_interval_mode;
            }
            if(captureinput){
                if (windowEvent.type == SDL_KEYDOWN &&
                    windowEvent.key.keysym.sym == SDLK_F5){
                    camera.ProcessKeyboard(CYCLECAMERAMODE, deltaTime);
                }
                if(windowEvent.type == SDL_MOUSEMOTION){
                    camera.ProcessMouseMovement(windowEvent.motion.xrel,0- windowEvent.motion.yrel);
                    //cout<<windowEvent.motion.xrel<<" "<<windowEvent.motion.yrel<<endl;
                }
                if(windowEvent.type == SDL_MOUSEWHEEL){
                    camera.ProcessMouseScroll(windowEvent.wheel.y);
                }
            }
            if (windowEvent.type == SDL_KEYDOWN &&
                 windowEvent.key.keysym.sym == SDLK_TAB){
                captureinput=!captureinput;
                if(captureinput)
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                else
                    SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        }

        glUseProgram(programID);
        if(geometry_update_needed){
            vertices.clear();
            vertices.shrink_to_fit();
            //cout<<vertices.size();
            float scale=(limit_max-limit_min)/gridlines;
            for (int i = 0; i <gridlines ; i++) {
                float x=scale*i+limit_min;
                for (int j = 0; j <gridlines ; j++) {
                    float y=scale*j+limit_min;
                    //z_cords[i][j]=sqrt(x*x+y*y)*sin(x*y);//(7*x*y)/pow(2.71828,x*x + y*y);//=(.1*x*y);
                    x_var=x;
                    y_var=y;
                    z_cords[i][j]=te_eval(expr);
                    //vertices.push_back(glm::vec3(i,(.1*i*j*j),j));
                }
                //cout<<i;
            }
            //te_free(expr);
            for (int i = 0; i <gridlines ; i++) {
                if(!(i%2)){
                    for (int j = 0; j <gridlines ; j++) {
                        vertices.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale));
                    }
                }
                else{
                    for(int j = gridlines-1; j>=0;j--){
                        vertices.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale));
                    }
                }
            }
            for (int j = 0; j <gridlines ; j++) {
                if(!(j%2)){
                    for (int i = 0; i <gridlines ; i++) {
                        vertices.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale));
                    }
                }
                else{
                    for(int i = gridlines-1; i>=0;--i){
                        vertices.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale));
                    }
                }
            }
            quads.clear();
            quads.shrink_to_fit();
            for(int i = 0; i<gridlines-1;i++){
                for(int j = 0; j<gridlines-1;j++){
                    quads.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale+10));
                    quads.push_back(glm::vec3(i*scale,z_cords[i][j+1],(j+1)*scale+10));
                    quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j+1],(j+1)*scale+10));
                    quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j],j*scale+10));
                }
            }

            normals.clear();
            normals.shrink_to_fit();
            for(int i = 0; i<quads.size()/4;i++){
                glm::vec3 dir=glm::cross((quads[i*4+1]-quads[i*4]),(quads[i*4+2]-quads[i*4]));
                //norm.x=0-norm.x;
                //norm.y=0-norm.y;
                //norm.z=0-norm.z;
                normals.push_back(dir);
                normals.push_back(dir);
                normals.push_back(dir);
                normals.push_back(dir);
            }
            //cout<<quads.size()<<" "<<normals.size()<<endl;

            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertexquadsbuffer);
            glBufferData(GL_ARRAY_BUFFER, quads.size() * sizeof(glm::vec3), &quads[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);
            geometry_update_needed=false;
            //cout<<quads.size()<<endl;
        }


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 MVP=projection*view*model;

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightpos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        lightingShader.setMat4("model", model);

        glUseProgram(programID);

        //glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );

        glDrawArrays(GL_LINE_STRIP, 0, vertices.size()); // 12*3 indices starting at 0 -> 12 triangles
        //glUseProgram(lightingid);
        //glEnableVertexAttribArray(0);
        lightingShader.use();

        glBindBuffer(GL_ARRAY_BUFFER, vertexquadsbuffer);
        glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );
        //glUseProgram(lightingid);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);

        glVertexAttribPointer(
                1,
                3,
                GL_FLOAT
                , GL_FALSE,
                3 * sizeof(float),
                (void*)0
                );
        glDrawArrays(GL_QUADS, 0, quads.size()); // 12*3 indices starting at 0 -> 12 triangles




// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderInt("Gridlines", &gridlines, 3, 450);            // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::SliderFloat("float", &f, -3.14f, 3.14f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("float2", &fl, -3.14f, 3.14f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("float3", &flo, -3.14f, 3.14f);            // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::SliderFloat("x min", &limit_min, -10, 10);
            ImGui::SliderFloat("x max", &limit_max, -10, 10);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("yaw = %f", camera.Yaw);
            ImGui::Text("pitch = %f", camera.Pitch);
            ImGui::Text("pos = %f %f %f", camera.Position.x, camera.Position.y,camera.Position.z);
            ImGui::InputText("input text", equation, IM_ARRAYSIZE(equation));
            double temp_x,temp_y;
            ImGui::InputDouble("x",&temp_x,.01,1,"%.8f");
            ImGui::InputDouble("y",&temp_y,.01,1,"%.8f");

            //cout<<te_interp("(x+5)", 0)<<endl;
            if( strcmp(equation,equation_old) != 0 ){
                cout<<"changed"<<endl;
                strcpy(equation_old,equation);
                //te_free(expr);
                int error;
                te_expr *expr2 = te_compile(equation, vars, 2, &error);
                if(error==0){
                    cout<<"compiled"<<endl;
                    te_free(expr);
                    expr=expr2;
                    //te_free(expr2);
                    geometry_update_needed=true;
                }

            } // strings are equal
            if(gridlines!=gridlines_old){
                gridlines_old=gridlines;
                geometry_update_needed=true;
            }
            if(limit_min!=limit_min_old){
                limit_min_old=limit_min;
                geometry_update_needed=true;
            }
            if(limit_max!=limit_max_old){
                limit_max_old=limit_max;
                geometry_update_needed=true;
            }

            x_var=temp_x;
            y_var=temp_y;
            //cout<<te_eval(expr)<<endl;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        //int display_w, display_h;
        //glfwGetFramebufferSize(window, &display_w, &display_h);
        //glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
