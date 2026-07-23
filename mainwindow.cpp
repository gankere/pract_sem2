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
#include <QTimer>
#include <QtMath>
#include <QAudioSink>
#include <QJsonArray>
#include <QJsonDocument>

MainWindow::MainWindow(bool isHost, QWidget *parent) : QMainWindow(parent), isHostMode(isHost) 
{
    // Инициализация настроек
    settings = new QSettings("PodcastApp", "MiniPodcast", this);
    loadSettings();

    // Инициализация аудио
    audioSource = nullptr;
    audioDevice = nullptr;
    activeHostTile = nullptr;
    micDeviceList = nullptr;
    speakerDeviceList = nullptr;
    vuTimer = nullptr;
    audioSink = nullptr;
    audioOutputDevice = nullptr;
    listenerCount = 0;
    listenersListWidget = nullptr;
    listenersListLayout = nullptr;
    listenersCountLabel = nullptr;
    meetingName = nullptr;
    host1NameLabel = nullptr;
    socket = nullptr;
    audioOutputDevice = nullptr;

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

    meetingName = new QLabel("Мини-подкаст: <название подкаста>");
    meetingName->setStyleSheet("QLabel { color: #E8E8E8; font-weight: bold; font-size: 14px; }");
    topLay->addWidget(meetingName);

    roomCodeLabel = new QLabel("Код: ----");
    roomCodeLabel->setStyleSheet("QLabel { color: #6B6B6B; font-size: 12px; font-family: 'Consolas', monospace; margin-left: 10px; }");
    topLay->addWidget(roomCodeLabel);   

    topLay->addStretch(1);

    durationLabel = new QLabel("00:00");
    durationLabel->setStyleSheet("QLabel { color: #8A8A8A; font-family: 'Consolas', monospace; font-size: 14px; }");
    durationLabel->setMinimumWidth(60);
    durationLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    topLay->addWidget(durationLabel);
    
    podcastTimer = new QTimer(this);
    podcastStartTimeSecs = 0;
    
    connect(podcastTimer, &QTimer::timeout, this, [this]() {
        updateDurationDisplay();
    });

    topLay->addSpacing(12);

    root->addWidget(topBar);

    //ЦЕНТРАЛЬНАЯ ОБЛАСТЬ + ЧАТ
    auto *middle = new QWidget;
    auto *middleLay = new QHBoxLayout(middle);
    middleLay->setContentsMargins(0, 0, 0, 0);
    middleLay->setSpacing(0);

    // --- Сетка участников ---
    auto *stage = new QFrame;
    stage->setStyleSheet("QFrame { background-color: #0A0A0A; }");
    auto *stageLay = new QVBoxLayout(stage);
    stageLay->setContentsMargins(16, 16, 16, 16);
    stageLay->setSpacing(10);

    // Ведущие
    auto *hostsLabel = new QLabel("ВЕДУЩИЕ В ЭФИРЕ");
    hostsLabel->setStyleSheet("QLabel { color: #6B6B6B; font-size: 11px; font-weight: bold; }");
    stageLay->addWidget(hostsLabel);

    auto *hostsGrid = new QGridLayout;
    hostsGrid->setSpacing(12);
    for (int i = 0; i < 2; ++i) {
        auto *tile = new QFrame;
        tile->setMinimumHeight(220);
        
        tile->setObjectName("hostTile");
        tile->setStyleSheet(
            "#hostTile { "
            "   background-color: #141414; "
            "   border: 3px solid transparent; "
            "   border-radius: 10px; "
            "}"
        );
    
        if (i == 0) {
            activeHostTile = tile;
        }
        
        auto *tl = new QVBoxLayout(tile);
        tl->setContentsMargins(0, 0, 0, 0);
        tl->setSpacing(0);

        auto *centerWidget = new QWidget;
        centerWidget->setStyleSheet("background-color: transparent;");
        auto *centerLay = new QVBoxLayout(centerWidget);
        centerLay->setContentsMargins(0, 0, 0, 0);
        centerLay->setSpacing(8);
        centerLay->addStretch(1);
        auto *avatar = new QLabel();
        avatar->setFixedSize(100, 100);
        avatar->setAlignment(Qt::AlignCenter);
        avatar->setStyleSheet(
            "QLabel { "
            "   background-color: #B5656B; "
            "   color: #E8E8E8; "
            "   border-radius: 50px; "
            "   font-size: 36px; "
            "   font-weight: bold; "
            "}");
        centerLay->addWidget(avatar, 0, Qt::AlignHCenter);
        
        auto *name = new QLabel(i == 0 ? "Анна" : "Анастасия");
        name->setAlignment(Qt::AlignCenter);
        name->setStyleSheet("QLabel { color: #E8E8E8; font-weight: bold; font-size: 16px; background-color: transparent; }");
        centerLay->addWidget(name);

        if (i == 0) {
            activeHostTile = tile;
            host1Avatar = avatar;
            host1NameLabel = name;
        } else {
            host2Tile = tile;
            host2Avatar = avatar;
            host2NameLabel = name;
            
            host2Tile->setVisible(false);
            host2NameLabel->setText("Ожидание...");
            host2Avatar->setText("?");
        }

        centerLay->addStretch(1);
        
        tl->addWidget(centerWidget, 1);

        auto *statusWidget = new QWidget;
        statusWidget->setStyleSheet("background-color: transparent;");
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
    auto *separator = new QFrame;
    separator->setFixedHeight(1);
    separator->setStyleSheet("QFrame { background-color: #1F1F1F; margin-top: 4px; margin-bottom: 12px; }");
    stageLay->addWidget(separator);

    listenersCountLabel = new QLabel("СЛУШАТЕЛИ (0/6)");
    listenersCountLabel->setStyleSheet("QLabel { color: #6B6B6B; font-size: 11px; font-weight: bold; letter-spacing: 0.5px; }");
    stageLay->addWidget(listenersCountLabel);

    // Контейнер для списка
    listenersListWidget = new QWidget;
    listenersListLayout = new QVBoxLayout(listenersListWidget);
    listenersListLayout->setContentsMargins(0, 0, 0, 0);
    listenersListLayout->setSpacing(4);
    listenersListLayout->setAlignment(Qt::AlignTop);
    
    stageLay->addWidget(listenersListWidget);
    stageLay->addStretch(1);
    
    //Чат справа
    auto *chat = new QFrame;
    chat->setFixedWidth(300);
    chat->setStyleSheet("QFrame { background-color: #141414; }");
    auto *chatLay = new QVBoxLayout(chat);
    chatLay->setContentsMargins(10, 10, 10, 10);
    chatLay->setSpacing(8);

    auto *chatHeader = new QHBoxLayout;
    auto *chatTitle = new QLabel("Чат");
    chatTitle->setStyleSheet("QLabel { color: #E8E8E8; font-weight: bold; }");
    chatHeader->addWidget(chatTitle);
    chatHeader->addStretch(1);
    chatLay->addLayout(chatHeader);

    // Область сообщений
    chatDisplay = new QTextEdit;
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet(
        "QTextEdit { "
        "   background-color: #0A0A0A; "
        "   color: #E8E8E8; "
        "   border: none; "
        "   border-radius: 10px; "
        "   font-size: 13px; "
        "   padding: 8px; "
        "} "
        "QTextEdit::selection { background-color: #B5656B; }"
    );
    chatDisplay->setPlaceholderText("Сообщения появятся здесь...");
    chatLay->addWidget(chatDisplay, 1);

    // Поле ввода
    auto *inputFrame = new QFrame;
    inputFrame->setFixedHeight(40);
    inputFrame->setStyleSheet("QFrame { background-color: #1A1A1A; border-radius: 10px; }");
    auto *inputLay = new QHBoxLayout(inputFrame);
    inputLay->setContentsMargins(8, 4, 8, 4);
    inputLay->setSpacing(8);

    chatInput = new QLineEdit;
    chatInput->setPlaceholderText("Задайте вопрос...");
    chatInput->setStyleSheet(
        "QLineEdit { "
        "   background-color: transparent; "
        "   color: #E8E8E8; "
        "   border: none; "
        "   font-size: 13px; "
        "} "
        "QLineEdit::placeholder { color: #5A5A5A; }"
    );
    inputLay->addWidget(chatInput, 1);

    auto *sendBtn = new QPushButton("➤");
    sendBtn->setFixedSize(32, 32);
    sendBtn->setStyleSheet(
        "QPushButton { background-color: #B5656B; color: #E8E8E8; border-radius: 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #C96969; }"
    );
    inputLay->addWidget(sendBtn);
    chatLay->addWidget(inputFrame);

    // Подключения
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(chatInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);

    middleLay->addWidget(stage, 1);
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
            "   padding: 0; "
            "   margin: 0; "
            "} "
            "QPushButton:hover { "
            "   background-color: #252525; "
            "   border-top-left-radius: 10px; "
            "   border-bottom-left-radius: 10px; "
            "   border-top-right-radius: 0px; "
            "   border-bottom-right-radius: 0px; "
            "   padding: 0; "
            "   margin: 0; "
            "}"
        );
        lay->addWidget(iconBtn);

        // Сохраняем указатели на иконки для изменения стиля при заглушении
        if (isInput) {
            micIconBtn = iconBtn;
        } else {
            speakerIconBtn = iconBtn;
        }

        // Подключаем клик по иконке к функциям заглушения
        connect(iconBtn, &QPushButton::clicked, this, [this, isInput]() {
            if (isInput) {
                toggleMicMute();
            } else {
                toggleSpeakerMute();
            }
        });

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

        // ПРАВАЯ кнопка-стрелочка
        auto *menuBtn = new QToolButton;
        menuBtn->setFixedSize(38, 42);
        menuBtn->setText("▼");
        menuBtn->setPopupMode(QToolButton::DelayedPopup);
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
        
        if (isInput) {
            micDeviceList = deviceList;
        } else {
            speakerDeviceList = deviceList;
        }
        
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

        // подключение сигнала
        QObject::connect(deviceList, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, deviceList, devices, isInput](int index) {
                if (index >= 0 && index < devices.size()) {
                    QString deviceName = devices[index].description();
                    settings->setValue(
                        isInput ? "audio/inputDevice" : "audio/outputDevice",
                        deviceName
                    );
                    
                    if (isInput) {
                        // переинициализация захвата микрофона
                        restartAudioCapture(devices[index]);
                    } else {
                        if (audioSink) {
                            audioSink->stop();
                            delete audioSink;
                            audioSink = nullptr;
                            audioOutputDevice = nullptr;
                            qDebug() << "🔄 [АУДИО] Устройство вывода изменено, поток будет пересоздан.";
                        }
                        
                        playTestSound(devices[index]);
                    }
                }
            });

        QObject::connect(volumeSlider, &QSlider::valueChanged,
            [this, isInput](int value) {
                // 1. Сохраняем настройку
                settings->setValue(
                    isInput ? "audio/inputVolume" : "audio/outputVolume",
                    value
                );
                
                // 2. Применяем настройку в реальном времени
                if (isInput) {
                    // Для микрофона обновляем множитель
                    micVolumeMultiplier = value / 100.0;
                } else {
                    // Для динамика меняем громкость аудиоприемника
                    if (audioSink) {
                        audioSink->setVolume(value / 100.0);
                    }
                }
            });

        auto *menuAction = new QWidgetAction(menu);
        menuAction->setDefaultWidget(titleWidget);
        menu->addAction(menuAction);

        // === Ручное управление меню ===
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
        QObject::connect(menu, &QMenu::aboutToHide, [menuBtn]() {
            menuBtn->setText("▼");
        });

        return container;
    };

    micContainerWidget = createAudioButtonWithMenu("🎙️", "", true, this);
    
    // Блокировка микрофона для слушателя
    if (!isHostMode) {
        micContainerWidget->setEnabled(false);
        micContainerWidget->setToolTip("Микрофон доступен только ведущему");
        micContainerWidget->setStyleSheet(micContainerWidget->styleSheet() + " opacity: 0.5; ");
    }
    bottomLay->addWidget(micContainerWidget);
    
    // Кнопка динамиков доступна всем
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
    
    if (isHostMode) {
        initMicrophone();
    } else {
        qDebug() << "🔇 Режим слушателя: микрофон полностью отключен на уровне системы.";
    }
}

