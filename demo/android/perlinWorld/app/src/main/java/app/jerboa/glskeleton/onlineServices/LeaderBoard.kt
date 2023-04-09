package app.jerboa.glskeleton.onlineServices

import android.app.Activity
import android.content.SharedPreferences
import android.content.res.Resources
import android.util.Log
import app.jerboa.glskeleton.R
import com.google.android.gms.games.PlayGames
import com.google.android.gms.games.leaderboard.LeaderboardVariant

const val SCORE_POST_RATE_LIMIT_MILLISECONDS = 5000
data class Score(
    val name: String,
    val score: Long
)

operator fun Score.compareTo(s: Score): Int {
    if (this.score < s.score){
        return -1
    }
    if (this.score > s.score){
        return 1
    }
    return 0
}

class LeaderBoard(
    resources: Resources,
    )
{

    private var localScores: MutableList<Score> = mutableListOf()
    private var playScores: MutableList<Score> = mutableListOf()

    private var userBest: Score = Score("",0)

    private lateinit var playServicesId: String
    private lateinit var name: String

    private var lastPostedScore = 0L
    private var newScoreIsBest = false

    init {
        val leaderboards = resources.getStringArray(R.array.leaderboards)
        if (leaderboards.isNotEmpty()) {
            name = leaderboards[0].split("=")[0]
            playServicesId = leaderboards[0].split("=")[1]
            Log.d("loaded leaderboard","$name, $playServicesId")
        }
    }

    fun getId(): String {return playServicesId}

    fun newScore(user: String, s: Score){
        if (s > userBest){
            userBest = s
            newScoreIsBest = true
        }
        newScoreIsBest = false
        postToLocal(user,s)
        Log.d("newScore","${s.score}, ${userBest.score}, $localScores")
    }

    fun postToLocal(user: String, s: Score){
        if (s.score == 0L){return}
        var place = 0
        if (localScores.size == 0){
            localScores.add(s)
        }
        for (i in 0 until localScores.size){
            if (localScores[i] > s){
                break
            }
            place = i
        }
        localScores.add(place,s)
        if (localScores.size > 100){
            localScores.removeAt(0)
        }
    }

    fun saveLocal(preferences: SharedPreferences){
        val edit = preferences.edit()
        var scores = ""

        for ((i,s) in localScores.withIndex()){
            scores += "${s.name}:${s.score}"
            if (i < localScores.size-1){
                scores += ","
            }
        }

        edit.putString(name,scores)

        edit.apply()
    }

    fun loadLocal(preferences: SharedPreferences){
        val newLocalScores = mutableListOf<Score>()
        if (preferences.contains(name)){
            val s = preferences.getString(name,"")
            if (s != null && s != ""){
                val items = s.split(",")
                for (i in items){
                    val player = i.split(":")[0]
                    val value = i.split(":")[1]
                    newLocalScores.add(Score(player,value.toLong()))
                }
            }
        }
        localScores = newLocalScores
    }

    fun saveToPlayServices(activity: Activity){

        if (userBest.score == 0L){return}

        if (System.currentTimeMillis() - lastPostedScore < SCORE_POST_RATE_LIMIT_MILLISECONDS){
            if (!newScoreIsBest) {return}
        }

        lastPostedScore = System.currentTimeMillis()

        val client = PlayGames.getLeaderboardsClient(activity)
        client.submitScore(playServicesId,userBest.score)
        Log.d("submitScore","${userBest.score}")

        newScoreIsBest = false
    }

    fun loadFromPlayServices(activity: Activity){
        val client = PlayGames.getLeaderboardsClient(activity)
        client.loadCurrentPlayerLeaderboardScore(
            playServicesId,
            LeaderboardVariant.TIME_SPAN_ALL_TIME,
            LeaderboardVariant.COLLECTION_PUBLIC
        ).addOnSuccessListener {
            val score = it.get()
            if (score != null){
                val raw = score.rawScore
                if (userBest.score < raw){
                    userBest = Score("",raw)
                }
                Log.d("loadedScore","${score.rawScore}")
            }

        }
    }


}