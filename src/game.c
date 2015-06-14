/**
 *  @file       game.c
 *  @brief      the game's main structure.
 *  @author     Yiwei Chiao <ywchiao@gmail.com>
 *  @date       05/03/2015 created.
 *  @date       06/09/2015 last modified.
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
 *  The game file of the game.
 **/

#include <stddef.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include <SDL2/SDL_image.h>

#include "dice.h"

#include "game.h"

// 內部函數 (private functions) 的前置宣告 (forward declarations)
static void game_init_(void);
static void game_loop_(void);
static void game_over_(void);
static void game_start_(void);

static void init_sdl_(void);
static Sprite *load_image_(char const *);
static void update_(void);

static void init_meteors_(Scene *);
static void init_meteor_sprites_(Scene *);
static Scene *init_scene_(void);
static Wings *init_wings_(void);

static void laser_destroy_(Laser *);

static void update_lasers_(void);
static void update_meteors_(void);
static void update_scene_(void);
static void update_wings_(void);
static void update_wings_damage_(int);
static void collide_wings_(void);

static void vector_assign_(SDL_Point *, SDL_Point *);
static void vector_neg_(SDL_Point *);
static int vector_dot_(SDL_Point *, SDL_Point *);
static void vector_minus_(SDL_Point *, SDL_Point *, SDL_Point *);

static void gjk_support_(SDL_Rect const *, SDL_Point const *, SDL_Point *);
static bool gjk_simplex_(SDL_Point *, SDL_Point *);
static bool gjk_collides_(SDL_Rect const *, SDL_Rect const *);

// 內部資料欄位 (private data) 宣告
static SDL_Renderer *renderer_ = (SDL_Renderer *)NULL;
static SDL_Window *window_ = (SDL_Window *)NULL;

// 公開 (public) 物件的宣告

/**
 *  The global Game object.
 *
 *  @since  0.1.0
 **/
Game game = {
    game_init_, game_over_, game_start_, (Wings *)NULL, (Scene *)NULL,
};  // game

// 函數 (方法) 的實作 (implementations)

/**
 *  Initialize SDL lib.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void init_sdl_(void) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL Error: %s\n", SDL_GetError());
  }  // fi

  SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
                              &window_,   // 視窗
                              &renderer_  // 渲染器
                              );

  if ((window_ == (SDL_Window *)NULL) || (renderer_ == (SDL_Renderer *)NULL)) {
    printf("SDL Error: %s\n", SDL_GetError());
  }  // fi

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
}  // init_sdl_()

/**
 *  Load image and return a pointer to the Sprite object in which
 *  the image stored.
 *
 *  @param char const * the image file name.
 *  @return Sprite * pointer to the Sprite object.
 *  @since  0.1.0
 **/
Sprite *load_image_(char const *f_name) {
  SDL_Surface *surface = (SDL_Surface *)NULL;
  Sprite *sprite = (Sprite *)malloc(sizeof(Sprite));

  surface = IMG_Load(f_name);

  if (surface == (SDL_Surface *)NULL) {
    printf("SDL Error: %s\n", SDL_GetError());

    exit(-1);
  }  // fi

  sprite->texture_ = SDL_CreateTextureFromSurface(renderer_, surface);

  if (sprite->texture_ == (SDL_Texture *)NULL) {
    printf("SDL Error: %s\n", SDL_GetError());

    exit(-1);
  }  // fi

  sprite->rect_.x = 0;
  sprite->rect_.y = 0;
  sprite->rect_.w = surface->w;
  sprite->rect_.h = surface->h;

  SDL_FreeSurface(surface);

  return sprite;
}  // load_image_()

