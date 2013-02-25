ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libbambamc.la
noinst_PROGRAMS = bamreadertest bamwritertest

libbambamc_la_SOURCES = \
	bambamc/BamBam_BamCollationHash.c		bambamc/BamBam_BamCollatorInterface.c  bambamc/BamBam_FormatAlignment.c \
	bambamc/BamBam_BamCollationHashEntry.c		bambamc/BamBam_BamHeader.c	       bambamc/BamBam_FormatNumber.c \
	bambamc/BamBam_BamCollationOutputVector.c	bambamc/BamBam_BamHeaderInfo.c	       bambamc/BamBam_Hash.c \
	bambamc/BamBam_BamCollationTempFileGenerator.c	bambamc/BamBam_BamWriter.c	       bambamc/BamBam_List.c \
	bambamc/BamBam_BamCollationVector.c		bambamc/BamBam_CharBuffer.c	       bambamc/BamBam_ListNode.c \
	bambamc/BamBam_BamCollator.c			bambamc/BamBam_Chromosome.c \
	bambamc/BamBam_GzipReader.c bambamc/BamBam_LineParsing.c bambamc/BamBam_BamFileHeader.c \
	bambamc/BamBam_BamAlignmentPut.c	bambamc/BamBam_BamSingleAlignment.c \
	bambamc/BamBam_BamFileDecoder.c	bambamc/BamBam_BamSingleAlignment_ToBam1.c
libbambamc_includedir=$(includedir)/bambamc
libbambamc_include_HEADERS= \
	bambamc/BamBam_BamAlignmentPut.h		bambamc/BamBam_BamFlagBase.h		    bambamc/BamBam_FormatAlignment.h \
	bambamc/BamBam_BamCollationHashEntry.h		bambamc/BamBam_BamHeader.h		    bambamc/BamBam_FormatNumber.h \
	bambamc/BamBam_BamCollationHash.h		bambamc/BamBam_BamHeaderInfo.h		    bambamc/BamBam_GzipReader.h \
	bambamc/BamBam_BamCollationOutputVector.h	bambamc/BamBam_BamSingleAlignment.h	    bambamc/BamBam_Hash.h \
	bambamc/BamBam_BamCollationTempFileGenerator.h	bambamc/BamBam_BamSingleAlignment_ToBam1.h  bambamc/BamBam_LineParsing.h \
	bambamc/BamBam_BamCollationVector.h		bambamc/BamBam_BamWriter.h		    bambamc/BamBam_List.h \
	bambamc/BamBam_BamCollator.h			bambamc/BamBam_CharBuffer.h		    bambamc/BamBam_ListNode.h \
	bambamc/BamBam_BamCollatorInterface.h		bambamc/BamBam_Chromosome.h		    bambamc/BamBam_MergeHeapEntry.h \
	bambamc/BamBam_BamFileDecoder.h			bambamc/BamBam_CollatorState.h		    bambamc/BamBam_Unused.h \
	bambamc/BamBam_BamFileHeader.h			bambamc/BamBam_FastQRead.h                  bambamc/BamBam_Config.h
libbambamc_la_CPPFLAGS=${SAMTOOLSCPPFLAGS} ${AM_CPPFLAGS}
libbambamc_la_LDFLAGS=-static -rpath $(libdir)

bamreadertest_SOURCES = test/bamreadertest.c
bamreadertest_CPPFLAGS = ${SAMTOOLSCPPFLAGS} ${AM_CPPFLAGS}
bamreadertest_LDADD = libbambamc.la ${SAMTOOLSLDFLAGS} ${SAMTOOLSLIBS}

bamwritertest_SOURCES = test/bamwritertest.c
bamwritertest_CPPFLAGS = ${SAMTOOLSCPPFLAGS} ${AM_CPPFLAGS}
bamwritertest_LDADD = libbambamc.la ${SAMTOOLSLDFLAGS} ${SAMTOOLSLIBS}
