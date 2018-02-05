#version 440

// *********************************
// Define the output colour for the shader here
uniform vec4 colour[2];
// *********************************

// Outgoing colour for the shader
layout(location = 0) out vec4 out_colour;

void main() {
  // *********************************
  // Set outgoing colour
  out_colour = mix(colour[0], colour[1], 0.5);
  // *********************************
}