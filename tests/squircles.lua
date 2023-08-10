s = 0.5*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,256 do

    x = 0.892; y = 0.36+0.02*i;
    -- x = math.random(); y = math.random();


    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {-0.5,-0.5,0.5},
            {0.5,0.5,0.5},
            {-0.5,0.5,0.5},
            {0.5,-0.5,0.5}
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end