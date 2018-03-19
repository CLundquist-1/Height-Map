#version 330 core
layout (location = 0) in vec3 aPos;							//In for position

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
uniform vec3 ourColor;

out vec3 vColor;

void main()
{
   gl_Position = project * view * model * vec4(aPos, 1.0);			//predefined output of the vertex shader
   //float colorLen = sqrt(aPos.x*aPos.x + aPos.y*aPos.y + aPos.x + aPos.z);
   //vec3 colorNorm = vec3(aPos.x/colorLen, aPos.y/colorLen, aPos.z/colorLen);
   //vColor = vec3(ourColor.x*colorNorm.x, ourColor.y*colorNorm.y, ourColor.z*colorNorm.z);
   //vColor = ourColor;
   vColor = vec3((mod(aPos.x, 10))*0.1, (mod(aPos.y, 10))*0.1, (mod(aPos.z, 10))*0.1);
};