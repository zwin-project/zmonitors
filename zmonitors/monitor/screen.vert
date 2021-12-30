#version 410

uniform mat4 zMVP;
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uvCoordsIn;

out vec2 uvCoords;

void
main()
{
  uvCoords = uvCoordsIn;
  gl_Position = zMVP * position;
}
