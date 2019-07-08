#version 330

uniform sampler2D depth;
uniform mat4 modelViewProjectionMatrix;
uniform ivec2 screenSize;
in vec4 position;

void main() {
  gl_Position = modelViewProjectionMatrix * position;
}
