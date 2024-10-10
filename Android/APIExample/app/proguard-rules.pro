# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

-keep class io.agora.**{*;}
-dontwarn javax.**
-dontwarn com.google.devtools.build.android.**

# sensetime
-keep class com.softsugar.**{*;}
-keep class com.sensetime.**{*;}

# bytedance
-keep class com.effectsar.**{*;}
-keep class com.bes.**{*;}
-keep class com.amazing.**{*;}

# ijkplayer
-keep class tv.danmaku.ijk.media.player.** {*;}
-keep class tv.danmaku.ijk.media.player.IjkMediaPlayer{*;}
-keep class tv.danmaku.ijk.media.player.ffmpeg.FFmpegApi{*;}

# exo
-keep class com.google.android.exoplayer2.**{*;}
