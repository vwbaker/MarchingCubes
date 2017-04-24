#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Texture.h"


using namespace std;

Program::Program() :
	vShaderName(""),
	fShaderName(""),
	pid(0),
	verbose(true)
{
	
}

Program::~Program()
{
	
}

void Program::setShaderNames(const string &v, const string &f)
{
	vShaderName = v;
	fShaderName = f;
}

bool Program::init()
{
	GLint rc;
	
	// Create shader handles
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);
	
	// Compile vertex shader
	glCompileShader(VS);
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	if(!rc) {
		if(isVerbose()) {
			GLSL::printShaderInfoLog(VS);
			cout << "Error compiling vertex shader " << vShaderName << endl;
		}
		return false;
	}
	
	// Compile fragment shader
	glCompileShader(FS);
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	if(!rc) {
		if(isVerbose()) {
			GLSL::printShaderInfoLog(FS);
			cout << "Error compiling fragment shader " << fShaderName << endl;
		}
		return false;
	}
	
	// Create the program and link
	pid = glCreateProgram();
	glAttachShader(pid, VS);
	glAttachShader(pid, FS);
	glLinkProgram(pid);
	glGetProgramiv(pid, GL_LINK_STATUS, &rc);
	if(!rc) {
		if(isVerbose()) {
			GLSL::printProgramInfoLog(pid);
			cout << "Error linking shaders " << vShaderName << " and " << fShaderName << endl;
		}
		return false;
	}
	
	GLSL::printError();
	assert(glGetError() == GL_NO_ERROR);
	return true;
}

void Program::bind()
{
	glUseProgram(pid);
   for(map<string,Texture*>::iterator it = textures.begin(); it != textures.end(); ++it) {
      it->second->bind();
   }
}

void Program::unbind()
{
   for(map<string,Texture*>::iterator it = textures.begin(); it != textures.end(); ++it) {
      it->second->unbind();
   }
	glUseProgram(0);
}

void Program::addAttribute(const string &name)
{
	attributes[name] = GLSL::getAttribLocation(pid, name.c_str(), isVerbose());
}

void Program::addUniform(const string &name)
{
	uniforms[name] = GLSL::getUniformLocation(pid, name.c_str(), isVerbose());
}

void Program::addTexture(Texture *texture)
{
   const string &name = texture->getName();
   GLint handle = GLSL::getUniformLocation(pid, name.c_str());
   texture->setHandle(handle);
   textures[name] = texture;
}



GLint Program::getAttribute(const string &name) const
{
	map<string,GLint>::const_iterator attribute = attributes.find(name.c_str());
	if(attribute == attributes.end()) {
		if(isVerbose()) {
			cout << name << " is not an attribute variable" << endl;
		}
		return 0;
	}
	return attribute->second;
}

GLint Program::getUniform(const string &name) const
{
	map<string,GLint>::const_iterator uniform = uniforms.find(name.c_str());
	if(uniform == uniforms.end()) {
		if(isVerbose()) {
			cout << name << " is not a uniform variable" << endl;
		}
		return 0;
	}
	return uniform->second;
}

Texture *Program::getTexture(const string &name) const
{
   map<string,Texture*>::const_iterator texture = textures.find(name.c_str());
   if(texture == textures.end()) {
      cout << name << " is not a texture" << endl;
      return 0;
   }
   return texture->second;
}

