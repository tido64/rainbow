NAME = OnWire
PACKAGE = com.bifrostgames.onwire
TARGET = $(NAME)-debug
BINDIR = bin

ANT = ant debug

default: $(BINDIR)/$(TARGET).apk

%.apk:
	@ndk-build
	@$(ANT)

create:
	@android create project --name $(NAME) --target 1 --path . --package $(PACKAGE) --activity $(NAME)

install: $(EXEC)
	@adb install $<

uninstall:
	@adb uninstall $(PACKAGE)

update: clean
	@android update project -p . -s -t 1

clean:
	@ndk-build clean
	@rm -fr $(BINDIR)/* gen/
