package io.agora.api.example.compose.data

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map

class SettingPreferencesRepository(private val context: Context) {

    private companion object {
        val Context.dataStore: DataStore<Preferences> by preferencesDataStore("setting_preferences")
        val VIDEO_RESOLUTION = stringPreferencesKey("video_resolution")
        val VIDEO_FRAME_RATE = stringPreferencesKey("video_frame_rate")
    }

    suspend fun saveVideoResolution(resolution: String?) {
        context.dataStore.edit { preferences ->
            if (resolution == null) {
                preferences.remove(VIDEO_RESOLUTION)
            } else {
                preferences[VIDEO_RESOLUTION] = resolution
            }
        }
    }

    suspend fun getVideoResolution(): String? = context.dataStore.data
        .map { preferences -> preferences[VIDEO_RESOLUTION] }
        .first()

}