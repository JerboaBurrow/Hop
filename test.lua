s = 0.5*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,10 do

    x = math.random(); y = math.random();

    object = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["shader"] = "circleObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {0.0,0.0,1.0}
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end