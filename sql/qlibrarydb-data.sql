SET
  AUTOCOMMIT = 0;


INSERT INTO
  category (name)
VALUES
  ('Fiction'),
  ('Non-Fiction'),
  ('Science Fiction'),
  ('Fantasy'),
  ('Mystery'),
  ('Thriller'),
  ('Romance'),
  ('Biography'),
  ('History'),
  ('Self-Help'),
  ('Cooking'),
  ('Travel'),
  ('Art'),
  ('Science'),
  ('Technology');


COMMIT;


INSERT INTO
  author (first_name, last_name)
VALUES
  ('Chinua', 'Achebe'),
  ('Dante', 'Alighieri'),
  ('Jane', 'Austen'),
  ('Samuel', 'Beckett'),
  ('Giovanni', 'Boccaccio'),
  ('Emily', 'Brontë'),
  ('Albert', 'Camus'),
  ('Paul', 'Celan'),
  ('Louis-Ferdinand', 'Céline'),
  ('Geoffrey', 'Chaucer'),
  ('Anton', 'Chekhov'),
  ('Joseph', 'Conrad'),
  ('Charles', 'Dickens'),
  ('Denis', 'Diderot'),
  ('Alfred', 'Döblin'),
  ('Fyodor', 'Dostoevsky'),
  ('George', 'Eliot'),
  ('Ralph', 'Ellison'),
  ('William', 'Faulkner'),
  ('Gustave', 'Flaubert'),
  ('Nikolai', 'Gogol'),
  ('Günter', 'Grass'),
  ('Knut', 'Hamsun'),
  ('Ernest', 'Hemingway'),
  ('Henrik', 'Ibsen'),
  ('James', 'Joyce'),
  ('Franz', 'Kafka'),
  ('Yasunari', 'Kawabata'),
  ('Nikos', 'Kazantzakis'),
  ('Halldór', 'Laxness'),
  ('Giacomo', 'Leopardi'),
  ('Doris', 'Lessing'),
  ('Astrid', 'Lindgren'),
  ('Lu', 'Xun'),
  ('Naguib', 'Mahfouz'),
  ('Thomas', 'Mann'),
  ('Herman', 'Melville'),
  ('Elsa', 'Morante'),
  ('Toni', 'Morrison'),
  ('Murasaki', 'Shikibu'),
  ('Robert', 'Musil'),
  ('Vladimir', 'Nabokov'),
  ('George', 'Orwell'),
  ('Fernando', 'Pessoa'),
  ('Marcel', 'Proust'),
  ('François', 'Rabelais'),
  ('Juan', 'Rulfo'),
  ('Salman', 'Rushdie'),
  ('Tayeb', 'Salih'),
  ('José', 'Saramago'),
  ('William', 'Shakespeare'),
  ('Laurence', 'Sterne'),
  ('Italo', 'Svevo'),
  ('Jonathan', 'Swift'),
  ('Leo', 'Tolstoy'),
  ('Mark', 'Twain'),
  ('Walt', 'Whitman'),
  ('Virginia', 'Woolf'),
  ('Marguerite', 'Yourcenar');


COMMIT;


INSERT INTO
  book (
    title,
    publication_date,
    copies_owned,
    description
  )
