#ifndef OBJECT
#define OBJECT


namespace Hop::System::Rendering
{
    // Layout MUST be specified for Android!! (else shit fps)
    static const char * objectVertexShader = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "layout(location=0) in vec4 a_position;\n"
        "layout(location=1) in vec4 a_offset;\n"
        "layout(location=2) in vec4 a_colour;\n"
        "layout(location=3) in vec4 a_texOffset;\n"
        "uniform mat4 proj;\n"
        "uniform float atlasN;\n"
        "out vec2 texCoord;\n"
        "out vec2 atlasTexCoord;\n"
        "out vec4 oColour;\n"
        "void main()\n"
        "{\n"
            "gl_Position = proj*vec4(a_offset.w*a_position.xy+a_offset.xy,0.0,1.0);\n"
            "texCoord = a_position.zw;\n"
            "vec2 tileCoord = (texCoord / atlasN);\n"
            "atlasTexCoord = tileCoord + tileCoord*a_texOffset.xy;\n"
            "oColour = a_colour;\n"
        "}";

    // circle

    static const char * circleObjectFragmentShader = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "in vec2 texCoord;\n"
        "in vec2 atlasTexCoord;\n"
        "in vec4 oColour;\n"
        "uniform sampler2D tex; out vec4 colour;\n"
        "void main()"
        "{\n"
        "   vec2 c = texCoord-vec2(0.5,0.5);\n"
        "   if (dot(c,c) > 0.5*0.5) {discard;}\n"
        "   colour = oColour;\n"
        "}";
}

#endif /* OBJECT */