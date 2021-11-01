#include "environment.h"
double Environment::time = 0;
double Environment::statInterval = 5;
double Environment::maxTime = 700;
double Environment::nextStatTime = Environment::time + Environment::statInterval;
std::vector<Event*> Environment::completedEvents = std::vector<Event*>();
std::function<int(void)> Environment::mainAction = []{return 0;};
std::function<void(void)> Environment::intervalAction = []{};

int Environment::NextMoment(){
    int ret = mainAction();
    Event* claim = Event::PopClosestEvent();
    if (claim!=nullptr){
        while(claim->Moment() >= nextStatTime){
            time = nextStatTime;
            nextStatTime = time+statInterval;
            intervalAction();
        }
        time = claim->Moment();
        if(claim->GetType() == EventType::Served)
            delete  claim;
        else
            completedEvents.push_back(claim);
    }
    return ret;
}

void Environment::Init(){
    time = 0;
    nextStatTime = time + statInterval;
    sort( completedEvents.begin(), completedEvents.end() );
    completedEvents.erase( unique( completedEvents.begin(), completedEvents.end() ), completedEvents.end() );
        for (auto p : completedEvents)
        {
                delete p;
        }
        completedEvents.clear();
        completedEvents.shrink_to_fit();
        Event::ClearEventStorage();
}

bool Environment::isFinished(){
    return maxTime<=time;
}

void Environment::SetIntervalAction(std::function<void ()> callback){
    Environment::intervalAction = callback;
}

void Environment::SetAction(std::function<int ()> callback){
    Environment::mainAction = callback;
}

void Environment::SetStatInterval(double interval){
    if(interval>0) statInterval = interval;
}
void Environment::SetMaxTime(double max){
    maxTime = max;
}

double Environment::MaxTime(){
    return maxTime;
}

double Environment::Time(){
    return time;
}

double Environment::StatInterval(){
    return statInterval;
}


