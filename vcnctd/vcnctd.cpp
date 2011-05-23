/**
 * vcnctd.cpp
 *
 * Copyright © 2011 kbinani.
 */

#include "vcnctd.h"
#include "Config.h"
#include <iostream>

int main( int argc, char *argv[] )
{
    vConnect synth;
    runtimeOptions opt;
    synth.synthesize( "", "", opt );
    
    vcnctd::Config::load();
	cout << "hello, vcnctd" << endl;
    cout << "confPath=" << vcnctd::Config::getConfPath() << endl;
    return 0;
}
