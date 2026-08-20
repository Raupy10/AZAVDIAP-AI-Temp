ifeq ($(OS), Windows_NT)
    GENERATOR = -G "MinGW Makefiles"
    RUN = build\AZAVDIAP.exe
else
    GENERATOR =
    RUN = ./build/AZAVDIAP
endif

.PHONY: build run clean configure

build: configure
	cmake --build build

configure:
	cmake -B build $(GENERATOR)

run: build
	$(RUN)

clean:
	rm -rf build