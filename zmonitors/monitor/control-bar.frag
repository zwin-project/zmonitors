#version 410 core

uniform sampler2D userTexture;
uniform vec3 color;

in vec2 uvCoords;
out vec4 outputColor;

void main()
{
  outputColor = vec4(color, texture(userTexture, uvCoords).a);
}
