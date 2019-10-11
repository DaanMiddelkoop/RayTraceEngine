R""(
#version 430 core

struct Light {
  vec3 color;
  float intensity;
};

struct Triangle {
  vec3 a;
  vec3 b;
  vec3 c;
};

struct Tree {
  vec3 minpos;
  int parent;

  vec3 maxpos;
  int leaf;

  vec3 offset;
  int leaf_id;

  vec3 rotation;
  int node1;

  int node2;
};

struct Ray {
  vec3 origin;
  vec3 dir;
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
uniform vec3 eye_dir;

uniform ivec2 size;

vec4 trace(vec3 eye, vec3 dir) {
  return vec4(eye.xyz, 1.0f);
}

bool TriangleHit() {

  return false;
}

bool AABBHit(Ray ray, vec3 aabbmin, vec3 aabbmax, float tmin, float tmax) {
  vec3 invD = 1.0 / ray.dir;
	vec3 t0s = (aabbmin - ray.origin) * invD;
	vec3 t1s = (aabbmax - ray.origin) * invD;

	vec3 tsmaller = min(t0s, t1s);
	vec3 tbigger  = max(t0s, t1s);

	tmin = max(tmin, max(tsmaller.x, max(tsmaller.y, tsmaller.z)));
	tmax = min(tmax, min(tbigger.x, min(tbigger.y, tbigger.z)));

	return (tmin < tmax);
}

vec3 trace(vec2 pos) {
  return vec3(1, 0, 0);
}

void main()
{
    float gemsize = (size.x + size.y) / 2.0f;

    vec3 normal1 = -normalize(cross(eye_dir, vec3(0.0f, 1.0f, 0.0f))) * (size.x / gemsize);
    vec3 normal2 = normalize(cross(eye_dir, normal1)) * (size.y / gemsize);


    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec2 pos = (vec2(pix) / vec2(size.x - 1, size.y - 1)) - vec2(0.5, 0.5);

    vec3 dir = eye_dir + ((pos.x * normal1) + (pos.y * normal2));

    Ray ray;
    ray.origin = eye;
    ray.dir = dir;

    //FragColor = vec4(pos.x, pos.y, 0.0f, 1.0f);
    if (AABBHit(ray, tree[0].minpos, tree[0].maxpos, 0.0f, 10000000.0f)) {
      FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
      FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    //FragColor = trace(eye, dir);
}
)""
