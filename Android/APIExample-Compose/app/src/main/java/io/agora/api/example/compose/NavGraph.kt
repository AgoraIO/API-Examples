package io.agora.api.example.compose

import androidx.compose.runtime.Composable
import androidx.navigation.NavType
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import androidx.navigation.navArgument
import io.agora.api.example.compose.model.Component
import io.agora.api.example.compose.model.Components
import io.agora.api.example.compose.model.Example
import io.agora.api.example.compose.ui.component.Component
import io.agora.api.example.compose.ui.example.Example
import io.agora.api.example.compose.ui.home.Home

@Composable
fun NavGraph() {
    val navController = rememberNavController()
    NavHost(
        navController = navController,
        startDestination = HomeRoute
    ) {
        composable(HomeRoute) {
            Home(
                components = Components,
                onComponentClick = { component -> navController.navigate(component.route()) },
            )
        }
        composable(
            route = "$ComponentRoute/" +
                    "{$ComponentIdArgName}",
            arguments = listOf(
                navArgument(ComponentIdArgName) { type = NavType.IntType }
            )
        ) { navBackStackEntry ->
            val arguments = requireNotNull(navBackStackEntry.arguments) { "No arguments" }
            val componentId = arguments.getInt(ComponentIdArgName)
            val component = Components.first { component -> component.id == componentId }
            val componentRoute = component.route()
            Component(
                component = component,
                onExampleClick = { example -> navController.navigate(example.route(component)) },
                onBackClick = { navController.popBackStack() },
            )
        }
        composable(
            route = "$ExampleRoute/" +
                    "{$ComponentIdArgName}/" +
                    "{$ExampleIndexArgName}",
            arguments = listOf(
                navArgument(ComponentIdArgName) { type = NavType.IntType },
                navArgument(ExampleIndexArgName) { type = NavType.IntType }
            )
        ) { navBackStackEntry ->
            val arguments = requireNotNull(navBackStackEntry.arguments) { "No arguments" }
            val componentId = arguments.getInt(ComponentIdArgName)
            val exampleIndex = arguments.getInt(ExampleIndexArgName)
            val component = Components.first { component -> component.id == componentId }
            val example = component.examples[exampleIndex]
            Example(
                component = component,
                example = example,
                onBackClick = { navController.popBackStack() },
            )
        }
    }

}

private fun Component.route() = "$ComponentRoute/$id"

private fun Example.route(component: Component) =
    "$ExampleRoute/${component.id}/${component.examples.indexOf(this)}"

private const val HomeRoute = "home"
private const val ComponentRoute = "component"
private const val ExampleRoute = "example"
private const val ComponentIdArgName = "componentId"
private const val ExampleIndexArgName = "exampleIndex"