#version 330 core
// vertex shader

uniform mat4 trans_matrix;

in vec3 vpos;
in vec3 vnml;
in vec2 vtex;

void main()
{
    gl_Position = trans_matrix * vec4(vpos, 1.0);
}
