#include <stdio.h>
#include <iostream>
#include <string.h>
#include <functional>
#include <emscripten.h>

#include "SDL.h"
#include <SDL_image.h>
#include <GLES2/gl2.h>
#include "emscripten.h"

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

  //  glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
   glVertexAttribPointer(PositionHandle, 3, GL_FLOAT, false, 12, 0);
   glEnableVertexAttribArray(0);


   glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

std::function<void()> loop;
void main_loop() { loop(); }
static SDL_Window *window = NULL;

int main() {
  try {

    SDL_Renderer *renderer = NULL;
    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &window, &renderer);

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    Init();

    loop = [&]
    {
       
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle from the 3 vertices
        Draw ();

        SDL_GL_SwapWindow(window);
    };

    emscripten_set_main_loop(main_loop, 0, true);

    
    printf("hello, world!\n");
    return EXIT_SUCCESS;
  } catch( const std::exception &e) {
    
    printf("Caught stuff %s\n", e.what());
  }
}
