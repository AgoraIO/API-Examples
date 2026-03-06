# ARCHITECTURE.md

Three independent Android projects, each with its own Gradle root and APK output.
For internal details of each project, see the project-level `ARCHITECTURE.md`.

---

## APIExample — Full Demo

- Package: `io.agora.api.example`
- SDK: `io.agora.rtc:full-sdk` + `full-screen-sharing`
- Language: Java + Kotlin mixed
- UI: XML layouts + ViewBinding, Jetpack Navigation
- Case registration: reflection-based via `@Example` annotation + `ClassUtils` DEX scan
- Optional modules: `agora-simple-filter` (C++ extension), `agora-stream-encrypt`
- Details: `APIExample/ARCHITECTURE.md`

---

## APIExample-Audio — Audio-Only Demo

- Package: `io.agora.api.example.audio`
- SDK: `io.agora.rtc:voice-sdk` (no video module)
- Language: Java + Kotlin mixed
- UI: XML layouts + ViewBinding, Jetpack Navigation
- Case registration: identical to APIExample — `@Example` annotation + `ClassUtils` DEX scan
- Details: `APIExample-Audio/ARCHITECTURE.md`

---

## APIExample-Compose — Jetpack Compose Demo

- Package: `io.agora.api.example.compose`
- SDK: `io.agora.rtc:full-sdk` + `full-screen-sharing`
- Language: Kotlin only
- UI: Jetpack Compose + Compose Navigation, no XML layouts
- Case registration: manual — add entry to `model/Examples.kt` + create `samples/MyCase.kt`
- Details: `APIExample-Compose/ARCHITECTURE.md`
