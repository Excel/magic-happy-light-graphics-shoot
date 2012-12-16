uniform float time;
    
varying vec3 vertex;		// The vector from the eye to the vertex
varying vec3 light;		// The normalized vector from the vertex to the light
varying vec3 eye;		// The normalized vector from the vertex to the eye
varying vec3 normal;		// The normal vector of the vertex, in eye space

uniform samplerCube envMap;	// The cube map containing the environment to reflect


float colorShift(float t, float s)
{
    return .5 + .5 * sin(4.0*(t + s));
}

void main()
{	
    vec4 color;
    color.w = 0.0;
    color.x = colorShift(time, 0.0);
    color.y = colorShift(time, 120.0);
    color.z = colorShift(time, 240.0);
    
    
    vec3 e = normalize(eye);
    vec3 n = normalize(normal);
    vec3 v = normalize(vertex);
    vec3 i = normalize(v - e);
    vec3 l = normalize(light);
    
    vec4 look = -gl_ModelViewMatrix * vec4(reflect(-l, n), 0);


	
    gl_FragColor = .6*textureCube(envMap, look.xyz) + .4*color;
    
    
}
