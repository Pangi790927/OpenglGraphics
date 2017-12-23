#version 130
 
// in vec3 Color;
out vec4 colorOut;

in vec2 fragTexCoord;
in vec4 color;

varying vec4 position;

uniform vec4 secondColor;

uniform sampler2D texture;
 
void main()
{
    // outputF = vec4(Color.r, Color.g, 0, 1.0);

    // vec4 pos = position / length(position);

	colorOut = mix(texture2D(texture, fragTexCoord), color, 0.0); 
    // vec4(gl_TexCoord[0].st, 1.0, 1.0); //texture2D(texture, gl_TexCoord[0].st);
}