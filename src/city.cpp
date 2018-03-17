#include <QDebug>
#include "city.h"
#include "utils.h"
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <QTimer>
#include <QTextStream>
#include <QFile>
#include <QString>
#include <algorithm>
using namespace std;
QString ROAD_DATA_DIR = "/home/ypbehere/Documents/srtp/RoadAnt/RoadAnt/data/roadData.txt";
QString STORE_DATA_DIR = "/home/ypbehere/Documents/srtp/RoadAnt/RoadAnt/data/storeData.txt";

CCity::CCity()
{
    QFile roadData(ROAD_DATA_DIR);
    QFile storeData(STORE_DATA_DIR);
    if (!roadData.open(QIODevice::ReadOnly | QIODevice::Text) || !storeData.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cout << "Open Failed" << endl;
    }

    QTextStream roadDataStream(&roadData);
    int x, y, level;
    while (!roadDataStream.atEnd()) {
        roadDataStream >> x >> y;
        CPos start(x, y);
        roadDataStream >> x >> y;
        CPos end(x, y);
        roadDataStream >> level;
        CRoad r(start, end, level);
        _roadList.push_back(r);
    }

    QTextStream storeDataStream(&storeData);
    int dist;
    while (!storeDataStream.atEnd()) {
        storeDataStream >> x >> y;
        CPos start(x, y);
        storeDataStream >> x >> y;
        CPos end(x, y);
        auto tmpRoad = find_if(_roadList.begin(), _roadList.end(), [=](CRoad r){return r.start() == start && r.end() == end;});
        storeDataStream >> dist;
        CStore s(*tmpRoad, dist);
        _storeList.push_back(s);
    }

    _roadNum = _roadList.size();
    _storeNum = _storeList.size();
    roadData.close();
    storeData.close();
}

void CCity::start() {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fresh()));
    timer->start(1000);
}

void CCity::generatePack() {
    int r = rand() % _storeNum;
    CStore s = _storeList.at(r);
    auto pos = find_if(_packWaiting.begin(), _packWaiting.end(), [=](CPack p) {return p.source() == s;});
    while (pos == _packWaiting.end()) {
        r = rand() % _storeNum;
        s = _storeList.at(r);
        pos = find_if(_packWaiting.begin(), _packWaiting.end(), [=](CPack p) {return p.source() == s;});
    }
    CRoad road = _roadList.at(r);
    int d = rand() % road.length();
    CTarget t(road, d);
    CPack p(s, t);
    _packWaiting.push_back(p);
}

void CCity::fresh() {
    emit needDraw();

//    for (int tmpCnt = 0; tmpCnt < _driverNum; tmpCnt++) {
//        Ant tempAnt(100, 1);
//        vector<int> tmp = tempAnt.dealWithData();

//        int i = 0;
//        while (tmp[i] != _driver[tmpCnt].tempPos) i++;
//        int front = i == 0 ? tmp[_storeNum - 1] : tmp[i-1];
//        int next = i == _storeNum - 1 ? tmp[0] : tmp[i+1];
//        double frontDis = store2StoreDis(_driver[tmpCnt].tempPos, front);
//        double nextDis = store2StoreDis(_driver[tmpCnt].tempPos, next);
//        int finalDicision = frontDis > nextDis ? next : front;

//        _driver[tmpCnt]._pos._x = _store[finalDicision].x();
//        _driver[tmpCnt]._pos._y = _store[finalDicision].y();
//        qDebug() << _driver[tmpCnt].tempPos;
//        clearStore(_driver[tmpCnt].tempPos);
//        _storeNum--;
//        _driver[tmpCnt].tempPos = finalDicision > _driver[tmpCnt].tempPos ? finalDicision - 1 : finalDicision;
//    }
}

void CDriver::catchPack(CPack& p) {
    p.setState();
    _packHolding.push_back(p);
}

int CDriver::dist2Target(CTarget c) {
    int result;
    CPos dStart = start();
    CPos dEnd = end();
    CPos cStart = c.start();
    CPos cEnd = c.end();

    if (dStart._x == dEnd._x) { // the driver's road is horizontal
        if (cStart._x == cEnd._x) { // the store's road is horizontal
            if (cStart._y == dStart._y) { // on the same column
                result = abs(cStart._x - dStart._x) + min(_dist + c.dist(), 2 * (cEnd._y - cStart._y) - _dist - c.dist());
            }
            else {
                result = abs(cStart._x - dStart._x) + abs(cStart._y + c.dist() - dStart._y - _dist);
            }
        }
        else { // the store's road is vertical
            result = abs(cStart._x + c.dist() - dStart._x) + abs(cStart._y - dStart._y - _dist);
        }
    }
    else { // the driver's road is vertical
        if (cStart._y == cEnd._y) { //the store's road is vertical
            if (cStart._x == dStart._x) { // on the same row
                result = abs(cStart._y - dStart._y) + min(_dist + c.dist(), 2 * (cEnd._x - cStart._x) - _dist - c.dist());
            }
            else {
                result = abs(cStart._y - dStart._y) + abs(cStart._x + c.dist() - dStart._x - _dist);
            }
        }
        else {
            result = abs(cStart._y + c.dist() - dStart._y) + abs(cStart._x - dStart._x - _dist);
        }
    }
    return result;
}
