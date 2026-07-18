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

    void loadSettings();
    void saveSettings();
    void initMicrophone();
    void addMessageToChat(const ChatMessage &msg);
    void restartAudioCapture(const QAudioDevice &device);
    void playTestSound(const QAudioDevice &device);
    
    QWidget* createListenerRow(const QString &name);
    void addListener(const QString &name);
    void updateListenersCount();
    void showAddListenerDialog();
};