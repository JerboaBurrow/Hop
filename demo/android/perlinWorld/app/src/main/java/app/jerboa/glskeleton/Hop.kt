package app.jerboa.glskeleton

class Hop
{
    companion object
    {
        init
        {
            System.loadLibrary("HopAndroid")
        }
    }

    external fun hop(resX: Int, resY: Int)

    // World

    external fun screenToWorld(x: Float, y: Float, rx: Float, ry: Float)

    // Rendering

    external fun render(refreshObjectShaders: Boolean)

    external fun renderText(
        text: String,
        x: Float,
        y: Float,
        scale: Float,
        r: Float,
        g: Float,
        b: Float,
        a: Float = 1.0f,
        centred: Boolean = false
    )

    // physics

    external fun stepPhysics(
        dt: Float = 0.0f,
        detectCollisions: Boolean = true
    )

    // object
    external fun spawnBall(
        x: Float,
        y: Float,
        s: Float,
        r: Float,
        g: Float,
        b: Float,
        a: Float
    )

    // Logging

    external fun printLog()
}