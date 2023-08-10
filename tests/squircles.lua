s = 2.0*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,5 do

    x = math.random()+0.5; y = math.random()+0.5;


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