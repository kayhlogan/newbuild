# detect platform, move libpd dylib to local folder on mac
UNAME = $(shell uname)
SOLIB_PREFIX = lib

ifeq ($(UNAME), Darwin)  # Mac
  SOLIB_EXT = dylib
  PDNATIVE_PLATFORM = mac
  CXXFLAGS = -D__MACOSX_CORE__
  AUDIO_API = -framework Foundation -framework CoreAudio
else
  ifeq ($(OS), Windows_NT)  # Windows, use Mingw
    SOLIB_EXT = dll
    SOLIB_PREFIX = 
    PDNATIVE_PLATFORM = windows
    CXXFLAGS = -D__WINDOWS_DS__
    AUDIO_API = -lole32 -loleaut32 -ldsound -lwinmm
  else  # Assume Linux
    SOLIB_EXT = so
    PDNATIVE_PLATFORM = linux
    CXXFLAGS = -D__UNIX_JACK__ -D__LINUX_ALSA__
    AUDIO_API = -ljack -lasound -pthread
  endif
endif

LIBPD_DIR = /home/pi/libpd
LIBPD_CPP = $(LIBPD_DIR)/libs/libpdcpp.$(SOLIB_EXT)

SRC_FILES = src/receiver.cpp src/RtAudio.cpp src/main.cpp
TARGET = play

CFLAGS += -I$(LIBPD_DIR)/pure-data/src -I$(LIBPD_DIR)/libpd_wrapper -I$(LIBPD_DIR)/cpp -I./src -DPD -O3
CXXFLAGS += $(CFLAGS) -std=c++11 -DLIBPD_USE_STD_MUTEX

.PHONY: clean clobber

$(TARGET): ${SRC_FILES:.cpp=.o} ${EXT_FILES:.c=.o} $(LIBPD_CPP)
	g++ -o $(TARGET) $^ $(CXXFLAGS) $(LIBPD_CPP) $(AUDIO_API)
	if [ $(PDNATIVE_PLATFORM) == "mac" ]; then mkdir -p ./libs && cp $(LIBPD_CPP) ./libs; fi

$(LIBPD_CPP):
	cd $(LIBPD_DIR) && make cpplib UTIL=true EXTRA=true

clean:
	rm -f src/*.o

clobber: clean
	rm -f $(TARGET)
	if [ $(PDNATIVE_PLATFORM) == "mac" ]; then rm -rf ./libs; fi
