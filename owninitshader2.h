#ifndef OWNINITSHADER2_H_
#define OWNINITSHADER2_H_

#include <SDL2/SDL.h>
//#include "SDL_ttf.h"
#include "GL/glew.h"
//#include "GL/freeglut.h"
//#include "GL/gl.h"
//#include "GL/glu.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;

GLuint create(GLenum type, const GLchar* shadeSource);
const GLchar* input(const char* filename);
GLuint createProgram(const vector<GLuint> shadeList);
void transform(GLuint program);

typedef struct{
  GLenum type; // GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
  const char* filename; //name of file to input
} ShaderInfo;

//create the shaders for your program
GLuint initShaders(ShaderInfo* shaders){
  ShaderInfo* shade=shaders;
  vector<GLuint> Shadersv; //creating a vector of shaders
  while(shade->type != GL_NONE){ //loop through all the shaders in the vector
    GLuint tempshader = create(shade->type, input(shade->filename)); //temporarally creates a shader to add to the list
    Shadersv.push_back(tempshader); //adding shaders to the vector
    ++shade; //going through the vector one at a time
  }
  GLuint program=createProgram(Shadersv); //creates the program which is an object all shaders can be linked to and compares the compatibility of the shaders
  glUseProgram(program);//installs a program object as part of current rendering state, then takes the attached, compiled, and linked shaders and creates an executable from them
  glm:: mat4 view;
  view = glm::lookAt(glm::vec3(0.0f,0.0f, 50.0f),
                     glm::vec3(0.0f,0.0f, 0.0f),
                     glm::vec3(0.0f,1.0f, 0.0f)
  );
  GLint tempLoc = glGetUniformLocation(program, "viewMatrix");
  glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &view[0][0]);

  glm::mat4 mainProjMatrix;
  mainProjMatrix = glm::perspective(57.0,1.0,.1,500.0);
  tempLoc = glGetUniformLocation(program, "Matrix");
  glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &mainProjMatrix[0][0]);

  return program;
}

//takes the vertex and fragment shaders and loads them
const GLchar* input(const char* filename){
  FILE* fshade = fopen(filename, "rb"); //open shader file
  if(!fshade){ //error checking
    fprintf(stderr,"Didn't open file '%s'\n",filename);
    return NULL;
  }
  fseek(fshade, 0, SEEK_END);
  long filesize=ftell(fshade); //length of the vertex and fragment shader files
  fseek(fshade, 0, SEEK_SET); 
  GLchar* shadingSource= new GLchar[filesize+1]; //creates memory for the shader files to be read in
  fread(shadingSource, 1, filesize, fshade);
  if(ftell(fshade) == 0){ //error checking
    fprintf(stderr, "File '%s' is empty.\n",filename);
    return NULL;
  }
  fclose(fshade); //close shader file
  shadingSource[filesize] = 0; //set the ending char at the end of array
  return const_cast<const GLchar*>(shadingSource);//overloads the const so the value with change per file  
  //if the file doesn't open or is empty this function will segmentation fault your opengl program
}

//creates your shader
GLuint create(GLenum type, const GLchar* shadeSource){
  GLuint shader = glCreateShader(type); //create vertex or fragment shader
  glShaderSource(shader, 1, &shadeSource, NULL); //sets the source code in shader equal to the source code in the shadesource.  The one is the number of strings in the shade source.
  glCompileShader(shader); //compiles the shader object that was loaded in the previous statement
  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus); //returns the compile status into the variable
  if(!compileStatus){ //error checking to see if the shader compiled
    GLint size; //size of the debug info
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size); //returns the size of the the source file into the variable
    GLchar* infoLog = new GLchar[size+1]; //creating memory for the debug info
    glGetShaderInfoLog(shader,size,&size,infoLog); //returns the error messages into the variable infoLog
    const char *shadeInfo= NULL; //error message
    switch(type){
      case GL_VERTEX_SHADER: shadeInfo = "vertex"; break;
      case GL_GEOMETRY_SHADER_EXT: shadeInfo = "geometric"; break;
      case GL_FRAGMENT_SHADER: shadeInfo = "fragment"; break;
    }
    fprintf(stderr,"\nCompile failure in %u shader: %s\n Error message:\n%s\n",type,shadeInfo,infoLog); //prints information needed to debug shaders
    delete[] infoLog;
  }
  return shader; //return the created and compiled shader
}

//creates the shading program to link the shaders too
GLuint createProgram(const vector<GLuint> shadeList){
  GLuint program = glCreateProgram(); //creates the program
  for(GLuint i=0;i<shadeList.size();i++){glAttachShader(program,shadeList[i]);} //attaches shaders to program
  glBindAttribLocation(program, 0, "in_position"); //binds the location of a userdefined 'in_position' attribute to a program
  glBindAttribLocation(program, 1, "in_color"); //binds the location a userdefined 'in_color' attribute to a program
  glLinkProgram(program); //links the shader program to the program written by user
  GLint linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus); //returns the status of linking the program into the variable
  if(!linkStatus){ //error checking on linking
    GLint size;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size); //returns the linking status into the variable
    GLchar *infoLog = new GLchar[size+1]; //creating memory for the debug info
    glGetProgramInfoLog(program,size,&size,infoLog); //gets the error messages
    fprintf(stderr,"\nShader linking failed: %s\n",infoLog); //error message
    delete[] infoLog;
    for(GLuint i=0;i<shadeList.size();i++){glDeleteShader(shadeList[i]);}
  }
  return program;
}
#endif
