R""(
#version 430 core

struct Light {
  vec3 color;
  float intensity;
};

struct Triangle {
  vec3 pos;
  int texture_id;
  vec2 texture_scaling;
  vec2 texture_offset;
  Light light;
};

struct Tree {
  bool leaf;
};

out vec4 FragColor;

layout (std430, binding = 1) buffer triangle_b
{
  Triangle triangles[];
};

layout (std430, binding = 2) buffer tree_b
{
  Tree tree[];
};

uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;

uniform ivec3 size;

vec4 trace(vec3 eye, vec3 dir) {
  return vec4(0.0f, 1.0f, 0.0f, 1.0f);
}




vec3 trace(vec2 pos) {
  return vec3(1, 0, 0);
}

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec2 pos = vec2(pix) / vec2(size.x - 1, size.y - 1);
    vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);

    FragColor = trace(eye, dir);
}
)""
