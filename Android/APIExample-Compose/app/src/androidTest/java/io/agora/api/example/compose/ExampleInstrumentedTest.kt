package io.agora.api.example.compose

import androidx.compose.ui.test.assertIsDisplayed
import androidx.compose.ui.test.junit4.createAndroidComposeRule
import androidx.compose.ui.test.onNodeWithContentDescription
import androidx.compose.ui.test.onNodeWithText
import androidx.compose.ui.test.performClick
import org.junit.Rule
import org.junit.Test

class ExampleInstrumentedTest {
    @get:Rule
    val composeRule = createAndroidComposeRule<MainActivity>()

    @Test
    fun homeAndSettingsNavigation_renderExpectedContent() {
        composeRule.onNodeWithText("Agora API Example").assertIsDisplayed()
        composeRule.onNodeWithText("Basic").assertIsDisplayed()
        composeRule.onNodeWithText("Join Video Channel").assertIsDisplayed()

        composeRule.onNodeWithContentDescription("Open settings").performClick()

        composeRule.onNodeWithText("Settings").assertIsDisplayed()
        composeRule.onNodeWithContentDescription("Navigate back").performClick()
        composeRule.onNodeWithText("Agora API Example").assertIsDisplayed()
    }
}
