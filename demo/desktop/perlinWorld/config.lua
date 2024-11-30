config = 
{
    ["timeStep"]  = 1.0/1800.0,
    ["subSample"] = 5,
    ["cofr"]      = 0.1
}

hop.configure(config);
dofile("mix.lua")