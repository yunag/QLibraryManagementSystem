#ifndef CATEGORYSECTION_H
#define CATEGORYSECTION_H

#include <QWidget>

namespace Ui {
class CategorySection;
}

class ErrorMessagePopup;

class CategorySection : public QWidget {
  Q_OBJECT

  enum DataRole { IdRole = Qt::UserRole + 1 };

public:
  explicit CategorySection(QWidget *parent = nullptr);
  ~CategorySection() override;

  void loadCategories();
  void reload();

private:
  void updateLastSync();
  void syncronizeNowButtonClicked();
  void addButtonClicked();
  void deleteButtonClicked();

private:
  Ui::CategorySection *ui;

  ErrorMessagePopup *m_nameErrorMessage;
};

#endif  // CATEGORYSECTION_H
