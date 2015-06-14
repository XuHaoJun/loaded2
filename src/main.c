/**
 *  @file       main.c
 *  @brief      the entry point of worm.
 *  @author     Yiwei Chiao <ywchiao@gmail.com>
 *  @date       03/09/2015 created.
 *  @date       04/19/2015 last modified.
 *  @version    0.1.0
 *  @section    License (The MIT License)
 *
 *  Copyright (c) 2015, Yiwei Chiao
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
 *  The main file of the main.
 **/

//#include "about.h"
#include "game.h"

#include "main.h"

int main(int argc, char *argv[]) {
  //    extern About about;
  extern Game game;

  game.init();  // 初始化環境

  game.start();  // 遊戲開始

  game.over();  // 遊戲結束

  //    about.version();             // 顯示程式版本資訊

  return 0;
}  // main()

// main.c
