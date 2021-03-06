#include "codexdatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QPluginLoader>

Term::Term (CodexDatabase* db, QString target, QString translation) :
    linkedDb(db),
    targetStr(target),
    ease(2.5f),
    reps(1),
    interval(1),
    dateDue(QDate::currentDate())
{
    translations.push_back(translation);
}

Term::Term (CodexDatabase* db, QSqlRecord rec) : linkedDb(db)
{
    targetStr = rec.value("target").toString();
    ease = rec.value("ease").toDouble();
    reps = rec.value("reps").toInt();
    interval = rec.value("interval").toInt();
    dateDue = rec.value("date_due").toDate();
    //qDebug() << "Due Date is: " << dateDue;
    auto numTranslations = rec.value("numTranslations").toInt();
    for (int i = 0; i < numTranslations; ++i)
    {
        QString name = "tran" + QString::number(i);
        auto value = rec.value(name).toString();
        translations.push_back(value);
    }
}

Term::Term (const Term& other) : linkedDb(other.linkedDb)
{
    targetStr = other.targetStr;
    ease = other.ease;
    reps = other.reps;
    interval = other.interval;
    dateDue = other.dateDue;
    translations = other.translations;
}

// syntax:
// INSERT INTO terms(target, numTranslations, tran0, ease, reps, delay_interval, date_due);
// VALUES(target, numTranslations, tran0, ease, reps, delay_interval, date_due);
QString Term::sqlInsertCommand()
{
    QString insertStr = "INSERT INTO terms(target, numTranslations, ";
    for (int i = 0; i < (int)translations.size(); ++i)
    {
        QString tStr = "tran" + QString::number(i);
        insertStr += tStr + ", ";
    }
    insertStr += "ease, reps, delay_interval, date_due)";
    QString valueStr = "VALUES(";
    valueStr += ":target, :numTranslations, ";
    for (int i = 0; i < (int)translations.size(); ++i)
    {
        auto tStr = ":tran" + QString::number(i) + ", ";
        valueStr += tStr;
    }
    valueStr += ":ease, :reps, :delay_interval, :date_due);";
    return insertStr + valueStr;
}
 QSqlQuery Term::preparedInsertQuery(QSqlDatabase& db)
 {
     QSqlQuery query(db);
     query.prepare(sqlInsertCommand());
     bindValuesToQuery(query);
     return query;
 }
QSqlQuery Term::preparedUpdateQuery(QSqlDatabase& db)
{
    QSqlQuery query(db);
    QString cmd = "UPDATE terms SET numTranslations = :numTranslations, ";
    for (int i = 0; i < (int)translations.size(); ++i)
    {
        QString tStr = "tran" + QString::number(i);
        auto bindingStr = ":" + tStr;
        cmd += tStr;
        cmd += " = " + bindingStr + ", ";
    }
    cmd += "ease = :ease, reps = :reps, delay_interval = :delay_interval, date_due = :date_due ";
    cmd += "WHERE target = :target";
    query.clear();
    query.prepare(cmd);
    bindValuesToQuery(query);
    //auto fullQuery = query.lastQuery();
   // qDebug() << "Bound Query: " << fullQuery;
    return query;
}
float Term::getNormalizedEase()
{
    //returns the ease adjusted for the range 0 - 1
    return (float)(MAX_EASE / ease);
}

 void Term::bindValuesToQuery(QSqlQuery& query)
 {
     query.bindValue(":target", targetStr);
     //printf("Target: %s\n", targetStr.toStdString().c_str());
     query.bindValue(":numTranslations", (int)translations.size());
     for (int i = 0; i < (int)translations.size(); ++i)
     {
         auto str = ":tran" + QString::number(i);
         query.bindValue(str, translations[i]); }
     query.bindValue(":ease", ease);
     printf("Ease: %lf\n", ease);
     query.bindValue(":reps", reps);
     printf("Reps: %d\n", reps);
     query.bindValue(":delay_interval", interval);
     printf("Interval: %d\n", interval);
     auto dateStr = dateDue.toString(Qt::DateFormat::ISODate);
    // printf("Date due: %s\n", dateStr.toStdString().c_str());
     query.bindValue(":date_due", dateStr);
     printf ("Query values bound\n");
 }
 //returns the SM-2's delay length for a certain grade without updating the term's variables
 int Term::getIntervalFor(int grade)
 {
     if (grade >= 3)
     {
         if (reps == 0)
             return 1;
         else if (reps == 1)
             return 5;
         else
             return (int)std::floor((double)interval * ease);
     }
     return 1;
 }
 void Term::setDueAfter(int days)
 {
    auto today = QDate::currentDate();
    qDebug() << "Current Date: " << today;
    dateDue = today.addDays(days);
    qDebug() << "Date Due: " << dateDue;
 }
 QString CodexDatabase::sqlSelectCommand(const QString& word)
 {
     return "SELECT * FROM terms WHERE target = " + word + ";";
 }

 void Term::answerSM2(int grade)
 {
     interval = getIntervalFor(grade);
     if (grade >= 3)
         ++reps;
     else
         reps = 0;
     auto dEase = (0.1f - (float)(5.0f - grade) * (0.08f + (float)(5 - grade) * 0.02f));
     ease += dEase;
     setDueAfter(interval);
 }


