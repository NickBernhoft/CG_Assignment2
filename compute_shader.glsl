#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) writeonly uniform image2D img_out;

struct Atom {
    vec4 center_radius;
    vec4 color;
};

layout (std430, binding = 0) readonly buffer atomsDataBuffer {
    Atom atoms[];
};

// Camera uniforms updated every frame from C++
uniform vec3 uCamU; // Right axis
uniform vec3 uCamV; // Up axis
uniform vec3 uCamW; // Backwards axis (normalized)
uniform vec3 uCamE; // Camera eye position

struct Ray {
    vec3 O;
    vec3 d;
};

void main() {
    float fov = radians(30.0);

    vec4 value = vec4(0.00, 0.1, 0.1, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dims = imageSize(img_out);
    if (texelCoord.x >= dims.x || texelCoord.y >= dims.y) {
        return;
    }

    vec2 resolution = vec2(dims);
    float height = 2.0 * tan(fov / 2.0);
    float width = height * (resolution.x / resolution.y);

    float min_dist = 100000.0;
    int hit_index = -1;

    Ray ray;
    ray.O = uCamE;
    ray.d = normalize(
        -uCamW +
        (1.0 - (2.0 * float(texelCoord.y) + 1.0) / resolution.y) * (height / 2.0) * uCamV +
        (1.0 - (2.0 * float(texelCoord.x) + 1.0) / resolution.x) * (width  / 2.0) * uCamU
    );


    float raduis_scalar = 0.4;
    for (int i = 0; i < atoms.length(); i++) {
        float r = atoms[i].center_radius.w * raduis_scalar;

        float a = dot(ray.d, ray.d);
        vec3 o_c = ray.O - atoms[i].center_radius.xyz;
        float b = 2.0 * dot(ray.d, o_c);
        float c = dot(o_c, o_c) - (r * r);
        float disc = b * b - 4.0 * a * c;

        if (disc < 0.0) { continue; }

        float t = (-b - sqrt(disc)) / (2.0 * a);

        if (t > 0.0 && t < min_dist) {
            min_dist = t;
            hit_index = i;
        }
    }

    if (hit_index != -1) {
        vec3 hitPos = ray.O + ray.d * min_dist;
        vec3 normal = normalize(hitPos - atoms[hit_index].center_radius.xyz);
        vec3 lightDir = normalize(vec3(0.4, 0.8, 0.6));
        float diff = max(dot(normal, lightDir), 0.0);

        vec3 baseColor = atoms[hit_index].color.rgb;
        value = vec4(baseColor * (0.25 + 0.75 * diff), 1.0);
    }

    imageStore(img_out, texelCoord, value);
}
