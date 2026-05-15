#include "solitaire.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDebug>

Solitaire::Solitaire(QWidget *parent)
    : QWidget(parent)
    , gameStarted(false)
{
    setWindowFlags(Qt::Window);
    setWindowTitle("🃏 纸牌接龙");
    setFixedSize(650, 670);

    // 设置不透明背景
    setAttribute(Qt::WA_DeleteOnClose);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(255, 192, 192));
    setPalette(pal);

    setupUI();
    loadCardImages();
    dealNewGame();
}

void Solitaire::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 标题栏
    QHBoxLayout* topLayout = new QHBoxLayout();
    QPushButton* newGameBtn = new QPushButton("🔄 新游戏", this);
    QPushButton* pauseBtn = new QPushButton("⏸ 暂停", this);
    QPushButton* exitBtn = new QPushButton("❌ 退出", this);
    statusLabel = new QLabel("点击牌堆发牌", this);

    newGameBtn->setFixedSize(100, 25);
    pauseBtn->setFixedSize(100, 25);
    exitBtn->setFixedSize(100, 25);
    statusLabel->setStyleSheet("color: white; font-size: 14px;");

    connect(newGameBtn, &QPushButton::clicked, this, &Solitaire::dealNewGame);
    connect(pauseBtn, &QPushButton::clicked, this, &Solitaire::pauseGame);
    connect(exitBtn, &QPushButton::clicked, this, &Solitaire::exitGame);

    topLayout->addWidget(newGameBtn);
    topLayout->addWidget(pauseBtn);
    topLayout->addWidget(exitBtn);
    topLayout->addStretch();
    topLayout->addWidget(statusLabel);
    mainLayout->addLayout(topLayout);

    // 上方区域：牌堆 + 基础牌堆
    QHBoxLayout* topArea = new QHBoxLayout();

    // 牌堆区域
    QVBoxLayout* stockArea = new QVBoxLayout();
    stockBtn = new QPushButton(this);
    stockBtn->setFixedSize(64, 80);
    wasteBtn = new QPushButton(this);
    wasteBtn->setFixedSize(64, 80);
    stockArea->addWidget(stockBtn);
    stockArea->addWidget(wasteBtn);
    topArea->addLayout(stockArea);

    connect(stockBtn, &QPushButton::clicked, [this]() {
        if (!stock.isEmpty()) {
            waste.append(stock.takeLast());
            waste.last().faceUp = true;
            updateDisplay();
        } else if (!waste.isEmpty()) {
            while (!waste.isEmpty()) {
                Card c = waste.takeLast();
                c.faceUp = false;
                stock.append(c);
            }
            updateDisplay();
        }
    });
    connect(wasteBtn, &QPushButton::clicked, [this]() {
        if (waste.isEmpty()) return;
        onWasteClicked();
    });

    topArea->addStretch();

    // 基础牌堆（4个）
    for (int i = 0; i < 4; i++) {
        QPushButton* btn = new QPushButton(this);
        btn->setFixedSize(64, 80);
        foundationBtns.append(btn);
        topArea->addWidget(btn);
    }

    mainLayout->addLayout(topArea);

    // 下方区域：7列牌堆
    QHBoxLayout* tableauLayout = new QHBoxLayout();
    for (int i = 0; i < 7; i++) {
        QVBoxLayout* colLayout = new QVBoxLayout();
        colLayout->setAlignment(Qt::AlignTop);
        colLayout->setSpacing(5);

        for (int j = 0; j < 19; j++) {
            QPushButton* btn = new QPushButton(this);
            btn->setFixedSize(64, 80);
            btn->hide();
            tableauBtns[i].append(btn);
            colLayout->addWidget(btn);

            int col = i;
            int row = j;
            connect(btn, &QPushButton::clicked, [this, col, row]() {
                onCardClicked(col,row);
            });
        }
        tableauLayout->addLayout(colLayout);
    }
    mainLayout->addLayout(tableauLayout);

    // 为废牌堆添加点击事件
    connect(wasteBtn, &QPushButton::clicked, [this]() {
        if (!waste.isEmpty()) {
            Card card = waste.last();
            if (canMoveToFoundation(card, card.suit)) {
                foundation[card.suit].append(card);
                waste.removeLast();
                updateDisplay();
                autoMoveToFoundation();
            }
        }
    });

    setLayout(mainLayout);
}