VALUES
  (
    'Things Fall Apart',
    '1958-01-01',
    78,
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis ullamcorper luctus leo, et sollicitudin nulla condimentum id. Maecenas iaculis urna vitae magna finibus, a volutpat elit cursus. Proin eu felis at lectus condimentum tempus. Quisque justo odio, finibus non vulputate at, dapibus eget augue. Aliquam aliquam ligula ut mauris venenatis, nec sodales leo tincidunt. Nullam et blandit velit, sit amet vestibulum tortor. Suspendisse potenti. In eu imperdiet augue. Integer cursus, libero vel bibendum dapibus, felis lorem convallis lacus, vitae dignissim justo enim eu orci. Vivamus sed faucibus eros. Pellentesque posuere urna et ex luctus consectetur. Mauris facilisis nisi nec orci tristique suscipit.

Sed lobortis imperdiet tortor et pulvinar. Morbi aliquam ac nulla nec elementum. Fusce non lorem eu enim consequat viverra et eu urna. In vulputate neque velit, nec tincidunt felis ultricies eget. Morbi eu tellus rutrum, bibendum augue vel, elementum elit. Suspendisse facilisis, magna quis hendrerit blandit, magna nibh semper enim, sed consequat odio purus tristique augue. Vestibulum sodales massa sed suscipit sagittis. Suspendisse fermentum risus efficitur lectus congue, in placerat magna fermentum. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Praesent ac hendrerit leo, vitae euismod libero. Nam scelerisque felis nibh, non congue justo pellentesque quis. Curabitur eget orci vitae sem hendrerit tincidunt.'
  ),
  ('The Divine Comedy', '1315-01-01', 87, NULL),
  ('Pride and Prejudice', '1813-01-01', 91, NULL),
  (
    'Molloy, Malone Dies, The Unnamable, the trilogy',
    '1952-01-01',
    72,
    NULL
  ),
  ('The Decameron', '1351-01-01', 72, NULL),
  ('Wuthering Heights', '1847-01-01', 62, NULL),
  ('The Stranger', '1942-01-01', 52, NULL),
  ('Poems', '1952-01-01', 18, NULL),
  (
    'Journey to the End of the Night',
    '1932-01-01',
    19,
    NULL
  ),
  ('The Canterbury Tales', '1450-01-01', 75, NULL),
  ('Stories', '1886-01-01', 78, NULL),
  ('Nostromo', '1904-01-01', 84, NULL),
  (
    'Great Expectations',
    '1861-01-01',
    6,
    "Lorem Ipsum"
  ),
  (
    'Jacques the Fatalist',
    '1796-01-01',
    99,
    "Lorem Ipsum"
  ),
  ('Berlin Alexanderplatz', '1929-01-01', 81, NULL),
  ('Crime and Punishment', '1866-01-01', 56, NULL),
  ('The Idiot', '1869-01-01', 17, NULL),
  ('The Possessed', '1872-01-01', 74, NULL),
  ('The Brothers Karamazov', '1880-01-01', 45, NULL),
  ('Middlemarch', '1871-01-01', 36, NULL),
  ('Invisible Man', '1952-01-01', 28, NULL),
  ('Absalom, Absalom!', '1936-01-01', 41, NULL),
  ('The Sound and the Fury', '1929-01-01', 23, NULL),
  ('Madame Bovary', '1857-01-01', 54, NULL),
  ('Sentimental Education', '1869-01-01', 5, NULL),
  ('Dead Souls', '1842-01-01', 39, NULL),
  ('The Tin Drum', '1959-01-01', 64, NULL),
  ('Hunger', '1890-01-01', 43, NULL),
  ('The Old Man and the Sea', '1952-01-01', 89, NULL),
  ('A Doll''s House', '1879-01-01', 5, NULL),
  ('Ulysses', '1922-01-01', 10, NULL),
  ('Stories', '1924-01-01', 17, NULL),
  ('The Trial', '1925-01-01', 59, NULL),
  ('The Castle', '1926-01-01', 26, NULL),
  (
    'The Sound of the Mountain',
    '1954-01-01',
    10,
    NULL
  ),
  ('Zorba the Greek', '1946-01-01', 13, NULL),
  ('Independent People', '1934-01-01', 36, NULL),
  ('Poems', '1818-01-01', 84, NULL),
  ('The Golden Notebook', '1962-01-01', 81, NULL),
  ('Pippi Longstocking', '1945-01-01', 37, NULL),
  ('Diary of a Madman', '1918-01-01', 92, NULL),
  ('Children of Gebelawi', '1959-01-01', 15, NULL),
  ('Buddenbrooks', '1901-01-01', 9, NULL),
  ('The Magic Mountain', '1924-01-01', 46, NULL),
  ('Moby Dick', '1851-01-01', 57, NULL),
  ('History', '1974-01-01', 46, NULL),
  ('Beloved', '1987-01-01', 95, NULL),
  ('The Tale of Genji', '1006-01-01', 87, NULL),
  (
    'The Man Without Qualities',
    '1931-01-01',
    55,
    NULL
  ),
  ('Lolita', '1955-01-01', 84, NULL),
  ('Nineteen Eighty-Four', '1949-01-01', 31, NULL),
  ('The Book of Disquiet', '1928-01-01', 86, NULL),
  ('In Search of Lost Time', '1920-01-01', 52, NULL),
  (
    'Gargantua and Pantagruel',
    '1533-01-01',
    76,
    NULL
  ),
  ('Pedro Páramo', '1955-01-01', 67, NULL),
  ('Midnight''s Children', '1981-01-01', 59, NULL),
  (
    'Season of Migration to the North',
    '1966-01-01',
    14,
    NULL
  ),
  ('Blindness', '1995-01-01', 13, NULL),
  ('Hamlet', '1603-01-01', 94, NULL),
  ('King Lear', '1608-01-01', 9, NULL),
  ('Othello', '1609-01-01', 45, NULL),
  (
    'The Life And Opinions of Tristram Shandy',
    '1760-01-01',
    50,
    NULL
  ),
  ('Confessions of Zeno', '1923-01-01', 71, NULL),
  ('Gulliver''s Travels', '1726-01-01', 63, NULL),
  ('War and Peace', '1867-01-01', 67, NULL),
  ('Anna Karenina', '1877-01-01', 55, NULL),
  (
    'The Death of Ivan Ilyich',
    '1886-01-01',
    48,
    NULL
  ),
  (
    'The Adventures of Huckleberry Finn',
    '1884-01-01',
    19,
    NULL
  ),
  ('Leaves of Grass', '1855-01-01', 88, NULL),
  ('Mrs Dalloway', '1925-01-01', 62, NULL),
  ('To the Lighthouse', '1927-01-01', 72, NULL),
  ('Memoirs of Hadrian', '1951-01-01', 87, NULL);


