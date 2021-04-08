#!/bin/sh
#*************************************************
# Automatic Build Script - Setting file          *
#                                                *
#*************************************************

#********************************
# Include                       *
#********************************
. ../../BnO_fs1/script/make_lib.sh


#********************************
# Build Setting                 *
#********************************
#build generator
build_debug_en="1"
build_release_en="1"

#ota generator
ota_debug_en="0"
ota_release_en="1"

#clean build
clean_en="1"

#zip enable
zip_en="1"


#********************************
# Project Environment           *
#********************************
project=ca16
hw_ver=DVT

#folder
ota_folder="ota"
tmp_folder="tmp_files"

#pre-built bootloader
#  not define means re-build new piu/ubl
#  have define means use old image
piu_prebuilt_hex="ca16_piu_swv2.0.3_release.hex"
ubl_prebuilt_hex="ca16_ubl_swv2.0.3_release.hex"

#hex name
piu_build_hex="${project}_piu.hex"
ubl_build_hex="${project}_ubl.hex"
fw_build_hex="${project}_fw.hex"

#intermediate files
tmp_hex="tmp.hex"

#log files
tmp_log="hexmerger_t.log"
out_log="hexmerger.log"



#********************************
# Main Flow                     *
#********************************
build_start
