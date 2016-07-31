#!/bin/bash
# Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

APP_ID="com.bifrostentertainment.rainbow"
BUILD_TOOLS_VERSION=24.0.1
GRADLE_VERSION=2.10
MIN_SDK_VERSION=${MIN_SDK_VERSION:-15}
TARGET_SDK_VERSION=${TARGET_SDK_VERSION:-22}

PROJECT=$(cd -P "$(dirname $0)/.." && pwd)
TARGET=rainbow

# Auto-generate files
pushd $PROJECT > /dev/null
tools/shaders-gen.py
popd > /dev/null

echo -n "Removing stale files…"
rm -fr .gradle .idea build build.gradle* gradle gradlew gradlew.bat local.properties src *.iml
echo " done"

echo -n "Generating project files…"
android --silent create project \
        --name "Rainbow" \
        --activity "RainbowActivity" \
        --package "$APP_ID" \
        --gradle-version 1.5.+ \
        --target "android-$TARGET_SDK_VERSION" \
        --gradle \
        --path . \
    || exit 1

# Set Gradle Wrapper version
sed -e "s/gradle-[0-9]*\.[0-9]*/gradle-$GRADLE_VERSION/" -i '' gradle/wrapper/gradle-wrapper.properties
sed -e 's/http\\/https\\/' -i '' gradle/wrapper/gradle-wrapper.properties

# Add ndk.dir to local.properties
echo $(sed -e '$!d' -e 's/sdk.dir/ndk.dir/' local.properties)/ndk-bundle >> local.properties
NATIVE_APP_GLUE=$(sed -e '$!d' -e 's/ndk.dir=//' local.properties)/sources/android/native_app_glue

# Configure project
cat > build.gradle << BUILD_GRADLE
buildscript {
    repositories {
        jcenter()
    }
    dependencies {
        classpath "com.android.tools.build:gradle-experimental:0.7.2"

        // NOTE: Do not place your application dependencies here; they belong
        // in the individual module build.gradle files
    }
}

allprojects {
    repositories {
        jcenter()
    }
}

apply plugin: "com.android.model.application"

model {
    repositories {
        prebuilt(PrebuiltLibraries) {
            fmod {
                binaries.withType(SharedLibraryBinary) {
                    sharedLibraryFile = file("$PROJECT/lib/FMOD/lib/android/\${targetPlatform.getName()}/libfmod.so")
                }
            }
            fmodstudio {
                binaries.withType(SharedLibraryBinary) {
                    sharedLibraryFile = file("$PROJECT/lib/FMOD/lib/android/\${targetPlatform.getName()}/libfmodstudio.so")
                }
            }
        }
    }
    android {
        compileSdkVersion $TARGET_SDK_VERSION
        buildToolsVersion '$BUILD_TOOLS_VERSION'

        defaultConfig {
            applicationId '$APP_ID'
            minSdkVersion.apiLevel $MIN_SDK_VERSION
            targetSdkVersion.apiLevel $TARGET_SDK_VERSION
            versionCode 1
            versionName '1.0'
        }
        buildTypes {
            release {
                minifyEnabled = false
                ndk { debuggable true }
                proguardFiles.add(file('proguard-rules.pro'))
            }
        }
        ndk {
            moduleName 'rainbow'
            toolchain 'clang'
            CFlags.addAll([
                '-std=c11',
                '-w',
                '-I$PROJECT/src/ThirdParty/FreeType',
                '-I$PROJECT/src/ThirdParty/libpng',
                '-I$PROJECT/lib',
                '-I$PROJECT/lib/FreeType/include',
                '-I$PROJECT/lib/FreeType/src',
                '-I$PROJECT/lib/Lua',
                '-I$PROJECT/lib/libpng',
                '-I$PROJECT/lib/nanosvg/src',
                '-I$PROJECT/lib/nanovg/src',
                '-I$PROJECT/lib/spine-runtimes/spine-c/include',
                '-I$NATIVE_APP_GLUE',
            ])
            cppFlags.addAll([
                '-std=c++14',
                '-Wall',
                '-Wextra',
                '-Woverloaded-virtual',
                '-Wsign-promo',
                '-I$PROJECT/src',
                '-I$PROJECT/src/ThirdParty/FreeType',
                '-I$PROJECT/src/ThirdParty/libpng',
                '-I$PROJECT/lib',
                '-I$PROJECT/lib/box2d/Box2D',
                '-I$PROJECT/lib/FMOD/inc',
                '-I$PROJECT/lib/FreeType/include',
                '-I$PROJECT/lib/Lua',
                '-I$PROJECT/lib/libpng',
                '-I$PROJECT/lib/nanosvg/src',
                '-I$PROJECT/lib/nanovg/src',
                '-I$PROJECT/lib/spine-runtimes/spine-c/include',
                '-I$NATIVE_APP_GLUE',
                '-DRAINBOW_AUDIO_FMOD=1',
                '$@',
                '-fno-rtti',
                '-fno-exceptions',
            ])
            ldLibs.addAll(['android', 'EGL', 'GLESv2', 'log', 'z'])
            stl 'c++_shared'
        }
        productFlavors {
            create('arm') {
                ndk {
                    abiFilters.add('armeabi-v7a')
                }
            }
            //create('fat')
        }
        sources {
            main {
                jni {
                    dependencies {
                        library 'fmod'
                        library 'fmodstudio'
                    }
                }
            }
        }
    }
    android.lintOptions {
        disable 'GoogleAppIndexingWarning'
    }
}

