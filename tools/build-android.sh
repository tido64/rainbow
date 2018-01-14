#!/bin/bash
# Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

APP_NAME=${APP_NAME:-Rainbow}
APP_ID=com.bifrostentertainment.rainbow
BUILD_TOOLS_VERSION=${BUILD_TOOLS_VERSION:-27.0.3}
GRADLE_ANDROID_PLUGIN_VERSION=${GRADLE_ANDROID_PLUGIN_VERSION:-3.0.1}
GRADLE_VERSION=${GRADLE_VERSION:-4.4.1}
MIN_SDK_VERSION=${MIN_SDK_VERSION:-19}
TARGET_SDK_VERSION=${TARGET_SDK_VERSION:-26}

APP_MODULE=app
PROJECT=$(cd -P "$(dirname $0)/.." && pwd)

# Auto-generate files
pushd $PROJECT > /dev/null
tools/shaders-gen.py
popd > /dev/null

echo -n "Removing stale files..."
rm -fr *.iml .DS_Store .gradle .idea $APP_MODULE build build.gradle gradle local.properties log.txt settings.gradle
echo " done"

echo -n "Generating project files..."
mkdir -p $APP_MODULE/libs
mkdir -p $APP_MODULE/src/main/assets
mkdir -p $APP_MODULE/src/main/java/${APP_ID//./\/}
mkdir -p $APP_MODULE/src/main/res/values
mkdir -p gradle/wrapper

CMAKE_ARGUMENTS=""
for arg in $@; do
  CMAKE_ARGUMENTS+=$',\n                          '
  CMAKE_ARGUMENTS+="'$arg'"
done

cat > $APP_MODULE/build.gradle << APP_BUILD_GRADLE
apply plugin: 'com.android.application'

android {
    compileSdkVersion $TARGET_SDK_VERSION
    buildToolsVersion '$BUILD_TOOLS_VERSION'
    defaultConfig {
        applicationId '$APP_ID'
        minSdkVersion $MIN_SDK_VERSION
        targetSdkVersion $TARGET_SDK_VERSION
        versionCode 1
        versionName '1.0'
        externalNativeBuild {
            cmake {
                arguments '-DANDROID_ARM_NEON=TRUE',
                          '-DANDROID_STL=c++_shared'$CMAKE_ARGUMENTS
            }
        }
        ndk {
            abiFilters 'arm64-v8a'
        }
    }
    buildTypes {
        release {
            minifyEnabled false
            ndk {
                debuggable true
            }
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }
    externalNativeBuild {
        cmake {
            path '$PROJECT/CMakeLists.txt'
        }
    }
    lintOptions {
        disable 'GoogleAppIndexingWarning'
    }
    sourceSets {
        main {
            assets.srcDirs = ['src/main/assets']
            jniLibs.srcDirs = ['$PROJECT/lib/FMOD/lib/android']
        }
    }
}

dependencies {
    implementation fileTree(include: ['*.jar'], dir: 'libs')
}
APP_BUILD_GRADLE

cat > $APP_MODULE/src/main/AndroidManifest.xml << ANDROIDMANIFEST_XML
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
          package="$APP_ID">
  <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />
  <!-- uses-permission android:name="android.permission.RECORD_AUDIO" / -->
  <uses-feature android:glEsVersion="0x00020000" />
  <uses-feature android:name="android.hardware.screen.portrait" />
  <application android:allowBackup="true"
               android:icon="@mipmap/ic_launcher"
               android:label="@string/app_name"
               android:roundIcon="@mipmap/ic_launcher_round"
               android:theme="@android:style/Theme.DeviceDefault.NoActionBar.Fullscreen">
    <activity android:configChanges="orientation|screenSize"
              android:label="@string/app_name"
              android:launchMode="singleTask"
              android:name=".RainbowActivity"
              android:screenOrientation="sensorLandscape">
      <intent-filter>
        <action android:name="android.intent.action.MAIN" />
        <category android:name="android.intent.category.LAUNCHER" />
      </intent-filter>
      <meta-data android:name="android.app.lib_name" android:value="rainbow" />
    </activity>
  </application>
</manifest>
ANDROIDMANIFEST_XML

cat > build.gradle << BUILD_GRADLE
// Top-level build file where you can add configuration options common to all sub-projects/modules.

buildscript {
    repositories {
        google()
        jcenter()
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:$GRADLE_ANDROID_PLUGIN_VERSION'

        // NOTE: Do not place your application dependencies here; they belong
        // in the individual module build.gradle files
    }
}

allprojects {
    repositories {
        google()
        jcenter()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
BUILD_GRADLE

cat > gradle/wrapper/gradle-wrapper.properties << GRADLE_WRAPPER
# $(date)
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-$GRADLE_VERSION-all.zip
GRADLE_WRAPPER

echo "include ':$APP_MODULE'" > settings.gradle
echo " done"

echo -n "Gathering source files..."

# Link third-party libraries
ln -s $PROJECT/lib/FMOD/lib/android/fmod.jar $APP_MODULE/libs/

# Assets
cp $PROJECT/js/*.{js,js.map,png,ttf} $APP_MODULE/src/main/assets/

# Copy RainbowActivity.java
cp $PROJECT/src/Platform/Android/RainbowActivity.java $APP_MODULE/src/main/java/${APP_ID//./\/}/

# Resources
cp -R $PROJECT/build/android/$APP_MODULE/src/main/res/{drawable,mipmap}* $APP_MODULE/src/main/res/
cat > $APP_MODULE/src/main/res/values/strings.xml << STRINGS_XML
<resources>
    <string name="app_name">$APP_NAME</string>
</resources>
STRINGS_XML

echo " done"

gradle
