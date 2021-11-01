#ifndef CLAIM_H
#define CLAIM_H
#include <vector>
#include <algorithm>
enum class EventType { FromInputProcess,Called,Orbit,Shift,Served};

class Event{
private:
    static std::vector<Event*> storage;
    EventType type;
    double moment;
public:
    Event( double moment,EventType type = EventType::FromInputProcess);
    Event (Event* original);
    void SetMoment(double moment);
    static Event* PopClosestEvent();
    static void ClearEventStorage();
    double Moment();
    void SetType(EventType type);
    EventType GetType();
};


#endif // CLAIM_H
