s = 1.9*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

xs = 0.5
ys = 0.75
xw = 1.0

x = xs;
y = ys;

for i = 1,128 do

    selection = math.random(8)

    theta = 2.0*3.14159 * math.random(10000) / 10000.0;

    if selection == 1 then

        object = {

            ["transform"] = {x,y,theta,s},
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

            ["transform"] = {x,y,theta,s},
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

            ["transform"] = {x,y,theta,s},
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

            ["transform"] = {x,y,0.0,s},
            ["colour"] = {250/255,200/255,250/255,1.0},
            -- ["shader"] = "circleObjectShader",
            -- ["shader"] = "lineSegmentObjectShader",
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.5,-0.5, 0.25},
                {0.0,-0.5, 0.25},
                {0.5,-0.5, 0.25},
                {0.0, 0.5, 0.25},
                {-0.25, 0.0, 0.25},
                {0.25, 0.0, 0.25}
            },
            ["name"] = ""
    
        }

    elseif selection == 5 then

        object = {

            ["transform"] = {x,y,theta,s},
            ["colour"] = {200/255,250/255,250/255,1.0},
            -- ["shader"] = "circleObjectShader",
            -- ["shader"] = "lineSegmentObjectShader",
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.5, 0.5, 0.25},
                {-0.0, 0.5, 0.25},
                {-0.0, 0.0, 0.25},
                {0.5, 0.0, 0.25},
                {0.5, -0.5, 0.25}
            },
            ["name"] = ""
    
        }

    elseif selection == 6 then

        object = {

            ["transform"] = {x,y,theta,s},
            ["colour"] = {250/255,250/255,200/255,1.0},
            -- ["shader"] = "circleObjectShader",
            -- ["shader"] = "lineSegmentObjectShader",
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {0.5,0.0, 0.25},
                {0.3536, 0.3536, 0.25},
                {0.0, 0.5, 0.25},
                {-0.3536, 0.3536, 0.25},
                {-0.5, 0.0, 0.25},
                {-0.3536, -0.3536, 0.25},
                {0.3536, -0.3536, 0.25},
                {0.5, 0.0, 0.25}
            },
            ["name"] = ""
    
        }

    elseif selection == 7 then
        object = {
            ["transform"] = {x,y,0.0,s},
            ["colour"] = {200/255,200/255,250/255,1.0},
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.5,-0.5,-0.5,0.5,0.5,0.5,0.5,-0.5} -- model space
            },
            ["name"] = "object"
        }
    elseif selection == 8 then
        object = {
            ["transform"] = {x,y,0.0,s},
            ["colour"] = {200/255,200/255,250/255,1.0},
            ["moveable"] = true,
            ["collisionMesh"] =
            {
                {-0.5,-0.25,-0.5,0.25,0.5,0.25,0.5,-0.25} -- model space
            },
            ["name"] = "object"
        }
    end


    hop.loadObject(object)

    x = x + 0.04;
    if x > xs + xw then
        x = xs;
        y = y + 0.04
    end


end