#include "booksearchfilterdialog.h"
#include "ui_booksearchfilterdialog.h"

#include "model/bookrestmodel.h"

BookSearchFilterDialog::BookSearchFilterDialog(BookRestModel *model,
                                               QWidget *parent)
    : QDialog(parent), ui(new Ui::BookSearchFilterDialog), m_model(model) {

  ui->setupUi(this);

  ui->sortByComboBox->addItem("id", "id");
  ui->sortByComboBox->addItem("title", "title");
  ui->sortByComboBox->addItem("rating", "rating");

  QList<KRatingWidget *> ratingWidgets = {ui->rating0, ui->rating1,
                                          ui->rating2, ui->rating3,
                                          ui->rating4, ui->rating5};
  int currentRating = 0;
  for (KRatingWidget *rating : ratingWidgets) {
    rating->setIcon(QIcon(":/images/starRating.svg"));
    rating->setRating(currentRating);
    rating->setMouseTracking(false);
    rating->setAttribute(Qt::WA_TransparentForMouseEvents);
    currentRating += 2;
  }

  ui->publicationDateStart->setMinimumDate({600, 1, 1});
  ui->publicationDateEnd->setMinimumDate({600, 1, 1});
  connect(ui->publicationDateStart, &QDateEdit::dateChanged, this,
          [this](QDate /*date*/) {
    QDate dateInRange =
      qMax(ui->publicationDateStart->date(), ui->publicationDateEnd->date());
    ui->publicationDateEnd->setDate(dateInRange);
  });
  connect(ui->publicationDateEnd, &QDateEdit::dateChanged, this,
          [this](QDate /*date*/) {
    QDate dateInRange =
      qMin(ui->publicationDateStart->date(), ui->publicationDateEnd->date());
    ui->publicationDateStart->setDate(dateInRange);
  });
  connect(ui->publicationDateStartCheckBox, &QCheckBox::toggled,
          ui->publicationDateStart, &QDateEdit::setEnabled);
  connect(ui->publicationDateEndCheckBox, &QCheckBox::toggled,
          ui->publicationDateEnd, &QDateEdit::setEnabled);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &BookSearchFilterDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &BookSearchFilterDialog::reject);
}

BookSearchFilterDialog::~BookSearchFilterDialog() {
  delete ui;
}

void BookSearchFilterDialog::accept() {
  if (ui->publicationDateStart->date() > ui->publicationDateEnd->date()) {
    return;
  }

  QString field = ui->sortByComboBox->currentData().toString();
  QString orderBy = field;

  if (ui->ascRadioButton->isChecked()) {
    orderBy += "-asc";
  }

  QVariantMap filters = m_model->filters();
  if (ui->publicationDateStartCheckBox->isChecked()) {
    filters["publicationdatestart"] =
      ui->publicationDateStart->date().toString(Qt::ISODate);
  } else {
    filters.remove("publicationdatestart");
  }

  if (ui->publicationDateEndCheckBox->isChecked()) {
    filters["publicationdateend"] =
      ui->publicationDateEnd->date().toString(Qt::ISODate);
  } else {
    filters.remove("publicationdateend");
  }

  QVariantList ratings;
  if (ui->rating0_checkBox->isChecked()) {
    ratings.append({0, 1});
  }
  if (ui->rating1_checkBox->isChecked()) {
    ratings.append({2, 3});
  }
  if (ui->rating2_checkBox->isChecked()) {
    ratings.append({4, 5});
  }
  if (ui->rating3_checkBox->isChecked()) {
    ratings.append({6, 7});
  }
  if (ui->rating4_checkBox->isChecked()) {
    ratings.append({8, 9});
  }
  if (ui->rating5_checkBox->isChecked()) {
    ratings.append(10);
  }
  filters["ratings"] = ratings;

  m_model->setFilters(filters);
  m_model->setOrderBy(orderBy);

  QDialog::accept();
}
