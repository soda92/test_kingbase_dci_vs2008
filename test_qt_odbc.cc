#include <QCoreApplication>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QTimer>
#include <QString>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QKINGBASE");
    QTimer::singleShot(0, &app, SLOT(quit()));
    return app.exec();
}