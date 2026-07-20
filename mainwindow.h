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
#include <QAudioSink>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QTime> 
#include <QTcpSocket>
#include <QJsonObject>

struct ChatMessage {
    QString sender;      // кто отправил
    QString text;        // текст
    bool isLocal;        // true = я отправил, false = от сервера
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(bool isHost = false, QWidget *parent = nullptr);
    ~MainWindow();
    void setPodcastName(const QString &name);
    void setHostName(const QString &name);
    void addListener(const QString &name);
    void setRoomCode(const QString &code);
    void attachSocket(QTcpSocket *sock);
    void setMyName(const QString &name);

private slots:
    void openSettings();
    void sendMessage();
    void onMessageReceived(const ChatMessage &msg);
    void onServerDataReceived();

private:
    bool isHostMode;

    QPushButton *settingsBtn;
    QSettings *settings;
    
    // Аудио (Ввод)
    QAudioSource *audioSource;
    QIODevice *audioDevice;
    QFrame *activeHostTile;
    QTimer *vuTimer;
    
    // Аудио (Вывод)
    QAudioSink *audioSink;
    QIODevice *audioOutputDevice;
    
    QComboBox *micDeviceList;
    QComboBox *speakerDeviceList;

    // Чат
    QTextEdit *chatDisplay;
    QLineEdit *chatInput;
    
    // Для списка слушателей
    QVBoxLayout *listenersListLayout;
    QLabel *listenersCountLabel;
    QWidget *listenersListWidget;
    int listenerCount;

    // Таймер подкаста
    QTimer *podcastTimer;
    QLabel *durationLabel;
    QTime podcastStartTime;
    int podcastDurationSeconds;

    // Название подкаста
    QString podcastName;
    QLabel *meetingName;
    QLabel *host1NameLabel;
    QLabel *roomCodeLabel;

    QTcpSocket *socket;

    void loadSettings();
    void saveSettings();
    void initMicrophone();
    void addMessageToChat(const ChatMessage &msg);
    void restartAudioCapture(const QAudioDevice &device);
    void playTestSound(const QAudioDevice &device);
    
    QWidget* createListenerRow(const QString &name);
    
    void updateListenersCount();
    void showAddListenerDialog();

    void updateDurationDisplay();
    void startPodcastTimer();

    QByteArray receiveBuffer;
    QByteArray audioBuffer;

    QString myName;
    QLabel *host1Avatar;

    // Состояния заглушения
    bool isMicMuted = false;
    bool isSpeakerMuted = false;
    
    // Указатели на иконки кнопок для смены эмодзи
    QPushButton *micIconBtn = nullptr;
    QPushButton *speakerIconBtn = nullptr;

    // Новые методы
    void toggleMicMute();
    void toggleSpeakerMute();
};