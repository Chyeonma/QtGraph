#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include "settingsmanager.h"
#include <QWidget>

class QLabel;
class QComboBox;
class QSpinBox;
class QPushButton;
class QCheckBox;

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(SettingsManager *settingsManager, QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void loadSettingsIntoForm();

private:
    SettingsManager *settingsManager;
    QLabel *settingsPathLabel;
    QLabel *statusLabel;
    QComboBox *uiFontCombo;
    QSpinBox *uiFontSizeSpinBox;
    QComboBox *editorFontCombo;
    QSpinBox *editorFontSizeSpinBox;
    QCheckBox *defaultLineNumbersCheckBox;
    QPushButton *saveButton;
    QPushButton *resetButton;
};

#endif // SETTINGSTAB_H
