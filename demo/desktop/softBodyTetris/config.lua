config = 
{
    ["timeStep"]        = 1.0/(5*900.0),   -- physics time step
    ["subSample"]       = 10,              -- how many physics steps (collisions, movement, etc) per frame
    ["cofr"]            = 0.25,            -- coefficient of restitution,
    ["surfaceFriction"] = 0.1              -- friction at surfaces
}

hop.setSurfaceFriction(0.1);

-- can always call again to reconfigure, the magic of embedded Lua
hop.configure(config);

-- some code for mesh generation
require("meshes")

math.randomseed(os.time())