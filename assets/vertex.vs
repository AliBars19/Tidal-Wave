#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 worldParameters;
uniform mat4 model;

void main()
{
	gl_Position =  worldParameters * model * vec4(aPos, 1.0); // multiplication happens from right to left
	//															1. raw vertex position
	//															2. transforms it to the world position
	//															3. the screen projection converts it to screen coordinates		
	ourColor = aColor;
}