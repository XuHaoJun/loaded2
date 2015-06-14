/**
 *  @file       dice.c
 *  @brief      Defines the dice-related code.
 *  @author     Yiwei Chiao <ywchiao@gmail.com>
 *  @date       04/08/2014 created.
 *  @date       04/21/2015 last modified.
 *  @version    0.1.0
 *  @setion     License (The MIT License)
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
 *  The dice file.
 **/

#include <stdlib.h>

#include "dice.h"

// 內部函數 (private functions) 的前置宣告 (forward declarations)
static uint32_t roll_(uint32_t);

// 內部資料欄位 (private variables) 的宣告 (declarations)

// 公開 (public) 物件的宣告

/**
 *  The global Dice object.
 *
 *  @since  0.1.0
 **/
Dice dice =
{
    roll_
}; // dice

// 函數 (方法) 的實作 (implementations)

/**
 *  The wrapper function for the random().
 *
 *  @param  max the upper bound (ceiling) of the required range.
 *  @return a random number between min and max.
 *  @since  0.1.0
 **/
uint32_t roll_(uint32_t max)
{
    uint32_t points;

    points = (uint32_t)((rand() / ((double)RAND_MAX + 1)) * max);

    return points;
} // roll_()

// dice.c
