config = 
{
    ["timeStep"]  = 1.0/(5*900.0),
    ["subSample"] = 10,
    ["cofr"]      = 0.75
}

hop.configure(config);

require("meshes")

math.randomseed(os.time())