#include "searchfilterdialog.h"
#include "forms/ui_searchfilterdialog.h"

#include "booksectiondao.h"

SearchFilterDialog::SearchFilterDialog(BookSectionDAO *dao, QWidget *parent)
    : QDialog(parent), ui(new Ui::SearchFilterDialog) {
  ui->setupUi(this);

  m_dao = dao;

  struct {
    QString sortBy;
    BookSectionDAO::Column column;
  } sortByItems[] = {
    {"id", BookSectionDAO::Id},
    {"title", BookSectionDAO::Title},
    {"rating", BookSectionDAO::Rating},
  };

  for (auto item : sortByItems) {
    ui->sortByComboBox->addItem(item.sortBy, item.column);
  }

  ui->sortByComboBox->setCurrentIndex(0);

  connect(ui->sortByComboBox, &QComboBox::currentIndexChanged, this,
          &SearchFilterDialog::indexChanged);

  connect(ui->ascRadioButton, &QRadioButton::toggled, this,
          [this](bool checked) {
            m_order = checked ? Qt::AscendingOrder : Qt::DescendingOrder;
          });

  ui->ascRadioButton->toggle();
}

SearchFilterDialog::~SearchFilterDialog() {
  delete ui;
}

void SearchFilterDialog::indexChanged(int index) {
  m_column =
    ui->sortByComboBox->itemData(index).value<BookSectionDAO::Column>();
}

void SearchFilterDialog::accept() {
  m_dao->orderBy(m_column, m_order);

  QDialog::accept();
}
