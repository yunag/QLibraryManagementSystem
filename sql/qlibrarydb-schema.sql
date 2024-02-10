DROP SCHEMA IF EXISTS qlibrarydb;


CREATE SCHEMA qlibrarydb;


USE qlibrarydb;


--
-- Table structure for table `author`
--
CREATE TABLE IF NOT EXISTS author (
  author_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  first_name VARCHAR(45) NOT NULL,
  last_name VARCHAR(45) NOT NULL,
  PRIMARY KEY (author_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `category`
--
CREATE TABLE IF NOT EXISTS category (
  category_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL,
  PRIMARY KEY (category_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `book`
--
CREATE TABLE IF NOT EXISTS book (
  book_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  title VARCHAR(255) NOT NULL,
  cover_path VARCHAR(255),
  publication_date DATE,
  copies_owned SMALLINT UNSIGNED,
  PRIMARY KEY (book_id),
  KEY idx_title (title)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `book_author`
--
CREATE TABLE IF NOT EXISTS book_author (
  book_id INT UNSIGNED NOT NULL,
  author_id INT UNSIGNED NOT NULL,
  PRIMARY KEY (book_id, author_id),
  CONSTRAINT fk_book_author_book FOREIGN KEY (book_id) REFERENCES book (book_id) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT fk_book_author_author FOREIGN KEY (author_id) REFERENCES author (author_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `book_category`
--
CREATE TABLE IF NOT EXISTS book_category (
  book_id INT UNSIGNED NOT NULL,
  category_id INT UNSIGNED NOT NULL,
  PRIMARY KEY (book_id, category_id),
  CONSTRAINT fk_book_category_book FOREIGN KEY (book_id) REFERENCES book (book_id) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT fk_book_category_category FOREIGN KEY (category_id) REFERENCES category (category_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `user`
--
CREATE TABLE IF NOT EXISTS user (
  user_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  username VARCHAR(128) NOT NULL,
  password VARCHAR(255) NOT NULL,
  salt VARCHAR(64) NOT NULL,
  joined_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  active BOOLEAN NOT NULL DEFAULT TRUE,
  PRIMARY KEY (user_id)
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `loan`
--
CREATE TABLE IF NOT EXISTS loan (
  loan_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  book_id INT UNSIGNED NOT NULL,
  user_id INT UNSIGNED NOT NULL,
  loan_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  returned_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (loan_id),
  CONSTRAINT fk_loan_user FOREIGN KEY (user_id) REFERENCES user (user_id) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT fk_loan_book FOREIGN KEY (book_id) REFERENCES book (book_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- Table structure for table `fine`
--
CREATE TABLE IF NOT EXISTS fine (
  fine_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  loan_id INT UNSIGNED NOT NULL,
  user_id INT UNSIGNED NOT NULL,
  fine_amount DECIMAL(5, 2) NOT NULL,
  fine_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (fine_id),
  CONSTRAINT fk_fine_user FOREIGN KEY (user_id) REFERENCES user (user_id) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT fk_fine_loan FOREIGN KEY (loan_id) REFERENCES loan (loan_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4;


--
-- View structure for view `book_author_vw`
--
CREATE VIEW book_author_vw (
  book_id,
  book_title,
  categories,
  authors,
  cover_path
) AS
SELECT
  b.book_id,
  b.title,
  GROUP_CONCAT(c.name SEPARATOR ', ') AS categories,
  GROUP_CONCAT(
    DISTINCT CONCAT(a.first_name, ' ', a.last_name) SEPARATOR ', '
  ) AS authors,
  b.cover_path
FROM
  book AS b
  LEFT JOIN book_author ba ON ba.book_id = b.book_id
  LEFT JOIN author a ON ba.author_id = a.author_id
  LEFT JOIN book_category bc ON bc.book_id = b.book_id
  LEFT JOIN category c ON bc.category_id = c.category_id
GROUP BY
  b.book_id;