/**
 *  Update lasers' position.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void update_lasers_(void) {
  Laser *laser = (Laser *)NULL;

  laser = game.scene->lasers_;

  while (laser != (Laser *)NULL) {
    laser->idx = (laser->idx + 1) % 7;
    laser->sprite_ = game.wings->laser_sprites_[laser->idx];

    laser->box_.y -= laser->velocity_;

    if (laser->box_.y < 0) {
      Laser *tmp = laser->next_;

      laser_destroy_(laser);

      laser = tmp;
    }  // fi
    else {
      laser = laser->next_;
    }  // esle
  }    // od
}  // update_lasers_()

/**
 *  Update meteors' position.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void update_meteors_(void) {
  extern Dice dice;

  Meteor *meteors = (Meteor *)NULL;
  Scene *scene = (Scene *)NULL;

  scene = game.scene;

  for (int i = 0; i < scene->obj_counts_; ++i) {
    meteors = scene->meteors_;

    meteors[i].box_.y += meteors[i].velocity_;
    meteors[i].box_.x += meteors[i].velocitx_;

    if (meteors[i].box_.y > scene->box_.h) {
      int tmp = meteors[i].box_.x / 256;

      meteors[i].box_.x = dice.roll(128) + tmp * 256;
      meteors[i].box_.y = 0 - meteors[i].box_.h;
      meteors[i].velocity_ = dice.roll(3) + 1;

      if ((dice.roll(100) % 2) == 0) {
        meteors[i].visible_ = true;
      }  // fi
      else {
        meteors[i].visible_ = false;
      }  // esle

      tmp = dice.roll(scene->sprite_counts_);

      meteors[i].sprite_ = scene->meteor_sprites_[tmp];
    }  // fi
  }    // od
}  // update_meteors_()

/**
 *  Paint the Scene object to the screen.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void update_scene_(void) {
  Laser *laser = (Laser *)NULL;
  Meteor *meteors = (Meteor *)NULL;
  Scene *scene = game.scene;
  SDL_Rect dst;

  // Render the background texture to the screen
  SDL_RenderCopy(renderer_, scene->sprite_->texture_, (SDL_Rect *)NULL,
                 (SDL_Rect *)NULL);

  meteors = scene->meteors_;

  for (int i = 0; i < scene->obj_counts_; ++i) {
    if (meteors[i].visible_) {
      dst.x = meteors[i].box_.x;
      dst.y = meteors[i].box_.y;
      dst.w = meteors[i].box_.w;
      dst.h = meteors[i].box_.h;

      SDL_RenderCopy(renderer_, meteors[i].sprite_->texture_, (SDL_Rect *)NULL,
                     &dst);
    }  // fi
  }    // od

  laser = scene->lasers_;

  while (laser != (Laser *)NULL) {
    dst.x = laser->box_.x;
    dst.y = laser->box_.y;
    dst.w = laser->box_.w;
    dst.h = laser->box_.h;

    SDL_RenderCopy(renderer_, laser->sprite_->texture_, (SDL_Rect *)NULL, &dst);

    laser = laser->next_;
  }  // od
}  // update_scene_()

/**
 *  Paint the Wings object to the screen.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void update_wings_(void) {
  static int frame = 0;

  SDL_Rect dst;

  Wings *wings = game.wings;

  dst.x = wings->position_.x;
  dst.y = wings->position_.y;
  dst.w = wings->sprite_->rect_.w;
  dst.h = wings->sprite_->rect_.h;

  // Render the wings' texture to the screen
  SDL_RenderCopy(renderer_, wings->sprite_->texture_, (SDL_Rect *)NULL, &dst);

  dst.x = wings->position_.x +
          (wings->sprite_->rect_.w - wings->fire_[frame]->rect_.w) / 2;
  dst.y = wings->position_.y + wings->sprite_->rect_.h;
  dst.w = wings->fire_[frame]->rect_.w;
  dst.h = wings->fire_[frame]->rect_.h;

  SDL_RenderCopy(renderer_, wings->fire_[frame]->texture_, (SDL_Rect *)NULL,
                 &dst);

  frame = (frame + 1) % 8;
}  // update_wings_()

/**
 *  Render wings' damage pattern to the sprite based on its
 *  damage level.
 *
 *  @since  0.1.0
 **/
