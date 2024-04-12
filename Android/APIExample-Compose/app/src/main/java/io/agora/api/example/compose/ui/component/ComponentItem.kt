package io.agora.api.example.compose.ui.component

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.LocalContentColor
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedCard
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.ColorFilter
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.model.Component

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ComponentItem(
    component: Component,
    onClick: (component: Component) -> Unit
) {
    OutlinedCard(
        onClick = { onClick(component) },
        modifier = Modifier
            .height(ComponentItemHeight)
            .padding(ComponentItemOuterPadding)
    ) {
        Box(modifier = Modifier.fillMaxSize().padding(ComponentItemInnerPadding)) {
            Image(
                painter = painterResource(id = component.icon),
                contentDescription = null,
                modifier = Modifier
                    .size(ComponentItemIconSize)
                    .align(Alignment.Center),
                colorFilter = if (component.tintIcon) {
                    ColorFilter.tint(LocalContentColor.current)
                } else {
                    null
                },
                contentScale = ContentScale.Inside
            )
            Text(
                text = component.name,
                modifier = Modifier.align(Alignment.BottomStart),
                style = MaterialTheme.typography.bodySmall
            )
        }
    }
}

private val ComponentItemHeight = 180.dp
private val ComponentItemOuterPadding = 4.dp
private val ComponentItemInnerPadding = 16.dp
private val ComponentItemIconSize = 80.dp