COMMIT;


INSERT INTO
  book_author (book_id, author_id)
VALUES
  (1, 1),
  (2, 2),
  (3, 3),
  (4, 4),
  (5, 5),
  (6, 6),
  (7, 7),
  (8, 8),
  (9, 9),
  (10, 10),
  (11, 11),
  (12, 12),
  (13, 13),
  (14, 14),
  (15, 15),
  (16, 16),
  (17, 16),
  (18, 16),
  (19, 16),
  (20, 17),
  (21, 18),
  (22, 19),
  (23, 19),
  (24, 20),
  (25, 20),
  (26, 21),
  (27, 22),
  (28, 23),
  (29, 24),
  (30, 25),
  (31, 26),
  (32, 27),
  (33, 27),
  (34, 27),
  (35, 28),
  (36, 29),
  (37, 30),
  (38, 31),
  (39, 32),
  (40, 33),
  (41, 34),
  (42, 35),
  (43, 36),
  (44, 36),
  (45, 37),
  (46, 38),
  (47, 39),
  (48, 40),
  (49, 41),
  (50, 42),
  (51, 43),
  (52, 44),
  (53, 45),
  (54, 46),
  (55, 47),
  (56, 48),
  (57, 49),
  (58, 50),
  (59, 51),
  (60, 51),
  (61, 51),
  (62, 52),
  (63, 53),
  (64, 54),
  (65, 55),
  (66, 55),
  (67, 55),
  (68, 56),
  (69, 57),
  (70, 58),
  (71, 58),
  (72, 59);


COMMIT;


INSERT INTO
  book_category (book_id, category_id)
VALUES
  (1, 1),
  (2, 2),
  (3, 3),
  (4, 4),
  (5, 5),
  (5, 6),
  (6, 6),
  (6, 7),
  (7, 7),
  (7, 8),
  (8, 8),
  (9, 9),
  (10, 10),
  (11, 11),
  (12, 13),
  (13, 4),
  (14, 5),
  (15, 6),
  (16, 7),
  (17, 8),
  (18, 9),
  (19, 1),
  (20, 2),
  (21, 3),
  (22, 4),
  (23, 5),
  (24, 6),
  (25, 7),
  (26, 8),
  (27, 9),
  (28, 1),
  (29, 2),
  (30, 3),
  (31, 4),
  (32, 5),
  (33, 6),
  (34, 7),
  (35, 8),
  (36, 14),
  (37, 1),
  (38, 2),
  (39, 3),
  (40, 4),
  (41, 5),
  (42, 6),
  (43, 7),
  (44, 8),
  (45, 9),
  (46, 1),
  (47, 12),
  (48, 13),
  (49, 14),
  (50, 15),
  (51, 6),
  (52, 7),
  (53, 8),
  (54, 9),
  (55, 10),
  (56, 2),
  (57, 13),
  (58, 4),
  (59, 5),
  (60, 6),
  (61, 7),
  (62, 8),
  (63, 9),
  (64, 1),
  (65, 2),
  (66, 3),
  (67, 4),
  (68, 15),
  (69, 6),
  (70, 7),
  (71, 8),
  (72, 9);


COMMIT;


INSERT INTO
  user (username, password, salt, is_admin)
VALUES
  (
    'admin',
    'd82494f05d6917ba02f7aaa29689ccb444bb73f20380876cb05d1f37537b7892',
    'admin',
    1
  ),
  (
    'johnsmith',
    'dd11aea190ed9c8d90bf78e2094a329595d77262511ba9ae7932f80ab0193d1b',
    'johnsmith',
    0
  );


COMMIT;


INSERT INTO
  book_rating (user_id, book_id, rating)
VALUES
  (1, 2, 5),
  (1, 3, 4),
  (1, 4, 4),
  (1, 5, 5),
  (1, 6, 4),
  (1, 7, 5),
  (1, 8, 3),
  (1, 9, 3),
  (1, 10, 5),
  (1, 11, 4),
  (1, 12, 5),
  (1, 13, 1),
  (1, 14, 2),
  (1, 15, 5),
  (1, 16, 1),
  (2, 2, 4),
  (2, 3, 2),
  (2, 4, 3),
  (2, 5, 4),
  (2, 6, 2),
  (2, 7, 4),
  (2, 8, 2),
  (2, 9, 1),
  (2, 10, 4),
  (2, 11, 3),
  (2, 12, 4),
  (2, 13, 1),
  (2, 14, 1),
  (2, 15, 4),
  (2, 16, 1);


COMMIT;