MainWindow::~MainWindow()
{
    if (audioSource) {
        audioSource->stop();
        delete audioSource;
        audioSource = nullptr;
    }
    if (audioSink) {
        audioSink->stop();
        delete audioSink;
        audioSink = nullptr;
        audioOutputDevice = nullptr;
    }
    if (podcastTimer) {
        podcastTimer->stop();
    }
    if (vuTimer) {
        vuTimer->stop();
        delete vuTimer;
        vuTimer = nullptr;
    }
    saveSettings();
}
// Назначение второго ведущего (кнопка)
ListenerRowWidget::ListenerRowWidget(const QString &name, bool isHostMode, QWidget *parent)
    : QWidget(parent), listenerName(name), promoteBtn(nullptr)
{
    setStyleSheet("QWidget { background-color: transparent; }");
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 2, 0, 2);
    lay->setSpacing(6);
    
    auto *crownSpacer = new QWidget;
    crownSpacer->setFixedSize(28, 28);
    lay->addWidget(crownSpacer);
    
    if (isHostMode) {
        promoteBtn = new QPushButton("👑");
        promoteBtn->setFixedSize(28, 28);
        promoteBtn->setToolTip("Сделать вторым ведущим");
        promoteBtn->setVisible(false);
        promoteBtn->setStyleSheet(
            "QPushButton { "
            "   background-color: transparent; "
            "   border: none; "
            "   border-radius: 4px; "
            "   font-size: 16px; "
            "} "
            "QPushButton:hover { background-color: #2A2A2A; }"
        );
        
        connect(promoteBtn, &QPushButton::clicked, this, [this]() {
            emit promoteRequested(listenerName);
        });
        
        promoteBtn->setParent(this);
        promoteBtn->move(0, 2);
    }
    
    auto *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet( 
        "QLabel { "
        "   color: #8A8A8A; "
        "   font-size: 13px; "
        "   font-weight: 500; "
        "   font-family: 'Segoe UI', Arial, sans-serif; "  
        "}"
    );
    lay->addWidget(nameLabel, 1);
}

