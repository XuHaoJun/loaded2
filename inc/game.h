/**
 *  @file       game.h
 *  @brief      The game file's header information.
 *  @author     Yiwei Chiao <ywchiao@gmail.com>
 *  @date       05-03-2015 created.
 *  @date       06-09-2015 last modified.
 *  @version    0.1.0
 *  @setion     License (The MIT License)
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
 *  NONINFRINGEMENT.  IN NO HEARTBEAT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 *
 *  @section DESCRIPTION
 *
 *  The game header file.
 **/

#ifndef UXI_GAME_H
#define UXI_GAME_H

#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct {
  char* name_;

  SDL_Rect rect_;
  SDL_Texture* texture_;
} Sprite;

typedef struct Laser {
  bool body_enable;
  bool exploding;
  int exploding_idx;
  bool visible_;

  int velocity_;

  int idx;

  SDL_Rect box_;

  Sprite* sprite_;

  struct Laser* next_;
} Laser;

typedef struct {
  bool visible_;

  int velocity_;

  int velocitx_;

  SDL_Rect box_;

  Sprite* sprite_;
} Meteor;

typedef struct {
  Laser* lasers_;
  Meteor* meteors_;
} RecycleBin;

typedef struct {
  char* name_;

  int obj_counts_;
  int sprite_counts_;

  SDL_Rect box_;

  Laser* lasers_;
  Meteor* meteors_;
  Sprite* sprite_;
  Sprite** meteor_sprites_;
} Scene;

typedef struct {
  bool alive;
  int health;
  int num_life;
  Uint32 shot_laser_next_time;

  SDL_Point position_;

  SDL_Rect hitbox_[2];

  Sprite* laser_sprites_[11];
  Sprite* damages_[3];
  Sprite* fire_[8];
  Sprite* sprite_;
} Wings;

typedef struct {
  int count_;

  Wings* wings;
} Swarm;

typedef struct {
  void (*init)(void);
  void (*over)(void);
  void (*start)(void);

  Wings* wings;
  Scene* scene;
} Game;

#endif  // UXI_GAME_H

// game.h
