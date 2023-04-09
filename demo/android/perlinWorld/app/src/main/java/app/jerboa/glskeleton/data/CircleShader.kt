package app.jerboa.glskeleton.data

data class CircleShader (
    override val vertexShader: String = "#version 300 es\n"+
    "layout(location=0) in vec2 a_position; uniform mat4 proj; uniform float time;\n"+
    "void main(){\n"+
            "gl_Position = proj*vec4(a_position.xy,0.0,1.0);\n"+
            "gl_PointSize = 8.0*time;\n"+
    "}"
    ,
    override val fragmentShader: String = "#version 300 es\n"+
    "precision highp float;\n"+
    "out vec4 colour;\n"+
    "void main(void){\n"+
    "vec2 circCoord = 2.0 * gl_PointCoord - 1.0;"+
    "float dd = length(circCoord);\n"+
    "float alpha = 1.0-smoothstep(0.9,1.1,dd);\n"+
    "colour = vec4(1.0,0.0,0.0,alpha);\n"+
    "if (colour.a == 0.0){discard;}}"
) : Shader(vertexShader,fragmentShader)