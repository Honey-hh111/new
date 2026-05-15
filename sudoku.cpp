#include "sudoku.h"
#include <QMessageBox>
#include <QRandomGenerator>
//#include <QDebug>

Sudoku::Sudoku(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Window);//独立窗口
    setAttribute(Qt::WA_DeleteOnClose);
    setAutoFillBackground(true);//不透明背景
    QPalette pal =palette();
    pal.setColor(QPalette::Window,QColor(44,62,80));
    setPalette(pal);
    setupUI();
    generatePuzzle(40);
}

void Sudoku::setupUI()
{
    setWindowTitle("🔢 数独");
    setFixedSize(500, 600);
    setStyleSheet("background-color: #EEFFF4;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel* title = new QLabel("数独 Sudoku", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 18px; color: #40924F; padding: 10px;");
    mainLayout->addWidget(title);

    // 数独网格
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(1);

    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            cells[row][col] = new QLineEdit(this);
            cells[row][col]->setAlignment(Qt::AlignCenter);
            cells[row][col]->setMaxLength(1);
            cells[row][col]->setFixedSize(50, 50);

            // 在 setupUI() 中，设置每个格子的边框样式
            int left = (col % 3 == 0) ? 3 : 1;
            int top = (row % 3 == 0) ? 3 : 1;
            int right = ((col + 1) % 3 == 0) ? 3 : 1;
            int bottom = ((row + 1) % 3 == 0) ? 3 : 1;

            // 使用 outline 效果或调整颜色使粗线更明显
            cells[row][col]->setStyleSheet(QString(
                                               "font-size: 40px; font-weight: bold;"
                                               "border-top: %1px solid #2C3E50;"
                                               "border-bottom: %2px solid #2C3E50;"
                                               "border-left: %3px solid #2C3E50;"
                                               "border-right: %4px solid #2C3E50;"
                                               "background-color: white;"
                                               ).arg(top).arg(bottom).arg(left).arg(right));
            gridLayout->addWidget(cells[row][col], row, col);
            // 限制只能输入1-9
            connect(cells[row][col], &QLineEdit::textChanged, [this, row, col](const QString& text) {
                if (!text.isEmpty() && (text.toInt() < 1 || text.toInt() > 9)) {
                    cells[row][col]->clear();
                }
            });
        }
    }
    mainLayout->addLayout(gridLayout);

    // 按钮栏
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    checkBtn = new QPushButton("✓ 检查答案", this);
    hintBtn = new QPushButton("💡 提示", this);
    newBtn = new QPushButton("🔄 新游戏", this);

    checkBtn->setFixedHeight(40);
    hintBtn->setFixedHeight(40);
    newBtn->setFixedHeight(40);

    connect(checkBtn, &QPushButton::clicked, this, &Sudoku::checkSolution);
    connect(hintBtn, &QPushButton::clicked, this, &Sudoku::giveHint);
    connect(newBtn, &QPushButton::clicked, this, &Sudoku::newGame);

    buttonLayout->addWidget(checkBtn);
    buttonLayout->addWidget(hintBtn);
    buttonLayout->addWidget(newBtn);
    mainLayout->addLayout(buttonLayout);

    // 在按钮栏添加
    QPushButton* pauseBtn = new QPushButton("⏸ 暂停", this);
    QPushButton* exitBtn = new QPushButton("❌ 退出", this);

    connect(pauseBtn, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "暂停", "游戏已暂停");
    });

    connect(exitBtn, &QPushButton::clicked, [this]() {
        this->close();  // 关闭当前游戏窗口
    });

    // 状态栏
    statusLabel = new QLabel("填入数字，点击检查答案！", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #80E86E; padding: 10px;");
    mainLayout->addWidget(statusLabel);
}