void ListenerRowWidget::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);
    if (promoteBtn) {
        promoteBtn->setVisible(true);
        promoteBtn->raise();
    }
}

void ListenerRowWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    if (promoteBtn) {
        promoteBtn->setVisible(false);
    }
}

ListenerRowWidget::~ListenerRowWidget() {
}


// Инициализация микрофона и подсветка рамки ведущего при говорении
void MainWindow::initMicrophone()
{
    // доступные микрофоны
    QList<QAudioDevice> devices = QMediaDevices::audioInputs();
    if (devices.isEmpty()) {
        qDebug() << "❌ Микрофоны не найдены!";
        return;
    }

    // сохранённое устройство
    QString savedDeviceName = settings->value("audio/inputDevice", "").toString();
    QAudioDevice device;
    
    if (!savedDeviceName.isEmpty()) {
        for (const QAudioDevice &d : devices) {
            if (d.description() == savedDeviceName) {
                device = d;
                qDebug() << "✅ Используем сохранённый микрофон:" << device.description();
                break;
            }
        }
    }
    
    // Если не нашли — берём по умолчанию
    if (device.isNull()) {
        device = QMediaDevices::defaultAudioInput();
        qDebug() << "⚠️ Сохранённый микрофон не найден, используем по умолчанию:" << device.description();
    }
    restartAudioCapture(device);
}

