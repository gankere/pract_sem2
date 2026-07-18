#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QAudioSource>
#include <QIODevice>
#include <QFrame>
#include <QTextEdit>
#include <QLineEdit>
#include <QString>
#include <QComboBox>
#include <QMediaDevices>
#include <QAudioDevice>

struct ChatMessage {
    QString sender;      // кто отправил
    QString text;        // текст
    bool isLocal;        // true = я отправил, false = от сервера
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSettings();
    void sendMessage();
    void onMessageReceived(const ChatMessage &msg);

private:
    QPushButton *settingsBtn;
    QSettings *settings;
    
    // Аудио
    QAudioSource *audioSource;
    QIODevice *audioDevice;
    QFrame *activeHostTile;
    QTimer *vuTimer;
    
    // ComboBox'ы для устройств
    QComboBox *micDeviceList;
    QComboBox *speakerDeviceList;

    // Чат
    QTextEdit *chatDisplay;
    QLineEdit *chatInput;
    
    void loadSettings();
    void saveSettings();
    void initMicrophone();
    void addMessageToChat(const ChatMessage &msg);
    void restartAudioCapture(const QAudioDevice &device);
};