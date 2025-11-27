package io.agora.api.example.examples.advanced.audiosafety

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.CheckBox
import android.widget.EditText
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.fragment.app.setFragmentResult
import androidx.navigation.fragment.findNavController
import io.agora.api.example.R
import io.agora.api.example.annotation.Example
import io.agora.api.example.common.BaseFragment
import io.agora.api.example.common.model.Examples

/**
 * Configuration page for AudioSafety settings
 */
@Example(
    index = 23,
    group = Examples.ADVANCED,
    name = R.string.item_audio_safety,
    actionId = R.id.action_mainFragment_to_AudioSafetyConfig,
    tipsId = R.string.audio_safety
)
class AudioSafetyConfigFragment : BaseFragment() {

    private lateinit var cbEnableRegisterLocal: CheckBox
    private lateinit var etBufferDurationSeconds: EditText
    private lateinit var btnNext: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_audio_safety_config, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        // Initialize views
        cbEnableRegisterLocal = view.findViewById(R.id.cb_enable_register_local)
        etBufferDurationSeconds = view.findViewById(R.id.et_buffer_duration_seconds)
        btnNext = view.findViewById(R.id.btn_next)

        // Set up save button listener
        btnNext.setOnClickListener {
            if (saveSettings()) {
                // Came from mainFragment, navigate to AudioSafety with saved config
                val bundle = Bundle().apply {
                    putBoolean(ARG_ENABLE_REGISTER_LOCAL, cbEnableRegisterLocal.isChecked)
                    putInt(ARG_BUFFER_DURATION_SECONDS, etBufferDurationSeconds.text.toString().toIntOrNull() ?: 300)
                }
                findNavController().navigate(R.id.action_AudioSafetyConfig_to_AudioSafety, bundle)
            }
        }
    }

    private fun saveSettings(): Boolean {
        val bufferDurationText = etBufferDurationSeconds.text.toString().trim()

        if (bufferDurationText.isEmpty()) {
            etBufferDurationSeconds.error = "Buffer duration cannot be empty"
            etBufferDurationSeconds.requestFocus()
            return false
        }

        val newBufferDurationSeconds = try {
            bufferDurationText.toInt()
        } catch (e: NumberFormatException) {
            etBufferDurationSeconds.error = "Invalid number"
            etBufferDurationSeconds.requestFocus()
            return false
        }

        if (newBufferDurationSeconds <= 0) {
            etBufferDurationSeconds.error = "Buffer duration must be greater than 0"
            etBufferDurationSeconds.requestFocus()
            return false
        }

        if (newBufferDurationSeconds > 3600) {
            etBufferDurationSeconds.error = "Buffer duration should not exceed 3600 seconds (1 hour)"
            etBufferDurationSeconds.requestFocus()
            return false
        }

        // Return result to previous fragment via Fragment Result API
        val result = Bundle().apply {
            putBoolean(ARG_ENABLE_REGISTER_LOCAL, cbEnableRegisterLocal.isChecked)
            putInt(ARG_BUFFER_DURATION_SECONDS, newBufferDurationSeconds)
        }
        setFragmentResult(REQUEST_KEY, result)
        return true
    }

    companion object {
        const val ARG_ENABLE_REGISTER_LOCAL = "enable_register_local"
        const val ARG_BUFFER_DURATION_SECONDS = "buffer_duration_seconds"
        const val REQUEST_KEY = "audio_safety_config_result"
    }
}