void MainWindow::restartAudioCapture(const QAudioDevice &device)
{
    if (audioSource) { audioSource->stop(); delete audioSource; audioSource = nullptr; }
    if (vuTimer)     { vuTimer->stop();     delete vuTimer;     vuTimer = nullptr; }

    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!device.isFormatSupported(format)) format = device.preferredFormat();

    audioSource = new QAudioSource(device, format, this);
    audioDevice = audioSource->start();
    if (!audioDevice) { qDebug() << "❌ Не удалось запустить захват звука!"; return; }

    qDebug() << "🎙️ Микрофон запущен:" << device.description();

    const int SPEAK_THRESHOLD = 30;
    const int SILENT_THRESHOLD = 15;
    bool isSpeaking = false;

    vuTimer = new QTimer(this);
    connect(vuTimer, &QTimer::timeout, this, [this, SPEAK_THRESHOLD, SILENT_THRESHOLD, &isSpeaking]() {
        if (!audioDevice || !audioDevice->bytesAvailable()) return;

        // Если микрофон заглушен, гасим рамку и прерываем обработку (аудио не отправляется)
        if (isMicMuted) {
            if (activeHostTile) {
                activeHostTile->setStyleSheet(
                    "#hostTile { background-color: #141414; border: 3px solid transparent; border-radius: 10px; }");
            }
            return; 
        }

        QByteArray data = audioDevice->readAll();
        const qint16 *samples = reinterpret_cast<const qint16*>(data.constData());
        int sampleCount = data.size() / sizeof(qint16);
        if (sampleCount == 0) return;

        // Расчет громкости
        double sumSquares = 0;
        for (int i = 0; i < sampleCount; ++i) {
            double s = samples[i];
            sumSquares += s * s;
        }
        double rms = qSqrt(sumSquares / sampleCount);
        int level = qMin(100, static_cast<int>(rms / 327.68));

        // VAD управляет ТОЛЬКО UI
        if (activeHostTile) {
            if (!isSpeaking && level > SPEAK_THRESHOLD) {
                activeHostTile->setStyleSheet(
                    "#hostTile { background-color: #141414; border: 3px solid #50C878; border-radius: 10px; }");
                isSpeaking = true;
                if (socket && socket->isOpen()) {
                    QJsonObject msg;
                    msg["action"] = "HOST_SPEAKING";
                    msg["status"] = true;
                    msg["senderName"] = myName;
                    socket->write(QJsonDocument(msg).toJson(QJsonDocument::Compact));
                    socket->flush();
                }
            } else if (isSpeaking && level < SILENT_THRESHOLD) {
                activeHostTile->setStyleSheet(
                    "#hostTile { background-color: #141414; border: 3px solid transparent; border-radius: 10px; }");
                isSpeaking = false;
                if (socket && socket->isOpen()) {
                    QJsonObject msg; msg["action"] = "HOST_SPEAKING"; msg["status"] = false;
                    socket->write(QJsonDocument(msg).toJson(QJsonDocument::Compact));
                    socket->flush();
                }
            }
        }

        // === ПРИМЕНЕНИЕ ГРОМКОСТИ МИКРОФОНА ===
        if (micVolumeMultiplier != 1.0) {
            qint16 *samples = reinterpret_cast<qint16*>(data.data());
            int sampleCount = data.size() / sizeof(qint16);
            
            for (int i = 0; i < sampleCount; ++i) {
                double adjustedSample = samples[i] * micVolumeMultiplier;
                samples[i] = static_cast<qint16>(qBound(-32768.0, adjustedSample, 32767.0));
            }
        }

        // === ОТПРАВКА АУДИО ПОСТОЯННО ===
        if (isHostMode && socket && socket->isOpen()) {
            audioBuffer.append(data);
            const int CHUNK_SIZE = 3200;
            while (audioBuffer.size() >= CHUNK_SIZE) {
                QByteArray chunk = audioBuffer.left(CHUNK_SIZE);
                audioBuffer.remove(0, CHUNK_SIZE);
                socket->write("AUD:" + chunk);
                socket->flush();
            }
        }
    });

    vuTimer->start(50);
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

