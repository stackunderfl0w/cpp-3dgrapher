#include <iostream>
//#define IMGUI_IMPL_OPENGL_ES2
/*#ifdef __APPLEs__
#include <OpenGL/gl3.h>
#else
#include <glad/glad.h>
#endif*/

#ifdef __ANDROID__
#include "glhelpers/shaders_gles.h"
#include <SDL_opengles2.h>
#include <SDL.h>
#include <android/log.h>

#else
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>
#include "glhelpers/shaders.h"
#include <SDL2/SDL.h>
#include <chrono>

#endif

#ifdef NINTENDO_SWITCH
#include <switch.h>
#endif


#include <glm/vec3.hpp>
#include <array>

#include "glhelpers/shader.h"
#include "glhelpers/shaderloader.h"
#include "glhelpers/camera.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "tinyexpr/tinyexpr.h"
#ifdef IMGUI_IMPL_OPENGL_LOADER_GLEW
int x=2;
#endif
#define systime chrono::duration_cast<std::chrono::microseconds>(chrono::high_resolution_clock::now()-starttime).count()


Camera camera(glm::vec3(-5.0f, 10.0f, 5.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              -23.0,
              -56);

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
bool projection_mode;
bool camera_follow=false;

using namespace std;
float f;
float fl;
float flo;
float scale=1.0;
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;
int gridlines=30;
int gridlines_old=30;

float limit_max=2;
float limit_min=-2;
float limit_max_old=2;
float limit_min_old=-2;
int sdl_interval_mode=1;
bool invert_touch=false;
static char equation[128] = "sqrt(x*x+y*y)*sin(x*y)";
static char equation_old[128] = "sqrt(x*x+y*y)*sin(x*y)";
bool geometry_update_needed=true;

double x_var=5.0;
double y_var=5.0;

int main(int argc, char *argv[]) {
    te_variable vars[] = {{"x", &x_var}, {"y", &y_var}};
    te_expr *expr = te_compile(equation, vars, 2, nullptr);
    glm::vec3 lightpos(1,5,12);


    cout<<"hi"<<endl;
#if !defined(__APPLE__) && !defined(__ANDROID__)
    gladLoadGL();
#endif
    long long last_fps_update = 0;//systime
    long long last_frame_render = 0;//systime
    long long next_frame=0;
    long long last_frame[60];
    int max_fps=60;
    bool captureinput=false;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
#ifdef __ANDROID__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
    SDL_Window *window=SDL_CreateWindow("Stackunderfl0w opengl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
#ifndef __ANDROID__
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    auto starttime = chrono::high_resolution_clock::now();

#endif


    glClearColor(0.2f, 0.3f, 0.4f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    SDL_GL_SetSwapInterval(sdl_interval_mode);

#ifndef __ANDROID__
    cout<<glGetString(GL_VERSION)<<endl;
#endif
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    ImGuiStyle& style = ImGui::GetStyle();
    SDL_GetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    float uiscale=1.0;
#ifdef __ANDROID__
    uiscale=max(SCR_WIDTH,SCR_HEIGHT)/600;
#endif

    style.ScaleAllSizes(uiscale);
    io.FontGlobalScale = uiscale;

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, context);
#ifdef __ANDROID__
    ImGui_ImplOpenGL3_Init("#version 100");
#else
    ImGui_ImplOpenGL3_Init("#version 330");
#endif
    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

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









    Shader shade3d(simplevertextransform,singlecolorfragment);
    shade3d.use();
    Shader lightingShader(basiclightingvertextransform,basiclightingfragment);


#ifndef __ANDROID__
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
#endif
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



    //SDL_StartTextInput();
    bool running=true;
    SDL_Event windowEvent;
    bool backspace_pending;

    unsigned long long deltatime;
    double fps;
    bool mouse_down = false;
    float ddpi, hdpi, vdpi;
    SDL_GetDisplayDPI(0,&ddpi,&hdpi,&vdpi);
    while(running){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

#ifndef __ANDROID__
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
#endif
        SDL_GetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        glViewport( 0.f, 0.f, SCR_WIDTH, SCR_HEIGHT );
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
        if (backspace_pending){//some fuckery to fix backspace
            backspace_pending=false;
            io.KeysDown[42]=false;

        }
        while(SDL_PollEvent(&windowEvent)) {
            if (windowEvent.key.keysym.sym != SDLK_BACKSPACE){//some fuckery to fix backspace
                ImGui_ImplSDL2_ProcessEvent(&windowEvent);
            }
            if (windowEvent.type == SDL_QUIT) {
                running = false;
            }
            if (windowEvent.type == SDL_KEYUP &&
                windowEvent.key.keysym.sym == SDLK_ESCAPE){
                running=false;
            }
            if (windowEvent.type == SDL_MOUSEBUTTONDOWN ){
                mouse_down=true;
            }
            if (windowEvent.type == SDL_MOUSEBUTTONUP ){
                mouse_down=false;
            }
            if (windowEvent.type == SDL_KEYDOWN &&
                windowEvent.key.keysym.sym == SDLK_F5){
                camera.ProcessKeyboard(CYCLECAMERAMODE, deltaTime);
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
                if(windowEvent.type == SDL_MOUSEMOTION){
                    camera.ProcessMouseMovement(windowEvent.motion.xrel,0- windowEvent.motion.yrel);
                    //cout<<windowEvent.motion.xrel<<" "<<windowEvent.motion.yrel<<endl;
                }
            }
            if(windowEvent.type == SDL_MOUSEWHEEL){
                camera.ProcessMouseScroll(windowEvent.wheel.y);
            }
            if((mouse_down&!io.WantCaptureMouse)){
                if(windowEvent.type == SDL_MOUSEMOTION){
                    camera.ProcessMouseMovement(windowEvent.motion.xrel,0- windowEvent.motion.yrel);
                    //cout<<windowEvent.motion.xrel<<" "<<windowEvent.motion.yrel<<endl;
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
            if ((windowEvent.type == SDL_FINGERMOTION) &! io.WantCaptureMouse){
                if (!invert_touch)                {
                    camera.ProcessMouseMovement(windowEvent.tfinger.dx*1000,0- windowEvent.tfinger.dy*1000);
                }
                else{
                    camera.ProcessMouseMovement(0-windowEvent.tfinger.dx*1000, windowEvent.tfinger.dy*1000);
                }
                //__android_log_print(ANDROID_LOG_ERROR, APPNAME, "The value is %f", windowEvent.tfinger.dx*1000);

            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_BACKSPACE){
                //__android_log_print(ANDROID_LOG_ERROR, APPNAME, "The value is %d", 666);
                io.KeysDown[42]=true;//manualy set backspace key
                backspace_pending=true;

            }
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_BACKSPACE){
                //__android_log_print(ANDROID_LOG_ERROR, APPNAME, "The value is %d", 999);

            }
            //__android_log_print(ANDROID_LOG_ERROR, APPNAME, "The value is %d", windowEvent.type);

        }

        if (io.WantTextInput){
            SDL_StartTextInput();
        }
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


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
                    //quads.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale+10));
                    //quads.push_back(glm::vec3(i*scale,z_cords[i][j+1],(j+1)*scale+10));
                    //quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j+1],(j+1)*scale+10));
                    //quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j],j*scale+10));
                    quads.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale+10));//1
                    quads.push_back(glm::vec3(i*scale,z_cords[i][j+1],(j+1)*scale+10));//2
                    quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j+1],(j+1)*scale+10));//3

                    quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j+1],(j+1)*scale+10));//3
                    quads.push_back(glm::vec3((i+1)*scale,z_cords[i+1][j],j*scale+10));//4
                    quads.push_back(glm::vec3(i*scale,z_cords[i][j],j*scale+10));//1

                }
            }

            normals.clear();
            normals.shrink_to_fit();
            for(int i = 0; i<quads.size()/6;i++){
                glm::vec3 dir=glm::cross((quads[i*6+1]-quads[i*6]),(quads[i*6+2]-quads[i*6]));
                //norm.x=0-norm.x;
                //norm.y=0-norm.y;
                //norm.z=0-norm.z;
                normals.push_back(dir);
                normals.push_back(dir);
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
        //camera.Position.x=(float)sin(SDL_GetTicks()/1000.0);
        //cout<<sin(SDL_GetTicks()/1000.0)<<endl;
        //cout<<camera.Pitch<<endl;
        //camera.ProcessMouseMovement(0,0);

        glm::mat4 projection;
        if (projection_mode==0){
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        }
        else{
            projection = glm::ortho(-8*(float)SCR_WIDTH / (float)SCR_HEIGHT,8*(float)SCR_WIDTH / (float)SCR_HEIGHT,-9.f,9.f,0.f,1000.f);
        }
        if (camera_follow){
            camera.Position=glm::vec3(2.0,0,2)-glm::vec3(8)*camera.Front;
        }
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 MVP=projection*view*model;
        shade3d.use();
        shade3d.setMat4("MVP",MVP);

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

        shade3d.use();


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

        lightingShader.use();
        GLint vaPosAttributeIndex = glGetAttribLocation(lightingShader.ID, "aPos");
        GLint vaNormalAttributeIndex = glGetAttribLocation(lightingShader.ID, "aNormal");
        glBindBuffer(GL_ARRAY_BUFFER, vertexquadsbuffer);
        glEnableVertexAttribArray(vaPosAttributeIndex);
        glVertexAttribPointer(
                vaPosAttributeIndex,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
        );
        //glUseProgram(lightingid);
        glEnableVertexAttribArray(vaNormalAttributeIndex);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);

        glVertexAttribPointer(
                vaNormalAttributeIndex,
                3,
                GL_FLOAT
                , GL_FALSE,
                3 * sizeof(float),
                (void*)0
        );
        glDrawArrays(GL_TRIANGLES, 0, quads.size());
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
            ImGui::Checkbox("Invert input", &invert_touch);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Center camera",&camera_follow);
            ImGui::Checkbox("Projection (Perspective/Isometric)",&projection_mode);
            ImGui::SliderInt("Gridlines", &gridlines, 3, 450);            // Edit 1 float using a slider from 0.0f to 1.0f

            //ImGui::SliderFloat("float", &f, -3.14f, 3.14f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::SliderFloat("float2", &fl, -3.14f, 3.14f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::SliderFloat("float3", &flo, -3.14f, 3.14f);            // Edit 1 float using a slider from 0.0f to 1.0f

            //ImGui::SliderFloat("x min", &limit_min, -10, 10);
            //ImGui::SliderFloat("x max", &limit_max, -10, 10);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            //    counter++;
            //ImGui::SameLine();
            //ImGui::Text("counter = %d", counter);
            ImGui::Text("yaw = %f", camera.Yaw);
            ImGui::Text("pitch = %f", camera.Pitch);
            ImGui::Text("pos = %f %f %f", camera.Position.x, camera.Position.y,camera.Position.z);
            ImGui::SliderInt("Swap interval",&sdl_interval_mode,-1,3);

            ImGui::InputText("input text", equation, IM_ARRAYSIZE(equation));

            ImGui::Text("pos = %d",SDL_HasScreenKeyboardSupport());
            ImGui::Text("vertices = %d",quads.size());



            SDL_GL_SetSwapInterval(sdl_interval_mode);
            double temp_x,temp_y;

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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);

    printf("exiting\n");
    //SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("sdl window destroyed\n");
#ifndef __APPLE__ //deleting the context causes macos to freak out for some reason
    SDL_GL_DeleteContext(context);
    printf("gl context deleted\n");
#endif
    SDL_Quit();
    printf("sdl closed\n");
    //glDeleteProgram(programID);
    //glDeleteProgram(uiShader.ID);
    //glDeleteProgram(lightingShader.ID);
    //glDeleteProgram(textureshader.ID);
    return 0;
}