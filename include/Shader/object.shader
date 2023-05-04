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
        "out float theta;\n"
        "void main()\n"
        "{\n"
            "gl_Position = proj*vec4(a_offset.w*a_position.xy+a_offset.xy,0.0,1.0);\n"
            "texCoord = a_position.zw;\n"
            "vec2 tileCoord = (texCoord / atlasN);\n"
            "atlasTexCoord = tileCoord + tileCoord*a_texOffset.xy;\n"
            "oColour = a_colour;\n"
            "theta = a_offset.z;\n"
        "}";

    // circle

    static const char * circleObjectFragmentShader = "#version " GLSL_VERSION "\n"
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

    static const char * lineSegmentObjectFragmentShader = "#version " GLSL_VERSION "\n"
        "precision lowp float;\n precision lowp int;\n"
        "in vec2 texCoord;\n"
        "in vec2 atlasTexCoord;\n"
        "in vec4 oColour;\n"
        "in float theta;\n"
        "uniform sampler2D tex; out vec4 colour;\n"
        "void main()"
        "{\n"
        "   float ctheta = cos(theta); float stheta = sin(theta);\n"
        "   vec2 c = texCoord-vec2(0.5,0.5);\n"
        "   c = vec2(ctheta*c.x-stheta*c.y, stheta*c.x + ctheta*c.y);\n"
        //                x*ctheta-y*stheta, x*stheta+y*ctheta
        "   vec2 a = vec2(0.5*stheta,-0.5*ctheta);\n"
        "   vec2 b = vec2(-0.5*stheta,0.5*ctheta);\n"
        "   vec2 ba = b-a; vec2 ca = c-a;\n"
        "   float h = clamp(dot(ca,ba)/dot(ba,ba),0.0,1.0);\n"
        "   float d = length(ca-h*ba);\n"
        "   if (d > 0.5) {colour = vec4(oColour.rgb,0.5);}\n"
        "   else {colour = oColour;}\n"
        "}";
}

#endif /* OBJECT */
