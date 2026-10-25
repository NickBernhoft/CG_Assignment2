#version 330 core
// fragment shader

in vec2 TexCoord;
in vec3 vNormal;

uniform sampler2D uTexture;

out vec4 fragment;
void main()
{
    // arbitrary light vector
    vec3 targetVector = normalize(vec3(0.0, 1.0, 1.5)); 
    vec3 N = normalize(vNormal);
    
    // min luminance of 20%
    float alignment = max(dot(N, targetVector), 0.0) * 0.8 + 0.2; 
    
    vec4 texColor = texture(uTexture, TexCoord);
    fragment = texColor * vec4(vec3(alignment), 1.0);
}
