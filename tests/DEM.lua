require("tests/mesh")

D_vertices =
{
    {-0.5, -0.5},
    {-0.5, 0.5},
    {0.08, 0.5},
    {0.33, 0.16},
    {0.33, -0.16},
    {0.08, -0.5}
}

E_vertices =
{
    {0.1, -0.5},
    {-0.5, -0.5},
    {-0.5,  0.5},
    {0.25,   0.5},
    {},
    {-0.5, 0.0},
    {0.25, 0.0},
    {}
}

M_vertices =
{
    {-0.5, -0.5},
    {-0.5, 0.5},
    {0.0,  0.0},
    {0.5,  0.5},
    {0.5, -0.5},
    {}
}

E_mesh = createMesh(E_vertices, 0.1)
-- table.remove(E_mesh, 1)
D_mesh = createMesh(D_vertices, 0.1)
M_mesh = createMesh(M_vertices, 0.1)

s = 6.8*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

xx = 0.89-s;
x = xx
y = 0.47;

for i = 1, 16 do

    if (x > 1.5) then
        x = xx 
        y = y + s*1.5
    end

    x = x + s*1.5

    o = {

        ["transform"] = {x,y,0.0,s},
        -- ["colour"] = {200/255,200/255,250/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] = D_mesh,
        ["name"] = ""

    }

    hop.loadObject(o)

    x = x + s

    o = {

        ["transform"] = {x,y,0.0,s},
        -- ["colour"] = {200/255,200/255,250/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] = E_mesh,
        ["name"] = ""

    }

    hop.loadObject(o)

    x = x + s*0.75

    o = {

        ["transform"] = {x,y,0.0,s},
        -- ["colour"] = {200/255,200/255,250/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] = M_mesh,
        ["name"] = ""

    }

    hop.loadObject(o)
end