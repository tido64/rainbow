LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := librainbow
LOCAL_SRC_FILES :=\
	Rainbow/Hardware/Controls.cpp \
	Rainbow/Hardware/Touch.cpp \
	Rainbow/FontAtlas.cpp \
	Rainbow/Physics.cpp \
	Rainbow/Sprite.cpp \
	Rainbow/SpriteBatch.cpp \
	Rainbow/TextureAtlas.cpp

include $(BUILD_STATIC_LIBRARY)
