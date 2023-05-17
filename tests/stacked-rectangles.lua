s = 0.5*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,25 do

    -- x = 0.892; y = 0.36;
    x = 0.88; y = 0.36+0.005*i

    object = {

        ["transform"] = {x,y,0.45,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            -- {0.0,0.0,1.0}
            {-0.25,-0.25,-0.25,0.25,0.25,0.25,0.25,-0.25}
            -- {-0.25,-0.25,-0.25,0.5,0.25,0.5,0.25,-0.25}
            -- {-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5}
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end

-- x = 0.9; y = 0.345+0.01

-- object = {

--     ["transform"] = {x,y,0.0,s},
--     ["colour"] = {200/255,200/255,250/255,1.0},
--     -- ["shader"] = "circleObjectShader",
--     -- ["shader"] = "lineSegmentObjectShader",
--     ["moveable"] = true,
--     ["collisionMesh"] =
--     {
--         -- {0.0,0.0,1.0}
--         {-0.25,-0.25,-0.25,0.25,0.25,0.25,0.25,-0.25}
--         -- {-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5}
--     },
--     ["name"] = ""

-- }

-- hop.loadObject(object)

-- x = 0.9; y = 0.345+0.02

-- object = {

--     ["transform"] = {x,y,3.14/4.0,s},
--     ["colour"] = {200/255,200/255,250/255,1.0},
--     -- ["shader"] = "circleObjectShader",
--     -- ["shader"] = "lineSegmentObjectShader",
--     ["moveable"] = true,
--     ["collisionMesh"] =
--     {
--         -- {0.0,0.0,1.0}
--         {-0.25,-0.25,-0.25,0.25,0.25,0.25,0.25,-0.25}
--         -- {-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5}
--     },
--     ["name"] = ""

-- }

-- hop.loadObject(object)