
package io.agora.api.example.compose.ui.common

//import androidx.compose.material3.HorizontalDivider
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarScrollBehavior
import androidx.compose.runtime.Composable
import androidx.compose.ui.text.style.TextOverflow

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun APIExampleTopAppBar(
    title: String,
    showBackNavigationIcon: Boolean = false,
    showSettingIcon: Boolean = false,
    scrollBehavior: TopAppBarScrollBehavior? = null,
    onBackClick: () -> Unit = {},
    onSettingClick: () -> Unit = {},
) {
    TopAppBar(
        title = {
            Text(
                text = title,
                maxLines = 1,
                overflow = TextOverflow.Ellipsis,
            )
        },
        actions = {
            Box {
                Row {
                    if(showSettingIcon){
                        IconButton(onClick = { onSettingClick() }) {
                            Icon(
                                imageVector = Icons.Default.Settings,
                                contentDescription = ""
                            )
                        }
                    }
                }
            }
        },
        navigationIcon = {
            if (showBackNavigationIcon) {
                IconButton(onClick = onBackClick) {
                    Icon(
                        imageVector = Icons.AutoMirrored.Default.ArrowBack,
                        contentDescription = ""
                    )
                }
            }
        },
        scrollBehavior = scrollBehavior
    )
}