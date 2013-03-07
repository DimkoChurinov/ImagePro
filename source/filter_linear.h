#pragma once

#include <QMap>
#include "filter.h"

class QFile;
class FilterLinear : public Filter {
public :
  virtual bool Load() = 0;
  virtual void Save() const = 0;
};

class FilterLinear3x3 : public FilterLinear {
public :  
  struct FilterData {
    FilterData(){}
    FilterData(FilterData const&other)
    {
      CopyData(other.m_filterData);
    }
    FilterData& operator=(FilterData const&other)
    {
      CopyData(other.m_filterData);
      return *this;
    }
    union {
      uchar m_filterData[11];
      struct {
        char    m_aperture[9];
        uchar   m_divisor;
        char    m_bias;
      };
    };
  private:
    void CopyData(uchar const*data) { memcpy(m_filterData, data, 11); }
  };
  typedef QMap<QString, FilterData> Apertures;
  FilterLinear3x3() : m_currentFilter("") {Load();}

  virtual bool Load();
  virtual void Save() const;
  void Apply(QString const&name, FilterData const&data);
  void Remove(QString const&name);
  void Rename(QString const&oldName, QString const&newName);
  QString const&GetCurrent() const { return m_currentFilter; }
  QStringList GetNames() const { return QStringList(m_apertures.keys()); }
  FilterData GetData(QString const&name) const 
  {
    return m_apertures[name]; 
  }
  virtual bool ShowParamsDialog(QWidget *parent);

  virtual void Apply(const QImage &imageSrc, QImage &imageDest) const;

private:    
  Apertures m_apertures;
  QString   m_currentFilter;
};

class FilterLinear5x5 : public FilterLinear {
public :  
  struct FilterData {
    FilterData(){}
    FilterData(FilterData const&other)
    {
      CopyData(other.m_filterData);
    }
    FilterData& operator=(FilterData const&other)
    {
      CopyData(other.m_filterData);
      return *this;
    }
    union {
      uchar m_filterData[27];
      struct {
        char    m_aperture[25];
        uchar   m_divisor;
        char    m_bias;
      };
    };
  private:
    void CopyData(uchar const*data) { memcpy(m_filterData, data, 27); }
  };
  typedef QMap<QString, FilterData> Apertures;
  FilterLinear5x5() : m_currentFilter("") {Load();}

  virtual bool Load();
  virtual void Save() const;
  void Apply(QString const&name, FilterData const&data);
  void Remove(QString const&name);
  void Rename(QString const&oldName, QString const&newName);
  QString const&GetCurrent() const { return m_currentFilter; }
  QStringList GetNames() const { return QStringList(m_apertures.keys()); }
  FilterData GetData(QString const&name) const 
  {
    return m_apertures[name]; 
  }
  virtual bool ShowParamsDialog(QWidget *parent);

  virtual void Apply(const QImage &imageSrc, QImage &imageDest) const;

private:    
  Apertures m_apertures;
  QString   m_currentFilter;
};
