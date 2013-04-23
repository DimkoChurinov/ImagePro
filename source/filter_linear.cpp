#include <QFile>
#include <QImage>
#include "linear3x3_filter_dialog.h"
#include "linear5x5_filter_dialog.h"
#include "filter_linear.h"
#include <math.h>

static const char g_FileNameFilters3x3[] = "filters3x3.dat";

bool FilterLinear3x3::Load ()
{
  QFile file(g_FileNameFilters3x3);
  if (!file.open(QFile::ReadOnly)) {
    return true;
  } 
  int count = 0;
  int r = file.read((char *)&count, 4);
  if (r != 4) {
    file.close();
    return false;
  }
  if (count == 0) {
    return true;
  }
  for (int i = 0; i < count; i++) {
    int nameLen;
    int r = file.read((char *)&nameLen, 4);
    if (r != 4) {
      file.close();
      return false;
    }
    char *name = new char[nameLen + 1];
    r = file.read(name, nameLen);
    if (r != nameLen) {
      file.close();
      return false;
    }
    name[nameLen] = 0;
    uchar data[11];
    r = file.read((char *)data, 11);
    if (r != 11) {
      file.close();
      return false;
    }
    m_apertures[name] = FilterData();
    FilterData &filter = m_apertures[name];
    delete name;
    memcpy(filter.m_filterData, data, 11); 
  }
  m_currentFilter = m_apertures.begin().key();
  file.close();
  return true;
}


void FilterLinear3x3::Save () const
{
  QFile file(g_FileNameFilters3x3);
  if (!file.open(QFile::WriteOnly)) {
    return;
  } 
  int count = m_apertures.size();
  file.write((const char *)&count, 4);
  QList<QString> filterKeys = m_apertures.keys();
  foreach(QString str, filterKeys)
  {
    const FilterData &filter = m_apertures[str];
    int len = str.length();
    file.write((const char *)&len, 4);
    file.write((const char *)str.toAscii().data(), len);
    file.write((const char *)filter.m_filterData, 11);
  }
  file.close();
  return;
}


bool FilterLinear3x3::ShowParamsDialog (QWidget *parent)
{
  Linear3x3FilterDialog dlg(*this, parent);
  
  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  QString curr = dlg.GetSelected();
  if(curr.isEmpty())
    return false;
  m_currentFilter = curr;
  return true;
}


void FilterLinear3x3::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const FilterData &filter = m_apertures[m_currentFilter];
  const uchar *src = imageSrc.constBits();
  uchar       *dest = imageDest.bits();


  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      for (int c = 0; c < 3; c++) {
        int accum = 0;
        for (int j = -1; j <= 1; j++) {
          for (int i = -1; i <= 1; i++) {
            int xx = x + i;
            int yy = y + j;
            if (xx < 0) { xx += sx; }
            if (xx >= sx) { xx -= sx; }
            if (yy < 0) { yy += sy; }
            if (yy >= sy) { yy -= sy; }
            accum += src[yy * bpl + (xx << 2) + c] * 
                     filter.m_aperture[(j + 1) * 3 + i + 1];
          }
        }
        accum /= filter.m_divisor;
        accum += filter.m_bias;
        accum = clamp(accum, 0, 255);
        dest[y * bpl + (x << 2) + c] = accum;
      }
    }
  }

}

void FilterLinear3x3::Apply( QString const&name, FilterData const&data )
{
  m_apertures[name] = data;
}

void FilterLinear3x3::Remove( QString const&name )
{
  m_apertures.remove(name);
}

void FilterLinear3x3::Rename( QString const&oldName, QString const&newName )
{
  FilterData data = m_apertures[oldName];
  Remove(oldName);
  Apply(newName, data);
}


static const char g_FileNameFilters5x5[] = "filters5x5.dat";

bool FilterLinear5x5::Load ()
{
  QFile file(g_FileNameFilters5x5);
  if (!file.open(QFile::ReadOnly)) {
    return true;
  } 
  int count = 0;
  int r = file.read((char *)&count, 4);
  if (r != 4) {
    file.close();
    return false;
  }
  if (count == 0) {
    return true;
  }
  for (int i = 0; i < count; i++) {
    int nameLen;
    int r = file.read((char *)&nameLen, 4);
    if (r != 4) {
      file.close();
      return false;
    }
    char *name = new char[nameLen + 1];
    r = file.read(name, nameLen);
    if (r != nameLen) {
      file.close();
      return false;
    }
    name[nameLen] = 0;
    uchar data[27];
    r = file.read((char *)data, 27);
    if (r != 27) {
      file.close();
      return false;
    }
    m_apertures[name] = FilterData();
    FilterData &filter = m_apertures[name];
    delete name;
    memcpy(filter.m_filterData, data, 27); 
  }
  m_currentFilter = m_apertures.begin().key();
  file.close();
  return true;
}


