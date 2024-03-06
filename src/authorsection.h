#ifndef AUTHORSECTION_H
#define AUTHORSECTION_H

#include <QWidget>

namespace Ui {
class AuthorSection;
}

class AuthorSection : public QWidget {
  Q_OBJECT

public:
  explicit AuthorSection(QWidget *parent = nullptr);
  ~AuthorSection();

private:
  Ui::AuthorSection *ui;
};

#endif  // AUTHORSECTION_H
