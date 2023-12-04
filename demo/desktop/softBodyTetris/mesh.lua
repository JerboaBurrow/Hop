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

    place = {0.0, 0.0}
    place[1] = vertices[1][1]
    place[2] = vertices[1][2]
    lastPlace = {0.0, 0.0}
    lastPlace[1] = place[1]
    lastPlace[2] = place[2]
    table.insert(v, {place[1], place[2], radius})
    postCorner = false
    cornerDist = 0.0

    for i = 1, N do
        if (#vertices[i] > 0 and #vertices[next(i, N)] > 0) then
        
            n = {0.0, 0.0}

            n[1] = vertices[next(i, N)][1] - vertices[i][1]
            n[2] = vertices[next(i, N)][2] - vertices[i][2]

            d = norm(n)
            diff = {place[1]-lastPlace[1], place[2]-lastPlace[2]}

            if (d > 0) then

                l = 0.0
                if postCorner then
                    l = cornerDist
                    postCorner = false
                end

                while l < d or math.abs(l - d) < 1e-15 do
                    nextPlace = {vertices[i][1]+l*n[1]/d, vertices[i][2]+l*n[2]/d}
                    if (l < d or math.abs(l - d) < 1e-15) then 
                        if (i == N-1) then 
                            diff = {nextPlace[1]-v[1][1], nextPlace[2]-v[1][2]}
                            if (norm(diff)<2.0*radius) then
                                break
                            end
                        end
                        -- can place, not off end
                        place[1] = nextPlace[1]
                        place[2] = nextPlace[2]
                        table.insert(v, {place[1], place[2], radius})
                    end

                    l = l + radius*2.0
                end

            end
            
        end

        lastPlace[1] = place[1]
        lastPlace[2] = place[2]

        postCorner = true
        l = norm({vertices[next(i, N)][1]-lastPlace[1], vertices[next(i, N)][2]-lastPlace[2]})
        ai = i
        bi = next(i,N)
        ci = next(next(i,N),N)

        s = {vertices[ai][1] - vertices[bi][1], vertices[ai][2] - vertices[bi][2]}
        t = {vertices[ci][1] - vertices[bi][1], vertices[ci][2] - vertices[bi][2]}

        ctheta = (s[1]*t[1]+s[2]*t[2])/(norm(s)*norm(t))
        if (math.abs(ctheta) < 1e-3) then 
            cornerDist = math.sqrt(4.0*radius*radius-l*l)
        elseif ((ctheta*ctheta-1.0)*l*l+4.0*radius*radius < 0.0) then
                cornerDist = math.sqrt(4.0*radius*radius-l*l)
        else
            s1 = l*ctheta + math.sqrt((ctheta*ctheta-1.0)*l*l+4.0*radius*radius)
            s2 = l*ctheta - math.sqrt((ctheta*ctheta-1.0)*l*l+4.0*radius*radius)
            cornerDist = math.max(s1,s2)
        end

    end

    return v

end
