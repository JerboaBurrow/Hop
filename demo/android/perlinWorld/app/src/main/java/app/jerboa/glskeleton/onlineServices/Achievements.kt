package app.jerboa.glskeleton.onlineServices

import android.app.Activity
import android.content.SharedPreferences
import android.content.res.Resources
import android.util.Log
import app.jerboa.glskeleton.R
import com.google.android.gms.games.PlayGames
import com.google.android.gms.games.achievement.Achievement.TYPE_INCREMENTAL
import com.google.android.gms.games.achievement.AchievementBuffer

data class Achievement(
    val name: String,
    val playServicesCode: String,
    var state: Int = 0,
    var finalState: Int = 1
)

class Achievements(
    resources: Resources
) {

    private val achievements: Map<String,Achievement> = populateAchievementsFromResource(resources)
    private lateinit var nameToId: Map<String,String>

    init {
        val m = mutableMapOf<String,String>()
        for (ach in achievements.values){
            m[ach.name] = ach.playServicesCode
        }
        nameToId = m.toMap()
    }

    fun updateAchievement(
        name: String,
        increment: Int
    ): Boolean {
        Log.d("ethemeral update"," $name to $increment")
        if (nameToId[name] != null && achievements[nameToId[name]] != null){
            val ach = achievements[nameToId[name]]!!
            if (ach.state != ach.finalState) {
                ach.state += increment
                if (ach.state > ach.finalState){ach.state = ach.finalState}
                Log.d("ethemeral updated"," $name to $increment")
                return true
            }
        }
        Log.d("ethemeral achievement not found"," $name to $increment")
        return false
    }

    fun getAchievementStates(): Map<String, Pair<Int,Int>> {
        val m = mutableMapOf<String,Pair<Int,Int>>()
        for (ach in achievements.values){
            m[ach.name] = Pair(ach.state,ach.finalState)
        }
        return m
    }

    private fun populateAchievementsFromResource(resources: Resources): Map<String, Achievement> {

        val achievements = mutableMapOf<String,Achievement>()

        for (ach in resources.getStringArray(R.array.achievements)){
            val name = ach.split("=")[0]
            val id = ach.split("=")[1]

            Log.d("found Achievement ","$name, $id")

            achievements[id] = Achievement(
                name,
                id,
                0,
                1
            )

        }

        for (ach in resources.getStringArray(R.array.achievements_incrementable)){
            val name = ach.split("=")[0]
            val id = ach.split("=")[1].split("_")[0]
            val final = ach.split("=")[1].split("_")[1].toInt()
            Log.d("found incrementable Achievement ","$name, $id, $final")
            achievements[id] = Achievement(
                name,
                id,
                0,
                final
            )
        }

        return achievements
    }

    fun saveToLocal(preferences: SharedPreferences){

        for (ach in achievements.values){
            val edit = preferences.edit()
            edit.putString("achievements_${ach.name}","${ach.state}:${ach.finalState}")
            edit.apply()
        }

    }

    fun saveToLocal(name: String, preferences: SharedPreferences){
        if (nameToId[name]!= null && achievements[nameToId[name]] != null) {
            val ach = achievements[nameToId[name]]!!
            val edit = preferences.edit()
            edit.putString("achievements_${ach.name}", "${ach.state}:${ach.finalState}")
            edit.apply()
        }
    }

    fun loadFromLocal(preferences: SharedPreferences){
        for (ach in achievements.values){
            if (preferences.contains("achievements_${ach.name}")){
                val lAch = preferences.getString("achievements_${ach.name}","0:1")
                Log.d("read local achievement","${ach.name}, $lAch")
                if (lAch == null || lAch.length != 2){
                    continue
                }
                ach.state = lAch.split(":")[0].toInt()
            }
            else{
                Log.d("could not read local achievement","${ach.name}")
                val edit = preferences.edit()
                edit.putString("achievements_${ach.name}","${ach.state}:${ach.finalState}")
                edit.apply()
            }
        }
    }

    fun saveToPlayServices(activity: Activity, name: String){
        if (nameToId[name]!= null && achievements[nameToId[name]] != null){
            val ach = achievements[nameToId[name]]!!
            if (ach.state == ach.finalState && ach.finalState == 1){
                PlayGames.getAchievementsClient(activity).unlock(ach.playServicesCode)
                Log.d("updated play services achievement","${ach.name}, unlocked")
            }
            else{
                PlayGames.getAchievementsClient(activity).setSteps(ach.playServicesCode,
                    Integer.min(ach.state,ach.finalState)
                )
            }

            if (ach.finalState > 1 && ach.state < ach.finalState){
                PlayGames.getAchievementsClient(activity).setSteps(ach.playServicesCode,
                    Integer.min(ach.state,ach.finalState)
                )
                Log.d("updated play services achievement","${ach.name}, ${ach.state}")
            }
        }
    }

    fun saveToPlayServices(activity: Activity){
        for (ach in achievements.values){
            saveToPlayServices(activity,ach.name)
        }
    }

    fun loadFromPlayServices(activity: Activity){
        PlayGames.getAchievementsClient(activity).load(true).addOnCompleteListener { task ->
            if (task.isSuccessful) {
                val achievements: AchievementBuffer? = task.result.get()
                if (achievements != null) {
                    for (i in 0 until achievements.count){
                        val ach: com.google.android.gms.games.achievement.Achievement = achievements.get(i)
                        val id = ach.achievementId
                        val state = ach.state
                        val appAch = this.achievements[id]

                        if (appAch==null){
                            Log.w("achievements","Client out of sync with local (check achievements.xml), tried to access $id")
                            continue
                        }

                        Log.d("play achievement loaded","${appAch.name}")

                        val unlocked = if(state == com.google.android.gms.games.achievement.Achievement.STATE_UNLOCKED){1}else{0}

                        if (state == TYPE_INCREMENTAL && appAch.finalState > 1){
                            if (appAch.state == appAch.finalState && ach.currentSteps != ach.totalSteps){
                                // unlocked locally but not in play
                                // keep local, leave for next push
                            }
                            else{
                                // pull state
                                appAch.state = ach.currentSteps
                                appAch.finalState = ach.totalSteps
                                Log.d("incrementable achievement from play services","${appAch.name}, ${appAch.state}, ${appAch.finalState}")
                            }

                        }
                        else{
                            if (appAch.state == appAch.finalState && unlocked==0){
                                // unlocked locally but not in play
                                // keep local, leave for next push
                            }
                            else {
                                appAch.state = unlocked
                                appAch.finalState = 1
                            }
                        }
                    }
                }
            }
        }
    }

}