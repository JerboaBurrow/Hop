require("tests/mesh")

vertices =
{
    {-0.5, 0.5},
    {-0.5, 0.5},
    {0.08, 0.5},
    {0.33, 0.16},
    {0.33, -0.16},
    {0.08, -0.5}
}

function foo(a)
    b = {1., 2.}
    return a-b
end

foo({1., 0.})

mesh = createMesh(vertices, 0.1)

-- s = 2.8*hop.maxCollisionPrimitiveSize()

-- math.randomseed(os.time())

-- x = 0.9;
-- y = 0.5;

-- D = {

--     ["transform"] = {x,y,0.0,s},
--     ["colour"] = {200/255,200/255,250/255,1.0},
--     -- ["shader"] = "circleObjectShader",
--     -- ["shader"] = "lineSegmentObjectShader",
--     ["moveable"] = true,
--     ["collisionMesh"] = 
--     {
--         {-0.5, 0.5, 0.125},
--         {-0.5, 0.25, 0.125},
--         {-0.5, 0.0, 0.125},
--         {-0.5, -0.25, 0.125},
--         {-0.5, -0.5, 0.125},
--         {-0.25, 0.5, 0.125},
--     },
--     ["name"] = ""

-- }

-- hop.loadObject(D)