## Hop Style guide

If in doubt follow the [C++ Core Guidelines (Stroustrup and Sutter)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

### Bracing 

- Use Allman style
- And with function calls/definitions/declarations which contain many variables

```c++
while (keepWorking)
{
    doSomething
    (
        with,
        a,
        lot,
        of,
        parameters
    );

    checkForWork(keepWorking);
}
```

### Public and Private

- Align ```public:``` and ```private``` with braces.
- Use a space below each
- public then private when possible
-
```c++
    class ACoolClass
    {

    public:

        uint64_t countOfFish;

    private:

        uint64_t secretCountOfFish;

    }
```

### Spacing
- Group statements conceptually
- Use space to separate statements to aid comprehension of flow
```c++

glBindBuffer(GL_ARRAY_BUFFER,VBOoffset);

glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    sizeof(float)*3*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
    renderOffsets.get()
);

glBindBuffer(GL_ARRAY_BUFFER,0);
```

- Highly related statements may work better grouped without space
```c++
value = false;

float u = ix*xPeriod / size;
float v = iy*yPeriod / size;

float t = u+v+turbulence*getTurbulence(ix,iy,size,0);
```
- Here the symmetry of defining ```u``` and ```v``` is emphasised by 
using no spacing, compared with setting ```value``` and ```t```
- An error in defining ```v``` could be more easily seen 

### Type
 - Use CamelCase for our classes
 - Use camelCase for variables and functions
 - Use SCREAMING_SNAKE_CASE for constant variables
 ```c++
    class OurCoolClass
    {

    public:

        double positionX;
        double positionY;

        double orientation;

    private:

        const double MAX_SPEED = 10.0;

    }
 ```