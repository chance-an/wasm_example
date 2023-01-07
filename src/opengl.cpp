#include <functional>
#include <emscripten.h>
#include <SDL_opengles2.h>
#include "opengl.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "SDL.h"
#include <SDL_image.h>
#include <GLES2/gl2.h>
#include "emscripten.h"

std::function<void()> loop;
void main_loop() { loop(); }

// Shader sources
const GLchar* vertexSource =
    "attribute vec4 position;                     \n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_Position = vec4(position.xyz, 1.0);     \n"
    "}                                            \n";
const GLchar* fragmentSource =
    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor[0] = gl_FragCoord.x/640.0;    \n"
    "  gl_FragColor[1] = gl_FragCoord.y/480.0;    \n"
    "  gl_FragColor[2] = 0.5;                     \n"
    "}                                            \n";

// an example of something we will control from the javascript side
bool background_is_black = true;

static bool quitting = false;
static SDL_Window *window = NULL;
static SDL_GLContext gl_context;



GLuint LoadShader ( GLenum type, char * shaderSrc )
{
   GLuint shader;
   GLint compiled;
   
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   	return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char *)malloc (sizeof(char) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         std::cout << "Error compiling shader:" << infoLog << std::endl;            
         
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

GLuint programObject;

void Init() {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    char const * vShaderStr =  
      "attribute vec4 vPosition;    \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = vPosition;  \n"
      "}                            \n";
   
    char const * fShaderStr =  
      "precision mediump float;\n"\
      "void main()                                  \n"
      "{                                            \n"
      "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
      "}                                            \n";

    // Load the vertex/fragment shaders
    vertexShader = LoadShader ( GL_VERTEX_SHADER, (char*)vShaderStr );
    fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, (char*)fShaderStr );

    // Create the program object
    programObject = glCreateProgram ( );
    
    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );

    // Bind vPosition to attribute 0   
   glBindAttribLocation ( programObject, 0, "vPosition" );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked ) 
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char *)malloc (sizeof(char) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         std::cout << "Error linking program:" << infoLog << std::endl;            
         
         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return;
   }

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return;
}

void Draw ()
{
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f, 
                           -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };

   // No clientside arrays, so do this in a webgl-friendly manner
   GLuint vertexPosObject;
   glGenBuffers(1, &vertexPosObject);
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);
   
   // Set the viewport
   glViewport ( 0, 0, 640, 480 );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( programObject );

   // Load the vertex data
   glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
   int PositionHandle = glGetAttribLocation(programObject, "vPosition");

//    glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
   glVertexAttribPointer(PositionHandle, 3, GL_FLOAT, false, 12, 0);
   glEnableVertexAttribArray(0);


   glDrawArrays ( GL_TRIANGLES, 0, 3 );
    // glVertex2f(0.5, 0.5)
}

// the function called by the javascript code
// extern "C" void EMSCRIPTEN_KEEPALIVE toggle_background_color() { background_is_black = !background_is_black; }
void try_opengl() {
    SDL_Renderer *renderer = NULL;
    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &window, &renderer);

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return;
    }

    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // // Create a Vertex Buffer Object and copy the vertex data to it
    // GLuint vbo;
    // glGenBuffers(1, &vbo);

    // GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // // Create and compile the vertex shader
    // GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    // glCompileShader(vertexShader);

    // // Create and compile the fragment shader
    // GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    // glCompileShader(fragmentShader);

    // // Link the vertex and fragment shader into a shader program
    // GLuint shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);
    // glUseProgram(shaderProgram);

    // // Specify the layout of the vertex data
    // GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    // glEnableVertexAttribArray(posAttrib);
    // glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    Init();

    loop = [&]
    {
        // move a vertex
        const uint32_t milliseconds_since_start = SDL_GetTicks();
        // std::cout << "tick" <<  milliseconds_since_start << std::endl;
        const uint32_t milliseconds_per_loop = 3000;
        // vertices[0] = ( milliseconds_since_start % milliseconds_per_loop ) / float(milliseconds_per_loop) - 0.5f;
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Clear the screen
        if( background_is_black )
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        else
            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle from the 3 vertices
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        Draw ();

        SDL_GL_SwapWindow(window);
    };

    emscripten_set_main_loop(main_loop, 0, true);
}

// void try_opengl2() {
//     SDL_Window *window;
//     SDL_CreateWindowAndRenderer(640, 480, 0, &window, nullptr);

//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//     SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

//     // Create a Vertex Buffer Object and copy the vertex data to it
//     GLuint vbo;
//     glGenBuffers(1, &vbo);

//     GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // Create and compile the vertex shader
//     GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexSource, nullptr);
//     glCompileShader(vertexShader);

//     // Create and compile the fragment shader
//     GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
//     glCompileShader(fragmentShader);

//     // Link the vertex and fragment shader into a shader program
//     GLuint shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);
//     glUseProgram(shaderProgram);

//     // Specify the layout of the vertex data
//     GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
//     glEnableVertexAttribArray(posAttrib);
//     glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

//     loop = [&]
//     {
//         // move a vertex
//         const uint32_t milliseconds_since_start = SDL_GetTicks();
//         const uint32_t milliseconds_per_loop = 3000;
//         vertices[0] = ( milliseconds_since_start % milliseconds_per_loop ) / float(milliseconds_per_loop) - 0.5f;
//         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//         // Clear the screen
//         if( background_is_black )
//             glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//         else
//             glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         // Draw a triangle from the 3 vertices
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         SDL_GL_SwapWindow(window);
//     };

//     emscripten_set_main_loop(main_loop, 0, true);

// }