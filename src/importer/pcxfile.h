#pragma once

#include <iostream>
#include "importer/palette.h"
#include "importer/bitmap.h"

/** Loader for PCX files */
class cPCXFile
{
public:
  /** Loads file from stream. Warning: will play with whole stream starting at current position. */
  bool loadFrom(std::istream & file);

  const c8bppBitmap & bitmap() const { return m_Bitmap; }
  const cPalette & palette() const { return m_Palette; }

private:
  struct SPCXHeader
  {
    char m_Manufacturer;
    char m_Version;
    char m_Encoding;
    char m_Bpp;
    short m_StartX;
    short m_StartY;
    short m_EndX;
    short m_EndY;
    short m_DpiX;
    short m_DpiY;
    char m_Palette[48];
    char m_Reserved;
    char m_Planes;
    short m_BPLine;
    short m_PaletteInfo;
    short m_HScreen;
    short m_VScreen;
    char m_Unused[54];
  };
  SPCXHeader		m_Header;
  cPalette		m_Palette;
  c8bppBitmap m_Bitmap;
};
