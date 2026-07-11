#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QAudioSource>
#include <QIODevice>
#include <QFrame>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSettings();

private:
    QPushButton *settingsBtn;
    QSettings *settings;
    
    // Аудио
    QAudioSource *audioSource;
    QIODevice *audioDevice;
    QFrame *activeHostTile;
    
    void loadSettings();
    void saveSettings();
    void initMicrophone();
};