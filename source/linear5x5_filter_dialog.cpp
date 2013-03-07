#include <QMessageBox>
#include "linear5x5_filter_dialog.h"
#include "ui_ChooseNameDialog.h"
unsigned const c_rowCount = 5;

namespace 
{
  QSpinBox* CreateValSpinBox(QWidget *parent)
  {
    QSpinBox *newSpBox = new QSpinBox(parent);
    newSpBox->setMaximum(127);
    newSpBox->setMinimum(-128);
    newSpBox->setSingleStep(1);
    newSpBox->setValue(0);
    return newSpBox;
  }
}

Linear5x5FilterDialog::Linear5x5FilterDialog( FilterLinear5x5 &filter, QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
:QDialog(parent, f),
  m_filter(filter)
{
  ui.setupUi(this);
  setModal(true);
  setSizeGripEnabled(false);
  
  for(unsigned i = 0; i < c_rowCount * c_rowCount; ++i)
    ui.tableWidget->setCellWidget(i / c_rowCount, i % c_rowCount, CreateValSpinBox(ui.tableWidget));
  
  ui.filterComboBox->addItems(filter.GetNames());
  if (ui.filterComboBox->count())
  {
    ui.filterComboBox->setCurrentIndex(ui.filterComboBox->findText(filter.GetCurrent()));
    Load(m_filter.GetData(filter.GetCurrent()));
  }  
  UpdateControls();
    
  
  connect(ui.AddFilterButton, SIGNAL(clicked()), this, SLOT(AddFilter()));
  connect(ui.RenameFilterButton, SIGNAL(clicked()), this, SLOT(RenameFilter()));
  connect(ui.RemoveFilterButton, SIGNAL(clicked()), this, SLOT(RemoveFilter()));
  connect(ui.SaveFilterButton, SIGNAL(clicked()), this, SLOT(SaveFilter())); 
  connect(ui.filterComboBox, SIGNAL(currentIndexChanged(QString const&)), this, SLOT(ItemChanged(QString const&)));
}

void Linear5x5FilterDialog::AddFilter()
{
  QDialog dlg(this);
  Ui::ChooseNameDialog _ui;
  _ui.setupUi(&dlg);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) {
    return;
  }
  
  QString name = _ui.lineEdit->text();
  if(name.isEmpty())
  {
    QMessageBox::critical(this, "Error!", "Please, enter non-empty name!");
    return;
  }
  if(TextAlreadyExists(name))
  {
    QMessageBox::critical(this, "Error!", QString("Filter \"%1\" already exists!").arg(name));
    return;
  }

  m_filter.Apply(name, ToFilterData());
  m_filter.Save();
  ui.filterComboBox->addItem(name);
  ui.filterComboBox->setCurrentIndex(ui.filterComboBox->count() - 1);  
  UpdateControls();
}

void Linear5x5FilterDialog::RenameFilter()
{
  QString oldName = ui.filterComboBox->currentText();
  QDialog dlg(this);
  Ui::ChooseNameDialog _ui;
  _ui.setupUi(&dlg);
  _ui.lineEdit->setText(oldName);
  dlg.setModal(true);
  dlg.setSizeGripEnabled(false);
  int rc = dlg.exec();
  if (!rc) 
  {
    return;
  }  
  QString name = _ui.lineEdit->text();
  if(oldName == name)
    return;
  if(name.isEmpty())
  {
    QMessageBox::critical(this, "Error!", "Please, enter non-empty name!");
    return;
  }
  if(TextAlreadyExists(name))
  {
    QMessageBox::critical(this, "Error!", QString("Filter \"%1\" already exists!").arg(name));
    //RemoveFilter();
    return;
  }
  m_filter.Rename(oldName, name);
  m_filter.Save();
  ui.filterComboBox->removeItem(ui.filterComboBox->currentIndex());
  ui.filterComboBox->addItem(name);
  ui.filterComboBox->setCurrentIndex(ui.filterComboBox->count() - 1);    
}

void Linear5x5FilterDialog::RemoveFilter()
{
  QString oldName = ui.filterComboBox->currentText();
  m_filter.Remove(oldName);
  m_filter.Save();
  ui.filterComboBox->removeItem(ui.filterComboBox->currentIndex());
  TryLoadCurrent();
  UpdateControls();
}

void Linear5x5FilterDialog::SaveFilter()
{
  m_filter.Apply(ui.filterComboBox->currentText(), ToFilterData());
  m_filter.Save();
}

FilterLinear5x5::FilterData Linear5x5FilterDialog::ToFilterData() const
{
  FilterLinear5x5::FilterData newData;
  for(unsigned i = 0; i < c_rowCount * c_rowCount; ++i)
    newData.m_aperture[i] = ((QSpinBox*)ui.tableWidget->cellWidget(i / c_rowCount, i % c_rowCount))->value();
  newData.m_bias = ui.biasSpinBox->value();
  newData.m_divisor = ui.divisorSpinBox->value();
  return newData;
}

void Linear5x5FilterDialog::Load( FilterLinear5x5::FilterData const&data )
{
  for(unsigned i = 0; i < c_rowCount * c_rowCount; ++i)
    ((QSpinBox*)ui.tableWidget->cellWidget(i / c_rowCount, i % c_rowCount))->setValue(data.m_aperture[i]);
  ui.biasSpinBox->setValue(data.m_bias);
  ui.divisorSpinBox->setValue(data.m_divisor);
}

void Linear5x5FilterDialog::UpdateControls()
{
  ui.RemoveFilterButton->setEnabled(ui.filterComboBox->count() != 0);
  ui.RenameFilterButton->setEnabled(ui.filterComboBox->count() != 0);
  ui.SaveFilterButton->setEnabled(ui.filterComboBox->count() != 0);
  QList<QAbstractButton *> butns = ui.buttonBox->buttons();
  foreach(QAbstractButton *btn, butns)
  {
    if(ui.buttonBox->buttonRole(btn) == QDialogButtonBox::AcceptRole)//disable ok button
    {
      btn->setEnabled(ui.filterComboBox->count() != 0);
      break;
    }
  }
}

void Linear5x5FilterDialog::TryLoadCurrent()
{
  if (!ui.filterComboBox->count())
    return;  
  ui.filterComboBox->setCurrentIndex(0);
  Load(m_filter.GetData(ui.filterComboBox->currentText()));  
}

void Linear5x5FilterDialog::accept()
{  
  SaveFilter();
  if(!ui.filterComboBox->count())
  {
    m_current = "";
  }
  else
  {
    m_current = ui.filterComboBox->currentText();
  }
  QDialog::accept();
}

void Linear5x5FilterDialog::ItemChanged(QString const&text)
{
  if(!ui.filterComboBox->count())
    return;
  Load(m_filter.GetData(text));
}