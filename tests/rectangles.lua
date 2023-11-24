s = 0.9*hop.maxCollisionPrimitiveSize()

math.randomseed(1)

for i = 1,2000 do

    -- x = 0.897; y = 0.6;
    x = 0.85+math.random(); y = math.random()+0.6;

    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5} -- model space
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end