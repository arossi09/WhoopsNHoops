
#include "Program.h"
#include <cassert>
#include <fstream>
#include <iostream>

#include "GLSL.h"

std::string readFileAsString(const std::string &fileName) {
  std::string result;
  std::ifstream fileHandle(fileName);

  if (fileHandle.is_open()) {
    fileHandle.seekg(0, std::ios::end);
    result.reserve((size_t)fileHandle.tellg());
    fileHandle.seekg(0, std::ios::beg);

    result.assign((std::istreambuf_iterator<char>(fileHandle)),
                  std::istreambuf_iterator<char>());
  } else {
    std::cerr << "Could not open file: '" << fileName << "'" << std::endl;
  }

  return result;
}

void Program::setShaderNames(const std::string &v, const std::string &f,
                             const std::string &g, const std::string &tcs,
                             const std::string &tes) {
  vShaderName = v;
  fShaderName = f;
  gShaderName = g;
  tesShaderName = tes;
  tcsShaderName = tcs;
}

bool Program::addShader(GLenum shader_type, std::string &shader_name) {
  if (!pid) {
    std::cout << "Error trying to add shader before program created!\n";
    return false;
  }
  GLint rc;
  // create shader handle
  GLuint shader = glCreateShader(shader_type);

  // bind source code to shader handle
  std::string shader_src_string = readFileAsString(shader_name);
  const char *shader_src = shader_src_string.c_str();
  CHECKED_GL_CALL(glShaderSource(shader, 1, &shader_src, NULL));

  // compile the shader
  CHECKED_GL_CALL(glCompileShader(shader));
  CHECKED_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &rc));
  if (!rc) {
    if (isVerbose()) {
      GLSL::printShaderInfoLog(shader);
      std::cout << "Error compiling " << shader_name << "\n";
    }
    return false;
  }

  CHECKED_GL_CALL(glAttachShader(pid, shader));
	glDeleteShader(shader);//TODO this might be bad
  return true;
}

bool Program::init() {
  GLint rc;
  bool tes_shader_set = !tesShaderName.empty();
  bool tcs_shader_set = !tcsShaderName.empty();
	bool g_shader_set   = !gShaderName.empty();

  // Create the program and link
  pid = glCreateProgram();

  // add shaders to created program
  addShader(GL_VERTEX_SHADER, vShaderName);
  addShader(GL_FRAGMENT_SHADER, fShaderName);
  if (tes_shader_set && tcs_shader_set) {
    addShader(GL_TESS_EVALUATION_SHADER, tesShaderName);
    addShader(GL_TESS_CONTROL_SHADER, tcsShaderName);
  }
	if(g_shader_set)
		addShader(GL_GEOMETRY_SHADER, gShaderName);
	
  CHECKED_GL_CALL(glLinkProgram(pid));
  CHECKED_GL_CALL(glGetProgramiv(pid, GL_LINK_STATUS, &rc));
  if (!rc) {
    if (isVerbose()) {
      GLSL::printProgramInfoLog(pid);
      std::cout << "Error linking shaders " << vShaderName << " and "
                << fShaderName << std::endl;
    }
    return false;
  }


  return true;
}

void Program::bind() { CHECKED_GL_CALL(glUseProgram(pid)); }

void Program::unbind() { CHECKED_GL_CALL(glUseProgram(0)); }

void Program::addAttribute(const std::string &name) {
  attributes[name] = GLSL::getAttribLocation(pid, name.c_str(), isVerbose());
}

void Program::addUniform(const std::string &name) {
  uniforms[name] = GLSL::getUniformLocation(pid, name.c_str(), isVerbose());
}

GLint Program::getAttribute(const std::string &name) const {
  std::map<std::string, GLint>::const_iterator attribute =
      attributes.find(name.c_str());
  if (attribute == attributes.end()) {
    if (isVerbose()) {
      std::cout << name << " is not an attribute variable" << std::endl;
    }
    return -1;
  }
  return attribute->second;
}

GLint Program::getUniform(const std::string &name) const {
  std::map<std::string, GLint>::const_iterator uniform =
      uniforms.find(name.c_str());
  if (uniform == uniforms.end()) {
    if (isVerbose()) {
      std::cout << name << " is not a uniform variable" << std::endl;
    }
    return -1;
  }
  return uniform->second;
}
