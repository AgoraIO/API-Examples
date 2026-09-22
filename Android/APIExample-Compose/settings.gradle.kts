pluginManagement {
    repositories {
        google {
            content {
                includeGroupByRegex("com\\.android.*")
                includeGroupByRegex("com\\.google.*")
                includeGroupByRegex("androidx.*")
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        // Resolve RTC SDK modules and their infrastructure dependencies from the official source.
        exclusiveContent {
            forRepository {
                maven { url = uri("https://download.agora.io/maven/") }
            }
            filter {
                includeGroup("io.agora.rtc")
                includeGroup("io.agora.infra")
            }
        }
        google()
        mavenCentral()
    }
}

rootProject.name = "Agora-APIExample-Compose"
include(":app")
