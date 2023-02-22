#version 440 core                     //sets version of OpenGL to operate
layout (location = 0) in vec3 aPos;   // Position of vertices as a vector size 3 used in glVertexAttribPointer method
layout (location = 1) in vec3 aColor; // Position of rgb color vertices as vector size 3 also used in pointer method
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNorm;

out vec3 ourColor;                    // outputs color to fragment shader file
out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

//uniform variables for MVP model
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);    //sets postion
    FragPos = vec3(model * vec4(aPos, 1.0));
    ourColor = aColor;                //sets color
    TexCoord = aTexCoord;

     Normal = mat3(transpose(inverse(model))) * aNorm; 
}