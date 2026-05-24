#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
uniform float uAlpha; // <1 for the soft halo pass, 1 for the bright core
void main()
{
  FragColor = vec4(uColor * uAlpha, uAlpha); // premultiplied alpha (laser glow)
}
