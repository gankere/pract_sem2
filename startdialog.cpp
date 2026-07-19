#include "startdialog.h"
#include <QVBoxLayout>
#include <QInputDialog>
#include <QHBoxLayout>
#include "mainwindow.h"

StartDialog::StartDialog(QWidget *parent)
    : QDialog(parent), creatorMode(false)
{
    setWindowTitle("Мини-подкаст с живым участием");
    setFixedSize(600, 450);

    setStyleSheet(
        "QDialog { "
        "   background-color: #0A0A0A; "
        "   font-family: 'Segoe UI', Arial, sans-serif; "
        "}"
    );

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    mainLayout->setSpacing(16);

    auto *titleLabel = new QLabel("Мини-подкаст с живым участием");
    titleLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 22px; "
        "   font-weight: bold; "
        "   color: #B5656B; "
        "}"
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(10);

    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Введите ваше имя");
    nameEdit->setStyleSheet(
        "QLineEdit { "
        "   background-color: #1A1A1A; "
        "   color: #FFFFFF; "
        "   border: 2px solid #2A2A2A; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   font-size: 15px; "
        "}"
        "QLineEdit:focus { border-color: #B5656B; }"
        "QLineEdit::placeholder { color: #6B6B6B; }"
    );
    mainLayout->addWidget(nameEdit);

    auto *createContainer = new QWidget;
    auto *createLayout = new QHBoxLayout(createContainer);
    createLayout->setContentsMargins(0, 0, 0, 0);
    createLayout->addStretch(1);

    createButton = new QPushButton("Создать комнату");
    createButton->setFixedSize(200, 40);
    createButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #B5656B; "
        "   color: #FFFFFF; "
        "   border: none; "
        "   border-radius: 8px; "
        "   font-size: 16px; "
        "   font-weight: 600; "
        "   padding: 0; "
        "}"
        "QPushButton:hover { background-color: #C96969; }"
        "QPushButton:pressed { background-color: #9B5A5F; }"
    );
    connect(createButton, &QPushButton::clicked, this, &StartDialog::onCreateRoomClicked);
    createLayout->addWidget(createButton);

    createLayout->addStretch(1);
    mainLayout->addWidget(createContainer);

    mainLayout->addSpacing(8);
    auto *separatorLabel = new QLabel("────── или ─────");
    separatorLabel->setStyleSheet("color: #6B6B6B; font-size: 13px;");
    separatorLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(separatorLabel);
    mainLayout->addSpacing(8);

    codeEdit = new QLineEdit;
    codeEdit->setPlaceholderText("Код комнаты (например, POD-1234)");
    codeEdit->setStyleSheet(
        "QLineEdit { "
        "   background-color: #1A1A1A; "
        "   color: #FFFFFF; "
        "   border: 2px solid #2A2A2A; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   font-size: 15px; "
        "}"
        "QLineEdit:focus { border-color: #B5656B; }"
        "QLineEdit::placeholder { color: #6B6B6B; }"
    );
    mainLayout->addWidget(codeEdit);

    auto *joinContainer = new QWidget;
    auto *joinLayout = new QHBoxLayout(joinContainer);
    joinLayout->setContentsMargins(0, 0, 0, 0);
    joinLayout->addStretch(1);

    joinButton = new QPushButton("Присоединиться");
    joinButton->setFixedSize(200, 40);
    joinButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #2A2A2A; "
        "   color: #FFFFFF; "
        "   border: none; "
        "   border-radius: 8px; "
        "   font-size: 16px; "
        "   font-weight: 600; "
        "   padding: 0; "
        "}"
        "QPushButton:hover { background-color: #3A3A3A; }"
        "QPushButton:pressed { background-color: #1A1A1A; }"
    );
    connect(joinButton, &QPushButton::clicked, this, &StartDialog::onJoinRoomClicked);
    joinLayout->addWidget(joinButton);

    joinLayout->addStretch(1);
    mainLayout->addWidget(joinContainer);

    errorLabel = new QLabel;
    errorLabel->setStyleSheet("color: #C96969; font-size: 13px; font-weight: 500;");
    errorLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(errorLabel);
    mainLayout->addStretch(1);
}