void Sudoku::generatePuzzle(int difficulty)
{
    // 先生成一个完整的有效数独
    int fullBoard[9][9] = {};
    fullBoard[0][0] = 5; fullBoard[0][1] = 3; fullBoard[0][4] = 7;
    fullBoard[1][0] = 6; fullBoard[1][3] = 1; fullBoard[1][4] = 9; fullBoard[1][5] = 5;
    fullBoard[2][1] = 9; fullBoard[2][2] = 8; fullBoard[2][7] = 6;
    fullBoard[3][0] = 8; fullBoard[3][4] = 6; fullBoard[3][8] = 3;
    fullBoard[4][0] = 4; fullBoard[4][3] = 8; fullBoard[4][5] = 3; fullBoard[4][8] = 1;
    fullBoard[5][0] = 7; fullBoard[5][4] = 2; fullBoard[5][8] = 6;
    fullBoard[6][1] = 6; fullBoard[6][6] = 2; fullBoard[6][7] = 8;
    fullBoard[7][3] = 4; fullBoard[7][4] = 1; fullBoard[7][5] = 9; fullBoard[7][8] = 5;
    fullBoard[8][4] = 8; fullBoard[8][7] = 7; fullBoard[8][8] = 9;

    solve(fullBoard);

    // 复制到solution
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            solution[i][j] = fullBoard[i][j];

    // 随机挖空
    int cellsToRemove = difficulty;
    for (int i = 0; i < cellsToRemove; i++) {
        int row = QRandomGenerator::global()->bounded(9);
        int col = QRandomGenerator::global()->bounded(9);
        if (fullBoard[row][col] != 0) {
            fullBoard[row][col] = 0;
        } else {
            i--;
        }
    }

    // 显示题目
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            puzzle[row][col] = fullBoard[row][col];
            if (fullBoard[row][col] != 0) {
                cells[row][col]->setText(QString::number(fullBoard[row][col]));
                cells[row][col]->setReadOnly(true);
                cells[row][col]->setStyleSheet(
                    "font-size: 18px; font-weight: bold; color: #1E211E;"
                    "border: 1px solid #1E211E;"
                    );

            } else {
                cells[row][col]->clear();
                cells[row][col]->setReadOnly(false);
                cells[row][col]->setStyleSheet(
                    "font-size: 18px; color:#40924F;"
                    "border: 1px solid #1E211E;"
                    );
            }
        }
    }
}

bool Sudoku::isValid(int board[9][9], int row, int col, int num)
{
    for (int x = 0; x < 9; x++)
        if (board[row][x] == num) return false;
    for (int x = 0; x < 9; x++)
        if (board[x][col] == num) return false;

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i + startRow][j + startCol] == num) return false;
    return true;
}

bool Sudoku::solve(int board[9][9])
{
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (board[row][col] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (isValid(board, row, col, num)) {
                        board[row][col] = num;
                        if (solve(board)) return true;
                        board[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

void Sudoku::checkSolution()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            QString text = cells[i][j]->text();
            if (text.isEmpty()) {
                statusLabel->setText("❌ 还有空白格子！");
                return;
            }
            int userNum = text.toInt();
            if (userNum != solution[i][j]) {
                statusLabel->setText(QString("❌ 第 %1 行第 %2 列错误！正确答案是 %3")
                                         .arg(i+1).arg(j+1).arg(solution[i][j]));
                return;
            }
        }
    }
    statusLabel->setText("🎉 完美！你解开了数独！🎉");
    QMessageBox::information(this, "恭喜", "你成功解开了数独！");
}

void Sudoku::giveHint()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] == 0 && cells[i][j]->text().isEmpty()) {
                cells[i][j]->setText(QString::number(solution[i][j]));
                cells[i][j]->setStyleSheet("color: #e74c3c;");
                statusLabel->setText("💡 已经给你一个提示了！");
                return;
            }
        }
    }
    statusLabel->setText("没有空格可以提示了！");
}

void Sudoku::newGame()
{
    generatePuzzle(40);
    statusLabel->setText("新游戏！祝你玩得开心！");
}
void Sudoku::pauseGame()
{
    QMessageBox::information(this, "⏸ 暂停", "数独游戏已暂停\n点击确定继续");
}

void Sudoku::exitGame()
{
    this->close();  // 关闭当前窗口
}