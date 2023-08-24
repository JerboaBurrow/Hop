s = 0.5*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,100 do

    -- x = 0.892; y = 0.36;
    x = math.random(); y = math.random();


    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {0.0,0.0,1.0}
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end
