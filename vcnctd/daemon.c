/*
 * daemon.c
 *
 * Copyright (C) 2011 kbinani.
 */
#include "daemon.h"

int daemon( int nochdir, int noclose )
{
    switch( fork() )
    {
        case 0:{
            break;
        }
        case  -1:{
            return -1;
        }
        default:{
            _exit( 0 );
        }
    }
    
    if( setsid() < 0 )
    {
        return -1;
    }
    
    switch( fork() )
    {
        case 0:{
            break;
        }
        case -1:{
            return -1;
        }
        default:{
            _exit( 0 );
        }
    }
    
    if( !nochdir ){
        chdir( "/" );
    }
    
    if( !noclose ){
        closeall( 0 );
        open( "/dev/null", O_RDWR );
        dup( 0 );
        dup( 0 );
    }
    return 0;
}

void closeall( int fd )
{
    int fdlimit = sysconf( _SC_OPEN_MAX );
    
    while( fd < fdlimit ){
        close( fd++ );
    }
}

