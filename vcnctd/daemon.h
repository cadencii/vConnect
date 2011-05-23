/*
 * daemon.h
 *
 * Copyright (C) 2011 kbinani.
 */
#ifndef __daemon_h__
#define __daemon_h__

#include <fcntl.h>
#include <unistd.h>

int daemon( int nochdir, int noclose );

void closeall( int fd );

#endif