dependencies {
    compile fileTree(dir: 'src/main/jniLibs', include: ['*.jar'])
}

task wrapper(type: Wrapper) {
    gradleVersion = '$GRADLE_VERSION'
}
BUILD_GRADLE
echo " done"

echo -n "Gathering source files…"
SOURCE_FILES=$(cmake -DANDROID=1 $@ -L $PROJECT | grep SOURCE_FILES | sed -e 's/^SOURCE_FILES:STRING=//' -e 's/[^;]*\.h;//g' -e 's/;/ /g')
SOURCE_FILES=${SOURCE_FILES//$PROJECT\//}
rm -fr CMakeCache.txt CMakeFiles Makefile cmake_install.cmake
mkdir src/main/jni
for file in $SOURCE_FILES; do
  ln -s $PROJECT/$file src/main/jni/
done
ln -s $NATIVE_APP_GLUE/android_native_app_glue.c src/main/jni/

# Replace placeholder RainbowActivity with real implementation
MODULE_PATH=${APP_ID//./\/}
rm src/main/java/$MODULE_PATH/RainbowActivity.java
ln -s $PROJECT/src/Platform/Android/RainbowActivity.java src/main/java/$MODULE_PATH/

# Link third-party libraries
mkdir -p src/main/jniLibs
ln -s $PROJECT/lib/FMOD/lib/android/fmod.jar src/main/jniLibs/
echo " done"

echo -n "Generating src/main/AndroidManifest.xml…"
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

echo -n "Generating src/main/res/values/themes.xml…"
mkdir -p src/main/res/values
cat > src/main/res/values/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
  <style name="Rainbow.Theme.Default" parent="@android:style/Theme.NoTitleBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Generating src/main/res/values-v11/themes.xml…"
mkdir -p src/main/res/values-v11
cat > src/main/res/values-v11/themes.xml << THEMES_XML
<?xml version="1.0" encoding="utf-8"?>
<resources>
  <style name="Rainbow.Theme.Default" parent="@android:style/Theme.Holo.NoActionBar.Fullscreen"></style>
</resources>
THEMES_XML
echo " done"

echo -n "Removing unused resources…"
rm src/main/res/layout/main.xml
rmdir src/main/res/layout
echo " done"

./gradlew
