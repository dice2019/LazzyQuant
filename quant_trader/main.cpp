#include <QCoreApplication>

#include "config.h"
#include "quant_trader.h"
#include "strategy_status.h"
#include "connection_manager.h"

#include "sinyee_replayer_interface.h"
#include "market_watcher_interface.h"
#include "trade_executer_interface.h"

com::lazzyquant::sinyee_replayer *pReplayer = nullptr;
com::lazzyquant::market_watcher *pWatcher = nullptr;
com::lazzyquant::trade_executer *pExecuter = nullptr;
StrategyStatusManager *pStatusManager = nullptr;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("quant_trader");
    QCoreApplication::setApplicationVersion(VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription("Lazzy Quant Trader.");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        // replay mode (-r, --replay)
        {{"r", "replay"},
            QCoreApplication::translate("main", "Replay Mode"), "ReplayDate"},
    });

    parser.process(a);
    bool replayMode = parser.isSet("replay");
    QString replayDate = QString();
    if (replayMode) {
        replayDate = parser.value("replay");
    }

    if (replayMode) {
        qDebug() << "run in replay mode";
        pReplayer = new com::lazzyquant::sinyee_replayer(REPLAYER_DBUS_SERVICE, REPLAYER_DBUS_OBJECT, QDBusConnection::sessionBus());
    } else {
        qDebug() << "run in real mode";
        pWatcher = new com::lazzyquant::market_watcher(WATCHER_DBUS_SERVICE, WATCHER_DBUS_OBJECT, QDBusConnection::sessionBus());
    }
    pExecuter = new com::lazzyquant::trade_executer(EXECUTER_DBUS_SERVICE, EXECUTER_DBUS_OBJECT, QDBusConnection::sessionBus());
    pStatusManager = new StrategyStatusManager();
    QuantTrader quantTrader;
    ConnectionManager manager({pReplayer, pWatcher}, {&quantTrader});
    if (replayMode) {
        pReplayer->startReplay(replayDate);
    }
    int ret = a.exec();
    delete pStatusManager;
    delete pExecuter;
    if (pReplayer) {
        delete pReplayer;
    }
    if (pWatcher) {
        delete pWatcher;
    }
    return ret;
}
