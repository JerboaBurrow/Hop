s = (1.0-hop.maxCollisionPrimitiveSize()*4.0)/(3*9)

if lastTime == nil then
    lastTime = hop.timeMillis()

    mesh = meshes[math.random(#meshes)]

    x = 0.5 + math.random()*0.5 -0.25
    y = 0.5

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {30000.0, 16.0, 1.0},
        ["translationalDrag"] = 0.0,
        ["rotationalDrag"] = 100.0,
        ["mass"] = 1.0,
        ["inertia"] = 0.01,
        ["name"] = ""

    }

    hop.loadObject(o)
end

time = hop.timeMillis()

if time-lastTime > 1000*5 then

    if (hop.kineticEnergy() > 0.1) then
        lastTime = hop.timeMillis()
    else
        mesh = meshes[math.random(#meshes)]

        x = 0.5 + math.random()*0.5 -0.25
        y = 1.25
        
        r = math.random(255)
        g = math.random(255)
        b = math.random(255)

        o = {

            ["transform"] = {x,y,0.0,s},
            ["colour"] = {r/255,g/255,b/255,1.0},
            ["moveable"] = true,
            ["collisionMesh"] = mesh,
            ["meshParameters"] = {15000000.0, 1.0, 1.0},
            ["translationalDrag"] = 0.00,
            ["rotationalDrag"] = 0.01,
            ["mass"] = 1.0,
            ["inertia"] = 0.01,
            ["name"] = ""
    
        }

        hop.loadObject(o)
        lastTime = hop.timeMillis()
    end
end
