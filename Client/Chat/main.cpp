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
//https://qna.habr.com/q/1007635

#include <map>
#include <unordered_map>

using namespace std;

// Function to print all subarrays with a zero-sum in a given array
void printAllSubarrays(int nums[], int n)
{
    // create an empty multimap to store the ending index of all
    // subarrays having the same sum
    unordered_multimap<int, int> map;

    // insert (0, -1) pair into the map to handle the case when
    // subarray with zero-sum starts from index 0
    map.insert(pair<int, int>(0, -1));

    int sum = 0;

    // traverse the given array
    for (int i = 0; i < n; i++)
    {
        // sum of elements so far
        sum += nums[i];

        // if the sum is seen before, there exists at least one
        // subarray with zero-sum
        if (map.find(sum) != map.end())
        {
            auto it = map.find(sum);

            // find all subarrays with the same sum
            while (it != map.end() && it->first == sum)
            {
                cout << "Subarray [" << it->second + 1 << "…" << i << "]\n";
                it++;
            }
        }

        // insert (sum so far, current index) pair into multimap
        map.insert(pair<int, int>(sum, i));
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<AuthenticationForm>("Authentication", 1, 0, "AuthenticationForm");

//    qmlRegisterType<Coordinator>("Coordinator", 1, 0, "Coordinator");
//    qmlRegisterType<ChatListModel>("ChatListModel", 1, 0, "chatsList");

    ///////////////////////////////////////////////////////////

    int nums[] = { 3, 4, -7, 3, 1, 3, 1, -4, -2, -2 };
    int n = sizeof(nums)/sizeof(nums[0]);

     printAllSubarrays(nums, n);
/*
    std::multimap<int, int> map;

    map.insert(std::pair<int, int>(0, -1));
    map.insert(std::pair<int, int>(0, 1));
    map.insert(std::pair<int, int>(0, 2));
    map.insert(std::pair<int, int>(0, 3));
    map.insert(std::pair<int, int>(0, 4));

    auto it = map.find(0);
    while (it != map.end())
    {
        std::cout << "Subarray [" << it->second + 1 << "]\n";
        it++;
    }
*/
    int k = 10;
    ///////////////////////////////////////////////////////////
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
/*
cmake -S ./test_cmake -B ./debug -G "CodeBlocks - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug
cmake --build ./debug
*/
