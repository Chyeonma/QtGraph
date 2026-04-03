#ifndef SETTINGSTAB_H
#define SETTINGSTAB_H

#include "services/settingsservice.h"
#include <QWidget>

class IFontProvider;
class QLabel;
class QComboBox;
class QSpinBox;
class QPushButton;
class QCheckBox;

class SettingsTab : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsTab(IFontProvider *fontProvider, SettingsService *settingsService, QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void loadSettingsIntoForm();

private:
    IFontProvider *fontProvider;
    SettingsService *settingsService;
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