void update_wings_damage_(int level) {
  SDL_Rect dst;

  Wings *wings = game.wings;

  dst.x = wings->position_.x;
  dst.y = wings->position_.y;
  dst.w = wings->sprite_->rect_.w;
  dst.h = wings->sprite_->rect_.h;

  // Render the wings' shatters texture to the screen
  SDL_RenderCopy(renderer_, wings->damages_[level]->texture_, (SDL_Rect *)NULL,
                 &dst);
}  // update_wings_damage_()

/**
 *  Update screen.
 *
 *  @since  0.1.0
 **/
void update_(void) {
  // Clear screen
  SDL_RenderClear(renderer_);

  // update the background 更新背景
  update_scene_();

  // update the wings 更新使用者戰機
  update_wings_();

  if (game.wings->health != 100) {
    update_wings_damage_((100 - game.wings->health) / 30);
  }  // fi

  // Show up
  SDL_RenderPresent(renderer_);
}  // update_()

/**
 *  Assign one vector's content to another.
 *
 *  @since  0.1.0
 **/
void vector_assign_(SDL_Point *p, SDL_Point *q) {
  q->x = p->x;
  q->y = p->y;
}  // vector_assign_()

/**
 *  Negation operation on vector, i.e. v = -v;
 *
 *  @since  0.1.0
 **/
void vector_neg_(SDL_Point *p) {
  p->x = 0 - p->x;
  p->y = 0 - p->y;
}  // vector_neg_()

/**
 *  Caculate and return vectors' dot product.
 *
 *  @since  0.1.0
 **/
int vector_dot_(SDL_Point *p, SDL_Point *q) {
  return ((p->x * q->x) + (p->y * q->y));
}  // vector_dot_()

/**
 *  Do *Minkowski difference* on passed-in vectors.
 *
 *  @since  0.1.0
 **/
void vector_minus_(SDL_Point *p, SDL_Point *q, SDL_Point *r) {
  r->x = p->x - q->x;
  r->y = p->y - q->y;
}  // vector_minus_()

/**
 *  Support function used in gjk-algorithm.
 *
 *  @since  0.1.0
 **/
void gjk_support_(SDL_Rect const *rect, SDL_Point const *vec,
                  SDL_Point *point) {
  point->x = rect->x;
  point->y = rect->y;

  if (vec->x >= 0) {
    point->x += rect->w;
  }  // fi

  if (vec->y >= 0) {
    point->y += rect->h;
  }  // fi
}  // gjk_support_()

/**
 *  Try to find a triangle which contains the origin.
 *
 *  @since  0.1.0
 **/
bool gjk_simplex_(SDL_Point *tri, SDL_Point *d) {
  bool contain_origin = false;

  SDL_Point ao;
  SDL_Point ab;
  SDL_Point ac;

  vector_assign_(&tri[2], &ao);
  vector_neg_(&ao);

  vector_minus_(&tri[1], &tri[2], &ab);

  d->x = 0 - ab.y;
  d->y = ab.x;

  if (vector_dot_(d, &tri[0]) > 0) {
    vector_neg_(d);
  }  // fi

  if (vector_dot_(d, &ao) > 0) {
    vector_assign_(&tri[2], &tri[1]);

    return contain_origin;
  }  // fi

  vector_minus_(&tri[0], &tri[2], &ac);

  d->x = 0 - ac.y;
  d->y = ac.x;

  if (vector_dot_(d, &tri[1]) > 0) {
    vector_neg_(d);
  }  // fi

  if (vector_dot_(d, &ao) > 0) {
    vector_assign_(&tri[2], &tri[0]);

    return contain_origin;
  }  // fi

  contain_origin = true;

  return contain_origin;
}  // gjk_simplex_()