void Solitaire::loadCardImages()
{
    // 加载牌背
    if (!cardBack.load(":/images/card_back.png")) {
        cardBack = QPixmap(64, 80);
        cardBack.fill(Qt::blue);
    }
    // 加载52张牌面
    for (int i = 1; i <= 52; i++) {
        QString path = QString(":/images/card_%1.png").arg(i);
        if (!cardFronts[i-1].load(path)) {
            qDebug() << "加载失败:" << path;
            // 创建备用牌面
            cardFronts[i-1] = QPixmap(64, 80);
            cardFronts[i-1].fill(Qt::lightGray);
        }
    }
    qDebug() << "牌面加载完成";
}

void Solitaire::dealNewGame()
{
    // 初始化52张牌
    QVector<Card> deck;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 1; rank <= 13; rank++) {
            deck.append(Card(suit, rank, false));
        }
    }

    // 洗牌
    for (int i = 0; i < 200; i++) {
        int a = QRandomGenerator::global()->bounded(52);
        int b = QRandomGenerator::global()->bounded(52);
        std::swap(deck[a], deck[b]);
    }

    // 清空
    stock.clear();
    waste.clear();
    for (int i = 0; i < 7; i++) tableau[i].clear();
    for (int i = 0; i < 4; i++) foundation[i].clear();

    // 发牌到7列
    int idx = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j <= i; j++) {
            Card c = deck[idx++];
            c.faceUp = (j == i);
            tableau[i].append(c);
        }
    }

    // 剩余牌到牌堆
    for (int i = idx; i < 52; i++) {
        stock.append(deck[i]);
    }

    updateDisplay();
    gameStarted = true;
    statusLabel->setText("游戏开始！点击牌堆发牌");
}

void Solitaire::updateDisplay()
{
    // 更新牌堆
    if (stock.isEmpty()) {
        stockBtn->setIcon(QIcon());
    } else {
        QIcon icon(cardBack);
        stockBtn->setIcon(icon);
    }
    stockBtn->setIconSize(QSize(64, 80));

    // 更新废牌堆
    if (waste.isEmpty()) {
        wasteBtn->setIcon(QIcon());
    } else {
        int cardIdx = waste.last().suit * 13 + waste.last().rank - 1;
        if (cardIdx >= 0 && cardIdx < 52 && !cardFronts[cardIdx].isNull()) {
            QIcon icon(cardFronts[cardIdx]);
            wasteBtn->setIcon(icon);
        } else {
            wasteBtn->setIcon(QIcon());
        }
        wasteBtn->setIconSize(QSize(64, 80));
    }

    // 更新基础牌堆
    for (int i = 0; i < 4 && i < foundationBtns.size(); i++) {
        if (foundation[i].isEmpty()) {
            foundationBtns[i]->setIcon(QIcon());
        } else {
            int cardIdx = foundation[i].last().suit * 13 + foundation[i].last().rank - 1;
            if (cardIdx >= 0 && cardIdx < 52 && !cardFronts[cardIdx].isNull()) {
                QIcon icon(cardFronts[cardIdx]);
                foundationBtns[i]->setIcon(icon);
            }
        }
        foundationBtns[i]->setIconSize(QSize(64, 80));
    }

    // 更新7列牌堆
    for (int col = 0; col < 7; col++) {
        // 更新每张牌
        for (int row = 0; row < tableau[col].size() && row < tableauBtns[col].size(); row++) {
            const Card& card = tableau[col][row];
            QPushButton* btn = tableauBtns[col][row];

            if (card.faceUp) {
                int cardIdx = card.suit * 13 + card.rank - 1;
                if (cardIdx >= 0 && cardIdx < 52 && !cardFronts[cardIdx].isNull()) {
                    QIcon icon(cardFronts[cardIdx]);
                    btn->setIcon(icon);
                }
            } else {
                QIcon icon(cardBack);
                btn->setIcon(icon);
            }
            btn->setIconSize(QSize(64, 80));
            btn->show();
        }

        // 隐藏多余的按钮
        for (int row = tableau[col].size(); row < tableauBtns[col].size(); row++) {
            tableauBtns[col][row]->hide();
        }
    }

    // 强制刷新
    repaint();
}

