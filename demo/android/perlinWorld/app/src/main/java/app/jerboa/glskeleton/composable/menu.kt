package app.jerboa.glskeleton.composable

import androidx.compose.animation.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.IconButton
import androidx.compose.material.MaterialTheme
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.alpha
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import app.jerboa.glskeleton.AppInfo

@OptIn(ExperimentalAnimationApi::class)
@Composable
fun menu(
    displayingMenu: Boolean,
    width75Percent: Double,
    height10Percent: Double,
    menuItemHeight: Double,
    images: Map<String,Int>,
    info: AppInfo,
    onDisplayingAboutChanged: (Boolean) -> Unit,
    onRequestAchievements: () -> Unit,
    onRequestLeaderboards: () -> Unit,
) {

    AnimatedVisibility(
        visible = displayingMenu,
        enter = slideInVertically(
            // Enters by sliding down from offset -fullHeight to 0.
            initialOffsetY = { fullHeight -> -fullHeight }
        ),
        exit = slideOutVertically(
            // Exits by sliding up from offset 0 to -fullHeight.
            targetOffsetY = { fullHeight -> -fullHeight }
        )
    ) {

        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier.fillMaxWidth()
        ) {
            Box(
                Modifier
                    .width(width75Percent.dp)
                    .height(height10Percent.dp)
                    .background(
                        color = MaterialTheme.colors.secondary,
                        shape = RoundedCornerShape(75)
                    )
                    .animateContentSize()
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.Center,
                    modifier = Modifier.fillMaxWidth()
                )
                {
                    Row(
                        modifier = Modifier.width(width75Percent.dp),
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        IconButton(onClick = { onDisplayingAboutChanged(true) }) {
                            Image(
                                modifier = Modifier
                                    .fillMaxHeight()
                                    .size(menuItemHeight.dp),
                                painter = painterResource(id = images["about"]!!),
                                contentDescription = "Image"
                            )
                        }
                        Column(
                            modifier = Modifier.fillMaxHeight().width((2.0*menuItemHeight).dp),
                            verticalArrangement = Arrangement.Center,
                            horizontalAlignment = Alignment.CenterHorizontally
                        ) {
                            Row(
                                modifier = Modifier.height((0.66*menuItemHeight).dp).width((2.0*menuItemHeight).dp),
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.SpaceBetween
                            )
                            {
                                IconButton(onClick = { onRequestLeaderboards() }) {
                                    Image(
                                        modifier = Modifier
                                            .size((0.66*menuItemHeight).dp)
                                            .alpha(
                                                if (!info.playGamesServices) {
                                                    0.33f
                                                } else {
                                                    1f
                                                }
                                            ),
                                        painter = painterResource(id = images["play-lead"]!!),
                                        contentDescription = "leaderboards"
                                    )
                                }
                                IconButton(onClick = { onRequestAchievements() }) {
                                    Image(
                                        modifier = Modifier
                                            .size((0.66*menuItemHeight).dp)
                                            .alpha(
                                                if (!info.playGamesServices) {
                                                    0.33f
                                                } else {
                                                    1f
                                                }
                                            ),
                                        painter = painterResource(id = images["play-ach"]!!),
                                        contentDescription = "achievements"
                                    )
                                }
                            }
                            Image(
                                modifier = Modifier
                                    .size((0.33*menuItemHeight).dp)
                                    .alpha(
                                        if (!info.playGamesServices) {
                                            0.33f
                                        } else {
                                            1f
                                        }
                                    ),
                                painter = painterResource(id = images["play-controller"]!!),
                                contentDescription = "play"
                            )
                        }
                    }
                }
            }
        }
    }
}