/**
 *  The GJK collision-detection algorithm.
 *
 *  @since  0.1.0
 **/
bool gjk_collides_(SDL_Rect const *rect_a, SDL_Rect const *rect_b) {
  bool collided = false;

  SDL_Point p;
  SDL_Point q;
  SDL_Point d = {1, 0};
  SDL_Point a[3];

  gjk_support_(rect_a, &d, &p);

  d.x = -1;
  gjk_support_(rect_b, &d, &q);

  vector_minus_(&p, &q, &a[0]);

  vector_assign_(&a[0], &d);
  gjk_support_(rect_b, &d, &q);

  vector_neg_(&d);
  gjk_support_(rect_a, &d, &p);

  vector_minus_(&p, &q, &a[1]);

  if (vector_dot_(&d, &a[1]) <= 0) {
    return collided;
  }  // fi

  vector_assign_(&a[1], &d);
  vector_neg_(&d);

  while (true) {
    gjk_support_(rect_a, &d, &p);
    vector_neg_(&d);
    gjk_support_(rect_b, &d, &q);
    vector_neg_(&d);

    vector_minus_(&p, &q, &a[2]);

    if (vector_dot_(&d, &a[2]) <= 0) {
      return collided;
    }  // fi

    if (gjk_simplex_(a, &d)) {
      collided = true;

      break;
    };
  }  // od

  return collided;
}  // gjk_collides_()

void collide_lasers_(void) {
  Scene *scene = (Scene *)NULL;
  Meteor *meteors = (Meteor *)NULL;
  Laser *laser = (Laser *)NULL;

  scene = game.scene;

  for (int i = 0; i < scene->obj_counts_; ++i) {
    meteors = scene->meteors_;
    if (!meteors[i].visible_) {
      continue;
    }  // fi

    laser = game.scene->lasers_;

    while (laser != (Laser *)NULL) {
      if (gjk_collides_(&laser->box_, &meteors[i].box_)) {
        Laser *tmp = laser->next_;

        laser_destroy_(laser);

        laser = tmp;

        meteors[i].visible_ = false;
      }  // fi
      else {
        laser = laser->next_;
      }  // esle
    }    // od
  }
}

/**
 *  Check if wings has been hit by some meteors.
 *
 *  @since  0.1.0
 **/
void collide_wings_(void) {
  Meteor *meteors = (Meteor *)NULL;
  Scene *scene = (Scene *)NULL;
  Wings *wings = (Wings *)NULL;
  SDL_Rect hitbox;

  scene = game.scene;
  wings = game.wings;

  for (int i = 0; i < scene->obj_counts_; ++i) {
    meteors = scene->meteors_;

    if (!meteors[i].visible_) {
      continue;
    }  // fi

    hitbox.x = wings->position_.x + wings->hitbox_[0].x;
    hitbox.y = wings->position_.y + wings->hitbox_[0].y;
    hitbox.w = wings->hitbox_[0].w;
    hitbox.h = wings->hitbox_[0].h;

    if (gjk_collides_(&hitbox, &meteors[i].box_)) {
      meteors[i].visible_ = false;
      wings->health -= 30;

      if (wings->health <= 0) {
        wings->alive = false;
        if (wings->num_life > 0) {
          wings->alive = true;
          wings->health = 100;
          int width;
          int height;
          SDL_GetWindowSize(window_, &width, &height);
          wings->position_.x = ((width - wings->sprite_->rect_.w) / 2);
          wings->position_.y = ((height / 2) + wings->sprite_->rect_.h);
          wings->num_life -= 1;
        }
        break;
      }

      continue;
    }  // fi

    hitbox.x = wings->position_.x + wings->hitbox_[1].x;
    hitbox.y = wings->position_.y + wings->hitbox_[1].y;
    hitbox.w = wings->hitbox_[1].w;
    hitbox.h = wings->hitbox_[1].h;

    if (gjk_collides_(&hitbox, &meteors[i].box_)) {
      meteors[i].visible_ = false;
      wings->health -= 30;
      if (wings->health <= 0) {
        wings->alive = false;
        if (wings->num_life > 0) {
          wings->alive = true;
          wings->health = 100;
          int width;
          int height;
          SDL_GetWindowSize(window_, &width, &height);
          wings->position_.x = ((width - wings->sprite_->rect_.w) / 2);
          wings->position_.y = ((height / 2) + wings->sprite_->rect_.h);
          wings->num_life -= 1;
        }
        break;
      }

      continue;
    }  // fi
  }    // od
}  // collide_wings_()