void StartDialog::onCreateRoomClicked()
{
    if (nameEdit->text().trimmed().isEmpty()) {
        errorLabel->setText("⚠️ Введите ваше имя ⚠️");
        return;
    }
    errorLabel->clear();

    QDialog inputDialog(this);
    inputDialog.setWindowTitle("Создание комнаты");
    inputDialog.setFixedSize(400, 205);
    inputDialog.setStyleSheet(
        "QDialog { "
        "   background-color: #0A0A0A; "
        "   font-family: 'Segoe UI', Arial, sans-serif; "
        "}"
        "QLabel { "
        "   color: #6B6B6B; "
        "   font-size: 13px; "
        "   font-weight: bold; "
        "   padding: 5px; "
        "}"
        "QLineEdit { "
        "   background-color: #1A1A1A; "
        "   color: #FFFFFF; "
        "   border: 2px solid #2A2A2A; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   font-size: 15px; "
        "}"
        "QLineEdit:focus { border-color: #B5656B; }"
        "QPushButton { "
        "   color: #FFFFFF; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 8px 24px; "
        "   font-size: 15px; "
        "   font-weight: 600; "
        "   min-width: 115px; "
        "   min-height: 22px; "
        "}"
        "QPushButton#okButton { "
        "   background-color: #B5656B; "
        "}"
        "QPushButton#okButton:hover { background-color: #C96969; }"
        "QPushButton#okButton:pressed { background-color: #9B5A5F; }"
        "QPushButton#cancelButton { "
        "   background-color: #2A2A2A; "
        "}"
        "QPushButton#cancelButton:hover { background-color: #3A3A3A; }"
        "QPushButton#cancelButton:pressed { background-color: #1A1A1A; }"
    );

    auto *layout = new QVBoxLayout(&inputDialog);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    auto *label = new QLabel("Введите название подкаста:");
    layout->addWidget(label);

    auto *lineEdit = new QLineEdit;
    lineEdit->setText("Мой подкаст");
    lineEdit->setPlaceholderText("Введите название подкаста");
    layout->addWidget(lineEdit);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(12);

    auto *okButton = new QPushButton("OK");
    okButton->setObjectName("okButton");
    auto *cancelButton = new QPushButton("Отмена");
    cancelButton->setObjectName("cancelButton");

    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch(1);

    layout->addLayout(buttonLayout);

    QObject::connect(okButton, &QPushButton::clicked, [&inputDialog, &lineEdit]() {
        if (!lineEdit->text().trimmed().isEmpty()) {
            inputDialog.accept();
        }
    });
    QObject::connect(cancelButton, &QPushButton::clicked, [&inputDialog]() {
        inputDialog.reject();
    });
    QObject::connect(lineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);

    lineEdit->setFocus();
    lineEdit->selectAll();

    if (inputDialog.exec() == QDialog::Accepted) {
        QString podcastName = lineEdit->text().trimmed();
        if (!podcastName.isEmpty()) {
            clientName = nameEdit->text().trimmed();
            creatorMode = true;
            emit createRoomRequested(podcastName);

            MainWindow *mainWindow = new MainWindow;
            mainWindow->setPodcastName(podcastName);
            mainWindow->setHostName(clientName);
            mainWindow->setWindowTitle("Мини-подкаст: " + podcastName);
            mainWindow->show();
            
            nameEdit->clear();
            errorLabel->clear();
        }
    }
}

void StartDialog::onJoinRoomClicked()
{
    if (nameEdit->text().trimmed().isEmpty()) {
        errorLabel->setText("⚠️ Введите ваше имя ⚠️");
        return;
    }
    if (codeEdit->text().trimmed().isEmpty()) {
        errorLabel->setText("⚠️ Введите код комнаты ⚠️");
        return;
    }
    errorLabel->clear();

    clientName = nameEdit->text().trimmed();
    roomCode = codeEdit->text().trimmed();
    creatorMode = false;

    emit joinRoomRequested(roomCode, clientName);
    
    MainWindow *mainWindow = new MainWindow;
    mainWindow->setPodcastName("Комната " + roomCode);
    mainWindow->setWindowTitle("Мини-подкаст: " + roomCode);
    mainWindow->show();
    
    nameEdit->clear();
    codeEdit->clear();
    errorLabel->clear();
}