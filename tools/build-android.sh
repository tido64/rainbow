#!/bin/bash
# Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

APP_NAME=${APP_NAME:-Rainbow}
APP_ID=com.bifrostentertainment.rainbow
BUILD_TOOLS_VERSION=${BUILD_TOOLS_VERSION:-25.0.0}
GRADLE_ANDROID_PLUGIN_VERSION=${GRADLE_ANDROID_PLUGIN_VERSION:-2.2.2}
GRADLE_VERSION=${GRADLE_VERSION:-2.14.1}
MIN_SDK_VERSION=${MIN_SDK_VERSION:-15}
TARGET_SDK_VERSION=${TARGET_SDK_VERSION:-23}

APP_MODULE=app
PROJECT=$(cd -P "$(dirname $0)/.." && pwd)

# Auto-generate files
pushd $PROJECT > /dev/null
tools/shaders-gen.py
popd > /dev/null

echo -n "Removing stale files..."
rm -fr *.iml .gradle .idea $APP_MODULE build build.gradle gradle gradlew gradlew.bat local.properties settings.gradle src
echo " done"

echo -n "Generating project files..."
android --silent create project \
        --name "$APP_NAME" \
        --activity "RainbowActivity" \
        --package $APP_ID \
        --gradle-version $GRADLE_ANDROID_PLUGIN_VERSION \
        --target android-$TARGET_SDK_VERSION \
        --gradle \
        --path . \
    || exit 1

# Set Gradle Wrapper version
sed -e "s/gradle-[0-9]*\.[0-9]*/gradle-$GRADLE_VERSION/" -i '' gradle/wrapper/gradle-wrapper.properties
sed -e 's/http\\/https\\/' -i '' gradle/wrapper/gradle-wrapper.properties

# Add ndk.dir to local.properties
echo $(sed -e '$!d' -e 's/sdk.dir/ndk.dir/' local.properties)/ndk-bundle >> local.properties

# Configure project
cat > build.gradle << BUILD_GRADLE
// Top-level build file where you can add configuration options common to all sub-projects/modules.

buildscript {
    repositories {
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
        jcenter()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}
BUILD_GRADLE

mkdir $APP_MODULE
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
                abiFilters 'armeabi-v7a'
                arguments '-DANDROID_ARM_NEON=TRUE', '-DANDROID_PLATFORM=android-$TARGET_SDK_VERSION', '-DANDROID_STL=c++_shared'
            }
        }
    }
    buildTypes {
        release {
            minifyEnabled = false
            ndk { debuggable true }
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
    compile fileTree(dir: 'libs', include: ['*.jar'])
}
APP_BUILD_GRADLE

echo "include ':$APP_MODULE'" > settings.gradle
echo " done"

echo -n "Gathering source files..."
mkdir src/main/cpp

# Replace placeholder RainbowActivity with real implementation
MODULE_PATH=${APP_ID//./\/}
cp $PROJECT/src/Platform/Android/RainbowActivity.java src/main/java/$MODULE_PATH/

# Link third-party libraries
APP_MODULE_LIBS=$APP_MODULE/libs
mkdir -p $APP_MODULE_LIBS
ln -s $PROJECT/lib/FMOD/lib/android/fmod.jar $APP_MODULE_LIBS/
echo " done"

echo -n "Generating $APP_MODULE/src/main/AndroidManifest.xml..."
cat > src/main/AndroidManifest.xml << ANDROIDMANIFEST_XML
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
          package="com.bifrostentertainment.rainbow">
  <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" />
  <!-- uses-permission android:name="android.permission.RECORD_AUDIO" / -->
  <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"
                   android:maxSdkVersion="18" />
  <uses-feature android:name="android.hardware.screen.portrait"
                android:glEsVersion="0x00020000" />
  <application android:allowBackup="true"
               android:icon="@drawable/ic_launcher"
               android:label="@string/app_name"
               android:theme="@style/Rainbow.Theme.Default">
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
echo " done"

echo -n "Generating $APP_MODULE/src/main/res/values/themes.xml..."
mkdir -p src/main/res/values
cat > src/main/res/values/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
  <style name="Rainbow.Theme.Default" parent="@android:style/Theme.NoTitleBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Generating $APP_MODULE/src/main/res/values-v11/themes.xml..."
mkdir -p src/main/res/values-v11
cat > src/main/res/values-v11/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
  <style name="Rainbow.Theme.Default" parent="@android:style/Theme.Holo.NoActionBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Removing unused resources..."
rm src/main/res/layout/main.xml
rmdir src/main/res/layout
mv src $APP_MODULE/
echo " done"

./gradlew