//============================================================
void SqlUtil::setSafeMode(QSqlDatabase& db, bool isSafe)
{
    QString qStr = "SET SQL_SAFE_UPDATES = ";
    if (isSafe)
        qStr += "1;";
    else
        qStr += "0;";
    QSqlQuery query(qStr, db);
    query.exec();
}

CodexDatabase::CodexDatabase() :
    db (QSqlDatabase::addDatabase("QMYSQL")),
    userDb (nullptr)
{
    db.setHostName ("127.0.0.1");
    db.setUserName ("root");
    db.setPassword ("Nebuchadnezzar21");
    db.setDatabaseName ("codex_vocab");
    if(db.open())
        printf ("database opened\n");
    else
        printf( "Error: database not opened\n");
}

CodexDatabase::~CodexDatabase()
{
    if(db.isOpen())
        db.close();
}

bool CodexDatabase::attemptLogin(QString username, QString password)
{
    printf ("attempting login...\n");
    QSqlQuery query("SELECT * FROM users", db);
    while (query.next())
    {
        QString usr = query.value(0).toString();
        QString pswd = query.value(1).toString();
        if (usr == username && password == pswd)
        {
            printf ("user found\n");
            setUserDatabase(username);
            return true;
        }
    }
    printf ("no matching user found\n");
    return false;
}

void CodexDatabase::setUserDatabase(QString username)
{
    userDb = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    userDb->setHostName ("127.0.0.1");
    userDb->setUserName ("root");
    userDb->setPassword ("Nebuchadnezzar21");
    userDb->setDatabaseName (username);
    if (userDb->open())
        printf("User database opened\n");
    else
        printf("no user database!");

}

void CodexDatabase::addTerm(QString target, QString translation)
{
    auto term = new Term (this, target, translation);
    auto query = term->preparedInsertQuery (*userDb);
    if (query.exec())
        printf ("Term added\n");
    delete term;
}
//don't call this on an actual big database ofc
std::vector<Term*> CodexDatabase::getAllTerms()
{
    std::vector<Term*> output;
    QSqlQuery query("SELECT * FROM terms;", db);
    while (query.next())
    {
        auto record = query.record();
        output.push_back(new Term(this, record));
    }
    return output;
}

Term* CodexDatabase::getTerm(const QString& word)
{
    //TODO: figure out whether this is fast enough and what can be done if not
    auto cmd = sqlSelectCommand(word);
    QSqlQuery query(cmd, db);
    while (query.next())
    {
        auto rec = query.record();
        if (rec.value("target").toString() == word)
        {
            return new Term(this, rec);
        }
    }
    return nullptr;
}

QString CodexDatabase::sqlSelectDueTerms()
{
    auto dateStr = QDate::currentDate().toString(Qt::DateFormat::ISODate);
    //qDebug() << dateStr;
    return "SELECT * FROM terms WHERE date_due < \'" + dateStr + "\';";
}
void CodexDatabase::termsDueNow(std::vector<Term>& terms)
{
    terms.clear();
    QSqlQuery query(sqlSelectDueTerms(), db);
    while (query.next())
    {
        Term term(this, query.record());
        terms.push_back(term);
    }
}
void CodexDatabase::updateTerm(Term* term)
{
    auto updateQuery = term->preparedUpdateQuery(*userDb);
    if (updateQuery.exec())
        printf("Term updated on Server\n");
    else
        qDebug() << updateQuery.lastError();
}

std::vector<QString> CodexDatabase::getContentNames()
{
    std::vector<QString> names;
    QSqlQuery query("SELECT * FROM content;", *userDb);
    while(query.next())
    {
        auto rec = query.record();
        auto name = rec.value("content_name").toString();
        names.push_back(name);
    }


    return names;
}
QSqlRecord CodexDatabase::getContentRecord(const QString& name)
{
    QSqlQuery query("SELECT * FROM content;", *userDb);
    while (query.next())
    {
        auto rec = query.record();
        auto currentName = rec.value("content_name").toString();
        if(currentName == name)
        {
            return rec;
        }
    }
    return query.record();
}
std::vector<QSqlRecord> CodexDatabase::allLessonRecords()
{
    std::vector<QSqlRecord> records;
    QString qStr = "SELECT * FROM content;";
    QSqlQuery query(qStr, *userDb);
    while (query.next())
        records.push_back(query.record());
    return records;
}
