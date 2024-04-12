package io.agora.api.example.compose.ui.home

import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.calculateEndPadding
import androidx.compose.foundation.layout.calculateStartPadding
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.lazy.grid.items
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalLayoutDirection
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.model.Component
import io.agora.api.example.compose.ui.common.CatalogScaffold
import io.agora.api.example.compose.ui.component.ComponentItem

@Composable
fun Home(
    components: List<Component>,
    onComponentClick: (component: Component) -> Unit,
    onSettingClick: () -> Unit,
) {
    val ltr = LocalLayoutDirection.current
    CatalogScaffold(
        topBarTitle = "APIExample",
        showSettingIcon = true,
        onSettingClick = onSettingClick,
    ) { paddingValues ->
        LazyVerticalGrid(
            modifier = Modifier.consumeWindowInsets(paddingValues),
            columns = GridCells.Adaptive(HomeCellMinSize),
            content = {
                items(components) { component ->
                    ComponentItem(
                        component = component,
                        onClick = onComponentClick
                    )
                }
            },
            contentPadding = PaddingValues(
                start = paddingValues.calculateStartPadding(ltr) + HomePadding,
                top = paddingValues.calculateTopPadding() + HomePadding,
                end = paddingValues.calculateEndPadding(ltr) + HomePadding,
                bottom = paddingValues.calculateBottomPadding() + HomePadding
            )
        )
    }
}

private val HomeCellMinSize = 180.dp
private val HomePadding = 12.dp