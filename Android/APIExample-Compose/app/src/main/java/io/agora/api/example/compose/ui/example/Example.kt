package io.agora.api.example.compose.ui.example

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import io.agora.api.example.compose.model.Component
import io.agora.api.example.compose.model.Example
import io.agora.api.example.compose.ui.common.CatalogScaffold

@OptIn(ExperimentalLayoutApi::class)
@Composable
fun Example(
    component: Component,
    example: Example,
    onBackClick: () -> Unit,
) {
    CatalogScaffold(
        topBarTitle = example.name,
        showBackNavigationIcon = true,
        guidelinesUrl = component.guidelinesUrl,
        docsUrl = component.docsUrl,
        sourceUrl = example.sourceUrl,
        onBackClick = onBackClick,
    ) { paddingValues ->
        Box(
            modifier = Modifier
                .fillMaxSize()
                .consumeWindowInsets(WindowInsets.safeDrawing)
                .padding(paddingValues),
            contentAlignment = Alignment.Center
        ) {
            example.content()
        }
    }
}