void MainWindow::sendMessage()
{
    QString text = chatInput->text().trimmed();
    if (text.isEmpty()) return;

    // 1. Отображаем у себя сразу, используя реальное имя
    ChatMessage msg;
    msg.sender = myName.isEmpty() ? "Вы" : myName; 
    msg.text = text;
    msg.isLocal = true;
    addMessageToChat(msg);
    chatInput->clear();

    // 2. Отправление сообщения на сервер
    if (socket && socket->isOpen()) {
        QJsonObject chatJson;
        chatJson["action"] = "CHAT_MESSAGE";
        chatJson["sender"] = msg.sender;
        chatJson["text"] = text;
        socket->write(QJsonDocument(chatJson).toJson(QJsonDocument::Compact));
        socket->flush();
    }

    qDebug() << "[CHAT] Отправлено:" << text;
}

void MainWindow::onMessageReceived(const ChatMessage &msg)
{
    addMessageToChat(msg);
    qDebug() << "[CHAT] Получено от" << msg.sender << ":" << msg.text;
}

void MainWindow::addMessageToChat(const ChatMessage &msg)
{
    // Локальный пользователь: фирменный цвет #B5656B
    // Удаленные пользователи: ЛИЛОВЫЙ #9B59B6 (вместо зелёного)
    QString color = msg.isLocal ? "#B5656B" : "#9B59B6"; 
    
    QString html = QString(
        "<div style='margin-bottom: 8px; line-height: 1.4;'>"
        "<span style='color: %1; font-weight: bold;'>%2:</span> "
        "<span style='color: #E8E8E8;'>%3</span>"
        "</div>"
    ).arg(color, msg.sender, msg.text);

    chatDisplay->append(html);
}

void MainWindow::playTestSound(const QAudioDevice &device)
{
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    QAudioSink *testSink = new QAudioSink(device, format, this);
    QIODevice *testOutput = testSink->start();

    if (!testOutput) {
        testSink->deleteLater();
        return;
    }

    int sampleRate = format.sampleRate();
    int durationMs = 400;
    int sampleCount = (sampleRate * durationMs) / 1000;
    
    QByteArray buffer(sampleCount * 2, 0);
    qint16 *data = reinterpret_cast<qint16*>(buffer.data());

    double amplitude = 9000.0;
    double frequency = 300.0;
    const double PI = 3.14159265358979323846;

    for (int i = 0; i < sampleCount; ++i) {
        double t = (double)i / sampleRate;
        double progress = (double)i / sampleCount;
        double signal = qSin(2 * PI * frequency * t) + 0.3 * qSin(2 * PI * frequency * 2.0 * t);
        double envelope = qSin(PI * progress);
        data[i] = static_cast<qint16>(signal * envelope * amplitude);
    }

    testOutput->write(buffer);

    QTimer::singleShot(durationMs + 100, this, [testSink, testOutput]() {
        if (testOutput) testOutput->close();
        if (testSink) {
            testSink->stop();
            testSink->deleteLater();
        }
    });
    
    qDebug() << "🔊 Тестовый звук на:" << device.description();
}

