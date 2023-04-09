package app.jerboa.glskeleton.ViewModel

import android.util.Log
import androidx.lifecycle.*

enum class SOCIAL {NOTHING, WEB, PLAY, YOUTUBE, GITHUB}

class RenderViewModel : ViewModel() {

    // Menu

    private val _displayingMenu = MutableLiveData(false)
    val displayingMenu: MutableLiveData<Boolean> = _displayingMenu
    fun onDisplayingMenuChanged(newVal: Boolean){
        _displayingMenu.value = !_displayingMenu.value!!
        if (_displayingMenu.value==false){
            _displayingAbout.value = false
        }
    }


    private val _displayingAbout = MutableLiveData(false)
    val displayingAbout: MutableLiveData<Boolean> = _displayingAbout
    fun onDisplayingAboutChanged(newVal: Boolean){
        _displayingAbout.value = !_displayingAbout.value!!
    }

    // SOCIAL
    private val _requestingSocial = MutableLiveData(SOCIAL.NOTHING)
    val requestingSocial: MutableLiveData<SOCIAL> = _requestingSocial

    fun onRequestingSocial(v: SOCIAL){
        _requestingSocial.value = v
    }


    // Game flow
    private val _pausingGame = MutableLiveData(false)
    val pausingGame: MutableLiveData<Boolean> = _pausingGame

    // scoring

    private val _score = MutableLiveData(0L)
    val score: MutableLiveData<Long> = _score
    fun onScored(score: Long){
        Log.d("on scored", "$score")
        _score.postValue(score)
    }

    // privacy (if ads)
    private val _displayingPrivacy = MutableLiveData(false)
    val displayingPrivacy: MutableLiveData<Boolean> = _displayingPrivacy

    private val _agreedToPrivacyPolicy = MutableLiveData(false)
    val agreedToPrivacyPolicy: MutableLiveData<Boolean> = _agreedToPrivacyPolicy
    fun onDismissPrivacy(agreed: Boolean){
        _displayingPrivacy.value = false
        _agreedToPrivacyPolicy.value = agreed
    }

    fun onPrivacyPopUp(){
        _displayingAbout.value = false
        _displayingMenu.value = false
        if (_agreedToPrivacyPolicy.value == false){_displayingPrivacy.value=true}
    }

    // Play services

    private val _requestingPlayServicesAchievementsUI = MutableLiveData(false)
    val requestingPlayServicesAchievementsUI: MutableLiveData<Boolean> = _requestingPlayServicesAchievementsUI
    fun onRequestPlayServicesAchievementsUI(){
        _requestingPlayServicesAchievementsUI.value = true
        _pausingGame.value = true
    }

    private val _requestingPlayServicesLeaderBoardsUI = MutableLiveData(false)
    val requestingPlayServicesLeaderBoardsUI: MutableLiveData<Boolean> = _requestingPlayServicesLeaderBoardsUI
    fun onRequestPlayServicesLeaderBoardUI(){
        _requestingPlayServicesLeaderBoardsUI.value = true
        _pausingGame.value = true
    }

    // Achievements

    data class AchievementUpdateData(
        val name: String,
        val increment: Int,
        val slot: Int = 0
    )

    private val _updateAchievement = MutableLiveData(Pair("",0))
    val updateAchievement: MutableLiveData<Pair<String,Int>> = _updateAchievement

    private val _updateAchievementSlot2 = MutableLiveData(Pair("",0))
    val updateAchievementSlot2: MutableLiveData<Pair<String,Int>> = _updateAchievementSlot2

    fun onAchievementStateChanged(data: AchievementUpdateData){
        Log.d("view model update Achievement","${data.name}, ${data.increment}")
        if (data.slot == 1){
            _updateAchievementSlot2.postValue(Pair(data.name,data.increment))
        }
        else {
            _updateAchievement.postValue(Pair(data.name,data.increment))
        }
    }

    private val _requestingLicenses = MutableLiveData(false)
    val requestingLicenses: MutableLiveData<Boolean> = _requestingLicenses
    fun onRequestingLicenses() {
        requestingLicenses.value = true
    }
}