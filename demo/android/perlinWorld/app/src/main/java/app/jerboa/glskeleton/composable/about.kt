package app.jerboa.glskeleton.composable

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.ExperimentalAnimationApi
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Text
import androidx.compose.material.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import app.jerboa.glskeleton.AppInfo
import app.jerboa.glskeleton.R
import app.jerboa.glskeleton.ViewModel.SOCIAL

@OptIn(ExperimentalAnimationApi::class)
@Composable
fun about(
    displayingAbout: Boolean,
    width75Percent: Double,
    images: Map<String,Int>,
    info: AppInfo,
    onRequestingLicenses: () -> Unit,
    onRequestingSocial: (SOCIAL) -> Unit
){
    AnimatedVisibility(
        visible = displayingAbout,
        enter = fadeIn(),
        exit = fadeOut()
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier.fillMaxWidth()
        ) {
            Spacer(modifier = Modifier.size(8.dp))
            Box(
                Modifier
                    .width(width75Percent.dp)
                    .height(width75Percent.dp)
                    .background(
                        color = MaterialTheme.colors.secondary,
                        shape = RoundedCornerShape(5)
                    )
            ) {
                Column(
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.SpaceBetween,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Image(
                        modifier = Modifier
                            .size((width75Percent * 0.33).dp)
                            .weight(1f),
                        painter = painterResource(id = images["logo"]!!),
                        contentDescription = "Logo"
                    )
                    Text(
                        stringResource(id = R.string.tagline),
                        textAlign = TextAlign.Center,
                        modifier = Modifier.weight(1f),
                        fontSize = MaterialTheme.typography.body1.fontSize * info.density
                    )
                    Text(
                        stringResource(id = R.string.description),
                        textAlign = TextAlign.Center,
                        modifier = Modifier.weight(1f),
                        fontSize = MaterialTheme.typography.body1.fontSize * info.density
                    )
                    TextButton(onClick = { onRequestingLicenses() }) {
                        Text(
                            stringResource(id = R.string.OSSprompt),
                            textAlign = TextAlign.Center,
                            modifier = Modifier.weight(1f),
                            fontSize = MaterialTheme.typography.body1.fontSize * info.density
                        )
                    }
                    Text(
                        stringResource(R.string.attrib),
                        modifier = Modifier.weight(1f),
                        fontSize = MaterialTheme.typography.overline.fontSize * info.density,
                        textAlign = TextAlign.Center
                    )
                    Text(
                        "version: " + info.versionString, modifier = Modifier.weight(1f),
                        fontSize = MaterialTheme.typography.overline.fontSize * info.density,
                        textAlign = TextAlign.Center
                    )
                }
            }
            Spacer(modifier = Modifier.size(8.dp))
            socials(images, info, onRequestingSocial)
        }
    }

}