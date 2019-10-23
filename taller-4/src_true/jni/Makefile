APP  = saxpy_par
SRCS  = $(APP).c

run: compile upload
	adb shell /data/local/tmp/$(APP)

upload:
	adb push ../libs/armeabi-v7a/$(APP) /data/local/tmp

compile:
	/opt/android-ndk-r20/ndk-build