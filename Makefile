CXX						:= clang++
BUILD_TYPE		:= Debug

PROJECT				:= final_work/basketball
# TARGET				:= main
TARGET				:= Basketball


SRCDIR				:= src/$(PROJECT)
BUILDDIR			:= obj
TARGETDIR			:= bin
OUTPUTDIR			:= output

# extensions
SRCEXT				:= cpp
DEPEXT				:= d
OBJEXT				:= o

# flags
CXXFLAGS			:= -MMD -MP -std=c++14 -framework OpenGL -framework CoreVideo -framework Cocoa -framework IOKit
CXX_DEBUG_FLAGS		:= -Wall -g -O0
CXX_RELEASE_FLAGS	:= -O2
LDFLAGS				:=
INCLUDE				:= -I./common
LIBS				:= -L/usr/lib -L/usr/local/lib -lglfw3
# LIBS				:= -lglfw

#---------------------------------------------------------------------------------
# DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------

ifeq ($(BUILD_TYPE),Release)
	CXXFLAGS += $(CXX_RELEASE_FLAGS)
else ifeq ($(BUILD_TYPE),Debug)
	CXXFLAGS += $(CXX_DEBUG_FLAGS)
else
	$(error buildtype must be release, debug, profile or coverage)
endif

sources			:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
objects			:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(subst $(SRCEXT),$(OBJEXT),$(sources)))
dependencies 	:= $(subst .$(OBJEXT),.$(DEPEXT),$(objects))

# Defauilt Make
all: directories $(TARGETDIR)/$(TARGET)

run: all
	$(TARGETDIR)/$(TARGET)

# Remake
remake: cleaner all

# make directory
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(OUTPUTDIR)

# remove directory for intermediate products
clean:
	@$(RM) -rf $(BUILDDIR)

# remove directories for both intermediate and final products
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

-include $(dependencies)

# generate binary by linking objects
$(TARGETDIR)/$(TARGET): $(objects)
	$(CXX) $(CXXFLAGS) -o $(TARGETDIR)/$(TARGET) $^ $(LIBS)

# generate objects by compiling sources
# save dependencies of source as .d
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@sed -e 's|$${WORKING_DIR}|$(PWD)|' -e 's|$${PROJECT_DIR}|$(PROJECT)|' ./common/common.h.in > ./common/common.h
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

# Non-File Targets
.PHONY: all run remake clean cleaner
