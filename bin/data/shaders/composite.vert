#version 330

uniform mat4 modelViewProjectionMatrix;
uniform ivec2 screenSize;
in vec4 position;

void main() {
  gl_Position = modelViewProjectionMatrix * position;
}