/**
 *  Initialize a Laser object.
 *
 *  @param Scene * the pointer to the Scene object to which these
 *         meteor belong.
 *  @return none.
 *  @since  0.1.0
 **/
void init_laser_(Scene *scene) {
  Laser *laser;
  Wings *wings;

  laser = (Laser *)malloc(sizeof(Laser));

  laser->next_ = scene->lasers_;
  scene->lasers_ = laser;

  wings = game.wings;
  laser->sprite_ = wings->laser_sprites_[0];
  laser->idx = 0;

  // 設定雷射的位置在飛機的位置
  laser->box_.x = wings->position_.x +
                  ((wings->sprite_->rect_.w - laser->sprite_->rect_.w) / 2);
  laser->box_.y = wings->position_.y - laser->sprite_->rect_.h;
  laser->box_.w = laser->sprite_->rect_.w;
  laser->box_.h = laser->sprite_->rect_.h;

  laser->velocity_ = 5;
  laser->visible_ = true;
}  // init_laser_()

/**
 *  Remove and relase laser object.
 *
 *  @since  0.1.0
 **/
void laser_destroy_(Laser *laser) {
  Laser *laser_c;
  Scene *scene;

  scene = game.scene;

  laser_c = scene->lasers_;

  if (laser_c == laser) {
    scene->lasers_ = laser_c->next_;
  }  // fi
  else {
    while (laser_c->next_ != laser) {
      laser_c = laser_c->next_;
    }  // od

    laser_c->next_ = laser->next_;
  }  // esle

  free(laser);
}  // laser_destroy_()

/**
 *  Initialize the array of meteors.
 *
 *  @param Scene * the pointer to the Scene object to which these
 *         meteor belong.
 *  @return none.
 *  @since  0.1.0
 **/
void init_meteors_(Scene *scene) {
  int rows = 0;
  int cols = 0;

  Meteor *meteors;

  // 將畫面劃分成大小為 256 * 192 的格子
  // 每個格子有一個隕石
  cols = scene->box_.w / 256;
  rows = scene->box_.h / 192;

  scene->obj_counts_ = rows * cols;

  meteors = (Meteor *)malloc(sizeof(Meteor) * scene->obj_counts_);

  for (int i = 0; i < scene->obj_counts_; ++i) {
    extern Dice dice;

    int tmp = 0;

    tmp = dice.roll(scene->sprite_counts_);

    meteors[i].sprite_ = scene->meteor_sprites_[tmp];

    meteors[i].velocity_ = dice.roll(5) + 1;
    meteors[i].velocitx_ = dice.roll(3) + 1;
    if (dice.roll(2) == 0) {
      meteors[i].velocitx_ = -meteors[i].velocitx_;
    }

    // 設定隕石的位置
    meteors[i].box_.x = dice.roll(128) + (i % cols) * 256;
    meteors[i].box_.y = dice.roll(96) + (i / cols) * 192;
    meteors[i].box_.w = meteors[i].sprite_->rect_.w;
    meteors[i].box_.h = meteors[i].sprite_->rect_.h;

    if ((dice.roll(100) % 2) == 1) {
      meteors[i].visible_ = true;
    }  // fi
    else {
      meteors[i].visible_ = false;
    }  // esle
  }    // od;

  scene->meteors_ = meteors;
}  // init_meteors_()

