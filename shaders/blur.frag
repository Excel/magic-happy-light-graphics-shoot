const int MAX_KERNEL_SIZE = 128;
uniform sampler2D tex;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE]; 
uniform float kernel[MAX_KERNEL_SIZE];
void main(void) { 
    // TODO: Step 2 - Fill this in!
    vec4 color = vec4(0, 0, 0, 0);

    vec2 textcoord = gl_TexCoord[0].st;
    for(int i = 0; i < arraySize; i++){
	color += kernel[i] * texture2D(tex, offsets[i] + textcoord);
    }    
    gl_FragColor = color;
}
