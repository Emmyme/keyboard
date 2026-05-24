#version 330 core
layout(location = 0) in vec2 aPos; // unit quad [0,1]
uniform vec2 uRectMin;             // NDC lower-left
uniform vec2 uRectMax;             // NDC upper-right
out vec2 vUv;                      // [0,1] within the key cell
void main()
{
  vUv = aPos;
  vec2 ndc = mix(uRectMin, uRectMax, aPos);
  gl_Position = vec4(ndc, 0.0, 1.0);
}
