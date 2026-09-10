package io.agora.api.example.compose.model

data class Component(
    val id: Int,
    val name: String,
    val examples: List<Example>
)

val basicComponent = Component(
    0,
    "Basic",
    examples = BasicExampleList
)

val advancedComponent = Component(
    1,
    "Advanced",
    examples = AdvancedExampleList
)

val Components = listOf(
    basicComponent,
    advancedComponent
)
