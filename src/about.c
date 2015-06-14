/**
 *  @file       about.c
 *  @brief      Serve the version info. of this project.
 *  @author     Yiwei Chiao <ywchiao@gmail.com>
 *  @date       03/27/2014 created.
 *  @date       04/14/2015 last modified.
 *  @version    0.1.0
 *  @section    License (The MIT License)
 *
 *  Copyright (c) 2014 - 2015, Yiwei Chiao
 *  All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom
 *  the Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 *
 *  @section DESCRIPTION
 *
 *  The about file.
 **/

#include <stdio.h>

#include "about.h"

// 內部函數 (private functions) 的前置宣告 (forward declarations)
static void version_(void);

// 宣告一個公開的 AboutObject 物件，並初始化
About about = {version_};  // about

/**
 *  Display the version info. in the Proj. 0.0.0b0 format.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void version_(void) {
  printf("%s %d.%d.%db%d, %s, %s\n", UXI_STR(PRJ_NAME), VER_MAJOR, VER_MINOR,
         VER_PATCH, VER_BUILD, UXI_STR(VER_BTIME), UXI_STR(VER_BDATE));
}  // version_()

// about.c
