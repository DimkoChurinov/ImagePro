#include <QColorDialog>
#include "grid_dialog.h"

GridDialog::GridDialog( QWidget *parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
 : QDialog(parent, f)
{
  ui.setupUi(this);
  setModal(true);
  setSizeGripEnabled(false);
  m_currColor = QColor(255 ,255, 255);
  RefreshPalette();
  connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(ColorButtonClicked()));
}

void GridDialog::ColorButtonClicked()
{
  m_currColor = QColorDialog::getColor(m_currColor, this, "Select color");
  RefreshPalette();
}

void GridDialog::RefreshPalette()
{
  QPalette pal = ui.frame->palette();
  pal.setColor(QPalette::Background, m_currColor);
  ui.frame->setPalette(pal);  
}

