CREATE TABLE Musician (
    musicianNo      int,
    ssn             int,
    fName           varchar(64),
    lName           varchar(64),
    phoneNum        int,
    PRIMARY KEY (musicianNo),
    FOREIGN KEY (phoneNum) REFERENCES Place (phoneNum)
);

CREATE TABLE Place (
    phoneNum        int, 
    street          varchar(64),
    city            varchar(64),
    state           varchar(64),
    zipCode         int,
    PRIMARY KEY (phoneNum)
);

CREATE TABLE Instrument (
    instrumentNo    int,
    name            varchar(64),
    musicalKey      varchar(64),
    PRIMARY KEY (instrumentNo)
);

CREATE TABLE Ability (
    musicianNo      int,
    instrumentNo    int,
    PRIMARY KEY (musicianNo, instrumentNo),
    FOREIGN KEY (musicianNo) REFERENCES Musician (musicianNo),
    FOREIGN KEY (instrumentNo) REFERENCES Instrument (instrumentNo)
);

CREATE TABLE Song (
    songNo          int,
    title           varchar(64),
    author          varchar(64),
    albumNo         int,
    PRIMARY KEY (songNo),
    FOREIGN KEY (albumNo) REFERENCES Album (albumNo)
);

CREATE TABLE Performance (
    musicianNo      int,
    songNo          int,
    PRIMARY KEY (musicianNo, songNo),
    FOREIGN KEY (musicianNo) REFERENCES Musician (musicianNo) ON DELETE CASCADE,
    FOREIGN KEY (songNo) REFERENCES Song (songNo) ON DELETE CASCADE
);

CREATE TABLE Album (
    albumNo         int,
    title           varchar(64),
    copyrightDate   date, 
    format          varchar(64),
    producerNo      int,
    PRIMARY KEY (albumNo),
    FOREIGN KEY (producerNo) REFERENCES Musician (musicianNo)
);


INSERT INTO Musician VALUES (1, 122223, 'Grant', 'Kimes', 2223331111);
INSERT INTO Musician VALUES (2, 334433, 'Kieran', 'Harrison', 2223331111);
INSERT INTO Musician VALUES (3, 123321, 'Emily', 'Kordick', 4442229999);

INSERT INTO Place VALUES (2223331111, '123 Home St.', 'Chicago', 'IL', 60011);
INSERT INTO Place VALUES (4442229999, '34 Walk Dr', 'Miami', 'FL', 11223);

INSERT INTO Instrument VALUES (1, 'Saxophone', 'E');
INSERT INTO Instrument VALUES (2, 'Saxophone', 'B flat');
INSERT INTO Instrument VALUES (3, 'Piano', 'C');
INSERT INTO Instrument VALUES (4, 'Clarinet', 'A flat');

INSERT INTO Ability VALUES (1, 1);
INSERT INTO Ability VALUES (1, 2);
INSERT INTO Ability VALUES (1, 3);
INSERT INTO Ability VALUES (2, 4);
INSERT INTO Ability VALUES (3, 3);
INSERT INTO Ability VALUES (3, 4);

INSERT INTO Album VALUES (1, 'The First Album', DATE '2010-01-01', 'CD', 1);
INSERT INTO Album VALUES (2, 'Two is a Date', DATE '2011-01-01', 'CD', 1);
INSERT INTO Album VALUES (3, 'Now We Are Talking', DATE '2011-02-02', 'CD', 2);
INSERT INTO Album VALUES (4, 'Jeezy', DATE '2011-03-03', 'CD', 3);

INSERT INTO Song VALUES (1, 'Numba One', 'Grakim', 1);
INSERT INTO Song VALUES (2, 'Johnny is Home', 'Grakim', 1);
INSERT INTO Song VALUES (3, 'The Crew', 'Grakim', 2);
INSERT INTO Song VALUES (4, 'One of the Best Songs', 'Grakim', 2);
INSERT INTO Song VALUES (5, 'Still Improving', 'Jenkins', 3);
INSERT INTO Song VALUES (6, 'Finale', 'Jenkins', 4);

INSERT INTO Performance VALUES (1, 1);
INSERT INTO Performance VALUES (1, 2);
INSERT INTO Performance VALUES (1, 3);
INSERT INTO Performance VALUES (1, 4);
INSERT INTO Performance VALUES (2, 5);
INSERT INTO Performance VALUES (3, 1);
INSERT INTO Performance VALUES (3, 2);
INSERT INTO Performance VALUES (3, 6);
