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

out vec4 FragColor;

flat layout (location = 1) in Triangle triangle_data;




vec3 trace(vec2 pos) {
  return vec3(1, 0, 0);
}

void main()
{
    vec4 pos = gl_FragCoord;

    vec3 color = trace(pos.xy);

    FragColor = vec4(color.xyz, 1.0f);
}
)""
