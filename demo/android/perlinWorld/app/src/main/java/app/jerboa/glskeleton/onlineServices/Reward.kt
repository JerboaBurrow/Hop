package app.jerboa.glskeleton.onlineServices
//
//import android.app.Activity
//import android.os.Bundle
//import android.util.Log
//import app.jerboa.glskeleton.R
//import com.google.ads.mediation.admob.AdMobAdapter
//import com.google.android.gms.ads.AdError
//import com.google.android.gms.ads.AdRequest
//import com.google.android.gms.ads.FullScreenContentCallback
//import com.google.android.gms.ads.LoadAdError
//import com.google.android.gms.ads.OnUserEarnedRewardListener
//import com.google.android.gms.ads.rewarded.RewardedAd
//import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback
//
//class Reward {
//
//    private var rewardedAd: RewardedAd? = null
//
//    fun isAvailable(): Boolean {
//        return rewardedAd != null
//    }
//
//    fun loadRewardedAd(activity: Activity){
//        val bundleExtra = Bundle()
//        bundleExtra.putString("npa", "1")
//        var adRequest = AdRequest.Builder()
//            .addNetworkExtrasBundle(AdMobAdapter::class.java, bundleExtra)
//            .build()
//
//        RewardedAd.load(
//            activity,
//            activity.resources.getString(R.string.rewarded_ad_id),
//            adRequest,
//            object : RewardedAdLoadCallback() {
//                override fun onAdFailedToLoad(adError: LoadAdError) {
//                    Log.d("rewardedAdLoad",adError?.message)
//                    rewardedAd = null
//                }
//
//                override fun onAdLoaded(ad: RewardedAd) {
//                    Log.d("rewardedAdLoad","Ad was loaded. ")
//                    rewardedAd = ad
//                    Log.d("rewardedAdLoad",(rewardedAd).toString())
//                }
//            }
//        )
//
//        rewardedAd?.fullScreenContentCallback = object : FullScreenContentCallback() {
//            override fun onAdShowedFullScreenContent() {
//                // Called when ad is shown.
//                Log.d("reward",(rewardedAd).toString())
//            }
//
//            override fun onAdFailedToShowFullScreenContent(p0: AdError) {
//                // Called when ad fails to show.
//                Log.d("reward"," fail")
//                rewardedAd = null
//                loadRewardedAd(activity)
//            }
//
//            override fun onAdDismissedFullScreenContent() {
//                // Called when ad is dismissed.
//                // Set the ad reference to null so you don't show the ad a second time.
//                Log.d("reward"," dismiss")
//                rewardedAd = null
//                loadRewardedAd(activity)
//            }
//        }
//    }
//
//    fun requestRewardedAd(activity: Activity, onAddReward : () -> Unit){
//        if (rewardedAd != null) {
//            rewardedAd?.show(activity, OnUserEarnedRewardListener {rewardItem ->
//                var rewardAmount = rewardItem.amount
//                var rewardType = rewardItem.type
//                Log.d("reward"," user rewarded")
//
//                onAddReward()
//                loadRewardedAd(activity)
//
//            }
//            )
//        } else {
//            Log.d("adShown","The rewarded ad wasn't ready yet.")
//        }
//    }
//}