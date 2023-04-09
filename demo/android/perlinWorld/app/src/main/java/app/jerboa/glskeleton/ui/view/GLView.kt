package app.jerboa.glskeleton.ui.view

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.GestureDetector
import android.view.MotionEvent
import androidx.core.view.GestureDetectorCompat
import app.jerboa.glskeleton.ViewModel.RenderViewModel

class GLView (
    context: Context,
    attr: AttributeSet? = null,
    private val resolution: Pair<Int,Int>,
    private val onDisplayingMenuChanged: (Boolean) -> Unit,
    private val onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    private val onScored: (Long) -> Unit
    ) : GLSurfaceView(context,attr), GestureDetector.OnGestureListener {

    private val renderer =
        GLRenderer(resolution,onAchievementStateChanged, onScored)
    private val gestures: GestureDetectorCompat = GestureDetectorCompat(context,this)
    var isDisplayingMenuChanged: Boolean = false

    init {
        setEGLContextClientVersion(3)
        preserveEGLContextOnPause = true
        setRenderer(renderer)
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onPause() {
        super.onPause()
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        return if (gestures.onTouchEvent(event)) {
            true
        } else {
            super.onTouchEvent(event)
        }
    }

    override fun onDown(p0: MotionEvent): Boolean {
        return true
    }

    override fun onShowPress(p0: MotionEvent) {
        return
    }

    override fun onSingleTapUp(p0: MotionEvent): Boolean {
        val x: Float = p0!!.x
        val y: Float = p0!!.y

        renderer.tap(x,y)

        return true
    }

    override fun onScroll(p0: MotionEvent, p1: MotionEvent, p2: Float, p3: Float): Boolean {
        return true
    }

    override fun onLongPress(p0: MotionEvent) {
        return
    }

    override fun onFling(p0: MotionEvent, p1: MotionEvent, p2: Float, p3: Float): Boolean {
        if (p0 != null) {
            if (resolution.second-p0.y < resolution.second*0.25f){
                Log.d("fling", p0.y.toString())
                isDisplayingMenuChanged = !isDisplayingMenuChanged
                onDisplayingMenuChanged(true)
            }
        }
        return true
    }

}