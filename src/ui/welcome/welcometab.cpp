#include "welcometab.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QFrame>

WelcomeTab::WelcomeTab(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #1e1e1e;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 36, 36, 36);
    layout->setSpacing(20);

    // ==========================================
    // LOGO / TIÊU ĐỀ
    // ==========================================

    QLabel *titleLabel = new QLabel("QtGraph");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #569cd6; font-weight: bold; background: transparent;");

    QLabel *subtitleLabel = new QLabel("Project Explorer and Code Preview Workspace");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #858585; font-size: 16px; background: transparent;");

    // ==========================================
    // LỜI CHÀO
    // ==========================================

    QLabel *welcomeLabel = new QLabel(
        "QtGraph giup ban mo mot thu muc du an, xem nhanh cau truc file, doc noi dung source code theo tab, "
        "va chuyen sang Canvas de nhin tong quan project theo kieu truc quan."
    );
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(
        "color: #cccccc; font-size: 14px; background: transparent; padding: 10px;"
    );
    welcomeLabel->setWordWrap(true);

    QFrame *featurePanel = new QFrame();
    featurePanel->setStyleSheet("QFrame { background: #252526; border: 1px solid #333333; border-radius: 10px; }");

    QVBoxLayout *featureLayout = new QVBoxLayout(featurePanel);
    featureLayout->setContentsMargins(20, 20, 20, 20);
    featureLayout->setSpacing(12);

    QLabel *featureTitle = new QLabel("Ban co the lam gi trong app?");
    featureTitle->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold; background: transparent;");

    QLabel *featureList = new QLabel(
        "- Open Folder de nap mot project vao cay thu muc\n"
        "- Mo file theo tab va doc source code co syntax highlight\n"
        "- Bat Canvas View de xem cac node file va folder\n"
        "- Dieu chinh font trong tab Setting de phu hop cach lam viec cua ban"
    );
    featureList->setStyleSheet("color: #bdbdbd; font-size: 13px; background: transparent;");
    featureList->setWordWrap(true);

    featureLayout->addWidget(featureTitle);
    featureLayout->addWidget(featureList);

    // ==========================================
    // NÚT BẤM
    // ==========================================

    openFolderButton = new QPushButton("Open Folder");
    openFolderButton->setCursor(Qt::PointingHandCursor);
    openFolderButton->setFixedSize(260, 48);
    openFolderButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #0e639c; color: #ffffff; border: none; border-radius: 6px;"
        "  font-size: 15px; font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #1177bb; }"
        "QPushButton:pressed { background-color: #0d5689; }"
    );

    // ==========================================
    // GỢI Ý PHÍM TẮT
    // ==========================================

    QLabel *hintLabel = new QLabel("Hoặc sử dụng File → Open Folder từ thanh menu");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet(
        "color: #5a5a5a; font-size: 12px; background: transparent;"
    );

    // ==========================================
    // GHÉP LAYOUT
    // ==========================================

    layout->addStretch(1);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addSpacing(12);
    layout->addWidget(welcomeLabel);
    layout->addWidget(featurePanel);
    layout->addSpacing(10);
    layout->addWidget(openFolderButton, 0, Qt::AlignCenter);
    layout->addWidget(hintLabel);
    layout->addStretch(2);

    // Kết nối nút bấm → signal
    connect(openFolderButton, &QPushButton::clicked, this, &WelcomeTab::openFolderRequested);
}
