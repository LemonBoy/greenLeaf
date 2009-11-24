#!/bin/sh

#######################################################################
# Copyright (C)2009 SquidMan (Alex Marshall)   <SquidMan72@gmail.com> #
# Protected under the GNU GPLv2.                                      #
#######################################################################

BINUTILS_VER=2.19
BINUTILS_DIR="binutils-$BINUTILS_VER"
BINUTILS_TARBALL="binutils-$BINUTILS_VER.tar.bz2"
BINUTILS_URI="http://ftp.gnu.org/gnu/binutils/$BINUTILS_TARBALL"

MIPS_TARGET=mips-elf64

export PATH=$MIPSDEV/bin:$PATH

die() {
	echo $@
	exit 1
}

download() {
	DL=1
	if [ -f "$2" ]; then
		echo "Testing $2..."
		tar tjf "$2" >/dev/null && DL=0
	fi

	if [ $DL -eq 1 ]; then
		echo "Downloading $2..."
		wget "$1" -c -O "$2" || die "Could not download $2"
	fi
}

extract() {
	echo "Extracting $1..."
	tar xjf "$1" -C "$2" || die "Error unpacking $1"
}

buildbinutils() {
	TARGET=$1
	(
		cd $MIPSDEV/$BINUTILS_DIR								&& \
		./configure --target=$TARGET --prefix=$MIPSDEV						&& \
		make && make install
	) || die "Error building binutils for target $TARGET"
}

buildmips() {
	cleanbuild
	makedirs
	echo "******* Building MIPS binutils"
	buildbinutils $MIPS_TARGET
	echo "******* Creating symlinks!"
	ln -s $MIPSDEV/bin/$MIPS_TARGET-addr2line		$MIPSDEV/bin/mips-addr2line		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-ar			$MIPSDEV/bin/mips-ar			 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-as			$MIPSDEV/bin/mips-as			 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-c++filt			$MIPSDEV/bin/mips-c++filt		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-gprof			$MIPSDEV/bin/mips-gprof			 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-ld			$MIPSDEV/bin/mips-ld			 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-nm			$MIPSDEV/bin/mips-nm			 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-objcopy			$MIPSDEV/bin/mips-objcopy		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-objdump			$MIPSDEV/bin/mips-objdump		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-ranlib			$MIPSDEV/bin/mips-ranlib		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-readelf			$MIPSDEV/bin/mips-readelf		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-size			$MIPSDEV/bin/mips-size			 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-strings			$MIPSDEV/bin/mips-strings		 >/dev/null
	ln -s $MIPSDEV/bin/$MIPS_TARGET-strip			$MIPSDEV/bin/mips-strip			 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-addr2line.1	$MIPSDEV/man/man1/mips-addr2line.1	 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-ar.1		$MIPSDEV/man/man1/mips-ar.1		 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-as.1		$MIPSDEV/man/man1/mips-as.1		 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-c++filt.1		$MIPSDEV/man/man1/mips-c++filt.1	 >/dev/null
	rm -f $MIPSDEV/man/man1/$MIPS_TARGET-dlltool.1							 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-gprof.1		$MIPSDEV/man/man1/mips-gprof.1		 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-ld.1		$MIPSDEV/man/man1/mips-ld.1		 >/dev/null
	rm -f $MIPSDEV/man/man1/$MIPS_TARGET-nlmconv.1							 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-nm.1		$MIPSDEV/man/man1/mips-nm.1		 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-objcopy.1		$MIPSDEV/man/man1/mips-objcopy.1	 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-objdump.1		$MIPSDEV/man/man1/mips-objdump.1	 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-ranlib.1		$MIPSDEV/man/man1/mips-ranlib.1		 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-readelf.1		$MIPSDEV/man/man1/mips-readelf.1	 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-size.1		$MIPSDEV/man/man1/mips-size.1		 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-strings.1		$MIPSDEV/man/man1/mips-strings.1	 >/dev/null
	ln -s $MIPSDEV/man/man1/$MIPS_TARGET-strip.1		$MIPSDEV/man/man1/mips-strip.1		 >/dev/null
	rm -f $MIPSDEV/man/man1/$MIPS_TARGET-windmc.1							 >/dev/null
	rm -f $MIPSDEV/man/man1/$MIPS_TARGET-windres.1							 >/dev/null
	echo "******* MIPS toolchain built and installed"
}

if [ -z "$MIPSDEV" ]; then
	die "Please set MIPSDEV in your environment."
fi

rm -rf $MIPSDEV/$BINUTILS_DIR
download "$BINUTILS_URI" "$BINUTILS_TARBALL"
extract "$BINUTILS_TARBALL" "$MIPSDEV"
buildmips
rm -rf $MIPSDEV/$BINUTILS_DIR