void Solitaire::pauseGame()
{
    QMessageBox::information(this, "⏸ 暂停", "游戏已暂停\n点击确定继续");
}

void Solitaire::exitGame()
{
    this->close();
}

bool Solitaire::isRedSuit(const Card& card)
{
    return card.suit == 2 || card.suit == 3;
}

bool Solitaire::isBlackSuit(const Card& card)
{
    return card.suit == 0 || card.suit == 1;
}

bool Solitaire::canMoveToTableau(const Card& card, int col)
{
    if (tableau[col].isEmpty()) {
        return card.rank == 13;
    }
    const Card& targetCard = tableau[col].last();
    return (card.rank == targetCard.rank - 1) &&
           ((isRedSuit(card) == isRedSuit(targetCard))||(isBlackSuit(card) == isBlackSuit(targetCard)));
}

bool Solitaire::canMoveToFoundation(const Card& card, int suit)
{
    if (foundation[suit].isEmpty()) {
        return card.rank == 1;
    }

    const Card& topCard = foundation[suit].last();

    bool sameColor = (isRedSuit(card) && isRedSuit(topCard)) ||
                     (isBlackSuit(card) && isBlackSuit(topCard));

    return sameColor && (card.rank == topCard.rank + 1);
}

void Solitaire::moveToFoundation(int col, int cardIndex)
{
    if (cardIndex != tableau[col].size() - 1) {
        return;
    }
    Card card = tableau[col].last();
    int targetSuit = card.suit;
    if (canMoveToFoundation(card, targetSuit)) {
        foundation[targetSuit].append(card);
        tableau[col].removeLast();
        if (!tableau[col].isEmpty() && !tableau[col].last().faceUp) {
            tableau[col].last().faceUp = true;
        }
        updateDisplay();
        autoMoveToFoundation();
    }
}

void Solitaire::moveToTableau(int fromCol, int fromIndex, int toCol)
{
    if (fromIndex < 0 || fromIndex >= tableau[fromCol].size()) {
        return;
    }
    Card firstCard = tableau[fromCol][fromIndex];
    if (!canMoveToTableau(firstCard, toCol)) {
        return;
    }
    for (int i = fromIndex; i < tableau[fromCol].size(); i++) {
        tableau[toCol].append(tableau[fromCol][i]);
    }
    while (tableau[fromCol].size() > fromIndex) {
        tableau[fromCol].removeLast();
    }
    if (!tableau[fromCol].isEmpty() && !tableau[fromCol].last().faceUp) {
        tableau[fromCol].last().faceUp = true;
    }
    updateDisplay();
    autoMoveToFoundation();
}

void Solitaire::dealFromStock()
{
    if (!stock.isEmpty()) {
        Card card = stock.takeLast();
        card.faceUp = true;
        waste.append(card);
        updateDisplay();
    } else if (!waste.isEmpty()) {
        while (!waste.isEmpty()) {
            Card card = waste.takeLast();
            card.faceUp = false;
            stock.append(card);
        }
        updateDisplay();
    }
}

