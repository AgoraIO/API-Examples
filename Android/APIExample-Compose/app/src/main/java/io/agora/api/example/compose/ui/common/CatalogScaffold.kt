
package io.agora.api.example.compose.ui.common

import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.input.nestedscroll.nestedScroll
import androidx.compose.ui.platform.LocalContext
import io.agora.api.example.compose.util.GuidelinesUrl
import io.agora.api.example.compose.util.ReleasesUrl
import io.agora.api.example.compose.util.SourceUrl
import io.agora.api.example.compose.util.openUrl

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CatalogScaffold(
    topBarTitle: String,
    showBackNavigationIcon: Boolean = false,
    showSettingIcon: Boolean = false,
    guidelinesUrl: String = GuidelinesUrl,
    docsUrl: String = ReleasesUrl,
    sourceUrl: String = SourceUrl,
    onBackClick: () -> Unit = {},
    onSettingClick: () -> Unit = {},
    content: @Composable (PaddingValues) -> Unit
) {
    val context = LocalContext.current
    val scrollBehavior = TopAppBarDefaults.pinnedScrollBehavior()

    Scaffold(
        topBar = {
            CatalogTopAppBar(
                title = topBarTitle,
                showBackNavigationIcon = showBackNavigationIcon,
                showSettingIcon = showSettingIcon,
                scrollBehavior = scrollBehavior,
                onBackClick = onBackClick,
                onSettingClick = onSettingClick,
                onGuidelinesClick = { context.openUrl(guidelinesUrl) },
                onDocsClick = { context.openUrl(docsUrl) },
                onSourceClick = { context.openUrl(sourceUrl) },
            )
        },
        modifier = Modifier.nestedScroll(scrollBehavior.nestedScrollConnection),
        content = content
    )

}
