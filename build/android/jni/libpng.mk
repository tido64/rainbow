LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS :=

LOCAL_MODULE := libpng
LOCAL_SRC_FILES :=\
	$(LOCAL_MODULE)/png.c \
	$(LOCAL_MODULE)/pngerror.c \
	$(LOCAL_MODULE)/pngget.c \
	$(LOCAL_MODULE)/pngmem.c \
	$(LOCAL_MODULE)/pngpread.c \
	$(LOCAL_MODULE)/pngread.c \
	$(LOCAL_MODULE)/pngrio.c \
	$(LOCAL_MODULE)/pngrtran.c \
	$(LOCAL_MODULE)/pngrutil.c \
	$(LOCAL_MODULE)/pngset.c \
	$(LOCAL_MODULE)/pngtrans.c \
	$(LOCAL_MODULE)/pngwio.c \
	$(LOCAL_MODULE)/pngwrite.c \
	$(LOCAL_MODULE)/pngwtran.c \
	$(LOCAL_MODULE)/pngwutil.c

LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)
