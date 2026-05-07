---
name: review-case
description: >
  Review an APIExample-Audio case after it has been created or modified.
  Use when: checking lifecycle, permission, registration, and voice-SDK-only
  constraints before considering a case ready. Always run the minimum build
  verification command as part of the review.
  Keywords: review, audit, check, lifecycle, threading, permissions, compliance.
---

# Review Case — APIExample-Audio

## What this skill is for

Use this skill after a case has been created or modified in `APIExample-Audio/`. It checks lifecycle, permission, registration, and voice-SDK-only constraints, then runs the minimum build verification command.

## Source of truth

1. `APIExample-Audio/AGENTS.md`
2. `APIExample-Audio/ARCHITECTURE.md`
3. Target case source under `APIExample-Audio/app/src/main/java/io/agora/api/example/examples/**`
4. `APIExample-Audio/app/src/main/res/navigation/nav_graph.xml`
5. `APIExample-Audio/app/src/main/res/values/strings.xml`

## Procedure

1. Open the target case and verify teardown order: `leaveChannel()` before `RtcEngine.destroy()`, aligned with the audio project's documented lifecycle rule.
2. Verify all UI updates triggered from `IRtcEngineEventHandler` callbacks are dispatched back to the UI thread.
3. For cases that call `joinChannel()`, verify microphone permission is requested through the project's permission helper before join, and that the case remains audio-only.
4. Check for required engine-init safeguards used in this project, including backend reporting setup and private-cloud null-guarding where applicable.
5. Confirm the case registration still aligns across `@Example`, `nav_graph.xml`, `strings.xml`, and `ARCHITECTURE.md`.
6. Run the minimum build verification command from `APIExample-Audio/` before finalizing the review.

## Verify

- Run `./gradlew assembleDebug` from `APIExample-Audio/`
- Confirm the target case does not call `enableVideo()`, `setupLocalVideo()`, or `VideoCanvas`
- Confirm `@Example`, `nav_graph.xml`, `strings.xml`, and `ARCHITECTURE.md` still align

## Out of scope

- Fixing the case during review by default
- Treating compile success as proof of runtime correctness

## Never

- Never approve any video API usage in the audio-only project
- Never skip the build command