/**
 *  Initialize the array of meteor sprites.
 *
 *  @param Scene * the pointer to the Scene object to which these
 *         meteor sprites belong.
 *  @return none.
 *  @since  0.1.0
 **/
void init_meteor_sprites_(Scene *scene) {
  char *sprite_names[] = {
      "img/meteor_tiny1.png",  "img/meteor_tiny2.png", "img/meteor_small1.png",
      "img/meteor_small2.png", "img/meteor_med1.png",  "img/meteor_med3.png",
      "img/meteor_big1.png",   "img/meteor_big2.png",  "img/meteor_big3.png",
      "img/meteor_big4.png",
  };

  Sprite **sprites = (Sprite **)NULL;

  scene->sprite_counts_ = (sizeof(sprite_names) / sizeof(char *));

  sprites = (Sprite **)malloc(sizeof(Sprite *) * scene->sprite_counts_);

  // 依序載入 meteor 圖檔
  for (int i = 0; i < scene->sprite_counts_; ++i) {
    sprites[i] = load_image_(sprite_names[i]);
  }  // od

  scene->meteor_sprites_ = sprites;
}  // init_meteor_sprites_()

/**
 *  Initialize the Scene object.
 *
 *  @param none.
 *  @return Scene * pointer to the initialzed Scene object.
 *  @since  0.1.0
 **/
Scene *init_scene_(void) {
  int width = 0;
  int height = 0;
  Scene *scene = (Scene *)NULL;

  scene = (Scene *)malloc(sizeof(Scene));
  scene->sprite_ = load_image_("img/darkPurple.png");

  SDL_GetWindowSize(window_, &width, &height);

  scene->box_.w = width;
  scene->box_.h = height;

  // 初始化隕石 (meteor) 的 sprite 物件
  init_meteor_sprites_(scene);

  // 初始化 meteors 物件
  init_meteors_(scene);

  scene->lasers_ = (Laser *)NULL;

  return scene;
}  // init_scene_()

/**
 *  Initialize the Wings object.
 *
 *  @param none.
 *  @return Wings * pointer to the initialzed Wings object.
 *  @since  0.1.0
 **/
Wings *init_wings_(void) {
  char file_png[32];

  int width = 0;
  int height = 0;

  Wings *wings = (Wings *)NULL;

  wings = (Wings *)malloc(sizeof(Wings));
  wings->sprite_ = load_image_("img/ship.png");

  // 設定 Wings 的碎片圖檔
  for (int i = 0; i < 3; ++i) {
    sprintf(file_png, "img/damage%02d.png", i);
    wings->damages_[i] = load_image_(file_png);
  }  // od

  // 設定 Wings 的噴燄圖檔
  for (int i = 0; i < 8; ++i) {
    sprintf(file_png, "img/fire%02d.png", i);
    wings->fire_[i] = load_image_(file_png);
  }  // od

  // 設定 Wings 的雷射圖檔
  for (int i = 1; i < 8; ++i) {
    sprintf(file_png, "img/laserBlue%02d.png", i);
    wings->laser_sprites_[(i - 1)] = load_image_(file_png);
  }  // od

  SDL_GetWindowSize(window_, &width, &height);

  // 設定 Wings 的 hitbox
  wings->hitbox_[0].x = wings->sprite_->rect_.w / 2 - 10;
  wings->hitbox_[0].y = 0;
  wings->hitbox_[0].w = 10;
  wings->hitbox_[0].h = wings->sprite_->rect_.h;

  wings->hitbox_[1].x = 0;
  wings->hitbox_[1].y = wings->sprite_->rect_.h / 2;
  wings->hitbox_[1].w = wings->sprite_->rect_.w;
  wings->hitbox_[1].h = wings->sprite_->rect_.h / 2;

  // 將 Wings 移至畫面中間
  wings->position_.x = ((width - wings->sprite_->rect_.w) / 2);
  wings->position_.y = ((height / 2) + wings->sprite_->rect_.h);

  wings->alive = true;
  wings->health = 100;
  wings->num_life = 3;

  return wings;
}  // init_wings_()

