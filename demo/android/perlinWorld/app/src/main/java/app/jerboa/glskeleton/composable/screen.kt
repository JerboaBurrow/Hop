package app.jerboa.glskeleton.composable

import android.annotation.SuppressLint
import android.util.Log
import android.widget.ImageButton
import androidx.compose.animation.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Color.Companion.Blue
import androidx.compose.ui.graphics.Outline
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import app.jerboa.glskeleton.AppInfo
import app.jerboa.glskeleton.R
import app.jerboa.glskeleton.ViewModel.RenderViewModel
import app.jerboa.glskeleton.ViewModel.SOCIAL
import app.jerboa.glskeleton.ui.view.GLView
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.launch

@OptIn(ExperimentalAnimationApi::class)
@SuppressLint("CoroutineCreationDuringComposition")
@Composable
fun screen(
    displayingMenu: Boolean,
    displayingAbout: Boolean,
    resolution: Pair<Int,Int>,
    images: Map<String,Int>,
    info: AppInfo,
    onDisplayingMenuChanged: (Boolean) -> Unit,
    onDisplayingAboutChanged: (Boolean) -> Unit,
    onRequestAchievements: () -> Unit,
    onRequestLeaderboards: () -> Unit,
    onAchievementStateChanged: (RenderViewModel.AchievementUpdateData) -> Unit,
    onScored: (Long) -> Unit,
    onRequestingSocial: (SOCIAL) -> Unit,
    onRequestingLicenses: () -> Unit,
){

    val scaffoldState = rememberScaffoldState()
    val coroutineScope = rememberCoroutineScope()

    val seenHelp = remember { mutableStateOf(!info.firstLaunch) }

    val width75Percent = info.widthDp*0.75
    val height25Percent = info.heightDp*0.25
    val height10Percent = info.heightDp*0.1
    val menuItemHeight = height10Percent*0.66

    Column(
        modifier = Modifier.fillMaxWidth()
    ) {

        Scaffold(
            scaffoldState = scaffoldState,
            topBar = {

            },
            bottomBar = {
                menu(
                    displayingMenu,
                    width75Percent,
                    height10Percent,
                    menuItemHeight,
                    images,
                    info,
                    onDisplayingAboutChanged,
                    onRequestAchievements,
                    onRequestLeaderboards
                )
            }
        ) {
            AndroidView(
                factory = {
                    GLView(
                        it, null,
                        resolution,
                        onDisplayingMenuChanged,
                        onAchievementStateChanged,
                        onScored
                    )
                },
                update = { view ->
                    //null
                }
            )
            about(
                displayingAbout,
                width75Percent,
                images,
                info,
                onRequestingLicenses,
                onRequestingSocial
            )
            menuPrompt(images,displayingMenu,menuItemHeight,onDisplayingMenuChanged)
        }
    }
}