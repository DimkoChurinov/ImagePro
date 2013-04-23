#include "filter_nonlinear.h"

#include "ui_median.h"
#include "ui_perlin noise.h"
#include "ui_oil_settings.h"
#include <ctime>
#include <cmath>

bool FilterMedian::ShowParamsDialog (QWidget *parent)
{
  QDialog dlg(parent);
  Ui::MedianDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  radius = ui.spinBoxDispersion->value();
  return true;
}

void FilterMedian::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();


  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      QVector<int> colors[3];
      for (int c = 0; c < 3; c++) {
        colors[c].reserve((2 * radius + 1) * (2 * radius + 1));
      }
      for (int j = -radius; j <= radius; j++) {
        for (int i = -radius; i <= radius; i++) {
          int xx = x + i;
          int yy = y + j;
          if (xx < 0) { xx += sx; }
          if (xx >= sx) { xx -= sx; }
          if (yy < 0) { yy += sy; }
          if (yy >= sy) { yy -= sy; }
          for (int c = 0; c < 3; c++) {
            int cc = imageSrc.constBits()[yy * bpl + (xx << 2) + c];
            colors[c].push_back(cc);
          }
        }
      }
      for (int c = 0; c < 3; c++) {
        qSort(colors[c]);
        imageDest.bits()[y * bpl + (x << 2) + c] = colors[c][colors[c].size() / 2 + 1];
      }
    }
  }

}

bool FilterPerlinNoise::ShowParamsDialog (QWidget *parent)
{
  QDialog dlg(parent);
  Ui::PerlinNoiseDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  radius = ui.spinBoxDispersion->value();
  octaves = ui.octavesSpinBox->value();
  persistance = ui.persistanceSpinBox->value();
  period = ui.periodSpinBox->value();
  return true;
}


void FilterPerlinNoise::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const uchar *src = imageSrc.constBits();
  uchar       *dest = imageDest.bits();

  double kx = (double)qrand() / RAND_MAX;
  double ky = (double)qrand() / RAND_MAX;

  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      int nx = x + radius * (2 * turbulence((double)x / sx * period, (double)y / sy * period, kx) - 1);
      int ny = y + radius * (2 * turbulence((double)y / sy * period, (double)x / sx * period, ky) - 1);
      if (nx < 0) nx = 0;
      if (nx >= sx) nx = sx - 1;
      if (ny < 0) ny = 0;
      if (ny >= sy) ny = sy - 1;
      *(int *)&dest[y * bpl + (x << 2)] = *(int *)&src[ny * bpl + (nx << 2)];
    }
  }
}

void FilterPerlinNoise::createPerm() 
{
  for (int i = 0; i < size; i++) {
    perm[i] = i;
  }

  qsrand(time(0));
  for (int i = 0; i < size; i++) {
    qSwap(perm[qrand() % size], perm[qrand() % size]);
  }
}

void FilterPerlinNoise::fillTab ()
{
  for (int i = 0; i < size; i++) {
    tab[i] = (double)qrand() / RAND_MAX;
  }
}

double FilterPerlinNoise::noise (double x, double y, double z) const
{
  x = mod(x, size);
  y = mod(y, size);
  z = mod(z, size);

  double fx = mod(x, 1);
  double fy = mod(y, 1);
  double fz = mod(z, 1);

  int Xi = (int)x;
  int Yi = (int)y;
  int Zi = (int)z;

  int Xi1 = (Xi + 1) % size;
  int Yi1 = (Yi + 1) % size;
  int Zi1 = (Zi + 1) % size;

  fx = 3 * fx * fx - 2 * fx * fx * fx;
  fy = 3 * fy * fy - 2 * fy * fy * fy;
  fz = 3 * fz * fz - 2 * fz * fz * fz;

  double P = tab[index(Yi, Xi, Zi)] * (1 - fx) + tab[index(Yi, Xi1, Zi)] * fx;
  double Q = tab[index(Yi1, Xi, Zi)] * (1 - fx) + tab[index(Yi1, Xi1, Zi)] * fx;
  double A = P * (1 - fy) + Q * fy;

  double P1 = tab[index(Yi, Xi, Zi1)] * (1 - fx) + tab[index(Yi, Xi1, Zi1)] * fx;
  double Q1 = tab[index(Yi1, Xi, Zi1)] * (1 - fx) + tab[index(Yi1, Xi1, Zi1)] * fx;
  double B = P1 * (1 - fy) + Q1 * fy;

  return (A * (1 - fz) + B * fz);

}

double FilterPerlinNoise::turbulence( double i, double j, double k) const
{
  double col = 0;
  for (int f = 0; f <= octaves; f++) {
    col += noise(i * (1 << f), j * (1 << f), k) * pow(persistance, f);
  }
  return col;
}


FilterPerlinNoise::FilterPerlinNoise ()
{
  createPerm();
  fillTab();
}

