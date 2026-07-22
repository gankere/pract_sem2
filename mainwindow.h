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
#include <QDateTime>       // <-- ДОБАВЛЕНО для работы с временем
#include <QTcpSocket>
#include <QJsonObject>
#include <QEnterEvent>
#include <QMap>

struct ChatMessage {
    QString sender;      // кто отправил
    QString text;        // текст
    bool isLocal;        // true = я отправил, false = от сервера
};

class ListenerRowWidget : public QWidget {
    Q_OBJECT
public:
    ListenerRowWidget(const QString &name, bool isHostMode, QWidget *parent = nullptr);
    virtual ~ListenerRowWidget() override;
    QString getListenerName() const { return listenerName; } 
    
signals:
    void promoteRequested(const QString &name);
    
protected:
    void enterEvent(QEnterEvent *event) override; 
    void leaveEvent(QEvent *event) override;      
    
private:
    QString listenerName;
    QPushButton *promoteBtn;
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
    void setPodcastStartTime(qint64 secs) { 
        podcastStartTimeSecs = secs; 
        startPodcastTimer(); 
    }
    void appendToBuffer(const QByteArray &data) { receiveBuffer.append(data); }

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
    qint64 podcastStartTimeSecs = 0;

    // Название подкаста
    QString podcastName;
    QLabel *meetingName;
    QLabel *host1NameLabel;
    QLabel *roomCodeLabel;
    
    // Для второго ведущего
    QFrame *host2Tile = nullptr;
    QLabel *host2Avatar = nullptr;
    QLabel *host2NameLabel = nullptr;
    QString host2ActualName;
    QWidget *micContainerWidget = nullptr;

    // Карта для быстрого удаления слушателя из списка
    QMap<QString, QWidget*> listenerWidgets;
    void promoteToHost(const QString &listenerName);

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

    QTcpSocket *socket = nullptr;

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