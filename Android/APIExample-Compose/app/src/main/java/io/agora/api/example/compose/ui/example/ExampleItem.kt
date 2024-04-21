package io.agora.api.example.compose.ui.example

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedCard
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import io.agora.api.example.compose.model.Example

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ExampleItem(
    example: Example,
    onClick: (example: Example) -> Unit
) {
    OutlinedCard(
        onClick = { onClick(example) },
        modifier = Modifier.fillMaxWidth()
    ) {
        Row(modifier = Modifier.padding(ExampleItemPadding)) {
            Column(modifier = Modifier.weight(1f, fill = true)) {
                Text(
                    text = stringResource(id = example.name),
                    style = MaterialTheme.typography.titleSmall
                )
                if(example.description.isNotEmpty()){
                    Spacer(modifier = Modifier.height(ExampleItemTextPadding))
                    Text(
                        text = example.description,
                        style = MaterialTheme.typography.bodySmall,
                    )
                }
            }
            Spacer(modifier = Modifier.width(ExampleItemPadding))
            Icon(
                imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                contentDescription = null,
                modifier = Modifier.align(Alignment.CenterVertically)
            )
        }
    }
}

private val ExampleItemPadding = 16.dp
private val ExampleItemTextPadding = 8.dp