/**
 *  Game initializer.  Initialize the gaming environment.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void game_init_(void) {
  srand(time(NULL));

  init_sdl_();

  // 初始化背景
  game.scene = init_scene_();

  // 初始化戰機
  game.wings = init_wings_();
}  // game_init_()

/**
 *  Ends the game.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void game_over_(void) {
  Wings *wings = (Wings *)game.wings;
  Scene *scene = (Scene *)game.scene;

  for (int i = 0; i < 7; ++i) {
    SDL_DestroyTexture(wings->laser_sprites_[i]->texture_);
    free(wings->laser_sprites_[i]);
  }  // od

  for (int i = 0; i < 8; ++i) {
    SDL_DestroyTexture(wings->fire_[i]->texture_);
    free(wings->fire_[i]);
  }  // od

  SDL_DestroyTexture(wings->sprite_->texture_);
  free(wings->sprite_);
  free(wings);

  free(scene->meteors_);

  for (int i = 0; i < scene->sprite_counts_; ++i) {
    Sprite **sprites = scene->meteor_sprites_;

    SDL_DestroyTexture(sprites[i]->texture_);
    free(sprites[i]);
  }  // od

  free(scene->meteor_sprites_);

  SDL_DestroyTexture(scene->sprite_->texture_);
  free(scene->sprite_);
  free(scene);

  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);

  IMG_Quit();
  SDL_Quit();
}  // game_over_()

/**
 *  Starts the game.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void game_start_(void) { game_loop_(); }  // game_start_()

#define TICK_INTERVAL 40

static Uint32 next_time;

Uint32 time_left(void) {
  Uint32 now;

  now = SDL_GetTicks();
  if (next_time <= now)
    return 0;
  else
    return next_time - now;
}

/**
 *  The main-loop of the game.  Game over when the loop ends.
 *
 *  @param none.
 *  @return none.
 *  @since  0.1.0
 **/
void game_loop_(void) {
  Scene *scene = game.scene;
  Wings *wings = game.wings;

  next_time = SDL_GetTicks() + TICK_INTERVAL;

  bool up = false;
  bool down = false;
  bool left = false;
  bool right = false;
  bool space = false;

  while (wings->alive)  // 程式主迴圈 (game loop)
  {
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT:
          wings->alive = false;

          break;

        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDLK_UP:
              up = false;

              break;

            case SDLK_DOWN:
              down = false;

              break;

            case SDLK_LEFT:
              left = false;

              break;

            case SDLK_RIGHT:
              right = false;

              break;

            case SDLK_SPACE:
              space = false;

              break;

            default:
              break;
          }

          break;

        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_q:
              wings->alive = false;

              break;

            case SDLK_UP:
              up = true;

              break;

            case SDLK_DOWN:
              down = true;

              break;

            case SDLK_LEFT:
              left = true;

              break;

            case SDLK_RIGHT:
              right = true;

              break;

            case SDLK_SPACE:
              space = true;

              break;

            default:
              break;
          }  // esac

          break;

        default:
          break;
      }  // esac
    }    // od

    if (up) {
      wings->position_.y -= 10;
    }
    if (down) {
      wings->position_.y += 10;
    }
    if (left) {
      wings->position_.x -= 10;
    }
    if (right) {
      wings->position_.x += 10;
    }
    if (space) {
      init_laser_(scene);
    }
    update_lasers_();   // 移動 lasers 的位置
    update_meteors_();  // 捲動 meteors 的位置

    collide_lasers_();
    collide_wings_();

    update_();  // 更新畫面

    SDL_Delay(time_left());
    next_time += TICK_INTERVAL;
  }  // od
}  // game_loop_()

// game.c
