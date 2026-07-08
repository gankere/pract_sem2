#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QDialog>
#include <QMenu>
#include <QWidgetAction>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QComboBox>
#include <QSlider>
#include <QDebug>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Инициализация настроек
    settings = new QSettings("PodcastApp", "MiniPodcast", this);
    loadSettings();

    setStyleSheet("QWidget { font-family: 'Segoe UI', Arial; }");

    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // === ВЕРХНИЙ БАР ===
    auto *topBar = new QFrame;
    topBar->setFixedHeight(48);
    topBar->setStyleSheet("QFrame { background-color: #0D0D0D; }");
    auto *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(16, 0, 16, 0);

    auto *meetingName = new QLabel("Мини-подкаст: <название подкаста>");
    meetingName->setStyleSheet("QLabel { color: #E8E8E8; font-weight: bold; font-size: 14px; }");
    topLay->addWidget(meetingName);

    topLay->addStretch(1);

    auto *timer = new QLabel("<длительность подкаста>");
    timer->setStyleSheet("QLabel { color: #8A8A8A; font-family: Consolas; font-size: 14px; }");
    topLay->addWidget(timer);

    auto *encryption = new QLabel("🔒");
    encryption->setStyleSheet("QLabel { color: #8A8A8A; margin-left: 12px; }");
    topLay->addWidget(encryption);

    // Кнопка настроек в правом верхнем углу
    settingsBtn = new QPushButton("⚙️");
    settingsBtn->setFixedSize(36, 36);
    settingsBtn->setStyleSheet(
        "QPushButton { background-color: #1A1A1A; color: #E8E8E8; "
        "border-radius: 10px; font-size: 16px; }"
        "QPushButton:hover { background-color: #252525; }");
    settingsBtn->setToolTip("Настройки");
    topLay->addWidget(settingsBtn);

    connect(settingsBtn, &QPushButton::clicked, this, &MainWindow::openSettings);

    root->addWidget(topBar);

    // === ЦЕНТРАЛЬНАЯ ОБЛАСТЬ + ЧАТ ===
    auto *middle = new QWidget;
    auto *middleLay = new QHBoxLayout(middle);
    middleLay->setContentsMargins(0, 0, 0, 0);
    middleLay->setSpacing(0);

    // --- Сетка участников ---
    auto *stage = new QFrame;
    stage->setStyleSheet("QFrame { background-color: #0A0A0A; }");
    auto *stageLay = new QVBoxLayout(stage);
    stageLay->setContentsMargins(16, 16, 16, 16);
    stageLay->setSpacing(16);

    // Ведущие
    auto *hostsLabel = new QLabel("ВЕДУЩИЕ В ЭФИРЕ");
    hostsLabel->setStyleSheet("QLabel { color: #6B6B6B; font-size: 11px; font-weight: bold; }");
    stageLay->addWidget(hostsLabel);

    auto *hostsGrid = new QGridLayout;
    hostsGrid->setSpacing(12);
    for (int i = 0; i < 2; ++i) {
        auto *tile = new QFrame;
        tile->setMinimumHeight(220);
        tile->setStyleSheet("QFrame { background-color: #141414; border-radius: 10px; }");
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
            "QLabel { background-color: #B5656B; color: #E8E8E8; "
            "border-radius: 50px; font-size: 36px; font-weight: bold; }");
        centerLay->addWidget(avatar, 0, Qt::AlignHCenter);
        auto *name = new QLabel(i == 0 ? "Анна" : "Анастасия");
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("QLabel { color: #E8E8E8; font-weight: bold; font-size: 16px; }");
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
            micIcon->setStyleSheet("QLabel { color: #C96969; font-size: 16px; }");
            statusLay->addWidget(micIcon);
        }
        tl->addWidget(statusWidget);

        hostsGrid->addWidget(tile, 0, i);
    }
    stageLay->addLayout(hostsGrid);

    // Слушатели
    auto *listenersLabel = new QLabel("СЛУШАТЕЛИ (5/6)");
    listenersLabel->setStyleSheet("QLabel { color: #6B6B6B; font-size: 11px; font-weight: bold; }");
    stageLay->addWidget(listenersLabel);

    auto *listenersGrid = new QGridLayout;
    listenersGrid->setSpacing(8);
    for (int i = 0; i < 6; ++i) {
        auto *tile = new QFrame;
        tile->setMinimumHeight(100);
        tile->setStyleSheet("QFrame { background-color: #141414; border-radius: 10px; }");
        auto *tl = new QVBoxLayout(tile);
        tl->setContentsMargins(10, 10, 10, 10);
        tl->setSpacing(8);
        tl->addSpacing(12);

        auto *avatar = new QLabel(i < 5 ? QString::number(i + 1) : "+");
        avatar->setFixedSize(36, 36);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(
            "QLabel { background-color: #B5656B; color: #E8E8E8; border-radius: 18px; font-weight: bold; }");
        tl->addWidget(avatar, 0, Qt::AlignHCenter);

        auto *name = new QLabel(i < 5 ? QString("Слушатель %1").arg(i + 1) : "");
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet(
            QString("QLabel { color: %1; font-size: 12px; }")
                .arg(i < 5 ? "#E8E8E8" : "#5A5A5A"));
        tl->addWidget(name);

        listenersGrid->addWidget(tile, i / 3, i % 3);
    }
    stageLay->addLayout(listenersGrid);

    middleLay->addWidget(stage, 1);

    // --- Чат справа ---
    auto *chat = new QFrame;
    chat->setFixedWidth(300);
    chat->setStyleSheet("QFrame { background-color: #141414; }");
    auto *chatLay = new QVBoxLayout(chat);
    chatLay->setContentsMargins(10, 10, 10, 10);
    chatLay->setSpacing(8);

    auto *chatHeader = new QHBoxLayout;
    auto *chatTitle = new QLabel("💬 Чат");
    chatTitle->setStyleSheet("QLabel { color: #E8E8E8; font-weight: bold; }");
    chatHeader->addWidget(chatTitle);
    chatHeader->addStretch(1);
    auto *closeChat = new QLabel("✕");
    closeChat->setStyleSheet("QLabel { color: #8A8A8A; }");
    chatHeader->addWidget(closeChat);
    chatLay->addLayout(chatHeader);

    auto *chatBody = new QFrame;
    chatBody->setStyleSheet("QFrame { background-color: #0A0A0A; border-radius: 10px; }");
    auto *chatBodyLay = new QVBoxLayout(chatBody);
    auto *chatPlaceholder = new QLabel("(список вопросов)");
    chatPlaceholder->setStyleSheet("QLabel { color: #6B6B6B; }");
    chatBodyLay->addWidget(chatPlaceholder);
    chatBodyLay->addStretch(1);
    chatLay->addWidget(chatBody, 1);

    auto *chatInput = new QFrame;
    chatInput->setFixedHeight(40);
    chatInput->setStyleSheet("QFrame { background-color: #1A1A1A; border-radius: 10px; }");
    auto *chatInputLay = new QHBoxLayout(chatInput);
    auto *chatInputText = new QLabel("Задайте вопрос...");
    chatInputText->setStyleSheet("QLabel { color: #5A5A5A; }");
    chatInputLay->addWidget(chatInputText);
    chatLay->addWidget(chatInput);

    middleLay->addWidget(chat);
    root->addWidget(middle, 1);

    // === НИЖНЯЯ ПАНЕЛЬ УПРАВЛЕНИЯ ===
    auto *bottomBar = new QFrame;
    bottomBar->setFixedHeight(70);
    bottomBar->setStyleSheet("QFrame { background-color: #111111; }");
    auto *bottomLay = new QHBoxLayout(bottomBar);
    bottomLay->setContentsMargins(20, 10, 20, 10);
    bottomLay->setSpacing(16);

    // Функция создания кнопки с выпадающим меню
    auto createAudioButtonWithMenu = [this](const QString &icon, const QString &text, 
                                             bool isInput, QWidget *parent) {
        // Создаем ЕДИНЫЙ контейнер с закругленными углами
        auto *container = new QWidget;
        container->setFixedSize(90, 42);
        container->setStyleSheet(
            "QWidget { "
            "   background-color: #1A1A1A; "
            "   border-radius: 10px; "
            "}"
        );
        
        auto *lay = new QHBoxLayout(container);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(0);

        // ЛЕВАЯ кнопка с иконкой
        auto *iconBtn = new QPushButton(icon);
        iconBtn->setFixedSize(52, 42);
        iconBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: transparent; "
            "   color: #E8E8E8; "
            "   border: none; "
            "   border-right: 1px solid #1F1F1F; "
            "   border-top-left-radius: 10px; "
            "   border-bottom-left-radius: 10px; "
            "   border-top-right-radius: 0px; "
            "   border-bottom-right-radius: 0px; "
            "   font-size: 18px; "
            "} "
            "QPushButton:hover { "
            "   background-color: #252525; "
            "   border-top-left-radius: 10px; "
            "   border-bottom-left-radius: 10px; "
            "   border-top-right-radius: 0px; "
            "   border-bottom-right-radius: 0px; "
            "}"
        );
        lay->addWidget(iconBtn);

        // === Создаем меню (ПЕРЕД кнопкой-стрелочкой!) ===
        auto *menu = new QMenu(parent);
        menu->setStyleSheet(
            "QMenu { "
            "   background-color: #0F0F0F; "
            "   color: #E8E8E8; "
            "   border: 1px solid #050505; "
            "   border-radius: 10px; "
            "   padding: 8px 0; "
            "} "
            "QMenu::item { "
            "   padding: 8px 16px; "
            "   font-size: 13px; "
            "} "
            "QMenu::item:selected { "
            "   background-color: #B5656B; "
            "}"
        );

        // ПРАВАЯ кнопка-стрелочка (QToolButton)
        auto *menuBtn = new QToolButton;
        menuBtn->setFixedSize(38, 42);
        menuBtn->setText("▼");
        menuBtn->setPopupMode(QToolButton::DelayedPopup); // Qt не управляет позицией
        menuBtn->setStyleSheet(
            "QToolButton { "
            "   background-color: transparent; "
            "   color: #8A8A8A; "
            "   border: none; "
            "   border-top-left-radius: 0px; "
            "   border-bottom-left-radius: 0px; "
            "   border-top-right-radius: 10px; "
            "   border-bottom-right-radius: 10px; "
            "   font-size: 10px; "
            "} "
            "QToolButton:hover { "
            "   background-color: #252525; "
            "   border-top-left-radius: 0px; "
            "   border-bottom-left-radius: 0px; "
            "   border-top-right-radius: 10px; "
            "   border-bottom-right-radius: 10px; "
            "   color: #E8E8E8; "
            "} "
            "QToolButton::menu-indicator { image: none; }"
        );
        lay->addWidget(menuBtn);

        auto *titleWidget = new QWidget;
        auto *titleLay = new QVBoxLayout(titleWidget);
        titleLay->setContentsMargins(12, 8, 12, 8);
        titleLay->setSpacing(8);
        
        auto *titleLabel = new QLabel(isInput ? "Устройство ввода" : "Устройство вывода");
        titleLabel->setStyleSheet("QLabel { color: #8A8A8A; font-size: 11px; font-weight: bold; }");
        titleLay->addWidget(titleLabel);

        auto *deviceList = new QComboBox;
        deviceList->setStyleSheet(
            "QComboBox { "
            "   background-color: #0F0F0F; "
            "   color: #E8E8E8; "
            "   border: 1px solid #0F0F0F; "
            "   border-radius: 8px; "
            "   padding: 6px 8px; "
            "   font-size: 13px; "
            "} "
            "QComboBox::drop-down { "
            "   border: none; "
            "   width: 20px; "
            "} "
            "QComboBox QAbstractItemView { "
            "   background-color: #0F0F0F; "
            "   color: #E8E8E8; "
            "   border: 1px solid #1F1F1F; "
            "   selection-background-color: #B5656B; "
            "   border-radius: 8px; "
            "}"
        );

        QList<QAudioDevice> devices = isInput ? 
            QMediaDevices::audioInputs() : QMediaDevices::audioOutputs();
        
        for (const QAudioDevice &device : devices) {
            deviceList->addItem(device.description());
        }
        
        if (devices.isEmpty()) {
            deviceList->addItem("Устройства не найдены");
            deviceList->setEnabled(false);
        } else {
            QString savedDeviceName = settings->value(
                isInput ? "audio/inputDevice" : "audio/outputDevice", 
                ""
            ).toString();
            
            int savedIndex = -1;
            if (!savedDeviceName.isEmpty()) {
                for (int i = 0; i < devices.size(); ++i) {
                    if (devices[i].description() == savedDeviceName) {
                        savedIndex = i;
                        break;
                    }
                }
            }
            
            if (savedIndex != -1) {
                deviceList->setCurrentIndex(savedIndex);
            } else {
                QAudioDevice defaultDevice = isInput ? 
                    QMediaDevices::defaultAudioInput() : QMediaDevices::defaultAudioOutput();
                for (int i = 0; i < devices.size(); ++i) {
                    if (devices[i] == defaultDevice) {
                        deviceList->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }

        titleLay->addWidget(deviceList);

        auto *volumeLabel = new QLabel("Громкость");
        volumeLabel->setStyleSheet("QLabel { color: #8A8A8A; font-size: 11px; margin-top: 4px; }");
        titleLay->addWidget(volumeLabel);

        auto *volumeSlider = new QSlider(Qt::Horizontal);
        volumeSlider->setRange(0, 100);
        
        int savedVolume = settings->value(
            isInput ? "audio/inputVolume" : "audio/outputVolume", 
            100
        ).toInt();
        volumeSlider->setValue(savedVolume);
        
        volumeSlider->setStyleSheet(
            "QSlider::groove:horizontal { "
            "   background-color: #2A2A2A; "
            "   height: 4px; "
            "   border-radius: 2px; "
            "} "
            "QSlider::sub-page:horizontal { "
            "   background-color: #B5656B; "
            "   height: 4px; "
            "   border-radius: 2px; "
            "} "
            "QSlider::handle:horizontal { "
            "   background-color: #B5656B; "
            "   width: 12px; "
            "   margin: -4px 0; "
            "   border-radius: 6px; "
            "} "
            "QSlider::handle:horizontal:hover { "
            "   background-color: #C96969; "
            "}"
        );
        titleLay->addWidget(volumeSlider);

        QObject::connect(deviceList, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, deviceList, devices, isInput](int index) {
                if (index >= 0 && index < devices.size()) {
                    QString deviceName = devices[index].description();
                    settings->setValue(
                        isInput ? "audio/inputDevice" : "audio/outputDevice",
                        deviceName
                    );
                }
            });

        QObject::connect(volumeSlider, &QSlider::valueChanged,
            [this, isInput](int value) {
                settings->setValue(
                    isInput ? "audio/inputVolume" : "audio/outputVolume",
                    value
                );
            });

        auto *menuAction = new QWidgetAction(menu);
        menuAction->setDefaultWidget(titleWidget);
        menu->addAction(menuAction);

        // === Ручное управление меню (ВСЕГДА ВВЕРХ!) ===
        QObject::connect(menuBtn, &QToolButton::clicked, [menu, menuBtn]() {
            if (menu->isVisible()) {
                menu->hide();
                menuBtn->setText("▼");
            } else {
                // Вычисляем позицию НАД кнопкой
                QPoint pos = menuBtn->mapToGlobal(QPoint(0, -menu->sizeHint().height()));
                menu->popup(pos);
                menuBtn->setText("▲");
            }
        });

        // Обновляем стрелочку при закрытии меню (клик вне меню)
        QObject::connect(menu, &QMenu::aboutToHide, [menuBtn]() {
            menuBtn->setText("▼");
        });

        return container;
    };

    bottomLay->addWidget(createAudioButtonWithMenu("🎙️", "", true, this));
    bottomLay->addWidget(createAudioButtonWithMenu("🔊", "", false, this));

    bottomLay->addStretch(1);

    // Кнопка "Выйти"
    auto *leaveContainer = new QWidget;
    auto *leaveLay = new QVBoxLayout(leaveContainer);
    leaveLay->setContentsMargins(0, 0, 0, 0);
    leaveLay->setSpacing(2);

    auto *leaveBtn = new QPushButton("Выйти");
    leaveBtn->setFixedSize(80, 42);
    leaveBtn->setStyleSheet(
        "QPushButton { background-color: #B5656B; color: #E8E8E8; "
        "border-radius: 10px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #C96969; }");
    leaveLay->addWidget(leaveBtn, 0, Qt::AlignHCenter);

    bottomLay->addWidget(leaveContainer);

    root->addWidget(bottomBar);

    setCentralWidget(central);
    resize(1100, 750);
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::loadSettings()
{
    qDebug() << "Загрузка настроек...";
}

void MainWindow::saveSettings()
{
    settings->sync();
    qDebug() << "Настройки сохранены";
}

void MainWindow::openSettings()
{
    qDebug() << "Открытие настроек...";
}