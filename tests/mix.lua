s = 3.0*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

x = 0.9;
y = 0.5;


for i = 1,1000 do

    selection = math.random(4)

    if selection == 1 then

        object = {

            ["transform"] = {x,y,0.45,s},
            ["colour"] = {200/255,200/255,250/255,1.0},
            -- ["shader"] = "circleObjectShader",
            -- ["shader"] = "lineSegmentObjectShader",
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.6,0.0,0.2},
                {-0.2,0.0,0.2},
                {0.2,0.0,0.2},
                {0.6,0.0,0.2},
                {0.0,-0.6,0.2},
                {0.0,-0.2,0.2},
                {0.0,0.2,0.2},
                {0.0,0.6,0.2}
            },
            ["name"] = ""

        }
    elseif selection == 2 then

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

    elseif selection == 3 then

        object = {

            ["transform"] = {x,y,0.45,s},
            ["colour"] = {200/255,250/255,200/255,1.0},
            -- ["shader"] = "circleObjectShader",
            -- ["shader"] = "lineSegmentObjectShader",
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.5,0.0,0.25},
                {0.0,0.0,0.25},
                {0.5,0.0,0.25}
            },
            ["name"] = ""
    
        }

    elseif selection == 4 then 

        object = {

            ["transform"] = {x,y,0.45,s},
            ["colour"] = {250/255,200/255,250/255,1.0},
            -- ["shader"] = "circleObjectShader",
            -- ["shader"] = "lineSegmentObjectShader",
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.5,0.0,0.25},
                {0.0,0.0,0.25},
                {0.5,0.0,0.25}
            },
            ["name"] = ""
    
        }

    end


    hop.loadObject(object)

    x = x + 0.04;
    if x > 1.75 then
        x = 0.9;
        y = y + 0.04
    end

end