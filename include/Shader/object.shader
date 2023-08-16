#ifndef OBJECT
#define OBJECT


namespace Hop::System::Rendering
{
    // Layout MUST be specified for Android!! (else shit fps)
    static const char * objectVertexShader __attribute__((unused)) = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "layout(location=0) in vec4 a_position;\n"
        "layout(location=1) in vec4 a_offset;\n"
        "layout(location=2) in vec4 a_colour;\n"
        "layout(location=3) in vec4 a_texOffset;\n"
        "layout(location=4) in vec4 a_util;\n"
        "uniform mat4 proj;\n"
        "uniform float atlasN;\n"
        "out vec2 texCoord;\n"
        "out vec2 atlasTexCoord;\n"
        "out vec4 oColour;\n"
        "out float theta;\n"
        "out vec4 util;\n"
        "void main()\n"
        "{\n"
            "float cosine = cos(a_offset.z); float sine = sin(a_offset.z);\n"
            "mat2 rot = mat2(cosine,-sine,sine,cosine);"
            "vec2 pos = rot * a_position.xy;\n"
            "gl_Position = proj*vec4(a_offset.w*pos+a_offset.xy,0.0,1.0);\n"
            "texCoord = a_position.zw;\n"
            "vec2 tileCoord = (texCoord / atlasN);\n"
            "atlasTexCoord = tileCoord + tileCoord*a_texOffset.xy;\n"
            "oColour = a_colour;\n"
            "theta = a_offset.z;\n"
            "util = a_util;\n"
        "}";

    // circle

    static const char * circleObjectFragmentShader __attribute__((unused)) = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "in vec2 texCoord;\n"
        "in vec2 atlasTexCoord;\n"
        "in vec4 oColour;\n"
        "in float theta;\n"
        "uniform sampler2D tex; out vec4 colour;\n"
        "void main()"
        "{\n"
        "   vec2 c = texCoord-vec2(0.5,0.5);\n"
        "   if (dot(c,c) > 0.5*0.5) {discard;}\n"
        "   colour = oColour;\n"
        "}";

    // line-segment

    static const char * lineSegmentObjectVertexShader __attribute__((unused)) = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "layout(location=0) in vec4 a_position;\n"
        "layout(location=1) in vec4 a_offset;\n"
        "layout(location=2) in vec4 a_colour;\n"
        "layout(location=3) in vec4 a_texOffset;\n"
        "layout(location=4) in vec4 a_util;\n"
        "uniform mat4 proj;\n"
        "uniform float atlasN;\n"
        "out vec2 texCoord;\n"
        "out vec2 atlasTexCoord;\n"
        "out vec4 oColour;\n"
        "out float th;\n"
        "out vec2 a; out vec2 b;\n"
        "void main()\n"
        "{\n"
            "float cosine = cos(a_offset.z); float sine = sin(a_offset.z);\n"
            "mat2 rot = mat2(cosine,-sine,sine,cosine);"
            "vec2 pos = rot * a_position.xy;\n"
            "gl_Position = proj*vec4(a_offset.w*pos+a_offset.xy,0.0,1.0);\n"
            "vec2 tileCoord = (texCoord / atlasN);\n"
            "atlasTexCoord = tileCoord + tileCoord*a_texOffset.xy;\n"
            "oColour = a_colour;\n"
            "texCoord = pos;\n"
            "th = a_util.z;\n"
            "a = rot * vec2(a_util.x+a_util.z*0.5,a_util.y);\n"
            "b = rot * vec2(a_util.x+a_util.z*0.5,a_util.y+a_util.w);\n"
        "}";

    static const char * lineSegmentObjectFragmentShader __attribute__((unused)) = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "in vec2 texCoord;\n"
        "in vec2 atlasTexCoord;\n"
        "in vec4 oColour;\n"
        "in vec2 a; in vec2 b;\n"
        "in float th;\n"
        "uniform sampler2D tex; out vec4 colour;\n"
        "void main()"
        "{\n"
        "   float l = length(b-a);"
        "   vec2 d = (b-a)/l;\n"
        "   vec2 q = texCoord - (a+b)*0.5;\n"
        "   q = mat2(d.x,-d.y,d.y,d.x)*q;\n"
        "   q = abs(q)-vec2(l*0.5,th);\n"
        "   float sd = length(max(q,0.0)) + min(max(q.x,q.y),0.0);"
        "   if (sd <= 0.0) { colour = vec4(oColour.rgb,1.0); }"
        "   else { discard; }"
        "}";
}

#endif /* OBJECT */
