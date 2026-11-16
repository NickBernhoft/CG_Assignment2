#version 430

float atomScale = 0.5; // scaling against a default 1.0
float fov = radians(30.0);
float camera_dist = 120.0;

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) writeonly uniform image2D img_out;

struct Atom {
    vec4 center_radius;
    vec4 color;
};

struct Camera {
    vec4 U;
    vec4 V;
    vec4 W;
    vec4 e;
};

const Camera camera = Camera(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, camera_dist, 1.0)
);

layout (std430, binding = 0) readonly buffer atomsDataBuffer {
    Atom atoms[];
};

struct Ray {
    vec3 O;
    vec3 d;
};

void main() {
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
    int hit_idx = -1;

    Ray ray;
    ray.O = camera.e.xyz;
    ray.d = normalize(vec3(
        -camera.W +
        (1.0 - (2.0 * float(texelCoord.y) + 1.0) / resolution.y) * (height / 2.0) * camera.V +
        (1.0 - (2.0 * float(texelCoord.x) + 1.0) / resolution.x) * (width  / 2.0) * camera.U
    ));


    for (int i = 0; i < atoms.length(); i++) {

    	// min raduis value
       	float r = (atoms[i].center_radius.w > 0.01) ? atoms[i].center_radius.w : 1.0;
       	r = r * atomScale;

        float a = dot(ray.d, ray.d);
        vec3 oc = ray.O - atoms[i].center_radius.xyz;
        float b = 2.0 * dot(ray.d, oc);
        float c = dot(oc, oc) - (r * r);
        float disc = b * b - 4.0 * a * c;

        if (disc < 0.0) { continue; }

        float t = (-b - sqrt(disc)) / (2.0 * a);

        if (t > 0.0 && t < min_dist) {
            min_dist = t;
            hit_idx = i;
        }
    }


    if (hit_idx != -1) {
        // Compute 3D hit point and surface normal for simple sphere shading
        vec3 colision_pos = ray.O + ray.d * min_dist;
        vec3 normal = normalize(colision_pos - atoms[hit_idx].center_radius.xyz);
        vec3 light_dir = normalize(vec3(0.4, 0.8, 0.6));
        float diff = max(dot(normal, light_dir), 0.0);

        // Fallback to white if atom color in struct is (0,0,0)
        vec3 baseColor = atoms[hit_idx].color.rgb;
        if (length(baseColor) < 0.01) {
            baseColor = vec3(0.8, 0.8, 0.8);
        }

        value = vec4(baseColor * (0.25 + 0.75 * diff), 1.0);
    }

    imageStore(img_out, texelCoord, value);
}
