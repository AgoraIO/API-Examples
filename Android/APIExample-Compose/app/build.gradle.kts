import com.android.build.gradle.internal.api.ApkVariantOutputImpl
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Properties

plugins {
    alias(libs.plugins.androidApplication)
    alias(libs.plugins.jetbrainsKotlinAndroid)
}

val sdkVersionFile = file("../gradle.properties")
val properties = Properties()
properties.load(sdkVersionFile.inputStream())
val agoraSdkVersion = properties.getProperty("rtc_sdk_version")
println("${rootProject.project.name}   agoraSdkVersion: ${agoraSdkVersion}")
val localSdkPath = "${rootProject.projectDir.absolutePath}/../../sdk"

android {
    namespace = "io.agora.api.example.compose"
    compileSdk = 35

    defaultConfig {
        applicationId = "io.agora.api.example.compose"
        minSdk = 24
        targetSdk = 35
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        vectorDrawables {
            useSupportLibrary = true
        }

        val properties = Properties()
        properties.load(rootProject.file("local.properties").inputStream())
        val AGORA_APP_ID = properties.getProperty("AGORA_APP_ID", "")
        if (AGORA_APP_ID == "") {
            throw GradleException("请在项目根目录下local.properties文件里正确配置：AGORA_APP_ID=<您的声网AppId>")
        }
        val AGORA_APP_CERT = properties.getProperty("AGORA_APP_CERT", "")
        buildConfigField("String", "AGORA_APP_ID", "\"$AGORA_APP_ID\"")
        buildConfigField("String", "AGORA_APP_CERT", "\"$AGORA_APP_CERT\"")
    }

    signingConfigs(Action {
        create("myConfig") {
            storeFile = file(rootProject.rootDir.absolutePath + "/keystore.key")
            storePassword = "965606"
            keyAlias = "agora"
            keyPassword = "965606"
        }
    })

    buildFeatures {
        buildConfig = true
    }
    buildTypes {
        debug {
            isMinifyEnabled = false
            signingConfig = signingConfigs.getByName("myConfig")
        }
        release {
            isMinifyEnabled = true
            signingConfig = signingConfigs.getByName("myConfig")
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    buildFeatures {
        compose = true
    }
    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.14"
    }
    packaging {
        resources {
            excludes += "/META-INF/{AL2.0,LGPL2.1}"
        }
    }
    android.applicationVariants.all {
        outputs.all {
            if (this is ApkVariantOutputImpl) {
                this.outputFileName =
                    "${rootProject.name}_${agoraSdkVersion}_${
                        SimpleDateFormat("yyyyMMddHHmm").format(
                            Date()
                        )
                    }.apk"
            }
        }
    }
    sourceSets {
        getByName("main") {
            if (File(localSdkPath).exists()) {
                jniLibs.srcDirs(localSdkPath)
            }
        }

    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.ui)
    implementation(libs.androidx.ui.graphics)
    implementation(libs.androidx.ui.tooling.preview)
    implementation(libs.androidx.material3)
    implementation(libs.androidx.navigation.compose)
    implementation(libs.androidx.datastore)
    implementation(libs.androidx.datastore.preferences)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.ui.test.junit4)
    debugImplementation(libs.androidx.ui.tooling)
    debugImplementation(libs.androidx.ui.test.manifest)
    implementation(libs.androidx.material.icons.extended)
    implementation(libs.okhttp)
    implementation(libs.logging.interceptor)

    if (File(localSdkPath).exists()) {
        implementation(fileTree(localSdkPath).include("*.jar", "*.aar"))
    } else {
        implementation("io.agora.rtc:full-sdk:${agoraSdkVersion}")
        implementation("io.agora.rtc:full-screen-sharing:${agoraSdkVersion}")
//        implementation(libs.agora.full.sdk)
//        implementation(libs.agora.full.screen.sharing)
    }

}