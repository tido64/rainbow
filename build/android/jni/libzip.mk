# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libzip
LOCAL_SRC_FILES :=\
	$(LOCAL_MODULE)/zip_add.c \
	$(LOCAL_MODULE)/zip_add_dir.c \
	$(LOCAL_MODULE)/zip_close.c \
	$(LOCAL_MODULE)/zip_delete.c \
	$(LOCAL_MODULE)/zip_dirent.c \
	$(LOCAL_MODULE)/zip_entry_free.c \
	$(LOCAL_MODULE)/zip_entry_new.c \
	$(LOCAL_MODULE)/zip_err_str.c \
	$(LOCAL_MODULE)/zip_error.c \
	$(LOCAL_MODULE)/zip_error_clear.c \
	$(LOCAL_MODULE)/zip_error_get.c \
	$(LOCAL_MODULE)/zip_error_get_sys_type.c \
	$(LOCAL_MODULE)/zip_error_strerror.c \
	$(LOCAL_MODULE)/zip_error_to_str.c \
	$(LOCAL_MODULE)/zip_fclose.c \
	$(LOCAL_MODULE)/zip_file_error_clear.c \
	$(LOCAL_MODULE)/zip_file_error_get.c \
	$(LOCAL_MODULE)/zip_file_get_offset.c \
	$(LOCAL_MODULE)/zip_file_strerror.c \
	$(LOCAL_MODULE)/zip_filerange_crc.c \
	$(LOCAL_MODULE)/zip_fopen.c \
	$(LOCAL_MODULE)/zip_fopen_index.c \
	$(LOCAL_MODULE)/zip_fread.c \
	$(LOCAL_MODULE)/zip_free.c \
	$(LOCAL_MODULE)/zip_get_archive_comment.c \
	$(LOCAL_MODULE)/zip_get_archive_flag.c \
	$(LOCAL_MODULE)/zip_get_file_comment.c \
	$(LOCAL_MODULE)/zip_get_num_files.c \
	$(LOCAL_MODULE)/zip_get_name.c \
	$(LOCAL_MODULE)/zip_memdup.c \
	$(LOCAL_MODULE)/zip_name_locate.c \
	$(LOCAL_MODULE)/zip_new.c \
	$(LOCAL_MODULE)/zip_open.c \
	$(LOCAL_MODULE)/zip_rename.c \
	$(LOCAL_MODULE)/zip_replace.c \
	$(LOCAL_MODULE)/zip_set_archive_comment.c \
	$(LOCAL_MODULE)/zip_set_archive_flag.c \
	$(LOCAL_MODULE)/zip_set_file_comment.c \
	$(LOCAL_MODULE)/zip_source_buffer.c \
	$(LOCAL_MODULE)/zip_source_file.c \
	$(LOCAL_MODULE)/zip_source_filep.c \
	$(LOCAL_MODULE)/zip_source_free.c \
	$(LOCAL_MODULE)/zip_source_function.c \
	$(LOCAL_MODULE)/zip_source_zip.c \
	$(LOCAL_MODULE)/zip_set_name.c \
	$(LOCAL_MODULE)/zip_stat.c \
	$(LOCAL_MODULE)/zip_stat_index.c \
	$(LOCAL_MODULE)/zip_stat_init.c \
	$(LOCAL_MODULE)/zip_strerror.c \
	$(LOCAL_MODULE)/zip_unchange.c \
	$(LOCAL_MODULE)/zip_unchange_all.c \
	$(LOCAL_MODULE)/zip_unchange_archive.c \
	$(LOCAL_MODULE)/zip_unchange_data.c

LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)
