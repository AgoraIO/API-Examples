package io.agora.api.example.compose.ui.example

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.consumeWindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import io.agora.api.example.compose.model.Example
import io.agora.api.example.compose.ui.common.APIExampleScaffold

@Composable
fun Example(
    example: Example,
    onBackClick: () -> Unit,
) {
    APIExampleScaffold(
        topBarTitle = stringResource(id = example.name),
        showBackNavigationIcon = true,
        onBackClick = onBackClick,
    ) { paddingValues ->
        Box(
            modifier = Modifier
                .fillMaxSize()
                .consumeWindowInsets(WindowInsets.safeDrawing)
                .padding(paddingValues),
            contentAlignment = Alignment.Center
        ) {
            example.content(onBackClick)
        }
    }
}