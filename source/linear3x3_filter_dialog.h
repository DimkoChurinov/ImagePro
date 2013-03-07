#pragma once

#include <QDialog>
#include "filter_linear.h"
#include "ui_linear3x3filterDialog.h"

class Linear3x3FilterDialog : public QDialog
{
  Q_OBJECT
public:
  explicit Linear3x3FilterDialog(FilterLinear3x3 &filter, QWidget *parent = 0, Qt::WindowFlags f = 0);
  QString GetSelected() const { return m_current; }
private slots:
  void AddFilter();
  void RenameFilter();
  void RemoveFilter();
  void SaveFilter();
  void ItemChanged(QString const &text);
  
  virtual void accept();
private:
  FilterLinear3x3::FilterData ToFilterData() const;
  void Load(FilterLinear3x3::FilterData const&data);
  void UpdateControls();
  bool TextAlreadyExists(QString const&text) const { return (ui.filterComboBox->findText(text) != -1); }
  void TryLoadCurrent();
private:
  FilterLinear3x3 &m_filter;
  Ui::LinearFilterDialog ui;
  QString m_current;
};