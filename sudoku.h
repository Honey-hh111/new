#ifndef SUDOKU_H
#define SUDOKU_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class Sudoku : public QWidget
{
    Q_OBJECT

public:
    explicit Sudoku(QWidget *parent = nullptr);

private slots:
    void newGame();
    void checkSolution();
    void giveHint();
    void pauseGame();
    void exitGame();

private:
    void setupUI();
    void generatePuzzle(int difficulty);
    bool isValid(int board[9][9],int row, int col, int num);
    bool solve(int board[9][9]);

    QLineEdit* cells[9][9];
    int solution[9][9];
    int puzzle[9][9];
    QPushButton* checkBtn;
    QPushButton* hintBtn;
    QPushButton* newBtn;
    QLabel* statusLabel;
};

#endif