package app.jerboa.glskeleton.onlineServices

import android.app.Activity
import android.content.SharedPreferences
import android.content.pm.PackageManager
import android.content.res.Resources
import android.util.Log
import com.google.android.gms.games.AuthenticationResult
import com.google.android.gms.games.PlayGames
import com.google.android.gms.tasks.Task

class Client(
    resources: Resources,
    private val preferences: SharedPreferences
) {

    private val achievements = Achievements(resources)
    private val leaderBoard = LeaderBoard(resources)
    //private val reward = Reward()

    private var playSuccess: Boolean = false
    private val rcAchievementUI = 9003
    private val rcLeaderBoardUI = 9004

    init {

    }

    fun isGooglePlayGamesServicesInstalled(activity: Activity): Boolean {
        return activity.packageManager.getLaunchIntentForPackage("com.google.android.play.games") != null
    }

    fun showPlayAchievementsUI(activity: Activity){
        if (!isGooglePlayGamesServicesInstalled(activity)){return}
        if (!playSuccess){playGamesServicesLogin(activity)}
        PlayGames.getAchievementsClient(activity)
            .achievementsIntent
            .addOnSuccessListener { intent -> activity.startActivityForResult(intent, rcAchievementUI); Log.d("showAchievements","success") }
            .addOnFailureListener { Log.d("showAchievements failure",it.toString()) }
    }

    fun showPlayLeaderBoardUI(activity: Activity){
        if (!isGooglePlayGamesServicesInstalled(activity)){return}
        PlayGames.getLeaderboardsClient(activity)
            .getLeaderboardIntent(leaderBoard.getId())
            .addOnSuccessListener {intent -> activity.startActivityForResult(intent,rcLeaderBoardUI); Log.d("showLeaderBoard","success") }
            .addOnFailureListener { Log.d("showLeaderBoard failure",it.toString()) }
    }

    fun playGamesServicesLogin(activity: Activity){
        if (!isGooglePlayGamesServicesInstalled(activity)){return}

        val gamesSignInClient = PlayGames.getGamesSignInClient(activity)

        gamesSignInClient.isAuthenticated.addOnCompleteListener { isAuthenticatedTask: Task<AuthenticationResult> ->
            val isAuthenticated = isAuthenticatedTask.isSuccessful && isAuthenticatedTask.result.isAuthenticated
            if (isAuthenticated) {
                // Continue with Play Games Services
                playSuccess = true
                Log.d("playGames","success")
            } else {
                // Disable your integration with Play Games Services or show a
                // login button to ask  players to sign-in. Clicking it should
                // call GamesSignInClient.signIn().
                playSuccess = false
                Log.d("playGames failure",isAuthenticatedTask.result.toString())
            }
        }
    }

    fun postScore(user:String, s: Long, activity: Activity){
        leaderBoard.newScore(user, Score(user,s))
        leaderBoard.saveToPlayServices(activity)
    }
    fun saveLocalLeaderBoard(){leaderBoard.saveLocal(preferences)}
    fun loadLocalLeaderBoard(){leaderBoard.loadLocal(preferences)}

    fun loadLeaderBoardFromPlayServices(activity: Activity){leaderBoard.loadFromPlayServices(activity)}

    fun updatePlayServicesAchievements(activity: Activity){achievements.saveToPlayServices(activity)}
    fun updatePlayServicesAchievement(activity: Activity, name: String){achievements.saveToPlayServices(activity,name)}
    private fun loadAchievementsFromPlayServices(activity: Activity){achievements.loadFromPlayServices(activity)}

    fun updateLocalAchievement(name: String){achievements.saveToLocal(name, preferences)}
    fun updateLocalAchievements(){achievements.saveToLocal(preferences)}
    private fun loadAchievementsFromLocal(){achievements.loadFromLocal(preferences)}

    fun sync(activity: Activity){
        // try local first
        loadAchievementsFromLocal()
        loadAchievementsFromPlayServices(activity)
        updateLocalAchievements()

        loadLocalLeaderBoard()
        loadLeaderBoardFromPlayServices(activity)
        saveLocalLeaderBoard()
    }

    fun getAchievementStates(): Map<String, Pair<Int, Int>> {
        return achievements.getAchievementStates()
    }

    fun updateAchievement(
        name: String,
        increment: Int
    ): Boolean {
       return achievements.updateAchievement(name,increment)
    }

    fun updateAchievements(states: Map<String,Pair<Int,Int>>){
        for (ach in states){
            Log.d("client updating achievement","$ach")
            updateAchievement(ach.key,ach.value.first)
        }
    }

//    fun rewardAvailable(): Boolean {return reward.isAvailable()}
//    fun loadRewardedAd(activity: Activity){reward.loadRewardedAd(activity)}
//    fun requestRewardedAd(activity: Activity, onAddReward: () -> Unit){reward.requestRewardedAd(activity, onAddReward)}
}