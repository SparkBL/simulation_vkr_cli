#include "event.h"


Event::Event(double moment,EventType type) {
    this->type = type;
    this->moment =  moment;
    if(moment!=std::numeric_limits<double>::infinity())
        storage.push_back(this);
}

Event::Event(Event* original){
    this->type = original->type;
    this->moment = original->moment;
    if(moment!=std::numeric_limits<double>::infinity())
        storage.push_back(this);
}

void Event::SetMoment(double moment){
    this->moment = moment;
    if(std::find(storage.begin(), storage.end(), this) == storage.end()){
        if(moment!=std::numeric_limits<double>::infinity())
            storage.push_back(this);
    }
}

Event* Event::PopClosestEvent(){
    if(storage.size()>0){
        auto min = std::min_element(begin(storage),end(storage),
                                    [](Event* const& c1,Event* const& c2){
                return c1->moment< c2->moment;
    });
        Event* m = *min;
        storage.erase(min);
        return m;
    }
    return nullptr;
}

void Event::ClearEventStorage(){
  /* for (auto p : storage)
    {
        if(p->alive==EVENT_ALIVE)
            delete p;
    }*/
    storage.clear();
    //storage.shrink_to_fit();
}


double Event::Moment() {
    return moment;
}

void Event::SetType(EventType type){
    this->type = type;
    if(moment!=std::numeric_limits<double>::infinity())
        if(std::find(storage.begin(), storage.end(), this) == storage.end()){
            storage.push_back(this);
        }
}

EventType Event::GetType(){
    return this->type;
}

std::vector<Event*> Event::storage(0);

