function prev(i, n)
    if i-1 > 0 then return i-1 else return i-1 + n end
end

function next(i, n)
    return (i+1-1)%n+1
end

function norm(x)
    return math.sqrt(x[1]*x[1]+x[2]*x[2])
end

function createMesh(vertices, radius)

    v = {}
    N = #vertices

    for i = 1, N do
        
        n = vertices[next(i, N)]
        n[1] = n[1] - vertices[i][1]
        n[2] = n[2] - vertices[i][2]

        d = norm(n)

        if (d > 0) then
            dm = math.floor(d/(2.0*radius))
            dm = math.max(dm, 2)
            m = 0.0
            for j = 0, dm do
                c = vertices[i]
                c[1] = c[1] + n[1] * m/d
                c[2] = c[2] + n[2] * m/d
                table.insert(v, c)
                m = m + 2.0*radius
            end
        end

    end

    n = vertices[next(1)]
    n[1] = n[1] - vertices[1][1]
    n[2] = n[2] - vertices[1][2]
    nt = {n[2], -n[1]}
    dnt = norm(nt)
    nt[1] = nt[1] / dnt
    nt[2] = nt[2] / dnt 
    d = norm(n)

    t = vertices[1]
    t[1] = t[1] + n[1] * 0.5
    t[2] = t[2] + n[2] * 0.5

    c = t
    c[1] = c[1] + nt[1]*radius
    c[2] = c[2] + nt[2]*radius

    com = {0.0, 0.0}

    for i = 1, N do 
        com[1] = com[1] + vertices[i][1]
        com[2] = com[2] + vertices[i][2]
    end 
    com[1] = com[1] / N
    com[2] = com[2] / N

    a = {0.0, 0.0}
    b = {0.0, 0.0}

    a[1] = c[1]-com[1]
    a[2] = c[2]-com[2]
    b[1] = t[1]-com[1]
    b[2] = t[2]-com[2]

    s = norm(a) / norm(b)

    for i = 1, N do
        v[i][1] = s*(v[i][1]-com[1])+com[1]
        v[i][2] = s*(v[i][2]-com[2])+com[2]
    end

    return vertices

end