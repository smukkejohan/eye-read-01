#version 150

//out vec4 outputColor;
uniform sampler2DRect tex0;

in vec2 texCoordVarying;

out vec4 outputColor;

void main()
{
    vec4 rTxt = texture(tex0, texCoordVarying);
    //vec4 gTxt = texture(tex1, texCoordVarying);
    //vec4 bTxt = texture(tex2, texCoordVarying);
    //vec4 mask = texture(imageMask, texCoordVarying);
    
    //vec4 color = vec4(0, 0, 0, 0);
    /*color = mix(color, rTxt, mask.r);
    color = mix(color, rTxt, mask.g);
    color = mix(color, rTxt, mask.b);
    */
    outputColor = vec4(rTxt.r, rTxt.g, rTxt.b, rTxt.a);
}
