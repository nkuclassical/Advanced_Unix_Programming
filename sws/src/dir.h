/*
  dir.h
  finalxcode

  Created by Yanqiao Zhan on 11/28/15.
  Copyright © 2015 Yanqiao Zhan. All rights reserved.
*/

#ifndef dir_h
#define dir_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <string.h>
#include "define.h"
#include "util.h"

/* int direxe(NetRequest*); */
int direxe(NetRequest*netRequest,NetArg*serverArg,char*reqLine);
#endif /* dir_h */
