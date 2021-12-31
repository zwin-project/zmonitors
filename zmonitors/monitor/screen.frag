#version 410 core

uniform sampler2D userTexture;

in vec2 uvCoords;
out vec4 outputColor;

void main()
{
  outputColor = texture(userTexture, uvCoords);
}
