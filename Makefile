#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPRO)/libogc2/wii_rules
export	FREETYPE_CFLAGS 	:=	`$(DEVKITPRO)/portlibs/ppc/bin/powerpc-eabi-pkg-config --cflags freetype2`
export	FREETYPE_LIBS	:=	`$(DEVKITPRO)/portlibs/ppc/bin/powerpc-eabi-pkg-config --libs freetype2`

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
MPLAYER		:=	$(CURDIR)/source/mplayer
TARGET		:=	wiimc
BUILD		:=	build
SOURCES		:=	source source/libwiigui source/utils source/utils/unzip
DATA		:=	source/images source/lang source/fonts
INCLUDES	:=	source source/mplayer

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

# Compile settings, NTFS = 191KB, EXT2 = 91KB, Full = 282KB
ENABLE_NTFS                   = 1
ENABLE_EXT2                   = 0

CFLAGS		=	-g -O3 -Wall $(MACHDEP) $(INCLUDE) $(FREETYPE_CFLAGS) \
				-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Wframe-larger-than=8192
CXXFLAGS	=	$(CFLAGS)
LDFLAGS		=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map
LDFLAGS		+=	-L../buildtools

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS    := -lmplayerwii -lavformat -lavcodec -lswscale -lavutil \
                        -ljpeg -ldi -liso9660 -liconv -lpng -lz -lmxml $(FREETYPE_LIBS) \
                        -lfat -lwiiuse -lbte -logc -ltinysmb -lexif

ifeq ($(ENABLE_NTFS), 1)
CFLAGS += -DWANT_NTFS
LIBS += -lntfs
endif

ifeq ($(ENABLE_EXT2), 1)
CFLAGS += -DWANT_EXT2
LIBS += -lext2fs
endif

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGETDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.ttf) \
					$(wildcard $(dir)/*.lang) $(wildcard $(dir)/*.png) \
					$(wildcard $(dir)/*.jpg)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
 
export LIBPATHS	:= -L$(LIBOGC_LIB) $(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
				-L$(MPLAYER)/ \
				-L$(MPLAYER)/ffmpeg/libavcodec \
				-L$(MPLAYER)/ffmpeg/libavformat \
				-L$(MPLAYER)/ffmpeg/libavutil \
				-L$(MPLAYER)/ffmpeg/libswscale 

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	cd source/mplayer; $(MAKE) -f Makefile; cd ../..
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
test:
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile


clean:
	@echo clean ...
	rm -f $(BUILD)/*.d $(BUILD)/*.h $(BUILD)/*.ii $(BUILD)/*.lst $(BUILD)/*.map \
	$(BUILD)/*.o $(BUILD)/*.s
	@rm -fr $(OUTPUT).elf $(OUTPUT).dol
	cd source/mplayer; $(MAKE) -f Makefile clean

#---------------------------------------------------------------------------------
run:
	wiiload $(OUTPUT).dol

#---------------------------------------------------------------------------------
reload:
	wiiload -r $(OUTPUT).dol

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with .ttf, .lang, .png, and .jpg extensions
#---------------------------------------------------------------------------------
%.ttf.o %_ttf.h : %.ttf
	@echo $(notdir $<)
	$(bin2o)

%.lang.o %_lang.h : %.lang
	@echo $(notdir $<)
	$(bin2o)

%.png.o %_png.h : %.png
	@echo $(notdir $<)
	$(bin2o)

%.jpg.o %_jpg.h : %.jpg
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
