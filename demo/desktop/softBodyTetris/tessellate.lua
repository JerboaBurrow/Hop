s = (1.0-hop.maxCollisionPrimitiveSize()*4.0)/(3*9)

if lastTime == nil then
    lastTime = hop.timeMillis()

    mesh = meshes[1]

    x = 0.1375
    y = 0.5

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {15000.0, 1.0, 0.2},
        ["name"] = ""

    }

    hop.loadObject(o)

    mesh = meshes[2]

    x = 0.45
    y = 0.425

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {15000.0, 1.0, 0.2},
        ["name"] = ""

    }

    hop.loadObject(o)

    mesh = meshes[3]

    x = 0.74
    y = 0.475

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {15000.0, 1.0, 0.2},
        ["name"] = ""

    }

    hop.loadObject(o)

    mesh = meshes[2]

    x = 0.887
    y = 0.575

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,3.14/2.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {15000.0, 1.0, 0.2},
        ["name"] = ""

    }

    hop.loadObject(o)

    mesh = meshes[3]

    x = 0.1375+4.0*hop.maxCollisionPrimitiveSize()
    y = 0.6

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,3.14/2.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {15000.0, 1.0, 0.2},
        ["name"] = ""

    }

    hop.loadObject(o)

    mesh = meshes[5]

    x = 0.59
    y = 0.6

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,3.14/2.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {15000.0, 1.0, 0.2},
        ["name"] = ""

    }

    hop.loadObject(o)
end

time = hop.timeMillis()

if time-lastTime > 1000*5 then

    if (hop.kineticEnergy() > 0.5) then
        lastTime = hop.timeMillis()
    else
        mesh = meshes[4]

        x = 0.75
        y = 1.5
        
        r = math.random(255)
        g = math.random(255)
        b = math.random(255)

        o = {

            ["transform"] = {x,y,0.0,s},
            ["colour"] = {r/255,g/255,b/255,1.0},
            ["moveable"] = true,
            ["collisionMesh"] = mesh,
            ["meshParameters"] = {15000.0, 1.0, 0.2},
            ["name"] = "nutter"

        }

        hop.loadObject(o)
        lastTime = hop.timeMillis()
    end
end