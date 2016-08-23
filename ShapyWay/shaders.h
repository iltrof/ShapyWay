#pragma once
#include <string>

static const std::string blurFragShader = \
"uniform sampler2D texture;" \
"uniform float radius;" \
"uniform vec2 dimensions;" \
"void main()" \
"{" \
"	vec2 offx = vec2(1/dimensions.x, 0.0);" \
"	vec2 offy = vec2(0.0, 1/dimensions.y);" \
"	" \
"   vec4 pixel; " \
"   for(float i = -radius; i<radius+0.5; i+=1) {" \
"		for(float j = -radius; j<radius+0.5; j+=1) {" \
"           pixel = pixel + texture2D(texture, gl_TexCoord[0].xy + offx*i + offy*j); " \
"       }" \
"   }" \
"	" \
"	gl_FragColor = gl_Color * (pixel / ((radius*2+1)*(radius*2+1)));" \
"}";

static const std::string SVColorFragShader = \
"uniform float hueIndex;" \
"uniform float hueFactor;" \
"void main()" \
"{" \
"	float vmin = (1-gl_TexCoord[0].x)*gl_TexCoord[0].y;" \
"	float a = (gl_TexCoord[0].y-vmin)*hueFactor;" \
"	float vinc = vmin+a;" \
"	float vdec = gl_TexCoord[0].y-a;" \
"	if(hueIndex == 0) gl_FragColor = vec4(gl_TexCoord[0].y, vinc, vmin, 1);" \
"	else if(hueIndex == 1) gl_FragColor = vec4(vdec, gl_TexCoord[0].y, vmin, 1);" \
"	else if(hueIndex == 2) gl_FragColor = vec4(vmin, gl_TexCoord[0].y, vinc, 1);" \
"	else if(hueIndex == 3) gl_FragColor = vec4(vmin, vdec, gl_TexCoord[0].y, 1);" \
"	else if(hueIndex == 4) gl_FragColor = vec4(vinc, vmin, gl_TexCoord[0].y, 1);" \
"	else gl_FragColor = vec4(gl_TexCoord[0].y, vmin, vdec, 1);" \
"}";

static const std::string HColorFragShader = \
"void main()" \
"{" \
"	float hue =	gl_TexCoord[0].x;" \
"	float hueIndex = floor(hue*6);" \
"	hue -= (hueIndex/6);" \
"	float a = hue*6;" \
"	if(hueIndex == 0) gl_FragColor = vec4(1, a, 0, 1);" \
"	else if(hueIndex == 1) gl_FragColor = vec4(1-a, 1, 0, 1);" \
"	else if(hueIndex == 2) gl_FragColor = vec4(0, 1, a, 1);" \
"	else if(hueIndex == 3) gl_FragColor = vec4(0, 1-a, 1, 1);" \
"	else if(hueIndex == 4) gl_FragColor = vec4(a, 0, 1, 1);" \
"	else gl_FragColor = vec4(1, 0, 1-a, 1);" \
"}";

static const std::string defaultFragShader = \
"uniform sampler2D texture;" \
"void main()" \
"{" \
"   vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);" \
"	gl_FragColor = pixel;" \
"}";