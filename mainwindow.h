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
<<<<<<< HEAD
=======
#include <QAudioSink>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QTime> 
>>>>>>> Ann_work

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
<<<<<<< HEAD
=======
    void setPodcastName(const QString &name);
>>>>>>> Ann_work

private slots:
    void openSettings();
    void sendMessage();
    void onMessageReceived(const ChatMessage &msg);

private:
    QPushButton *settingsBtn;
    QSettings *settings;
    
<<<<<<< HEAD
    // Аудио
=======
    // Аудио (Ввод)
>>>>>>> Ann_work
    QAudioSource *audioSource;
    QIODevice *audioDevice;
    QFrame *activeHostTile;
    QTimer *vuTimer;
    
<<<<<<< HEAD
    // ComboBox'ы для устройств
=======
    // Аудио (Вывод)
    QAudioSink *audioSink;
    QIODevice *audioOutputDevice;
    
>>>>>>> Ann_work
    QComboBox *micDeviceList;
    QComboBox *speakerDeviceList;

    // Чат
    QTextEdit *chatDisplay;
    QLineEdit *chatInput;
    
<<<<<<< HEAD
=======
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

>>>>>>> Ann_work
    void loadSettings();
    void saveSettings();
    void initMicrophone();
    void addMessageToChat(const ChatMessage &msg);
    void restartAudioCapture(const QAudioDevice &device);
<<<<<<< HEAD
=======
    void playTestSound(const QAudioDevice &device);
    
    QWidget* createListenerRow(const QString &name);
    void addListener(const QString &name);
    void updateListenersCount();
    void showAddListenerDialog();

    void updateDurationDisplay();
    void startPodcastTimer();
>>>>>>> Ann_work
};