void Solitaire::autoMoveToFoundation()
{
    bool moved = true;
    while (moved) {
        moved = false;
        for (int col = 0; col < 7; col++) {
            if (!tableau[col].isEmpty() && tableau[col].last().faceUp) {
                Card card = tableau[col].last();
                if (canMoveToFoundation(card, card.suit)) {
                    foundation[card.suit].append(card);
                    tableau[col].removeLast();
                    if (!tableau[col].isEmpty() && !tableau[col].last().faceUp) {
                        tableau[col].last().faceUp = true;
                    }
                    moved = true;
                    updateDisplay();
                    break;
                }
            }
        }
        if (!waste.isEmpty()) {
            Card card = waste.last();
            if (canMoveToFoundation(card, card.suit)) {
                foundation[card.suit].append(card);
                waste.removeLast();
                moved = true;
                updateDisplay();
                continue;
            }
        }
    }
}

QString Solitaire::cardToText(const Card& card)
{
    QString suits[] = {"♣️", "♠️", "♦️", "♥️"};
    QString ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    return suits[card.suit] + ranks[card.rank - 1];
}// 处理卡片点击（双击自动移动 + 列间移动）
void Solitaire::onCardClicked(int col, int row)
{
    if (row >= tableau[col].size()) return;
    if (!tableau[col][row].faceUp) return;

    Card& clickedCard = tableau[col][row];

    //尝试移动到基础牌堆，如果能放就直接移
    if (canMoveToFoundation(clickedCard, clickedCard.suit)) {
        // 只移动最上面的牌
        if (row == tableau[col].size() - 1) {
            foundation[clickedCard.suit].append(clickedCard);
            tableau[col].removeLast();

            // 翻开新牌
            if (!tableau[col].isEmpty() && !tableau[col].last().faceUp) {
                tableau[col].last().faceUp = true;
            }

            updateDisplay();
            autoMoveToFoundation();
            statusLabel->setText(QString("将 %1 移动到基础牌堆").arg(cardToText(clickedCard)));
            return;
        }
    }

    //尝试移动到其他列，从第一列到最后一列找能放的位置
    bool moved = false;
    for (int targetCol = 0; targetCol < 7; targetCol++) {
        if (targetCol == col) continue;  // 不能移动到同一列

        if (canMoveToTableau(clickedCard, targetCol)) {
            // 找到移动整叠牌
            QVector<Card> cardsToMove;
            for (int i = row; i < tableau[col].size(); i++) {
                cardsToMove.append(tableau[col][i]);
            }

            // 添加到目标列
            for (const Card& card : cardsToMove) {
                tableau[targetCol].append(card);
            }

            // 删除原位置的牌
            while (tableau[col].size() > row) {
                tableau[col].removeLast();
            }

            // 翻开新牌
            if (!tableau[col].isEmpty() && !tableau[col].last().faceUp) {
                tableau[col].last().faceUp = true;
            }

            updateDisplay();
            autoMoveToFoundation();

            statusLabel->setText(QString("将牌移动到第 %1 列").arg(targetCol + 1));
            moved = true;
            break;
        }
    }

    if (!moved) {
        statusLabel->setText(QString("%1 无处可移动！").arg(cardToText(clickedCard)));
    }
}void Solitaire::onWasteClicked()
{
    if (waste.isEmpty()) return;
    Card card = waste.last();

    // 尝试移到基础牌堆
    if (canMoveToFoundation(card, card.suit)) {
        foundation[card.suit].append(card);
        waste.removeLast();
        updateDisplay();
        autoMoveToFoundation();
        return;
    }

    // 尝试移到各列
    for (int col = 0; col < 7; col++) {
        if (canMoveToTableau(card, col)) {
            tableau[col].append(card);
            waste.removeLast();
            updateDisplay();
            autoMoveToFoundation();
            return;
        }
    }

    statusLabel->setText("这张牌动不了，继续发牌吧");
}