void FilterLinear5x5::Save () const
{
  QFile file(g_FileNameFilters5x5);
  if (!file.open(QFile::WriteOnly)) {
    return;
  } 
  int count = m_apertures.size();
  file.write((const char *)&count, 4);
  QList<QString> filterKeys = m_apertures.keys();
  foreach(QString str, filterKeys)
  {
    const FilterData &filter = m_apertures[str];
    int len = str.length();
    file.write((const char *)&len, 4);
    file.write((const char *)str.toAscii().data(), len);
    file.write((const char *)filter.m_filterData, 27);
  }
  file.close();
  return;
}


bool FilterLinear5x5::ShowParamsDialog (QWidget *parent)
{
  Linear5x5FilterDialog dlg(*this, parent);

  int rc = dlg.exec();
  if (!rc) {
    return false;
  }
  QString curr = dlg.GetSelected();
  if(curr.isEmpty())
    return false;
  m_currentFilter = curr;
  return true;
}


void FilterLinear5x5::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const uchar *src = imageSrc.constBits();
  uchar       *dest = imageDest.bits();
  const FilterData &filter = m_apertures[m_currentFilter];

  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      for (int c = 0; c < 3; c++) {
        int accum = 0;
        for (int j = -2; j <= 2; j++) {
          for (int i = -2; i <= 2; i++) {
            int xx = x + i;
            int yy = y + j;
            if (xx < 0) { xx += sx; }
            if (xx >= sx) { xx -= sx; }
            if (yy < 0) { yy += sy; }
            if (yy >= sy) { yy -= sy; }
            accum += src[yy * bpl + (xx << 2) + c] * 
                     filter.m_aperture[(j + 2) * 5 + i + 2];
          }
        }
        accum /= filter.m_divisor;
        accum += filter.m_bias;
        accum = clamp(accum, 0, 255);
        dest[y * bpl + (x << 2) + c] = accum;
      }
    }
  }

}

void FilterLinear5x5::Apply( QString const&name, FilterData const&data )
{
  m_apertures[name] = data;
}

void FilterLinear5x5::Remove( QString const&name )
{
  m_apertures.remove(name);
}

void FilterLinear5x5::Rename( QString const&oldName, QString const&newName )
{
  FilterData data = m_apertures[oldName];
  Remove(oldName);
  Apply(newName, data);
}

void FilterSobel::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
      sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const uchar *src = imageSrc.constBits();
  uchar       *dest = imageDest.bits();
  int g1[] = {-1, 0, +1, -2, 0, +2, -1, 0, +1};
  int g2[] = {-1, -2, -1, 0, 0, 0, +1, +2, +1};

  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      for (int c = 0; c < 3; c++) {
        int accum1 = 0, accum2 = 0;
        for (int j = -1; j <= 1; j++) {
          for (int i = -1; i <= 1; i++) {
            int xx = x + i;
            int yy = y + j;
            if (xx < 0) { xx += sx; }
            if (xx >= sx) { xx -= sx; }
            if (yy < 0) { yy += sy; }
            if (yy >= sy) { yy -= sy; }
            accum1 += src[yy * bpl + (xx << 2) + c] * 
                      g1[(j + 1) * 3 + i + 1];
            accum2 += src[yy * bpl + (xx << 2) + c] * 
                      g2[(j + 1) * 3 + i + 1];
          }
        }
        int accum = (int)sqrt((double)accum1 * accum1 + accum2 * accum2);
        accum = clamp(accum, 0, 255);
        dest[y * bpl + (x << 2) + c] = accum;
      }
    }
  }
}

void FilterRoberts::Apply (const QImage &imageSrc, QImage &imageDest) const
{
  int sx = imageSrc.width(),
    sy = imageSrc.height();
  int bpl = imageSrc.bytesPerLine();
  const uchar *src = imageSrc.constBits();
  uchar       *dest = imageDest.bits();
  int g1[] = {+1, 0, 0, -1};
  int g2[] = {0, +1, -1, 0};

  for (int y = 0; y < sy; y++) {
    for (int x = 0; x < sx; x++) {
      for (int c = 0; c < 3; c++) {
        int accum1 = 0, accum2 = 0;
        for (int j = 0; j <= 1; j++) {
          for (int i = 0; i <= 1; i++) {
            int xx = x + i;
            int yy = y + j;
            if (xx >= sx) { xx -= sx; }
            if (yy >= sy) { yy -= sy; }
            accum1 += src[yy * bpl + (xx << 2) + c] * 
              g1[(j << 1) + i];
            accum2 += src[yy * bpl + (xx << 2) + c] * 
              g2[(j << 1) + i];
          }
        }
        int accum = (int)sqrt((double)accum1 * accum1 + accum2 * accum2);
        accum = clamp(accum, 0, 255);
        dest[y * bpl + (x << 2) + c] = accum;
      }
    }
  }

}