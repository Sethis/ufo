#ifndef CITYITEM_H
#define CITYITEM_H

#include <SDL/SDL.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include "gfx.h"
#include "citymap.h"

class ShaderProgram;

class CityItem
{
public:
    CityItem()
    {
      tz = 8.0f;
      tx = 0.1f * (rand() % 1000);
      ty = 0.1f * (rand() % 1000);
      frame = 0.0f;
	  start_frame = end_frame = 0;
      anim_speed = 0.7f;
      is_garbage = false;
    }

    /* Called in the beginnig on every frame */
    virtual void update()
    {
      frame += anim_speed;
      if ((int)frame > end_frame)
        frame = (float)start_frame;
    }

    virtual void renderAt(Rect & pos, const ShaderProgram & sp, float z)
    {
      if (!is_garbage)
      {
        Sprite * img = images->getSprite((size_t)frame);
        if (img)
          img->renderAt(pos,sp, z);
      }
    }

    virtual bool garbage() { return is_garbage; }

    SpritePack * images;
    int start_frame, end_frame;
    float frame, anim_speed;
    float tx, ty, tz;
    bool is_garbage;
};

class SingleShotItem : public CityItem
{
public:
  SingleShotItem() : CityItem()
  {

  }

  /* Called in the beginnig on every frame */
  virtual void update()
  {
    frame += anim_speed;
    if ((int)frame > end_frame)
      is_garbage = true;
  }
};

class MovableCityItem : public CityItem
{
public:
  MovableCityItem() : CityItem()
  {
    dx = tx; dy = ty; dz = tz;
    speed = 0.1f;
  }

  virtual void update()
  {
    CityItem::update();

    //immovable
    if (speed < 0.001)
      return;
 //TODO: rewrite this crap
    if ( fabs(tx - dx) > speed || fabs(ty - dy) > speed || fabs(tz - dz) > speed)
    {
        if (fabs(tx - dx) > speed)
          tx += speed*((dx - tx) / fabs((dx - tx)));
        if (fabs(ty - dy) > speed)
          ty += speed*((dy - ty) / fabs((dy - ty)));
        if (fabs(tz - dz) > speed)
          tz += speed*((dz - tz) / fabs((dz - tz)));
    }
    else
      arrived();

    if (tx > 100) tx = 100;
    if (tx < 0) tx = 0;
    if (ty > 100) ty = 100;
    if (ty < 0) ty = 0;
    if (tz > 9) tz = 9;
    if (tz < 0) tz = 0;
  }

  virtual void arrived()
  {
    dx = 0.1f * (rand() % 1000);
    dy = 0.1f * (rand() % 1000);
  }

  float dx, dy, dz;
  float speed;
};


class SingleShotMovableItem : public MovableCityItem
{
public:
  SingleShotMovableItem() : MovableCityItem()
  {

  }

  /* Called in the beginnig on every frame */
  virtual void arrived()
  {
    is_garbage = true;
  }
};

class PewPewItem : public SingleShotMovableItem
{
public:
  PewPewItem(const CityMap & c) : SingleShotMovableItem(), city(c)
  {
    hit = false;

  }

  virtual void update()
  {
    if (!hit)
    {
      SingleShotMovableItem::update();
      if (city.getTile((int)tx, (int)ty, (int)tz))
      {
        hit = true;
        is_garbage = true;
      }
    }
  }

  bool is_hit() const { return hit; }

private:
  const CityMap & city;
  bool hit;
};

class DimensionGate : public MovableCityItem
{
public:
  DimensionGate() : MovableCityItem()
  {
    arrived();
    speed=0.03f;
    anim_speed = 0.3f;
    std::cout << "Generated gate item at " << tx << "x" << ty <<
                 " Destination to " << dx << "x" << dy <<std::endl;
  }
  virtual void update()
  {
    MovableCityItem::update();
  }
};

class Ufo : public MovableCityItem
{
public:
  Ufo(CityItem * source,CityItem * dest) : MovableCityItem(), g_source(source), g_dest(dest)
  {
    tz = source->tz +1;
    tx = source->tx;
    ty = source->ty;
    speed = 0.05f;
    anim_speed = 0.15f;
    arrived();
    std::cout << "Generated ufo at" << tx << "x" << ty << std::endl;
  }

  void update()
  {
    dx = g_dest->tx;
    dy = g_dest->ty;

    MovableCityItem::update();
  }

  void arrived()
  {
      CityItem * d = g_source;
      g_source = g_dest;
      g_dest = d;
  }
  CityItem * g_source, *g_dest;
};

#endif // CITYITEM_H
