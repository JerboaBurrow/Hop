s = 1.4*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

x = 0.9;
y = 0.5;

for i = 1,100 do

    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            -- {0.0,0.0,1.0}
            {-0.5,-0.5,0.5},
            {0.5,0.5,0.5},
            {-0.5,0.5,0.5},
            {0.5,-0.5,0.5}
        },
        ["name"] = ""

    }

    hop.loadObject(object)

    x = x + 0.04;
    if x > 1.75 then
        x = 0.9;
        y = y + 0.04
    end

end