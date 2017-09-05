#include "common_utility.h"

#include <QMap>
#include <QFile>
#include <QSettings>
#include <QCoreApplication>

/*!
 * \brief getInstrumentName
 * 从期货或期权合约代码里提取交易代码
 * 比如 cu1703 --> cu, i1705 --> i, CF705 --> CF
 *      m1707-C-2700 --> m
 *
 * \param instrumentID 期货/期权合约代码
 * \return 交易品种名
 */
QString getCode(const QString &instrumentID)
{
    const int len = instrumentID.length();
    int i = 0;
    for (; i < len; i++) {
        if (instrumentID[i].isDigit()) {
            break;
        }
    }
    return instrumentID.left(i);
}

/*!
 * \brief parseOptionID
 * 解析期权合约代码, 提取其对应标的期货合约的代码, 期权类型(看涨/看跌) 以及执行价格
 * 比如 m1707-C-2700 --> 标的m1707, 类型为看涨, 执行价格2700
 *     SR705P6400   --> 标的SR705, 类型为看跌, 执行价格6400
 *
 * \param optionID 期权合约代码
 * \param futureID 标的期货合约代码 (输出参数)
 * \param type 看涨/看跌 (输出参数)
 * \param exercisePrice 执行价格 (输出参数)
 * \return 解析成功与否
 */
bool parseOptionID(const QString &optionID, QString &futureID, OPTION_TYPE &type, int &exercisePrice)
{
    const int len = optionID.length();
    int i = 0;
    for (; i < len; i++) {
        if (optionID[i].isDigit()) {
            break;
        }
    }
    for (; i < len; i++) {
        if (!optionID[i].isDigit()) {
            break;
        }
    }
    futureID = optionID.left(i);
    for (; i < len; i++) {
        if (optionID[i].isLetter()) {
            break;
        }
    }
    type = (optionID[i] == "C" ? CALL_OPT : PUT_OPT);
    for (; i < len; i++) {
        if (optionID[i].isDigit()) {
            break;
        }
    }
    exercisePrice = optionID.right(len - i).toInt();
    return true;    // TODO successfull or not
}

/*!
 * \brief makeOptionID
 * 根据期权合约中规定的格式合成期权合约代码
 *
 * \param futureID 标的期货合约代码
 * \param type 看涨/看跌
 * \param exercisePrice 执行价格
 * \return 期权合约代码
 */
QString makeOptionID(const QString &futureID, const OPTION_TYPE type, const int exercisePrice)
{
    static const auto optionIdPatternMap = []() -> QMap<QString, QString> {
        QMap<QString, QString> map;
        map.insert("m", "%1-%2-%3");
        map.insert("SR", "%1%2%3");
        return map;
    }();

    QString middle = (type == CALL_OPT) ? "C" : "P";
    return optionIdPatternMap[getCode(futureID)].arg(futureID).arg(middle).arg(exercisePrice);
}

/*!
 * \brief isOption
 * 判断一个合约代码是否是期权合约
 *
 * \param instrumentID 合约代码
 * \return 是否是期权合约
 */
bool isOption(const QString &instrumentID)
{
    return instrumentID.length() >= 8;  // FIXME
}

/*!
 * \brief getSettingsSmart
 * 如果存在本地配置文件, 则使用本地配置文件
 * 如果不存在本地配置文件, 则使用UserScope配置文件
 *
 * \param organization 公司或组织名
 * \param name 配置文件名
 * \return 配置文件对应的QSettings智能指针
 */
std::unique_ptr<QSettings> getSettingsSmart(const QString &organization, const QString &name, QObject *parent)
{
    const QString localFileName = QCoreApplication::applicationDirPath() + "/" + name + ".ini";
    QFile localFile(localFileName);
    if (localFile.exists()) {
        return std::unique_ptr<QSettings>(new QSettings(localFileName, QSettings::IniFormat, parent));
    } else {
        return std::unique_ptr<QSettings>(new QSettings(QSettings::IniFormat, QSettings::UserScope, organization, name, parent));
    }
}

#define String const QString&

// 通过合约名获得文件的扩展名
QString getSuffix(String instrumentID) {
    const QString instrumentLowerCase = getCode(instrumentID).toLower();
    for (String instr : SQ) {
        if (instrumentLowerCase == instr) {
            return ".SQ";
        }
    }
    for (String instr : SY) {
        if (instrumentLowerCase == instr) {
            return ".SY";
        }
    }
    for (String instr : DL) {
        if (instrumentLowerCase == instr) {
            return ".DL";
        }
    }
    for (String instr : DY) {
        if (instrumentLowerCase == instr) {
            return ".DY";
        }
    }
    for (String instr : ZZ) {
        if (instrumentLowerCase == instr) {
            return ".ZZ";
        }
    }
    for (String instr : ZY) {
        if (instrumentLowerCase == instr) {
            return ".ZY";
        }
    }
    for (String instr : ZJ) {
        if (instrumentLowerCase == instr) {
            return ".ZJ";
        }
    }
    return ".notfound";
}
#undef String
