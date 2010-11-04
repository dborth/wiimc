COMMAND1 = (make && sudo -E make install)
COMMAND2 = (CC=$(DEVKITPPC)/bin/powerpc-eabi-gcc CXX=$(DEVKITPPC)/bin/powerpc-eabi-g++ ./configure --host=powerpc-eabi --prefix="${DEVKITPRO}/portlibs/ppc" --libdir="${DEVKITPRO}/portlibs/ppc/lib" --disable-shared && \
      make && sudo -E make install)
COMMAND3 = (CC=$(DEVKITPPC)/bin/powerpc-eabi-gcc ./configure --host=powerpc-eabi --prefix="${DEVKITPRO}/portlibs/ppc" --libdir="${DEVKITPRO}/portlibs/ppc/lib" --enable-shared=no --enable-static=yes --with-cpu=ppc_nofpu --with-optimization=3 && \
      cd src/libmpg123 && make && sudo -E make install)

all: dir1 dir2 dir3 dir4 dir5 dir6
dir1:
   cd libntfs && $(COMMAND1)
dir2:
   cd fribidi && $(COMMAND2)
dir3:
   cd libexif && $(COMMAND2)
dir4:
   cd libiconv && $(COMMAND2)
dir5:
   cd pcre && $(COMMAND2)
dir6:
   cd libmpg123 && $(COMMAND3)
