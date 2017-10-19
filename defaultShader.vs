#version 130
 
uniform mat4 worldMatrix, viewMatrix, projectionMatrix;
 
varying vec4 color;
varying vec2 fragTexCoord;
varying vec4 position;

// in vec4 position;
// in vec3 color;
 
// out vec3 Color;
 
void main()
{
    // Color = color;
    color = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;// / 2.0f + vec4(0.5, 0.05, 0.05, 0.05); 

    position = gl_Vertex;

    gl_Position = projectionMatrix * viewMatrix * worldMatrix * gl_Vertex;
}