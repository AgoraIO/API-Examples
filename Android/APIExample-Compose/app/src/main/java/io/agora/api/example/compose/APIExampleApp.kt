package io.agora.api.example.compose

import androidx.compose.runtime.Composable
import io.agora.api.example.compose.ui.theme.APIExampleComposeTheme

@Composable
fun APIExampleApp(){
    APIExampleComposeTheme {
        NavGraph()
    }
}