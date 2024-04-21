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

val advanceComponent = Component(
    1,
    "Advance",
    examples = AdvanceExampleList
)

val Components = listOf(
    basicComponent,
    advanceComponent
)