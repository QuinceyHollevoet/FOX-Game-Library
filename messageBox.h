#include <QMessageBox>
#include <QWidget>
#include <QString>

class MsgBox {
public:
    enum Type { Info, Success, Warning, Error };

    static void show(QWidget* parent, Type type, const QString& title, const QString& text) {
        QMessageBox msgBox(parent);
        msgBox.setText(text);
        msgBox.setWindowTitle(title);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setStyleSheet(parent ? parent->styleSheet() : "");

        switch (type) {
            case Info:
                msgBox.setIcon(QMessageBox::Information);
                break;
            case Success:
                msgBox.setIcon(QMessageBox::Information);
                if (title.isEmpty()) msgBox.setWindowTitle("Success");
                break;
            case Warning:
                msgBox.setIcon(QMessageBox::Warning);
                if (title.isEmpty()) msgBox.setWindowTitle("Warning");
                break;
            case Error:
                msgBox.setIcon(QMessageBox::Critical);
                if (title.isEmpty()) msgBox.setWindowTitle("Error");
                break;
        }

        msgBox.exec();
    }

    static bool ask(QWidget* parent, const QString& title, const QString& text) {
        QMessageBox msgBox(parent);
        msgBox.setWindowTitle(title.isEmpty() ? "Confirm" : title);
        msgBox.setText(text);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        
        msgBox.setStyleSheet(parent ? parent->styleSheet() : "");

        return msgBox.exec() == QMessageBox::Yes;
    }
};
