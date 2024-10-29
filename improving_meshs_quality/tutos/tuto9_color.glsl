#version 330

#ifdef VERTEX_SHADER
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 vpMatrix;
out vec3 n;

void main( )
{
    n = normal;
    gl_Position = vpMatrix * vec4(position, 1);
    //gl_Position = vec4(position, 1);
}

#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;
in vec3 n;
vec3 light = normalize(vec3(0.5, -0.5, 0.3));

void main( )
{
    float cos_theta = dot(light, n);
    fragment_color = vec4(cos_theta, cos_theta, cos_theta, 1);
    //fragment_color = vec4(1, 1, 1, 1);
}

#endif
