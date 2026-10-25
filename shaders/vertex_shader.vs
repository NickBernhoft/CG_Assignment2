#version 330 core
// vertex shader

uniform mat4 trans_matrix;

in vec3 vpos;
in vec3 vnml;
in vec2 vtex;

out vec2 TexCoord;
out vec3 vNormal;

void main()
{
    gl_Position = trans_matrix * vec4(vpos, 1.0);

    TexCoord = vtex;
    vNormal = vnml;
}
