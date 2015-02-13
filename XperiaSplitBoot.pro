#-------------------------------------------------
#
# Project created by QtCreator 2014-12-05T16:01:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH+=src/zlib/inc
INCLUDEPATH+=inc/

TARGET = XperiaSplitBoot
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/splitbootimagedialog.cpp \
    src/makebootimage.cpp \
    src/cpioinflater.cpp \
    src/cpio_entry.cpp \
    src/zlib/src/adler32.c \
    src/zlib/src/compress.c \
    src/zlib/src/crc32.c \
    src/zlib/src/deflate.c \
    src/zlib/src/gzclose.c \
    src/zlib/src/gzlib.c \
    src/zlib/src/gzread.c \
    src/zlib/src/gzwrite.c \
    src/zlib/src/infback.c \
    src/zlib/src/inffast.c \
    src/zlib/src/inflate.c \
    src/zlib/src/inftrees.c \
    src/zlib/src/trees.c \
    src/zlib/src/uncompr.c \
    src/zlib/src/zutil.c \
    src/utils.cpp \
    src/bootimagemanager.cpp \
    src/bootsplashimage.cpp

HEADERS  += inc/mainwindow.h \
    inc/bootimage.h \
    elfio/elf_types.hpp \
    elfio/elfio.hpp \
    elfio/elfio_dump.hpp \
    elfio/elfio_dynamic.hpp \
    elfio/elfio_header.hpp \
    elfio/elfio_note.hpp \
    elfio/elfio_relocation.hpp \
    elfio/elfio_section.hpp \
    elfio/elfio_segment.hpp \
    elfio/elfio_strings.hpp \
    elfio/elfio_symbols.hpp \
    elfio/elfio_utils.hpp \
    inc/splitbootimagedialog.h \
    inc/makebootimage.h \
    inc/cpioinflater.h \
    inc/cpio_entry.h \
    src/zlib/inc/crc32.h \
    src/zlib/inc/deflate.h \
    src/zlib/inc/gzguts.h \
    src/zlib/inc/inffast.h \
    src/zlib/inc/inffixed.h \
    src/zlib/inc/inflate.h \
    src/zlib/inc/inftrees.h \
    src/zlib/inc/trees.h \
    src/zlib/inc/zconf.h \
    src/zlib/inc/zlib.h \
    src/zlib/inc/zutil.h \
    inc/utils.h \
    inc/bootimagemanager.h \
    inc/bootsplashimage.h

FORMS    += forms/mainwindow.ui \
    forms/splitbootimagedialog.ui \
    forms/makebootimage.ui

RESOURCES += \
    Resources.qrc

win32:RC_FILE = default.rc


OTHER_FILES += \
    default.rc