void FilterOil::Apply( const QImage &imageSrc, QImage &imageDest ) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();

  QVector<int> colors[3];
  QVector<int> intensityCount(m_intensityLevels, 0);
  for (int c = 0; c < 3; c++) {
    colors[c] = QVector<int>(m_intensityLevels, 0);
  }
  for (int y = 0; y < sy; y++) 
  {
    for (int x = 0; x < sx; x++) 
    {
      for(unsigned k = 0; k < m_intensityLevels; ++k)
      {
        for (unsigned c = 0; c < 3; ++c)
        {
          colors[c][k] = 0;
        }
        intensityCount[k] = 0;
      }
      
      for (int j = -radius; j <= radius; j++) {
        for (int i = -radius; i <= radius; i++) {
          int xx = x + i;
          int yy = y + j;
          if (xx < 0) { xx += sx; }
          if (xx >= sx) { xx -= sx; }
          if (yy < 0) { yy += sy; }
          if (yy >= sy) { yy -= sy; }
          int r = imageSrc.constBits()[yy * bpl + (xx << 2) + 0];
          int g = imageSrc.constBits()[yy * bpl + (xx << 2) + 1];
          int b = imageSrc.constBits()[yy * bpl + (xx << 2) + 2];
          int curIntensity = (int)(((double)(/*(r + g + b)/3*/r * 0.3 + g * 0.59 + b * 0.11) * m_intensityLevels) / 256.0);
          colors[0][curIntensity] += r;
          colors[1][curIntensity] += g;
          colors[2][curIntensity] += b;
          ++intensityCount[curIntensity];
        }
        unsigned curMax = intensityCount[0];
        unsigned maxIdx = 0;
        for(unsigned k = 1; k < m_intensityLevels; ++k)
        {
          if(intensityCount[k] > curMax)
          {
            maxIdx = k;
            curMax = intensityCount[k];
          }
        }
        for (int c = 0; c < 3; c++)
          imageDest.bits()[y * bpl + (x << 2) + c] = colors[c][maxIdx] / curMax;
      }
    }
  }
}

bool FilterOil::ShowParamsDialog( QWidget *parent )
{
  QDialog dlg(parent);
  Ui::OilDialog ui;
  ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  radius = ui.spinBoxDispersion->value();
  m_intensityLevels = ui.intensitySpinBox->value();
  return true;
}



bool FilterKuwahara::ShowParamsDialog( QWidget *parent )
{
  QDialog dlg(parent);
  Ui::MedianDialog ui;//todo  
  ui.setupUi(&dlg);
  ui.spinBoxDispersion->setMaximum(20);
  ui.spinBoxDispersion->setValue(3);
  dlg.setWindowTitle("Kuwahara's filter parameters");
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  radius = ui.spinBoxDispersion->value();
 return true;
}



void FilterKuwahara::MeanAndVariance( QImage const&img, int x0, int y0, int x1, int y1, RGBColor &color, double &variance ) const
{
  double dmin = 1.0;
  double dmax = 0.0;
  int  accumulated_r = 0;
  int  accumulated_g = 0;
  int accumulated_b = 0;
  int count         = 0;

  

  for (int y = y0; y <= y1; ++y)
    for (int x = x0; x <= x1; ++x)
    {
      RGBColor col = getColor(img, x, y);
      double v     = getGrayScale(col);
      accumulated_r   += col.r;
      accumulated_g   += col.g;
      accumulated_b   += col.b;
      ++count;
      dmin = std::min(dmin, v);
      dmax = std::max(dmax, v);
    }
  variance = dmax - dmin;
  color.r = accumulated_r / count;
  color.g = accumulated_g / count;
  color.b = accumulated_b / count;
}


void FilterKuwahara::BestMeanAndVar( QImage const&img, int x0, int y0, int x1, int y1, RGBColor &bestColor, double& bestVariance ) const
{
  RGBColor col;
  double variance;
  MeanAndVariance(img, x0, y0, x1, y1, col, variance);
  if (variance >= bestVariance)
    return;
  bestColor = col;
  bestVariance = variance;
}


FilterKuwahara::RGBColor FilterKuwahara::RGBKuwahara( QImage const& img, int x, int y ) const
{
  RGBColor bestColor;
  double bestVariance = 1.0;

  BestMeanAndVar(img, x - radius, y - radius, x, y, bestColor, bestVariance);
  BestMeanAndVar(img, x, y - radius, x + radius, y, bestColor, bestVariance);
  BestMeanAndVar(img, x, y, x + radius, y + radius, bestColor, bestVariance);
  BestMeanAndVar(img, x - radius, y, x, y + radius, bestColor, bestVariance);
  return bestColor;
}

void FilterKuwahara::Apply( const QImage &imageSrc, QImage &imageDest ) const
{
  int sx = imageSrc.width();
  int sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  for (int y = 0; y < sy; ++y) 
  {
    for (int x = 0; x < sx; ++x) 
    {
      RGBColor c =  RGBKuwahara(imageSrc, x, y);
      setColor(imageDest, x, y, c);
    }
  }
}