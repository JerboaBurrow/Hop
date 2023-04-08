package app.jerboa.glskeleton.ui.view

import android.opengl.GLES31.*
import android.opengl.GLSurfaceView
import android.opengl.Matrix
import android.util.Log
import app.jerboa.glskeleton.Hop
import app.jerboa.glskeleton.ViewModel.RenderViewModel
import app.jerboa.glskeleton.utils.*
import app.jerboa.glskeleton.data.*
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import java.util.*
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.jvm.internal.Intrinsics.Kotlin
import kotlin.random.Random
import android.opengl.GLES31 as gl3

class GLRenderer (
    private val resolution: Pair<Int,Int>,
    private val onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    private val onScored: (Long) -> Unit
    ) : GLSurfaceView.Renderer {

    // keep track of frame rate
    private var delta: Long = 0
    private var last: Long = System.nanoTime()
    // id for a frame (will be kep mod 60)
    private var frameNumber: Int = 0
    // rate-limit touch events
    private val touchRateLimit = 10 // frames
    private var lastTapped: Int = 0
    // smoothed framerate states
    private val deltas: FloatArray = FloatArray(60){0f}
    private val physDeltas = FloatArray(60){0f}
    private val renderDeltas = FloatArray(60){0f}

    private var muP: Float = 0f
    private var muR: Float = 0f

    private var tapped: Boolean = false

    private var scoreClock = 10
    private var scoreLastTapped = 0
    private var score: Long = 0L

    private val DEBUG_GL: Boolean = false

    private lateinit var hop: Hop

    init {}

    private fun achievements(){
        val dt = delta.toFloat()*1e-9f
        if (tapped) {
            onAchievementStateChanged(
                RenderViewModel.AchievementUpdateData(
                    "achievement_TEST_INCREMENTABLE",
                    1
                    )
            )
            onAchievementStateChanged(
                RenderViewModel.AchievementUpdateData(
                    "achievement_TEST1",
                    1,
                    1
                )
            )
            tapped = false
        }
    }

    fun screenToWorld(x: Float, y: Float): FloatArray {
        var rx: Float = 0.0f
        var ry: Float = 0.0f
        if (hop != null){
            hop.screenToWorld(x,y,rx,ry)
        }
        return floatArrayOf(rx,ry)
    }

    // propagate a tap event
    fun tap(x: Float,y: Float){
        val w = screenToWorld(x,resolution.second-y)
        val wx = w[0]
        val wy = w[1]
        if (lastTapped>touchRateLimit) {
            lastTapped = 0
            tapped = true
        }

        if (scoreLastTapped <= scoreClock){
            score += 1
            scoreLastTapped = 0
        }
    }

    fun initGPUData(){

        glError()

    }
    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {

        gl3.glClearColor(1f,1f,1f,1f)

        // instance textures
        initGPUData()

        // now we add a few uniforms that don't need to be updated
        Log.d("resolution","$resolution")
        hop = Hop()
        hop.hop(resolution.first,resolution.second)

        for (i in 1..1000){
            val x = Random.nextFloat()
            val y = Random.nextFloat()
            hop.spawnBall(x,y,0.2f,200f/255f, 200f/255f, 1f, 1f)
        }
        hop.render(true)
    }

    override fun onSurfaceChanged(p0: GL10?, p1: Int, p2: Int) {
        gl3.glViewport(0,0,p1,p2)
    }

    override fun onDrawFrame(p0: GL10?) {
        lastTapped++
        scoreLastTapped++
        gl3.glViewport(0,0,resolution.first,resolution.second)
        gl3.glEnable(gl3.GL_BLEND);
        gl3.glBlendFunc(gl3.GL_SRC_ALPHA, gl3.GL_ONE_MINUS_SRC_ALPHA);
        gl3.glClear(gl3.GL_COLOR_BUFFER_BIT or gl3.GL_DEPTH_BUFFER_BIT)

        val t0 = System.nanoTime()

        hop.stepPhysics()

        val t1 = System.nanoTime()

        hop.render(false)

        val t2 = System.nanoTime()

        hop.renderText(
            "Phys|Ren = ${muP}|${muR}",
            32f,
            32.0f,
            0.33f,
            0.0f,
            0.0f,
            0.0f
        )

        // measure time
        val t = System.nanoTime()
        delta = t-last
        last = t

        deltas[frameNumber] = 1.0f / (delta.toFloat()*1e-9f)
        renderDeltas[frameNumber] = (t2-t1)/1e9f
        physDeltas[frameNumber] = (t1-t0)/1e9f

        frameNumber += 1
        if (frameNumber >= 60){
            frameNumber = 0
            val mu = deltas.sum()/deltas.size
            muP = physDeltas.sum()/physDeltas.size
            muR = renderDeltas.sum()/renderDeltas.size
            Log.d("Runtime","FPS, Physics, Render, $mu, $muP, $muR")
        }

        if (frameNumber == 30){
            hop.printLog()
        }

        if (scoreLastTapped > scoreClock){
            if (score > 0){onScored(score)}
            score = 0
            scoreLastTapped = 0
        }

        achievements()
    }
}