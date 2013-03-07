#pragma once

#include <QDialog>
#include "filter_linear.h"
#include "ui_linear5x5filterDialog.h"

class Linear5x5FilterDialog : public QDialog
{
  Q_OBJECT
public:
  explicit Linear5x5FilterDialog(FilterLinear5x5 &filter, QWidget *parent = 0, Qt::WindowFlags f = 0);
  QString GetSelected() const { return m_current; }
private slots:
  void AddFilter();
  void RenameFilter();
  void RemoveFilter();
  void SaveFilter();
  void ItemChanged(QString const &text);
  
  virtual void accept();
private:
  FilterLinear5x5::FilterData ToFilterData() const;
  void Load(FilterLinear5x5::FilterData const&data);
  void UpdateControls();
  bool TextAlreadyExists(QString const&text) const { return (ui.filterComboBox->findText(text) != -1); }
  void TryLoadCurrent();
private:
  FilterLinear5x5 &m_filter;
  Ui::LinearFilterDialog5x5 ui;
  QString m_current;
};