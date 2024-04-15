package io.agora.api.example.compose.ui.home

import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.calculateEndPadding
import androidx.compose.foundation.layout.calculateStartPadding
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalLayoutDirection
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.model.Component
import io.agora.api.example.compose.model.Example
import io.agora.api.example.compose.ui.common.APIExampleScaffold
import io.agora.api.example.compose.ui.example.ExampleItem

@Composable
fun Home(
    components: List<Component>,
    onExampleClick: (example: Example, component: Component) -> Unit,
    onSettingClick: () -> Unit,
) {
    val ltr = LocalLayoutDirection.current
    APIExampleScaffold(
        topBarTitle = "APIExample",
        showSettingIcon = true,
        onSettingClick = onSettingClick,
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
            components.forEach { component ->
                item {
                    Text(
                        text = component.name,
                        style = MaterialTheme.typography.bodyLarge,
                    )
                    Spacer(modifier = Modifier.height(ComponentPadding))
                }
                items(component.examples) { example ->
                    ExampleItem(
                        example = example,
                        onClick = { onExampleClick(example, component) }
                    )
                    Spacer(modifier = Modifier.height(ExampleItemPadding))
                }
            }

        }
    }
}

private val ComponentPadding = 16.dp
private val ExampleItemPadding = 8.dp