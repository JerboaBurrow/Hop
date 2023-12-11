L_mesh = {
    {0, 0, 0.5},
    {1, 0, 0.5},
    {-1, 0, 0.5},
    {-2, 0, 0.5},
    {-2, 1, 0.5},
    {-2, 2, 0.5},
    {-2, 3, 0.5},
    {-2, 4, 0.5},
    {-2, 5, 0.5},
    {-1, 5, 0.5},
    {0, 5, 0.5},
    {0, 4, 0.5},
    {0, 3, 0.5},
    {0, 2, 0.5},
    {1, 2, 0.5},
    {1, 1, 0.5}
}

s = hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

xx = 0.5;
x = xx
y = 0.5;

o = {

    ["transform"] = {x,y,0.0,s},
    ["colour"] = {200/255,250/255,250/255,1.0},
    ["moveable"] = true,
    ["collisionMesh"] = L_mesh,
    ["meshParameters"] = {10000.0, 10.0, 0.1},
    ["name"] = ""

}

hop.loadObject(o)
