s = 3.0*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

xs = 2.75
ys = 0.75
xw = 1.0

x = xs;
y = ys;

for i = 1,256 do

    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {-0.6,0.0,0.2},
            {-0.2,0.0,0.2},
            {0.2,0.0,0.2},
            {0.6,0.0,0.2},
            {0.0,-0.6,0.2},
            {0.0,-0.2,0.2},
            {0.0,0.2,0.2},
            {0.0,0.6,0.2}
        },
        ["name"] = ""

    }

    hop.loadObject(object)

    x = x + 0.04;
    if x > xs + xw then
        x = xs;
        y = y + 0.04
    end

end