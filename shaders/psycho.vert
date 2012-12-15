varying float intensity;

uniform float time;
    
varying vec3 vertex;	// The position of the vertex, in eye space
varying vec3 light;	// The normalized vector from the vertex to the light
varying vec3 eye;	// The normalized vector from the vertex to the eye
varying vec3 normal;	// The normal vector of the vertex, in eye space
    
float computeScale(float t, float s)
{
    return .8 + .2 * sin(3*(t + s));
}
    
void main()
{
    
    light = normalize(gl_LightSource[0].position.xyz - vertex);
    eye = -vertex;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    //float scale = computeScale(time);
    
    vec4 vertex = gl_Vertex;
    vertex.z = vertex.z * computeScale(time, 0);
    vertex.x = vertex.x * computeScale(time, 100);
    float ds = vertex.x + vertex.x + vertex.z + vertex.z;
    vertex.y += sin(ds*sin(time));
    vertex.x += cos(time + 90);
        
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    
    light = normalize(gl_LightSource[0].position.xyz - vertex);
    eye = -vertex;
    normal = normalize(gl_NormalMatrix * gl_Normal);    
} 