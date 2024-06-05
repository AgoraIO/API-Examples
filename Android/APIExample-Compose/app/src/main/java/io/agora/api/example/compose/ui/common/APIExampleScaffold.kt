
package io.agora.api.example.compose.ui.common

import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.input.nestedscroll.nestedScroll

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun APIExampleScaffold(
    topBarTitle: String,
    showBackNavigationIcon: Boolean = false,
    showSettingIcon: Boolean = false,
    onBackClick: () -> Unit = {},
    onSettingClick: () -> Unit = {},
    content: @Composable (PaddingValues) -> Unit
) {
    val scrollBehavior = TopAppBarDefaults.pinnedScrollBehavior()

    Scaffold(
        topBar = {
            APIExampleTopAppBar(
                title = topBarTitle,
                showBackNavigationIcon = showBackNavigationIcon,
                showSettingIcon = showSettingIcon,
                scrollBehavior = scrollBehavior,
                onBackClick = onBackClick,
                onSettingClick = onSettingClick
            )
        },
        modifier = Modifier.nestedScroll(scrollBehavior.nestedScrollConnection),
        content = content
    )

}