void MainWindow::addListener(const QString &name)
{
    if (listenerCount >= 6) {
        qDebug() << " Максимум 6 слушателей!";
        return;
    }
    
    auto *listenerWidget = createListenerRow(name);
    listenersListLayout->addWidget(listenerWidget);
    listenerCount++;
    updateListenersCount();
}

QWidget* MainWindow::createListenerRow(const QString &name)
{
    ListenerRowWidget *row = new ListenerRowWidget(name, isHostMode, this);
    connect(row, &ListenerRowWidget::promoteRequested, this, &MainWindow::promoteToHost);
    listenerWidgets[name] = row;
    return row;
}

void MainWindow::promoteToHost(const QString &listenerName)
{
    if (!isHostMode || !socket || !socket->isOpen()) return;
    
    QJsonObject msg;
    msg["action"] = "PROMOTE_TO_HOST";
    msg["targetName"] = listenerName;
    socket->write(QJsonDocument(msg).toJson(QJsonDocument::Compact));
    socket->flush();
    
    qDebug() << "👑 [СЕТЬ] Запрос на повышение:" << listenerName;
}

void MainWindow::updateListenersCount()
{
    if (listenersCountLabel) {
        listenersCountLabel->setText(QString("СЛУШАТЕЛИ (%1/6)").arg(listenerCount));
    }
}

void MainWindow::showAddListenerDialog()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить слушателя",
                                         "Имя слушателя:",
                                         QLineEdit::Normal,
                                         QString("Слушатель %1").arg(listenerCount + 1),
                                         &ok);
    
    if (ok && !name.trimmed().isEmpty()) {
        addListener(name.trimmed());
    }
}

void MainWindow::updateDurationDisplay()
{
    if (podcastStartTimeSecs == 0) {
        durationLabel->setText("00:00");
        return;
    }

    // Подсчёт разницы в секундах от момента создания комнаты
    qint64 currentSecs = QDateTime::currentSecsSinceEpoch();
    int totalSeconds = currentSecs - podcastStartTimeSecs;
    if (totalSeconds < 0) totalSeconds = 0;

    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    
    QString timeString;
    if (hours > 0) {
        timeString = QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    } else {
        timeString = QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    
    durationLabel->setText(timeString);
}

void MainWindow::startPodcastTimer()
{
    updateDurationDisplay();
    if (!podcastTimer->isActive()) {
        podcastTimer->start(1000);
    }
}

void MainWindow::setPodcastName(const QString &name)
{
    podcastName = name;
    if (meetingName) {
        meetingName->setText("Мини-подкаст: " + name);
    }
}

void MainWindow::setHostName(const QString &name)
{
    if (host1NameLabel) {
        host1NameLabel->setText(name);
    }
    
    // Обновляем аватарку хоста первой буквой имени
    if (host1Avatar && !name.trimmed().isEmpty()) {
        QChar firstChar = name.trimmed()[0];
        host1Avatar->setText(QString(firstChar).toUpper());
    } else if (host1Avatar) {
        host1Avatar->setText("А");
    }
}

void MainWindow::setRoomCode(const QString &code)
{
    if (roomCodeLabel) {
        roomCodeLabel->setText("Код: " + code);
    }
}

void MainWindow::attachSocket(QTcpSocket *sock)
{
    socket = sock;
    if (socket) {
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onServerDataReceived);
        QString code = roomCodeLabel->text();// список присутствующих слушателей
        if (code.startsWith("Код: ")) {
            code = code.mid(5);
            if (!code.isEmpty() && code != "----") {
                QJsonObject request;
                request["action"] = "REQUEST_LISTENERS";
                request["roomCode"] = code;
                socket->write(QJsonDocument(request).toJson(QJsonDocument::Compact));
                socket->flush();
            }
        }
    }
}

