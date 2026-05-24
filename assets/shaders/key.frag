#version 330 core
in vec2 vUv;
out vec4 FragColor;
uniform vec3 uColor;
uniform float uIntensity; // ~0.55 idle, ~1.0 hover, ~1.6 pressed
uniform float uAspect;    // key NDC width / height, for isotropic rounding

void main()
{
  // Work in units of the key's shorter axis so corner radius and border width
  // stay isotropic regardless of key aspect ratio (e.g. the wide space bar).
  vec2 p = vUv * 2.0 - 1.0; // [-1,1]
  vec2 q, halfExtent;
  if (uAspect >= 1.0)
  {
    q = vec2(p.x * uAspect, p.y);
    halfExtent = vec2(0.86 * uAspect, 0.86);
  }
  else
  {
    q = vec2(p.x, p.y / uAspect);
    halfExtent = vec2(0.86, 0.86 / uAspect);
  }

  float radius = 0.20;
  vec2 d = abs(q) - (halfExtent - radius);
  float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;

  float border = 1.0 - smoothstep(0.0, 0.05, abs(dist)); // bright outline
  float fill   = (1.0 - smoothstep(-0.04, 0.0, dist)) * 0.16; // faint interior
  float halo   = smoothstep(0.30, 0.0, abs(dist)) * 0.45; // soft outer glow

  float a = clamp((border + fill + halo) * uIntensity, 0.0, 1.0);
  FragColor = vec4(uColor * a, a); // premultiplied alpha
}
