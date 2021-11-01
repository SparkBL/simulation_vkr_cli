#include "node.h"

INode::INode(DelayProvider* d) : Processable(d) {};




CalledClaimsNode::CalledClaimsNode(DelayProvider *d,double serveIntesnity,double calledServeIntensity) : INode(d){
    this->currentEvent=nullptr;
    this->calledServeIntensity = calledServeIntensity;
    this->serveIntensity = serveIntesnity;
}

Event* CalledClaimsNode::Process(){
    if (currentEvent != nullptr)
    {
        if (currentEvent->Moment() == Environment::Time())
        {
            Event* ret = new Event(currentEvent);
            currentEvent = nullptr;
            return ret;
        }
    }
    return nullptr;
}

Event* CalledClaimsNode::Income(Event *income){
    if ( currentEvent==nullptr && income!=nullptr)
    {
        currentEvent = income;
        if (income->GetType()==EventType::FromInputProcess ||income->GetType()==EventType::Orbit )
            currentEvent->SetMoment(d->GetDelay(serveIntensity));
        if(income->GetType()==EventType::Called)
            currentEvent->SetMoment(d->GetDelay(calledServeIntensity));
        return nullptr;
    }
    return income;
}


bool CalledClaimsNode::IsFree() {
    return currentEvent==nullptr;
}


