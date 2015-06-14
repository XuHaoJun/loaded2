/**
 *  @file       dice.h
 *  @brief      The dice file's header information.
 *  @author     Yiwei Chiao <ywchiao@gmail.com>
 *  @date       04-08-2014 created.
 *  @date       04-21-2015 last modified.
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
 *  The dice header file.
 **/

#ifndef UXI_DICE_H
#define UXI_DICE_H

#include <stdint.h>

typedef struct { uint32_t (*roll)(uint32_t); } Dice;

#endif  // UXI_DICE_H

// dice.h
