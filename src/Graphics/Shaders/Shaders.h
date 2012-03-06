#ifdef RAINBOW_IOS

#define GL_DRAW_FRAMEBUFFER  GL_DRAW_FRAMEBUFFER_APPLE

const char canvas_vsh[] =
"attribute vec2 texcoord;"
"attribute vec2 vertex;"

"uniform mat4 mvp_matrix;"

"varying lowp vec2 v_texcoord;"

"void main()"
"{"
"	v_texcoord = texcoord;"
"	gl_Position = vec4(vertex, 0.0, 1.0) * mvp_matrix;"
"}";

const char canvaseraser_fsh[] =
"precision highp float;"

"uniform lowp sampler2D canvas;"
"uniform lowp sampler2D texture;"
"uniform lowp vec4 color;"

"varying lowp vec2 v_texcoord;"

"void main()"
"{"
"	gl_FragColor = texture2D(canvas, v_texcoord);"
"	gl_FragColor.a -= texture2D(texture, v_texcoord).a;"
"}";

const char fixed2d_vsh[] =
"attribute vec4 color;"
"attribute vec2 texcoord;"
"attribute vec2 vertex;"

"uniform mat4 mvp_matrix;"

"varying lowp vec4 v_color;"
"varying lowp vec2 v_texcoord;"

"void main()"
"{"
"	v_color = color;"
"	v_texcoord = texcoord;"
"	gl_Position = vec4(vertex, 0.0, 1.0) * mvp_matrix;"
"}";

const char fixed2d_fsh[] =
"precision highp float;"

"uniform lowp sampler2D texture;"

"varying lowp vec4 v_color;"
"varying lowp vec2 v_texcoord;"

"void main()"
"{"
"	gl_FragColor = texture2D(texture, v_texcoord) * v_color;"
"}";

#endif
