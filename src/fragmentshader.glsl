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
uniform vec3 debug;

uniform ivec2 size;

vec4 trace(vec3 eye, vec3 dir) {
  return vec4(eye.xyz, 1.0f);
}

bool TriangleHit(Ray ray, Triangle tr, inout float t, inout float u, inout float v) {
  vec3 v0v1 = tr.b - tr.a;
  vec3 v0v2 = tr.c - tr.a;
  vec3 pvec = cross(ray.dir, v0v2);
  float det = dot(v0v1, pvec);

  if (det < 0.000001 && det > -0.00001) {
    return false;
  }

  float invDet = 1 / det;

  vec3 tvec = ray.origin - tr.a;
  u = dot(tvec, pvec) * invDet;
  if (u < 0 || u > 1) return false;

  vec3 qvec = cross(tvec, v0v1);
  v = dot(ray.dir, qvec) * invDet;
  if (v < 0 || u + v > 1) return false;

  t = dot(v0v2, qvec) * invDet;
  return true;
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

// Triangle hit, -1 for miss
int traverseTree(Ray ray, inout float t, inout float u, inout float v) {
  int treenodes[60];
  int trianglenodes[10];

  int currentNode = 0;
  int maxTreenodes = 0;

  // manually check for hit with root.
  if (AABBHit(ray, tree[0].minpos, tree[0].maxpos, 0, 20000.0)) {
    treenodes[0] = 0;
    maxTreenodes = 1;
  }
  int maxTriangles = 0;

  while (currentNode < maxTreenodes) {
    if (tree[treenodes[currentNode]].leaf == 1) {
        trianglenodes[maxTriangles] = tree[treenodes[currentNode]].leaf_id;
        maxTriangles += 1;
        currentNode += 1;
    }
    else
    {
      int node1 = tree[treenodes[currentNode]].node1;
      int node2 = tree[treenodes[currentNode]].node2;
      bool hit1 = AABBHit(ray, tree[node1].minpos, tree[node1].maxpos, 0, 20000);
      bool hit2 = AABBHit(ray, tree[node2].minpos, tree[node2].maxpos, 0, 20000);

      if (hit1) {
        treenodes[currentNode] = node1;
        if (hit2) {
          treenodes[maxTreenodes] = node2;
          maxTreenodes += 1;
        }
      }
      else
      {
        if (hit2) {
          treenodes[currentNode] = node2;
        }
        else
        {
          currentNode += 1;
        }
      }
    }
  }

  currentNode = 0;
  t = 9999999999.0;
  int triangle_id = -1;

  while (currentNode < maxTriangles) {
    float new_t;
    float new_u;
    float new_v;
    if (TriangleHit(ray, triangles[trianglenodes[currentNode]], new_t, new_u, new_v) && new_t < t) {
      u = new_u;
      v = new_v;
      triangle_id = currentNode;
    }
    currentNode += 1;
  }

  //return maxTreenodes;
  return triangle_id;
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

    float t;
    float u;
    float v;

    //FragColor = vec4(pos.x, pos.y, 0.0f, 1.0f);
    int tr = traverseTree(ray, t, u, v);
    if (tr >= 0) {
      FragColor = vec4(float(tr) / 4, 0.0, 0.0f, 1.0f);
    } else {
      FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    //FragColor = trace(eye, dir);
}
)""
