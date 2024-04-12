package io.agora.api.example.compose.ui.component


import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.calculateEndPadding
import androidx.compose.foundation.layout.calculateStartPadding
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.LocalContentColor
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.ColorFilter
import androidx.compose.ui.platform.LocalLayoutDirection
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.model.Component
import io.agora.api.example.compose.model.Example
import io.agora.api.example.compose.ui.common.CatalogScaffold
import io.agora.api.example.compose.ui.example.ExampleItem

@Composable
fun Component(
    component: Component,
    onExampleClick: (example: Example) -> Unit,
    onBackClick: () -> Unit
) {
    val ltr = LocalLayoutDirection.current
    CatalogScaffold(
        topBarTitle = component.name,
        showBackNavigationIcon = true,
        guidelinesUrl = component.guidelinesUrl,
        docsUrl = component.docsUrl,
        sourceUrl = component.sourceUrl,
        onBackClick = onBackClick,
    ) { paddingValues ->
        LazyColumn(
            modifier = Modifier.consumeWindowInsets(paddingValues),
            contentPadding = PaddingValues(
                start = paddingValues.calculateStartPadding(ltr) + ComponentPadding,
                top = paddingValues.calculateTopPadding() + ComponentPadding,
                end = paddingValues.calculateEndPadding(ltr) + ComponentPadding,
                bottom = paddingValues.calculateBottomPadding() + ComponentPadding
            )
        ) {
            item {
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(vertical = ComponentIconVerticalPadding)
                ) {
                    Image(
                        painter = painterResource(id = component.icon),
                        contentDescription = null,
                        modifier = Modifier
                            .size(ComponentIconSize)
                            .align(Alignment.Center),
                        colorFilter = if (component.tintIcon) {
                            ColorFilter.tint(LocalContentColor.current)
                        } else {
                            null
                        }
                    )
                }
            }
            item {
                Text(
                    text = "",
                    style = MaterialTheme.typography.bodyLarge
                )
                Spacer(modifier = Modifier.height(ComponentPadding))
                Text(
                    text = component.description,
                    style = MaterialTheme.typography.bodyMedium
                )
                Spacer(modifier = Modifier.height(ComponentDescriptionPadding))
            }
            item {
                Text(
                    text = "",
                    style = MaterialTheme.typography.bodyLarge
                )
                Spacer(modifier = Modifier.height(ComponentPadding))
            }
            if (component.examples.isNotEmpty()) {
                items(component.examples) { example ->
                    ExampleItem(
                        example = example,
                        onClick = onExampleClick
                    )
                    Spacer(modifier = Modifier.height(ExampleItemPadding))
                }
            } else {
                item {
                    Text(
                        text = "",
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Spacer(modifier = Modifier.height(ComponentPadding))
                }
            }
        }
    }
}

private val ComponentIconSize = 108.dp
private val ComponentIconVerticalPadding = 42.dp
private val ComponentPadding = 16.dp
private val ComponentDescriptionPadding = 32.dp
private val ExampleItemPadding = 8.dp