void MainWindow::onServerDataReceived()
{
    if (!socket || !socket->isOpen()) return;

    receiveBuffer.append(socket->readAll());

    while (!receiveBuffer.isEmpty()) {
        // 1. Обработка аудио-пакета
        if (receiveBuffer.startsWith("AUD:")) {
            const int HEADER_SIZE = 4;
            const int AUDIO_CHUNK = 3200;
            const int FULL_SIZE = HEADER_SIZE + AUDIO_CHUNK;

            if (receiveBuffer.size() < FULL_SIZE) {
                break;
            }

            QByteArray pcmData = receiveBuffer.mid(HEADER_SIZE, AUDIO_CHUNK);
            receiveBuffer.remove(0, FULL_SIZE);

            if (isSpeakerMuted) {
                continue;
            }

            if (!audioSink) {
                QAudioDevice device;
                if (speakerDeviceList && speakerDeviceList->currentIndex() >= 0) {
                    int index = speakerDeviceList->currentIndex();
                    QList<QAudioDevice> devices = QMediaDevices::audioOutputs();
                    if (index < devices.size()) {
                        device = devices[index];
                    }
                }
                if (device.isNull()) {
                    device = QMediaDevices::defaultAudioOutput();
                }

                QAudioFormat format;
                format.setSampleRate(16000); 
                format.setChannelCount(1);
                format.setSampleFormat(QAudioFormat::Int16);

                if (!device.isFormatSupported(format)) {
                    format = device.preferredFormat();
                }

                audioSink = new QAudioSink(device, format, this);
                audioSink->setBufferSize(3200 * 4);
                audioOutputDevice = audioSink->start();
            }

            if (audioSink && audioOutputDevice) {
                if (audioSink->state() == QAudio::StoppedState) {
                    audioOutputDevice = audioSink->start();
                }
                if (audioOutputDevice->isOpen()) {
                    audioOutputDevice->write(pcmData);
                }
            }
            continue;
        }

        // 2. Обработка JSON-сообщения
        int jsonStart = receiveBuffer.indexOf('{');
        if (jsonStart < 0) {
            if (receiveBuffer.size() > 10000) {
                qDebug() << "⚠️ [СЕТЬ] Очистка буфера от мусора";
                receiveBuffer.clear();
            }
            break;
        }

        if (jsonStart > 0) {
            receiveBuffer.remove(0, jsonStart);
        }

        int newlinePos = receiveBuffer.indexOf('\n');
        if (newlinePos < 0) {
            break;
        }

        QByteArray jsonData = receiveBuffer.left(newlinePos);
        receiveBuffer.remove(0, newlinePos + 1);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "⚠️ [СЕТЬ] Ошибка JSON:" << parseError.errorString();
            continue;
        }

        QJsonObject json = doc.object();
        QString action = json["action"].toString();

        if (action == "SYNC_LISTENERS") {
            QJsonArray listenersArray = json["listeners"].toArray();
            
            for (auto it = listenerWidgets.begin(); it != listenerWidgets.end(); ++it) {
                listenersListLayout->removeWidget(it.value());
                it.value()->deleteLater();
            }
            listenerWidgets.clear();
            listenerCount = 0;
            for (const QJsonValue &val : listenersArray) {
                addListener(val.toString());
            }
            
            updateListenersCount();
            qDebug() << "🔄 [СЕТЬ] Список слушателей синхронизирован. Всего:" << listenerCount;
        }
        else if (action == "JOIN_SUCCESS") {
            if (!json["roomName"].toString().isEmpty()) setPodcastName(json["roomName"].toString());
            if (!json["hostName"].toString().isEmpty()) setHostName(json["hostName"].toString());
            if (!json["roomCode"].toString().isEmpty()) setRoomCode(json["roomCode"].toString());

            if (json.contains("startTime")) {
                podcastStartTimeSecs = json["startTime"].toVariant().toLongLong();
                startPodcastTimer();
            }
        } 
        else if (action == "HOST_SPEAKING") {
            bool isSpeaking = json["status"].toBool();
            QString senderName = json["senderName"].toString();
            
            QFrame *targetTile = nullptr;
            if (senderName == myName) {
                targetTile = activeHostTile;
            } else if (!senderName.isEmpty()) {
                if (host2Tile && host2Tile->isVisible() && host2NameLabel && host2NameLabel->text() == senderName) {
                    targetTile = host2Tile;
                } else if (host1NameLabel && host1NameLabel->text() == senderName) {
                    targetTile = activeHostTile;
                }
            }
            
            if (targetTile) {
                if (isSpeaking) {
                    targetTile->setStyleSheet("#hostTile { background-color: #141414; border: 3px solid #50C878; border-radius: 10px; }");
                } else {
                    targetTile->setStyleSheet("#hostTile { background-color: #141414; border: 3px solid transparent; border-radius: 10px; }");
                }
            }
        }
        else if (action == "CHAT_MESSAGE") {
            ChatMessage msg;
            msg.sender = json["sender"].toString();
            msg.text = json["text"].toString();
            msg.isLocal = false;
            addMessageToChat(msg);
        }
        else if (action == "USER_PROMOTED") {
            QString newName = json["newHostName"].toString();
            QString oldHostName = json["oldHostName"].toString();
            
            qDebug() << "🎙️ [СЕТЬ]" << newName << "стал вторым ведущим вместо" 
                     << (oldHostName.isEmpty() ? "пустого слота" : oldHostName);

            // новый второй ведущий на плитке
            if (host2Tile && host2Avatar && host2NameLabel) {
                host2NameLabel->setText(newName);
                host2ActualName = newName;
                if (!newName.trimmed().isEmpty()) {
                    host2Avatar->setText(QString(newName.trimmed()[0]).toUpper());
                }
                host2Tile->setVisible(true);
            }

            if (newName == myName) {
                if (micContainerWidget) {
                    micContainerWidget->setEnabled(true);
                    micContainerWidget->setToolTip("");
                    micContainerWidget->setStyleSheet(micContainerWidget->styleSheet().replace(" opacity: 0.5; ", ""));
                }
                isHostMode = true;
                initMicrophone();
            }

            if (oldHostName == myName) {
                isHostMode = false;
                if (micContainerWidget) {
                    micContainerWidget->setEnabled(false);
                    micContainerWidget->setToolTip("Микрофон доступен только ведущему");
                    micContainerWidget->setStyleSheet(micContainerWidget->styleSheet() + " opacity: 0.5; ");
                }
                if (audioSource) {
                    audioSource->stop();
                    delete audioSource;
                    audioSource = nullptr;
                    audioDevice = nullptr;
                }
            }
        }
    }
}
void MainWindow::setMyName(const QString &name)
{
    myName = name;
}

