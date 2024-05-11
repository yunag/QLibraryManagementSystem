#include "searchfilterdialog.h"
#include "ui_searchfilterdialog.h"

SearchFilterDialog::SearchFilterDialog(BookSectionDao *dao, QWidget *parent)
    : QDialog(parent), ui(new Ui::SearchFilterDialog), m_dao(dao) {
  ui->setupUi(this);

  QMap<QString, BookSectionDao::Property> items;
  items["id"] = BookSectionDao::IdProperty;
  items["title"] = BookSectionDao::TitleProperty;
  items["rating"] = BookSectionDao::RatingProperty;

  for (const auto &[name, property] : items.asKeyValueRange()) {
    ui->sortByComboBox->addItem(name, property);
  }

  connect(ui->sortByComboBox, &QComboBox::currentIndexChanged, this,
          &SearchFilterDialog::indexChanged);

  connect(ui->ascRadioButton, &QRadioButton::toggled, this,
          [this](bool checked) {
            m_order = checked ? Qt::AscendingOrder : Qt::DescendingOrder;
          });
}

SearchFilterDialog::~SearchFilterDialog() {
  delete ui;
}

void SearchFilterDialog::indexChanged(int index) {
  m_column =
    ui->sortByComboBox->itemData(index).value<BookSectionDao::Property>();
}

void SearchFilterDialog::accept() {
  m_dao->orderBy(m_column, m_order);

  QDialog::accept();
}

void SearchFilterDialog::open() {
  for (int i = 0; i < ui->sortByComboBox->count(); ++i) {
    auto comboProp =
      ui->sortByComboBox->itemData(i).value<BookSectionDao::Property>();
    if (comboProp == m_dao->orderProperty()) {
      ui->sortByComboBox->setCurrentIndex(i);
      break;
    }
  }

  if (m_dao->order() == Qt::AscendingOrder) {
    ui->ascRadioButton->toggle();
  } else {
    ui->descRadioButton->toggle();
  }

  QDialog::open();
}
