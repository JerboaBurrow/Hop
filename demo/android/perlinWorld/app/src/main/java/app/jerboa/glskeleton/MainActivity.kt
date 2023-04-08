package app.jerboa.glskeleton

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import app.jerboa.glskeleton.ViewModel.RenderViewModel
import app.jerboa.glskeleton.ViewModel.SOCIAL
import app.jerboa.glskeleton.composable.renderScreen
import app.jerboa.glskeleton.onlineServices.Client
import app.jerboa.glskeleton.ui.theme.GLSkeletonTheme
import com.google.android.gms.games.AuthenticationResult
import com.google.android.gms.games.PlayGames
import com.google.android.gms.games.PlayGamesSdk
import com.google.android.gms.games.achievement.Achievement
import com.google.android.gms.games.achievement.AchievementBuffer
import com.google.android.gms.oss.licenses.OssLicensesMenuActivity
import com.google.android.gms.tasks.Task
import java.util.*

data class AppInfo(
    val versionString: String,
    val firstLaunch: Boolean,
    val playGamesServices: Boolean,
    val density: Float,
    val heightDp: Float,
    val widthDp: Float
)

class MainActivity : AppCompatActivity() {

    private val renderViewModel by viewModels<RenderViewModel>()

    private var playSuccess = false

    private lateinit var client: Client

    private val imageResources: Map<String,Int> = mapOf(
        "logo" to R.drawable.logo,
        "about" to R.drawable.about_,
        "play-controller" to R.drawable.games_controller_grey,
        "play-ach" to R.drawable.games_achievements,
        "play-lead" to R.drawable.games_leaderboards,
        "play-logo" to R.drawable.play_,
        "yt" to R.drawable.ic_yt,
        "web" to R.drawable.weblink_icon_,
        "github" to R.drawable.ic_github_mark_white,
        "burger" to R.drawable.burger_,
        "dismiss" to R.drawable.dismiss_
    )

    private fun playRate(){
        val intent = Intent(Intent.ACTION_VIEW).apply {
            data = Uri.parse(
                "https://play.google.com/store/apps/details?id=app.jerboa.spp")
            setPackage("com.android.vending")
        }
        startActivity(intent)
    }

    private fun showLicenses(){
        val intent = Intent(this.applicationContext, OssLicensesMenuActivity::class.java)
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        //on openning OSS sometimes there is a crash..
        //https://github.com/google/play-services-plugins/issues/100
        //com.google.android.gms.internal.oss_licenses.zzf.dummy_placeholder = getResources().getIdentifier("third_party_license_metadata", "raw", getPackageName());
        startActivity(intent)
    }
    private fun youtube(){

        val uri = Uri.parse("https://www.youtube.com/channel/UCP3KhLhmG3Z1CMWyLkn7pbQ")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)

    }

    private fun web(){

        val uri = Uri.parse("https://jerboa.app")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)

    }

    private fun github(){

        val uri = Uri.parse("https://github.com/Jerboa-app")
        val intent = Intent(Intent.ACTION_VIEW, uri)
        startActivity(intent)

    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // play game services
        PlayGamesSdk.initialize(this)

        val prefs = getSharedPreferences("jerboa.app.glskeleton.prefs", MODE_PRIVATE)

        client = Client(resources, getSharedPreferences("jerboa.app.glskeleton.prefs", MODE_PRIVATE))
        client.playGamesServicesLogin(this)
        client.sync(this)

        //client.loadRewardedAd(this)

        renderViewModel.requestingPlayServicesAchievementsUI.observe(
            this, androidx.lifecycle.Observer {
                    request -> if(request){client.showPlayAchievementsUI(this)}
            }
        )

        renderViewModel.requestingPlayServicesLeaderBoardsUI.observe(
            this, androidx.lifecycle.Observer {
                    request -> if(request){client.showPlayLeaderBoardUI(this)}
            }
        )

        renderViewModel.updateAchievement.observe(
            this, androidx.lifecycle.Observer {
                    s -> if(client.updateAchievement(s.first,s.second)){
                client.updatePlayServicesAchievement(this,s.first)
                client.updateLocalAchievement(s.first)
            }
            }
        )
        renderViewModel.updateAchievementSlot2.observe(
            this, androidx.lifecycle.Observer {
                    s -> if(client.updateAchievement(s.first,s.second)){
                client.updatePlayServicesAchievement(this,s.first)
                client.updateLocalAchievement(s.first)
            }
            }
        )

        renderViewModel.score.observe(
            this, androidx.lifecycle.Observer {
                    s -> if (s != null){client.postScore("",s, this)}
            }
        )

        renderViewModel.agreedToPrivacyPolicy.observe(
            this, androidx.lifecycle.Observer {
                val prefsEdit = prefs.edit()
                prefsEdit.putBoolean("seenPrivacy",it)
                prefsEdit.apply()

                prefsEdit.putBoolean("agreedPrivacy",it)
                prefsEdit.apply()
            }
        )

        renderViewModel.requestingLicenses.observe(
            this, androidx.lifecycle.Observer { request -> if(request){ showLicenses() }}
        )

        renderViewModel.requestingSocial.observe(
            this, androidx.lifecycle.Observer { request ->
                when (request){
                    SOCIAL.WEB -> web()
                    SOCIAL.PLAY -> playRate()
                    SOCIAL.YOUTUBE -> youtube()
                    SOCIAL.GITHUB -> github()
                }
            }
        )

        if (BuildConfig.DEBUG){
            prefs.edit().clear().apply()
        }

        if (!prefs.contains("firstLaunch")){
            val prefsEdit = prefs.edit()
            prefsEdit.putBoolean("firstLaunch",true)
            prefsEdit.apply()
        }

        val firstLaunch: Boolean = prefs.getBoolean("firstLaunch",true)

        Log.d("launch", firstLaunch.toString())

        val versionString = BuildConfig.VERSION_NAME + ": " + Date(BuildConfig.TIMESTAMP)

        val displayInfo = resources.displayMetrics
        val dpHeight = displayInfo.heightPixels / displayInfo.density
        val dpWidth = displayInfo.widthPixels / displayInfo.density
        val appInfo = AppInfo(
            versionString,
            firstLaunch,
            playSuccess,
            if (resources.getBoolean(R.bool.isTablet)){displayInfo.density}else{1f},
            dpHeight,
            dpWidth
        )


        Log.d("density",""+ resources.displayMetrics.density)

        window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_FULLSCREEN
        actionBar?.hide()

        val displayMetrics = DisplayMetrics()
        windowManager.defaultDisplay.getMetrics(displayMetrics)
        val height = displayMetrics.heightPixels
        val width = displayMetrics.widthPixels
        setContent {
            GLSkeletonTheme {
                // A surface container using the 'background' color from the theme
                renderScreen(
                    renderViewModel,
                    Pair(width,height),
                    imageResources,
                    appInfo
                )
            }
        }
    }
}