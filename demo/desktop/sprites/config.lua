config =
{
    ["timeStep"]  = 1.0/1800.0,
    ["subSample"] = 5,
    ["cofr"]      = 0.5
}

hop.configure(config);

objects = {
    {
        ["transform"] = {0.1, 0.1, 0.0, 0.1},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["texturePath"] = "HEART.png",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {0.0,0.0,1.0}
        },
        ["name"] = "heart"
    },
    {
        ["transform"] = {0.5, 0.5, 0.0, 0.1},
        ["colour"] = {200/255,200/255,250/255,1.0},
        ["texturePath"] = "Pi.png",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {0.0,0.0,1.0}
        },
        ["name"] = "pi"
    },
    {
        ["transform"] = {0.7, 0.25, 0.0, 0.1},
        ["colour"] = {200/255,200/255,250/255,0.5},
        ["texturePath"] = "random.png",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            {0.0,0.0,1.0}
        },
        ["name"] = "random"
    }
}

for i = 1, #objects do
    hop.loadObject(objects[i])
end