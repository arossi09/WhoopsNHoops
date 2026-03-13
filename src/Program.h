
#pragma once

#ifndef LAB471_PROGRAM_H_INCLUDED
#define LAB471_PROGRAM_H_INCLUDED

#include <map>
#include <string>

#include <glad/glad.h>

std::string readFileAsString(const std::string &fileName);

class Program {

public:
  void setVerbose(const bool v) { verbose = v; }
  bool isVerbose() const { return verbose; }

  void setShaderNames(const std::string &v, const std::string &f,
                      const std::string &g = "", const std::string &tcs = "",
                      const std::string &tes = "");
  virtual bool init();
  virtual void bind();
  virtual void unbind();

  void addAttribute(const std::string &name);
  void addUniform(const std::string &name);
  bool addShader(GLenum shader_type, std::string &shader_name);
  GLint getAttribute(const std::string &name) const;
  GLint getUniform(const std::string &name) const;

protected:
  std::string vShaderName;
  std::string fShaderName;
  std::string gShaderName;
  std::string tesShaderName;
  std::string tcsShaderName;

private:
  GLuint pid = 0;
  std::map<std::string, GLint> attributes;
  std::map<std::string, GLint> uniforms;
  bool verbose = true;
};

#endif // LAB471_PROGRAM_H_INCLUDED
