R""(
#version 430 core

struct Light {
  vec3 color;
  float intensity;
};

struct Triangle {
  vec3 a;
  int material;

  vec3 b;
  float u3;

  vec3 c;
  float v3;

  float u1;
  float v1;
  float u2;
  float v2;

  vec3 n1;
  float w1;

  vec3 n2;
  float w2;

  vec3 n3;
  float w3;
};

struct Tree {
  vec3 minpos;
  int parent;

  vec3 maxpos;
  int leaf;

  int leaf_id;
  int node1;
  int node2;
  int depth;

  int isObject;
  int transform_id;
  int transform_parent;
  float padding0;
};

struct Ray {
  vec3 origin;
  vec3 dir;
};

struct Material {
  int texture_id;
  float color_tint_red;
  float color_tint_green;
  float color_tint_blue;
};

struct Texture {
  int width;
  int height;
  int pixel_pointer;
  float padding1;
};

struct Transform {
  mat4 m;

  int node_id;
  int parent_id;
  float padding0;
  float padding1;
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

layout (std430, binding = 3) buffer material_b
{
  Material materials[];
};

layout (std430, binding = 4) buffer texture_b
{
  Texture textures[];
};

layout (std430, binding = 5) buffer pixel_b
{
  uint pixels[];
};

layout (std430, binding = 6) buffer transform_b
{
  Transform transforms[];
};

uniform vec3 eye;
uniform vec3 eye_dir;
uniform vec3 debug;

uniform ivec2 size;
uniform int scene_root;

vec4 getTextureColor(int tr, float u, float v) {
  Triangle t = triangles[tr];

  if (t.material >= 0) {
    Material mat = materials[t.material];


    Texture tex = textures[mat.texture_id];

    //return vec4(u, v, 0.0, 1.0);
    int x = int(((1 - u - v) * t.u1) + (u * t.u2) + (v * t.u3));
    int y = int(((1 - u - v) * t.v1) + (u * t.v2) + (v * t.v3));

    //return vec4(float(pixels[tex.pixel_pointer] / 255), 0.0, 0.0, 1.0);

    int pixel_loc = y * tex.width + x;

    uint color = pixels[tex.pixel_pointer + pixel_loc];

    //return vec4(float(tex.pixel_pointer + pixel_loc) / 255, float(0) / 255, float(tex.width) / 255, 1.0);


    uint r = color & (0xFF << 24);
    uint g = color & (0xFF << 16);
    uint b = color & (0xFF << 8);
    uint a = color & (0xFF);

    return vec4(float(r) / 255, float(g) / 255, float(b) / (255), float(a) / (255));
  } else {
    return vec4(0.0, 0.0, 0.0, 1.0);
  }
}

bool TriangleHit(Ray ray, Triangle tr, out float t, out float u, out float v) {
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

bool AABBHit(Ray ray, vec3 aabbmin, vec3 aabbmax, out float tmin) {
  vec3 invD = 1.0 / ray.dir;
	vec3 t0s = (aabbmin - ray.origin) * invD;
	vec3 t1s = (aabbmax - ray.origin) * invD;

	vec3 tsmaller = min(t0s, t1s);
	vec3 tbigger  = max(t0s, t1s);

	tmin = max(tsmaller.x, max(tsmaller.y, tsmaller.z));
	float tmax = min(tbigger.x, min(tbigger.y, tbigger.z));

	return (tmin <= tmax);
}


// Triangle hit, -1 for miss
int traverseTree(Ray ray, out float t, out float u, out float v) {
  int treenodes[25];

  mat4 tm = mat4(1.0, 0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0,
                  0.0, 0.0, 1.0, 0.0,
                  0.0, 0.0, 0.0, 1.0);


  t = 99999999999.0;
  int currentNode = 0;
  int nextNode = 0;

  int triangle_id = -1;
  int currentTransform = -1;

  // manually check for hit with root.
  float t_stuff;
  if (AABBHit(ray, tree[scene_root].minpos, tree[scene_root].maxpos, t_stuff)) {
    treenodes[0] = scene_root;
  } else {
    return -1;
  }

  int maxTreenodes = 0;


  while (nextNode > -1) {

    currentNode = nextNode;
    maxTreenodes += 1;

    int tree_index = treenodes[currentNode];



    // If node contains a transform, apply it.
    int transform_id = tree[tree_index].transform_id;
    if (transform_id != -1) {
      currentTransform = transform_id;
      tm = tm * inverse(transforms[transform_id].m);
    }

    Ray moved_ray;
    moved_ray.origin = (tm * vec4(ray.origin.xyz, 1)).xyz;
    moved_ray.dir    = (tm * vec4(ray.dir   .xyz, 0)).xyz;



    if (tree[tree_index].leaf == 1) {
        // Test triangle hit directly so we dont store possible triangles in way to large buffers slowing our stuff down, also less branching and ****
        int tr_id = tree[tree_index].leaf_id;

        float new_t;
        float new_u;
        float new_v;
        if (TriangleHit(moved_ray, triangles[tr_id], new_t, new_u, new_v) && new_t < t) {
          u = new_u;
          v = new_v;
          t = new_t;
          triangle_id = tr_id;

        }

        nextNode = currentNode - 1;
    }
    else
    {

      int node1 = tree[tree_index].node1;
      int node2 = tree[tree_index].node2;

      float boxdist1;
      float boxdist2;

      bool hit1 = AABBHit(moved_ray, tree[node1].minpos, tree[node1].maxpos, boxdist1);
      hit1 = hit1 && boxdist1 < t;

      bool hit2 = AABBHit(moved_ray, tree[node2].minpos, tree[node2].maxpos, boxdist2);
      hit2 = hit2 && boxdist2 < t;

      if (hit1) {
        if (hit2) {
          if (boxdist1 < boxdist2) {
            treenodes[currentNode] = node2;
            treenodes[currentNode + 1] = node1;
          } else {
            treenodes[currentNode] = node1;
            treenodes[currentNode + 1] = node2;
          }
          nextNode = currentNode + 1;
        } else {
          treenodes[currentNode] = node1;
        }
      }
      else
      {
        if (hit2) {
          treenodes[currentNode] = node2;
        }
        else
        {
          nextNode = currentNode - 1;
        }
      }
    }


    // revert all transforms required before jumping to next node.
    // if (nextNode > -1) {
    //   int goalDepth = tree[treenodes[nextNode]].depth;
    //   int currentDepth = tree[tree_index].depth;
    //
    //   while (goalDepth <= currentDepth) {
    //     if (tree[tree_index].transform_id != -1) {
    //       tm = transforms[tree[tree_index].transform_id] * tm;
    //     }
    //
    //     tree_index = tree[tree_index].transform_parent;
    //     if (tree_index == -1) {
    //       break;
    //     }
    //     currentDepth = tree[tree_index].depth;
    //   }
    // }

    if (nextNode > -1 && currentTransform != -1) {
      int goalDepth = tree[treenodes[nextNode]].depth;
      int transformDepth = tree[transforms[currentTransform].node_id].depth;
      while (goalDepth <= transformDepth) {
        tm = transforms[currentTransform].m * tm;

        currentTransform = transforms[currentTransform].parent_id;
        if (currentTransform == -1) {
          break;
        }
        transformDepth = tree[transforms[currentTransform].node_id].depth;
      }
    }

  }
  return maxTreenodes;
  return triangle_id;
}

vec3 debugTrace(Ray ray) {
  float t, u, v;
  //u = 0;
  //v = 0;

  int tr = traverseTree(ray, t, u, v);
  if (tr == -1) {
    return vec3(1, 1, 1);
  }
  return vec3(float(tr) / 255, 0.0, 0.0);
}

vec3 trace(Ray ray) {
  float t, u, v;

  int tr = traverseTree(ray, t, u, v);
  if (tr == -1) {
    return vec3(1, 1, 1);
  }

  return getTextureColor(tr, u, v).xyz;
}

void main()
{
    float gemsize = (size.x + size.y) / 2.0f;

    vec3 normal1 = -normalize(cross(eye_dir, vec3(0.0f, 1.0f, 0.0f))) * (size.x / gemsize);
    vec3 normal2 = normalize(cross(eye_dir, normal1)) * (size.y / gemsize);

    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec2 pos = (vec2(pix) / vec2(size.x - 1, size.y - 1)) - vec2(0.5, 0.5);

    vec3 dir = normalize(eye_dir) + ((pos.x * normal1) + (pos.y * normal2));

    Ray ray;
    ray.origin = eye;
    ray.dir = normalize(dir);

    FragColor = vec4(debugTrace(ray).xyz, 1.0);
}
)""
