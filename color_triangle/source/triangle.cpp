#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common/Angel.h"
#include "shader.h"

#include <stdlib.h>
#include <stdio.h>

using namespace Angel;

//GLFW error callback
static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

//GLFW keypress callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//Initialize everything
void init(GLuint &glShaderProgram, GLuint &triVao, GLuint &sqVao){
  
  GLuint vertex_shader, fragment_shader;
  GLint vpos_location, vcolor_location;
  GLuint tri_buffer;
  
  vec2 *triangle = new vec2[3];
  triangle[0] = vec2(-1.0, -1.0);
  triangle[1] = vec2(0.0,  1.0);
  triangle[2] = vec2(1.0, -1.0);
  size_t triangle_bytes = 3*sizeof(vec2);

  
  vec3 *triangle_colors = new vec3[3];
  triangle_colors[0] = vec3( 0.0, 0.0, 1.0);
  triangle_colors[1] = vec3( 1.0, 0.0, 0.0);
  triangle_colors[2] = vec3( 0.0, 1.0, 0.0);
  size_t triangle_color_bytes = 3*sizeof(vec3);

  
  //String with absolute paths to our shaders, shader_path set by CMAKE hack
  std::string vshader = shader_path + "vshader.glsl";
  std::string fshader = shader_path + "fshader.glsl";
  
  //Read in shader code to a character array
  GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
  GLchar* fragment_shader_source = readShaderSource(fshader.c_str());
  
  //Create and compile vertex shader
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  check_shader_compilation(vshader, vertex_shader);
  
  //Create and compile fragment shader
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  check_shader_compilation(fshader, fragment_shader);
  
  //Create shader program from the 2 shaders
  glShaderProgram = glCreateProgram();
  glAttachShader(glShaderProgram, vertex_shader);
  glAttachShader(glShaderProgram, fragment_shader);
  glLinkProgram(glShaderProgram);
  check_program_link(glShaderProgram);
  
  //bind or find the numerical location for shader variables
  glBindFragDataLocation(glShaderProgram, 0, "fragColor");
  vpos_location   = glGetAttribLocation(glShaderProgram, "vPos");
  vcolor_location = glGetAttribLocation(glShaderProgram, "vColor" );
  
  // Create a vertex array object
  glGenVertexArrays( 1, &triVao );
  //Set GL state to use vertex array object
  glBindVertexArray( triVao );
  
  //Generate buffer to hold our vertex data
  glGenBuffers( 1, &tri_buffer );
  //Set GL state to use this buffer
  glBindBuffer( GL_ARRAY_BUFFER, tri_buffer );
  
  //Create GPU buffer to hold vertices and color
  glBufferData( GL_ARRAY_BUFFER, triangle_bytes*triangle_color_bytes, NULL, GL_STATIC_DRAW );
  //First part of array holds vertices
  glBufferSubData( GL_ARRAY_BUFFER, 0, triangle_bytes, triangle );
  //Second part of array hold colors (offset by sizeof(triangle))
  glBufferSubData( GL_ARRAY_BUFFER, triangle_bytes, triangle_color_bytes, triangle_colors );
  
  glEnableVertexAttribArray(vpos_location);
  glEnableVertexAttribArray( vcolor_location );
  
  glVertexAttribPointer( vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(triangle_bytes) );

  glBindVertexArray(0);
  
  delete[] triangle;
  delete[] triangle_colors;
  
}

//Animation callback.  For now this does nothing
void animate(){
  //Do something every second
  if(glfwGetTime() > 1.0){
    glfwSetTime(0.0);
    //Do something Here
  
  }
}

//Main
int main(void)
{
  GLFWwindow* window;
  GLuint glShaderProgram;
  GLuint triVao;
  GLuint sqVao;
  
  //Set the error callback defined above
  glfwSetErrorCallback(error_callback);
  
  //Init glfw
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  //Enforce OpenGL 3.2
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  //Multisampling
  glfwWindowHint(GLFW_SAMPLES, 4);

  //Create window
  window = glfwCreateWindow(512, 512, "Rainbow triangle", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  //Set key callback
  glfwSetKeyCallback(window, key_callback);
  
  //GLFW setup calls
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  
  //Init the geomerty and shaders
  init(glShaderProgram, triVao, sqVao);
  
  //set the background/clear color
  glClearColor(1.0, 1.0, 1.0, 1.0);
  
  //main loop, loop forever until user closes window
  while (!glfwWindowShouldClose(window)){
    
    //Grab window size in pixels
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
   
    //Set viewport
    glViewport(0, 0, width, height);
    
    //Call animation update
    animate();
    
    //Clear color buffer bit
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Enable our shader program
    glUseProgram(glShaderProgram);
    
    //Draw
    glBindVertexArray( triVao );
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    
    //Swap our double buffers
    glfwSwapBuffers(window);
    
    //GLFW event handler
    glfwPollEvents();
  
  }
  
  //Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
