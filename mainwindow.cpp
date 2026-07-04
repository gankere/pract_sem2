#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setStyleSheet("QWidget { font-family: 'Segoe UI', Arial; }");

    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    //ВЕРХНИЙ БАР
    auto *topBar = new QFrame;
    topBar->setFixedHeight(48);
    topBar->setStyleSheet("QFrame { background-color: #1C1E22; }");
    auto *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(16, 0, 16, 0);

    auto *meetingName = new QLabel("Мини-подкаст: <название подкаста>");
    meetingName->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 14px; }");
    topLay->addWidget(meetingName);

    topLay->addStretch(1);

    auto *timer = new QLabel("<длительность подкаста>");
    timer->setStyleSheet("QLabel { color: #b9bbbe; font-family: Consolas; font-size: 14px; }");
    topLay->addWidget(timer);

    auto *encryption = new QLabel("🔒");
    encryption->setStyleSheet("QLabel { color: #b9bbbe; margin-left: 12px; }");
    topLay->addWidget(encryption);

    // Кнопка настроек в правом верхнем углу
    auto *settingsBtn = new QPushButton("⚙️");
    settingsBtn->setFixedSize(36, 36);
    settingsBtn->setStyleSheet(
        "QPushButton { background-color: #3B3E44; color: white; "
        "border-radius: 6px; font-size: 16px; }"
        "QPushButton:hover { background-color: #4A4D52; }");
    settingsBtn->setToolTip("Настройки");
    topLay->addWidget(settingsBtn);

    root->addWidget(topBar);

    // ЦЕНТРАЛЬНАЯ БАР + ЧАТ
    auto *middle = new QWidget;
    auto *middleLay = new QHBoxLayout(middle);
    middleLay->setContentsMargins(0, 0, 0, 0);
    middleLay->setSpacing(0);

    // Сетка участников
    auto *stage = new QFrame;
    stage->setStyleSheet("QFrame { background-color: #13151A; }");
    auto *stageLay = new QVBoxLayout(stage);
    stageLay->setContentsMargins(16, 16, 16, 16);
    stageLay->setSpacing(16);

    // Ведущие
    auto *hostsLabel = new QLabel("ВЕДУЩИЕ В ЭФИРЕ");
    hostsLabel->setStyleSheet("QLabel { color: #8e9297; font-size: 11px; font-weight: bold; }");
    stageLay->addWidget(hostsLabel);

    auto *hostsGrid = new QGridLayout;
    hostsGrid->setSpacing(12);
    for (int i = 0; i < 2; ++i) {
        auto *tile = new QFrame;
        tile->setMinimumHeight(220);
        tile->setStyleSheet("QFrame { background-color: #1C1E22; border-radius: 8px; }");
        auto *tl = new QVBoxLayout(tile);
        tl->setContentsMargins(0, 0, 0, 0);
        tl->setSpacing(0);

        auto *centerWidget = new QWidget;
        auto *centerLay = new QVBoxLayout(centerWidget);
        centerLay->setContentsMargins(0, 0, 0, 0);
        centerLay->setSpacing(8);
        centerLay->addStretch(1);
        auto *avatar = new QLabel(i == 0 ? "А" : "М");
        avatar->setFixedSize(100, 100);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(
            "QLabel { background-color: #5865f2; color: white; "
            "border-radius: 50px; font-size: 36px; font-weight: bold; }");
        centerLay->addWidget(avatar, 0, Qt::AlignHCenter);
        auto *name = new QLabel(i == 0 ? "Анна" : "Анастасия");
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 16px; }");
        centerLay->addWidget(name);
        centerLay->addStretch(1);
        
        tl->addWidget(centerWidget, 1);

        auto *statusWidget = new QWidget;
        auto *statusLay = new QHBoxLayout(statusWidget);
        statusLay->setContentsMargins(0, 8, 12, 12);
        statusLay->setSpacing(6);
        statusLay->addStretch(1);
        if (i == 1) {
            auto *micIcon = new QLabel("🔇");
            micIcon->setStyleSheet("QLabel { color: #e74c3c; font-size: 16px; }");
            statusLay->addWidget(micIcon);
        }
        tl->addWidget(statusWidget);

        hostsGrid->addWidget(tile, 0, i);
    }
    stageLay->addLayout(hostsGrid);

    // Слушатели
    auto *listenersLabel = new QLabel("СЛУШАТЕЛИ (5/6)");
    listenersLabel->setStyleSheet("QLabel { color: #8e9297; font-size: 11px; font-weight: bold; }");
    stageLay->addWidget(listenersLabel);

    auto *listenersGrid = new QGridLayout;
    listenersGrid->setSpacing(8);
    for (int i = 0; i < 6; ++i) {
        auto *tile = new QFrame;
        tile->setMinimumHeight(100);
        tile->setStyleSheet("QFrame { background-color: #1C1E22; border-radius: 6px; }");
        auto *tl = new QVBoxLayout(tile);
        tl->setContentsMargins(10, 10, 10, 10);
        tl->setSpacing(8);
        tl->addSpacing(12);

        auto *avatar = new QLabel(i < 5 ? QString::number(i + 1) : "+");
        avatar->setFixedSize(36, 36);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(
            "QLabel { background-color: #3498db; color: white; border-radius: 18px; font-weight: bold; }");
        tl->addWidget(avatar, 0, Qt::AlignHCenter);

        auto *name = new QLabel(i < 5 ? QString("Слушатель %1").arg(i + 1) : "");
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet(
            QString("QLabel { color: %1; font-size: 12px; }")
                .arg(i < 5 ? "white" : "#7f8c8d"));
        tl->addWidget(name);

        listenersGrid->addWidget(tile, i / 3, i % 3);
    }
    stageLay->addLayout(listenersGrid);

    middleLay->addWidget(stage, 1);

    // Чат
    auto *chat = new QFrame;
    chat->setFixedWidth(300);
    chat->setStyleSheet("QFrame { background-color: #1C1E22; }");
    auto *chatLay = new QVBoxLayout(chat);
    chatLay->setContentsMargins(10, 10, 10, 10);
    chatLay->setSpacing(8);

    auto *chatHeader = new QHBoxLayout;
    auto *chatTitle = new QLabel("💬 Чат");
    chatTitle->setStyleSheet("QLabel { color: white; font-weight: bold; }");
    chatHeader->addWidget(chatTitle);
    chatHeader->addStretch(1);
    auto *closeChat = new QLabel("✕");
    closeChat->setStyleSheet("QLabel { color: #b9bbbe; }");
    chatHeader->addWidget(closeChat);
    chatLay->addLayout(chatHeader);

    auto *chatBody = new QFrame;
    chatBody->setStyleSheet("QFrame { background-color: #13151A; border-radius: 6px; }");
    auto *chatBodyLay = new QVBoxLayout(chatBody);
    auto *chatPlaceholder = new QLabel("(список вопросов)");
    chatPlaceholder->setStyleSheet("QLabel { color: #8e9297; }");
    chatBodyLay->addWidget(chatPlaceholder);
    chatBodyLay->addStretch(1);
    chatLay->addWidget(chatBody, 1);

    auto *chatInput = new QFrame;
    chatInput->setFixedHeight(40);
    chatInput->setStyleSheet("QFrame { background-color: #2f3136; border-radius: 6px; }");
    auto *chatInputLay = new QHBoxLayout(chatInput);
    auto *chatInputText = new QLabel("Задайте вопрос...");
    chatInputText->setStyleSheet("QLabel { color: #72767d; }");
    chatInputLay->addWidget(chatInputText);
    chatLay->addWidget(chatInput);

    middleLay->addWidget(chat);
    root->addWidget(middle, 1);

    // НИЖНЯЯ ПАНЕЛЬ УПРАВЛЕНИЯ
    auto *bottomBar = new QFrame;
    bottomBar->setFixedHeight(90);
    bottomBar->setStyleSheet("QFrame { background-color: #26282A; }");
    auto *bottomLay = new QHBoxLayout(bottomBar);
    bottomLay->setContentsMargins(20, 10, 20, 10);
    bottomLay->setSpacing(8);

    auto makeZoomBtn = [](const QString &icon, const QString &text) {
        auto *container = new QWidget;
        auto *lay = new QVBoxLayout(container);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(2);

        auto *btn = new QPushButton(icon);
        btn->setFixedSize(52, 42);
        btn->setStyleSheet(
            "QPushButton { background-color: #3B3E44; color: white; "
            "border-radius: 6px; font-size: 18px; }"
            "QPushButton:hover { background-color: #4A4D52; }");
        lay->addWidget(btn, 0, Qt::AlignHCenter);

        auto *lbl = new QLabel(text);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("QLabel { color: #b9bbbe; font-size: 10px; }");
        lay->addWidget(lbl);

        return container;
    };

    bottomLay->addWidget(makeZoomBtn("🎙️", "Микрофон"));
    bottomLay->addWidget(makeZoomBtn("🔊", "Динамик"));

    bottomLay->addStretch(1);

    auto *leaveContainer = new QWidget;
    auto *leaveLay = new QVBoxLayout(leaveContainer);
    leaveLay->setContentsMargins(0, 0, 0, 0);
    leaveLay->setSpacing(2);

    auto *leaveBtn = new QPushButton("Выйти");
    leaveBtn->setFixedSize(80, 42);
    leaveBtn->setStyleSheet(
        "QPushButton { background-color: #D33651; color: white; "
        "border-radius: 6px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #E04560; }");
    leaveLay->addWidget(leaveBtn, 0, Qt::AlignHCenter);

    bottomLay->addWidget(leaveContainer);

    root->addWidget(bottomBar);

    setCentralWidget(central);
    resize(1100, 750);
}