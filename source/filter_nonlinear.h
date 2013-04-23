#pragma once

#include <QImage>
#include "filter.h"
#include "qglobal.h"

struct FilterMedian : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  uchar radius;
};

struct FilterOil : Filter {
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  uchar radius;
  unsigned m_intensityLevels;
};


struct FilterKuwahara : Filter 
{
  struct RGBColor
  {
    uchar r;
    uchar g;
    uchar b;
    RGBColor()
      : r(0),g(0), b(0){}
    RGBColor(uchar _r, uchar _g, uchar _b)
      : r(_r),
        g(_g),
        b(_b)
    {

    }
    RGBColor(RGBColor const&other)
      : r(other.r),
        g(other.g),
        b(other.b)
    {

    }
  };
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  RGBColor RGBKuwahara(QImage const& img, int x, int y) const;
  void BestMeanAndVar(QImage const&img, int x0, int y0, int x1, int y1, RGBColor &bestColor, double &bestVariance) const;
  void MeanAndVariance(QImage const&img, int x0, int y0, int x1, int y1, RGBColor &color, double& variance) const;
  inline double getGrayScale(RGBColor const&col) const
  {
    double gs = (col.r * 0.3 + col.g * 0.59 + col.b * 0.11) / 255.0;
    return gs;
  }
  inline RGBColor getColor(QImage const& img, int x, int y) const
  {
    int sx = img.width();
    int sy = img.height();
    if (x < 0) { x += sx; }
    if (x >= sx) { x -= sx; }
    if (y < 0) { y += sy; }
    if (y >= sy) { y -= sy; }
    int bpl = img.bytesPerLine();
    RGBColor col;
    col.r = img.constBits()[y * bpl + (x << 2) + 0];
    col.g = img.constBits()[y * bpl + (x << 2) + 1];
    col.b = img.constBits()[y * bpl + (x << 2) + 2];
    return col;
  }
  inline void setColor(QImage & img, int x, int y, RGBColor const &col) const
  {
    int bpl = img.bytesPerLine();
    img.bits()[y * bpl + (x << 2) + 0] = col.r;
    img.bits()[y * bpl + (x << 2) + 1] = col.g;
    img.bits()[y * bpl + (x << 2) + 2] = col.b;
  }

private:
  uchar radius;
};


struct FilterPerlinNoise : Filter {
  FilterPerlinNoise();
  void Apply(const QImage &imageSrc, QImage &imageDest) const ;
  bool ShowParamsDialog(QWidget *parent);
private:
  int index(int x, int y, int z) const;
  void createPerm();
  void fillTab();
  double mod(double x, double y) const; 

  double noise(double x, double y, double z) const;
  double turbulence( double i, double j, double k) const;
private:
  uchar radius;
  uchar period;
  uchar octaves;
  double persistance;

  static const int size = 256;
  static const int mask = size - 1;
  double tab[size];
  int    perm[size];
};

inline int FilterPerlinNoise::index (int x, int y, int z) const
{
  return perm[(x + perm[(y + perm[z & mask]) & mask]) & mask];
}

inline double FilterPerlinNoise::mod (double x, double y) const
{
  int n = (int)(x / y);
  x -= n * y;
  if (x < 0) {
    x += y;
  }
  return x;
}