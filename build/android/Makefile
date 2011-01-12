NAME = OnWire
PACKAGE = com.bifrostgames.onwire
TARGET = $(NAME)-debug
BINDIR = bin
APK = $(BINDIR)/$(TARGET).apk

ANT = ant debug

default: $(APK)

%.apk:
	@ndk-build
	@$(ANT)

create:
	@android create project --name $(NAME) --target 1 --path . --package $(PACKAGE) --activity $(NAME)

install: $(APK)
	@adb install $<

uninstall:
	@adb uninstall $(PACKAGE)

update: clean
	@android update project -p . -s -t 1

clean:
	@ndk-build clean
	@ant clean
