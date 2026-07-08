#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>

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
    void loadSettings();
    void saveSettings();
};