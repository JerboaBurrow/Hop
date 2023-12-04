require("tests/mesh")

Square =
{
    {-0.5, -0.5},
    {-0.5, 0.5},
    {0.5, 0.5},
    {0.5, -0.5}
}

Long =
{
    {-0.5, -0.5},
    {-0.5, 1.0},
    {0.25, 1.0},
    {0.25, -0.5},
    {-0.5, -0.5}
}


--S_mesh = createMesh(Square, 0.05)
L_mesh = createMesh(Long, 0.05)

s = 6.8*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

xx = 0.89-s;
x = xx
y = 0.47;

for i = 1, 1 do

    if (x > 1.5) then
        x = xx 
        y = y + s*1.5
    end

    -- x = x + s*2.0

    -- o = {

    --     ["transform"] = {x,y,0.0,s},
    --     ["colour"] = {200/255,250/255,250/255,1.0},
    --     ["moveable"] = true,
    --     ["collisionMesh"] = S_mesh,
    --     ["meshParameters"] = {10000.0, 10.0, 0.1},
    --     ["name"] = ""

    -- }

    -- hop.loadObject(o)

    x = x + s*2.0

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {200/255,250/255,250/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = L_mesh,
        ["meshParameters"] = {10000.0, 10.0, 0.1},
        ["name"] = ""

    }

    hop.loadObject(o)
end