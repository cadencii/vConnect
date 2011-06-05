/*
 * daemon.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __daemon_h__
#define __daemon_h__

#if !defined( WIN32 )
    #include <fcntl.h>
    #include <unistd.h>
#endif

int daemon( int nochdir, int noclose );

void closeall( int fd );

#endif
