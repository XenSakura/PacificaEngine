#pragma once
#include <shaderc/shaderc.hpp>

struct Shader
{

};

class ShaderManager
{
private:
public:
	ShaderManager();
	~ShaderManager();
	//for when we want to add shaders in real time and hot compile them
	void Add();
	void CompileAll();

};
