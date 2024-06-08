#ifndef AUTHORSECTION_H
#define AUTHORSECTION_H

#include <QTimer>
#include <QWidget>

class QAbstractButton;

class AuthorRestModel;
class AuthorAddDialog;
class AuthorSearchFilterDialog;

namespace Ui {
class AuthorSection;
}

class AuthorSection : public QWidget {
  Q_OBJECT

public:
  explicit AuthorSection(QWidget *parent = nullptr);
  ~AuthorSection() override;

  void loadAuthors();

signals:
  void authorDetailsRequested(quint32 authorId);

private slots:
  void reloadPage();
  void currentChanged(const QModelIndex &current, const QModelIndex &previous);

  void synchronizeNowButtonClicked();
  void searchTextChanged();
  void addButtonClicked();
  void showDetailsButtonClicked();
  void updateButtonClicked();
  void deleteButtonClicked();
  void viewChangeButtonToggled(QAbstractButton *button, bool checked);

private:
  void updateLastSync();
  void setAuthorsCount(int count);

private:
  Ui::AuthorSection *ui;

  QTimer m_loadPageTimer;
  QTimer m_searchTimer;

  AuthorRestModel *m_model;
  AuthorAddDialog *m_authorAddDialog;
  AuthorSearchFilterDialog *m_searchFilterDialog;
};

#endif  // AUTHORSECTION_H
