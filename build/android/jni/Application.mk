LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

EXT_LIB := $(LOCAL_PATH)/../../../../lib

APP_CFLAGS  := -g -Wall -I $(LOCAL_PATH)/../../../libs -I $(LOCAL_PATH)/../../../libs/freetype/include -I $(EXT_LIB) -I $(EXT_LIB)/libpng -I $(EXT_LIB)/libzip -ftree-vectorize
APP_MODULES := onwire rainbow box2d freetype png zip
APP_OPTIM   := debug
