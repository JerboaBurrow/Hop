s = 0.5*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,1 do

    x = 0.9; y = 0.4;

    object = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {0.5,0.5,0.0,1.0}
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end