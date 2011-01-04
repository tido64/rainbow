PRJ_PATH := $(call my-dir)
LIB_PATH := $(PRJ_PATH)/../../../libs
SRC_PATH := $(PRJ_PATH)/../../../src

include $(LIB_PATH)/Rainbow.mk
include $(LIB_PATH)/Box2D.mk
include $(LIB_PATH)/freetype/FreeType.mk
include $(LIB_PATH)/../../lib/libpng.mk

LOCAL_PATH := $(PRJ_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := onwire
LOCAL_STATIC_LIBRARIES := rainbow box2d freetype png
LOCAL_SRC_FILES := OnWireAndroid.cpp
LOCAL_LDLIBS += -llog -lGLESv1_CM #-landroid -lEGL

include $(BUILD_SHARED_LIBRARY)

#$(call import-module,android/native_app_glue)