void MainWindow::toggleMicMute()
{
    if (!isHostMode) return;
    
    isMicMuted = !isMicMuted;
    
    if (isMicMuted) {
        if (micIconBtn) {
            micIconBtn->setText("🎙️");
            micIconBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: transparent; "
                "   color: #E8E8E8; "
                "   border: none; "
                "   border-right: 1px solid #1F1F1F; "
                "   border-top-left-radius: 10px; "
                "   border-bottom-left-radius: 10px; "
                "   font-size: 16px; "
                "   opacity: 0.4; "
                "}"
            );
        }
        qDebug() << "🎙️ Микрофон заглушен";
    } else {
        if (micIconBtn) {
            micIconBtn->setText("🎙️");
            micIconBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: transparent; "
                "   color: #E8E8E8; "
                "   border: none; "
                "   border-right: 1px solid #1F1F1F; "
                "   border-top-left-radius: 10px; "
                "   border-bottom-left-radius: 10px; "
                "   font-size: 22px; "
                "   opacity: 1.0; "
                "}"
                "QPushButton:hover { background-color: #252525; }"
            );
        }
        qDebug() << "🎙️ Микрофон включен";
    }
}

void MainWindow::toggleSpeakerMute()
{
    isSpeakerMuted = !isSpeakerMuted;
    
    if (isSpeakerMuted) {
        if (speakerIconBtn) {
            speakerIconBtn->setText("🔇");
            speakerIconBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: transparent; "
                "   color: #E8E8E8; "
                "   border: none; "
                "   border-right: 1px solid #1F1F1F; "
                "   border-top-left-radius: 10px; "
                "   border-bottom-left-radius: 10px; "
                "   font-size: 16px; "
                "   opacity: 1.0; "
                "   padding: 0; "
                "   margin: 0; "
                "}"
            );
        }
        qDebug() << "🔇 Звук заглушен";
    } else {
        if (speakerIconBtn) {
            speakerIconBtn->setText("🔊");
            speakerIconBtn->setStyleSheet(
                "QPushButton { "
                "   background-color: transparent; "
                "   color: #E8E8E8; "
                "   border: none; "
                "   border-right: 1px solid #1F1F1F; "
                "   border-top-left-radius: 10px; "
                "   border-bottom-left-radius: 10px; "
                "   font-size: 22px; "
                "   opacity: 1.0; "
                "   padding: 0; "
                "   margin: 0; "
                "}"
                "QPushButton:hover { background-color: #252525; }"
            );
        }
        qDebug() << "🔊 Звук включен";
    }
}
