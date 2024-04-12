package io.agora.api.example.compose.model

import androidx.annotation.DrawableRes
import io.agora.api.example.compose.R

data class Component(
    val id: Int,
    val name: String,
    val description: String,
    @DrawableRes
    val icon: Int = R.drawable.ic_component,
    val tintIcon: Boolean = false,
    val guidelinesUrl: String,
    val docsUrl: String,
    val sourceUrl: String,
    val examples: List<Example>
)

val basicComponent = Component(
    0,
    "Basic",
    "Basic components are the foundation of any application. They are the building blocks that make up the user interface.",
    guidelinesUrl = "https://material.io/components",
    docsUrl = "https://material.io/components",
    sourceUrl = "",
    examples = BasicExampleList
)

val Components = listOf(
    basicComponent
)