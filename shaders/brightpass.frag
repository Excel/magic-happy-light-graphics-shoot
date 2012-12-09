uniform sampler2D tex;
const vec3 avgVector = vec3(0.299, 0.587, 0.114);
void main(void) {
    vec4 sample = texture2D(tex, gl_TexCoord[0].st);
    float luminance = max(0.0, dot(avgVector, sample.rgb));

    // TODO: Step 1 - update the shader so that we set the
    // fragment color to black if the luminance is less than 1
    
    sample.rgb = vec3(luminance, luminance, luminance);
    
    sample.rgb *= sign(luminance - 1.0);
    gl_FragColor = sample;
}
