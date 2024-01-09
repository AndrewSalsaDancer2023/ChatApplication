#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "authenticationform.h"
#include "stringlistmodel.h"
#include "conversationmodel.h"
#include "participantmodel.h"
#include "coordinator.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include  <initializer_list>
//#include <chrono>
#include <cstdint>
#include <cinttypes>
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

//https://techoverflow.net/2023/07/31/how-to-convert-stdchronotime_point-to-seconds-since-epoch/
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<AuthenticationForm>("Authentication", 1, 0, "AuthenticationForm");

//    qmlRegisterType<Coordinator>("Coordinator", 1, 0, "Coordinator");
//    qmlRegisterType<ChatListModel>("ChatListModel", 1, 0, "chatsList");
    QQmlApplicationEngine engine;
    Coordinator coord;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("Coordinator", &coord);

    StringListModel& chatList = coord.getChatsList();
    ctxt->setContextProperty("chatsList", &chatList);

    ParticipantModel& members = coord.getMembersList();
    ctxt->setContextProperty("members", &members);

    ConversationModel& conversation = coord.getConversationModel();
    ctxt->setContextProperty("conversation", &conversation);

    ParticipantModel& allUsers = coord.getAllUsers();
    ctxt->setContextProperty("allUsers", &allUsers);

    ParticipantModel& participants = coord.getParticipants();
    ctxt->setContextProperty("participants", &participants);

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
bsoncxx::document::value createChatDocument(const Database::chatMessage& curMsg)
{
    auto time = bsoncxx::types::b_date(curMsg.message.time);
    auto document = make_document(kvp("from", curMsg.message.userNickName), kvp("message", curMsg.message.userMessage), kvp("time", time));

    return document;
}
corutines
https://habr.com/ru/companies/piter/articles/491996/
https://habr.com/ru/companies/yandex/articles/420861/
https://habr.com/ru/companies/ruvds/articles/755246/
https://habr.com/ru/companies/wunderfund/articles/582000/

*/
//http://thisthread.blogspot.com/2018/03/boost-asio-strand-example.html
//https://copyprogramming.com/howto/what-is-the-advantage-of-strand-in-boost-asio
//https://habr.com/ru/articles/195006/ https://zhaowuluo.wordpress.com/2010/12/25/multithreading-boostasio/
