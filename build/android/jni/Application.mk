LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

APP_CFLAGS  := -O2 -Wall -I $(LOCAL_PATH)/../../../libs  -I $(LOCAL_PATH)/../../../../lib/libpng -ftree-vectorize
APP_MODULES := onwire rainbow box2d freetype png
APP_OPTIM   := debug
