#ifndef WELCOMETAB_H
#define WELCOMETAB_H

#include <QWidget>
#include <QPushButton>

class WelcomeTab : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeTab(QWidget *parent = nullptr);

signals:
    // Phát ra khi người dùng bấm nút "Open Folder"
    void openFolderRequested();

private:
    QPushButton *openFolderButton;
};

#endif // WELCOMETAB_H
