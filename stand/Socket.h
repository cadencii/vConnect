/**
 * Socket.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __Socket_h__
#define __Socket_h__

#ifdef WIN32
#include <io.h> // for 'read' system call
#else
#include <unistd.h>
#endif

typedef int Socket;

#endif
