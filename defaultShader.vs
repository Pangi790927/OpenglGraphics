#version 130

uniform mat4 worldMatrix, viewMatrix, projectionMatrix;

out vec4 color;
out vec2 fragTexCoord;
out vec4 position;

// in vec4 position;
// in vec3 color;
 
// out vec3 Color;
 
void main()
{
    // Color = color;
    color = gl_Color;
    fragTexCoord = gl_MultiTexCoord0;// / 2.0f + vec4(0.5, 0.05, 0.05, 0.05); 

    // position = gl_Vertex;

    position = projectionMatrix * viewMatrix * worldMatrix * gl_Vertex;
}