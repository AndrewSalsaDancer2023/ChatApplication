#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "authenticationform.h"
#include "stringlistmodel.h"
#include "coordinator.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include  <initializer_list>
#include <chrono>
/*
template <typename T>
void f(T v)
{
    std::cout << v << std::endl;
}
*/
//https://qml.ucoz.com/publ/stati_po_qml/integracija_qml_i_c/5-1-0-14
//https://evileg.com/ru/post/569/
//https://radioprog.ru/post/1531 https://evileg.com/ru/post/305/

//https://ru.cppreference.com/w/cpp/atomic/memory_order
//https://ru.cppreference.com/w/cpp/language/eval_order
//https://ru.cppreference.com/w/cpp/language/reference
//https://ru.cppreference.com/w/cpp/language/reference_initialization#.D0.92.D1.80.D0.B5.D0.BC.D1.8F_.D0.B6.D0.B8.D0.B7.D0.BD.D0.B8_.D0.B2.D1.80.D0.B5.D0.BC.D0.B5.D0.BD.D0.BD.D0.BE.D0.B3.D0.BE_.D0.BE.D0.B1.D1.8A.D0.B5.D0.BA.D1.82.D0.B0
//https://habr.com/ru/articles/517918/

//atomics
//https://linuxhint.com/cpp-std-atomic/
//https://www.cs.sfu.ca/~ashriram/Courses/CS431/assets/lectures/Part5/Atomics.pdf
//https://www.boost.org/doc/libs/1_78_0/doc/html/atomic.html
//https://www.youtube.com/watch?v=DS2m7T6NKZQ
//https://www.youtube.com/playlist?list=PLS0ecZsqDIUyAWsk63m8lB6o6-udJD724

std::string getTimeStr()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string s(50, '\0');
    std::strftime(&s[0], s.size(), "%H:%M:%S %Y-%m-%d", std::localtime(&now));
//     std::strftime(&s[0], s.size(), "%d-%m-%Y %H:%M:%S", std::localtime(&now));
    return s;
}
//https://techoverflow.net/2023/07/31/how-to-convert-stdchronotime_point-to-seconds-since-epoch/
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<AuthenticationForm>("Authentication", 1, 0, "AuthenticationForm");

//    qmlRegisterType<Coordinator>("Coordinator", 1, 0, "Coordinator");
//    qmlRegisterType<ChatListModel>("ChatListModel", 1, 0, "chatsList");
    QQmlApplicationEngine engine;

    /*
    PersonInfo one{"Bill Smith", "555 3264"};
    PersonInfo two{"John Brown", "555 8426"};
    PersonInfo three{"Sam Wise", "555 0473"};

    ChatListModel chatList;

    chatList.addData(one);
    chatList.addData(two);
    chatList.addData(three);

    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("chatsList", &chatList);
*/
    Coordinator coord;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("Coordinator", &coord);

    StringListModel& chatList = coord.getChatsList();
    ctxt->setContextProperty("chatsList", &chatList);

    StringListModel& partcpants = coord.getParticipantsList();
//    QStringListModel& partcpants = coord.getParticipantsList();
    ctxt->setContextProperty("participantsList", &partcpants);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
/*
void saveToCollection(mongocxx::collection& collection, int aqi)
{
    auto timeStamp = bsoncxx::types::b_date(std::chrono::system_clock::now());


    bsoncxx::builder::stream::document aqiDoc = bsoncxx::builder::stream::document{};
    aqiDoc << "timestamp" << timeStamp << "aqi" << aqi;
    collection.insert_one(aqiDoc.view());


    // Log to the console window.
    cout << " TimeStamp: " << timeStamp << " AQI: " << aqi << endl;
}

bsoncxx::document::value createChatDocument(const Database::chatMessage& curMsg)
{
    auto time = bsoncxx::types::b_date(curMsg.message.time);
    auto document = make_document(kvp("from", curMsg.message.userNickName), kvp("message", curMsg.message.userMessage), kvp("time", time));

    return document;
}

struct userMessage
{
    std::string userNickName;
    std::string userMessage;
    std::chrono::time_point<std::chrono::system_clock> time;
//    std::chrono::time_point<std::chrono::high_resolution_clock> time;
};

struct chatMessage
{
    std::string chatTitle;
    userMessage message;
    std::vector<userMessage> comments;
};

http://mongocxx.org/api/mongocxx-3.0.0/types_8hpp_source.html

https://protobuf.dev/reference/cpp/api-docs/google.protobuf.util.time_util/
https://stackoverflow.com/questions/48065142/how-do-i-rebuild-a-date-from-protobuf-timestamp-in-c
#include <chrono>

Settings s;

if (s.has_timestamp()) {
    chrono::nanoseconds ns = chrono::nanoseconds(google::protobuf::util::TimeUtil::TimestampToNanoseconds(s.timestamp()));
    chrono::time_point<chrono::system_clock> timestamp(ns);
}
*/

//http://thisthread.blogspot.com/2018/03/boost-asio-strand-example.html
//https://copyprogramming.com/howto/what-is-the-advantage-of-strand-in-boost-asio
//https://habr.com/ru/articles/195006/ https://zhaowuluo.wordpress.com/2010/12/25/multithreading-boostasio/
