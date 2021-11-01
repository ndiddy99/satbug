/*
    devcart.c: Saturn devcart client

    Written by Nathan Misner
    I place this file in the public domain.
*/

#include <sega_mth.h>
#include <sega_per.h>
#include "crc.h"
#include "release.h"


#define USB_FLAGS (*(volatile Uint8*)(0x22200001))
#define USB_RXF     (1 << 0)
#define USB_TXE     (1 << 1)
#define USB_FIFO (*(volatile Uint8*)(0x22100001))

enum {
    FUNC_NULL = 0,
    FUNC_DOWNLOAD,
    FUNC_UPLOAD,
    FUNC_EXEC,
    FUNC_PRINT,
    FUNC_QUIT,
    FUNC_CHGDIR
};

static inline Uint8 Devcart_GetByte(void) {
    while ((USB_FLAGS & USB_RXF) != 0);
    return USB_FIFO;
}

static Uint32 Devcart_GetDword(void) {
    Uint32 tmp = Devcart_GetByte();
    tmp = (tmp << 8) | Devcart_GetByte();
    tmp = (tmp << 8) | Devcart_GetByte();
    tmp = (tmp << 8) | Devcart_GetByte();

    return tmp;
}

static inline void Devcart_PutByte(Uint8 byte) {
    while ((USB_FLAGS & USB_TXE) != 0);
    USB_FIFO = byte;
}

int Devcart_LoadFile(char *filename, void *dest) {
    Uint8 *ptr = (Uint8 *)dest;
    Uint8 letter;
    int len;
    crc_t readchecksum;
    crc_t checksum = crc_init();

    //tell server we want to download a file
    Devcart_PutByte(FUNC_DOWNLOAD);
    //tell server the filename we want to download
    for (int i = 0;; i++) {
        letter = (Uint8)filename[i];
        Devcart_PutByte(letter);
        if (letter == '\0') {
            break;
        }
    }

    //pc server will send "upload" command back, read that byte
    Devcart_GetByte();
    //pc server sends address first, this is unnecessary since we're
    //specifying it
    Devcart_GetDword();
    len = (int)Devcart_GetDword(); //file length

    for (int i = 0; i < len; i++) {
        // inlining is 20K/s faster
        while ((USB_FLAGS & USB_RXF) != 0);
        ptr[i] = USB_FIFO;
    }

    readchecksum = Devcart_GetByte();

    checksum = crc_update(checksum, ptr, len);
    checksum = crc_finalize(checksum);

    if (checksum != readchecksum) {
        Devcart_PutByte(0x1);
    }
    else {
        Devcart_PutByte(0);
    }

    return (int)len;
}

void Devcart_PrintStr(char *string) {
    Devcart_PutByte(FUNC_PRINT);
    for (int i = 0;; i++) {
        Devcart_PutByte(string[i]);
        if (string[i] == '\0') {
            break;
        }
    }
}

void Devcart_Reset() {
    // quit server program
    Devcart_PutByte(FUNC_QUIT);
    // reset saturn
    PER_SMPC_SYS_RES();
}

void Devcart_ChangeDir(char *dir) {
    Devcart_PutByte(FUNC_CHGDIR);
    for (int i = 0;; i++) {
        Devcart_PutByte(dir[i]);
        if (dir[i] == '\0') {
            break;
        }
    }
}

