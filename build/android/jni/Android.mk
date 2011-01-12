PRJ_PATH := $(call my-dir)
LIB_PATH := $(PRJ_PATH)/../../../libs
SRC_PATH := ../../../src

include $(LIB_PATH)/Rainbow.mk
include $(LIB_PATH)/../../lib/Box2D.mk
include $(LIB_PATH)/freetype/FreeType.mk
include $(LIB_PATH)/../../lib/libpng.mk
include $(LIB_PATH)/../../lib/libzip.mk

LOCAL_PATH := $(PRJ_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := onwire
LOCAL_STATIC_LIBRARIES := rainbow box2d freetype png zip
LOCAL_SRC_FILES := $(SRC_PATH)/OnWireAndroid.cpp $(SRC_PATH)/OnWireGame.cpp
LOCAL_SRC_FILES += $(SRC_PATH)/Elements/Bird.cpp $(SRC_PATH)/Elements/Wind.cpp
LOCAL_SRC_FILES += $(SRC_PATH)/Objects/Avatar.cpp $(SRC_PATH)/Objects/Building.cpp $(SRC_PATH)/Objects/HUD.cpp $(SRC_PATH)/Objects/Line.cpp
LOCAL_LDLIBS += -llog -lGLESv1_CM -lz

include $(BUILD_SHARED_LIBRARY)
