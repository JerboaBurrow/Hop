s = 2.8*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

xs = 2.75
ys = 0.75
xw = 1.0

x = xs;
y = ys;

for i = 1,1000 do

    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {250/255,200/255,200/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {-0.5,0.0,0.25},
            {-0.146,0.354,0.25},
            {0.207, 0.7071,0.25},
            {0.561, 0.354, 0.25},
            {0.914, 0.0, 0.25}
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