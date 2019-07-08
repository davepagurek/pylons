#version 330

precision mediump float;

uniform mat4 modelViewProjectionMatrix;
in vec4 position;
in vec4 color;

out vec4 fragColor;

void main() {
  fragColor = color;
  gl_Position = modelViewProjectionMatrix * position;
}
