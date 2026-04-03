#include "settingstab.h"
#include "interfaces/ifontprovider.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsTab::SettingsTab(IFontProvider *fontProvider, SettingsService *settingsService, QWidget *parent)
    : QWidget(parent)
    , fontProvider(fontProvider)
    , settingsService(settingsService)
{
    setStyleSheet(
        "QWidget { background: #1e1e1e; color: #d4d4d4; }"
        "QGroupBox { border: 1px solid #333333; border-radius: 8px; margin-top: 16px; padding: 14px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; color: #ffffff; }"
        "QComboBox, QSpinBox { background: #2d2d2d; color: #ffffff; border: 1px solid #444444; border-radius: 6px; padding: 6px 8px; }"
        "QPushButton { background: #0e639c; color: #ffffff; border: none; border-radius: 6px; padding: 8px 14px; }"
        "QPushButton:hover { background: #1177bb; }"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(18);

    QLabel *titleLabel = new QLabel("Setting");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QLabel *descriptionLabel = new QLabel("Chinh font giao dien va font cho editor. Ban co the chon bat ky font nao da duoc bundle trong du an, va cau hinh se duoc luu vao thu muc settings.");
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet("color: #a8a8a8;");

    settingsPathLabel = new QLabel("Settings file: " + settingsService->settingsFilePath());
    settingsPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    settingsPathLabel->setStyleSheet("color: #7fba7a;");

    QGroupBox *fontGroup = new QGroupBox("Font");
    QFormLayout *fontLayout = new QFormLayout(fontGroup);
    fontLayout->setLabelAlignment(Qt::AlignLeft);
    fontLayout->setFormAlignment(Qt::AlignTop);
    fontLayout->setHorizontalSpacing(16);
    fontLayout->setVerticalSpacing(12);

    const QStringList fontFamilies = fontProvider->availableFontFamilies();

    uiFontCombo = new QComboBox();
    uiFontCombo->addItems(fontFamilies);

    uiFontSizeSpinBox = new QSpinBox();
    uiFontSizeSpinBox->setRange(8, 24);

    editorFontCombo = new QComboBox();
    editorFontCombo->addItems(fontFamilies);

    editorFontSizeSpinBox = new QSpinBox();
    editorFontSizeSpinBox->setRange(9, 24);

    defaultLineNumbersCheckBox = new QCheckBox("Show line numbers by default for text tabs");
    defaultLineNumbersCheckBox->setStyleSheet("color: #d4d4d4;");

    fontLayout->addRow("UI Font", uiFontCombo);
    fontLayout->addRow("UI Font Size", uiFontSizeSpinBox);
    fontLayout->addRow("Editor Font", editorFontCombo);
    fontLayout->addRow("Editor Font Size", editorFontSizeSpinBox);
    fontLayout->addRow("", defaultLineNumbersCheckBox);

    statusLabel = new QLabel("Chua co thay doi nao.");
    statusLabel->setStyleSheet("color: #9cdcfe;");

    saveButton = new QPushButton("Save");
    resetButton = new QPushButton("Reset Default");
    resetButton->setStyleSheet(
        "QPushButton { background: #3a3a3a; color: #ffffff; border: none; border-radius: 6px; padding: 8px 14px; }"
        "QPushButton:hover { background: #4a4a4a; }"
    );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();

    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("color: #333333;");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(settingsPathLabel);
    mainLayout->addWidget(divider);
    mainLayout->addWidget(fontGroup);
    mainLayout->addWidget(statusLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();

    connect(saveButton, &QPushButton::clicked, this, &SettingsTab::saveSettings);
    connect(resetButton, &QPushButton::clicked, settingsService, &SettingsService::resetToDefaults);
    connect(settingsService, &SettingsService::settingsChanged, this, &SettingsTab::loadSettingsIntoForm);

    loadSettingsIntoForm();
}

void SettingsTab::saveSettings()
{
    AppSettings newSettings;
    newSettings.uiFontFamily = uiFontCombo->currentText();
    newSettings.uiFontSize = uiFontSizeSpinBox->value();
    newSettings.editorFontFamily = editorFontCombo->currentText();
    newSettings.editorFontSize = editorFontSizeSpinBox->value();
    newSettings.defaultShowLineNumbers = defaultLineNumbersCheckBox->isChecked();

    if (settingsService->applySettings(newSettings)) {
        statusLabel->setText("Da luu setting thanh cong.");
    } else {
        statusLabel->setText("Khong the luu file setting.");
    }
}

void SettingsTab::loadSettingsIntoForm()
{
    const AppSettings &current = settingsService->settings();

    uiFontCombo->setCurrentText(current.uiFontFamily);
    uiFontSizeSpinBox->setValue(current.uiFontSize);
    editorFontCombo->setCurrentText(current.editorFontFamily);
    editorFontSizeSpinBox->setValue(current.editorFontSize);
    defaultLineNumbersCheckBox->setChecked(current.defaultShowLineNumbers);

    statusLabel->setText("Dang dung setting da luu.");
}
