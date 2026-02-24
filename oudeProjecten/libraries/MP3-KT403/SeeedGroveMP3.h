

#ifndef __SEEED_GROVE_MP3__
#define __SEEED_GROVE_MP3__ 

#include "KT403A_Player.h"
#include "WT2003S_Player.h"

template <class T>
class MP3Player  
{
public:
MP3Player()
{
	controller = new T();
}
 T *controller;
};

#endif
