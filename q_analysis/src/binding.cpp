#include "delay.hpp"
#include "model.hpp"
#include "producer.hpp"
#include "node.hpp"
#include "orbit.hpp"
#include "request.hpp"
#include "router.hpp"
#include "stream.hpp"
#include "utils.hpp"
//  #include <pybind11/complex.h>
//  #include <pybind11/functional.h>
//  #include <pybind11/chrono.h>
#include "Python.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
using namespace pybind11::literals;
namespace py = pybind11;

// PYBIND11_MAKE_OPAQUE(std::unordered_map<std::string, Producer *>);
// PYBIND11_MAKE_OPAQUE(std::unordered_map<std::string, Router *>);
// PYBIND11_MAKE_OPAQUE(std::vector<double, std::allocator<double>>);
// PYBIND11_MAKE_OPAQUE(std::vector<std::vector<double>>);
template <typename Sequence>
inline py::array_t<typename Sequence::value_type> as_pyarray(Sequence &&seq)
{
    // Move entire object to heap (Ensure is moveable!). Memory handled via Python capsule
    Sequence *seq_ptr = new Sequence(std::move(seq));
    auto capsule = py::capsule(seq_ptr, [](void *p)
                               { delete reinterpret_cast<Sequence *>(p); });
    return py::array(seq_ptr->size(), // shape of array
                     seq_ptr->data(), // c-style contiguous strides for Sequence
                     capsule          // numpy array references this parent
    );
}

template <typename Sequence>
inline py::array_t<typename Sequence::value_type> to_pyarray(const Sequence &seq)
{
    return py::array(seq.size(), seq.data());
}

PYBIND11_MODULE(simulation, m)
{
    m.attr("__name__") = "q_analysis.simulation";
    m.doc() = "Python library for retrial queuing system modeling";
    py::class_<Request>(m, "Request", "Basic unit of simulation.\nrtype - request type\nstatus_change_at\nstatus - current status of request\nmoment of model time, when request has to change its status\nemitted_at - moment of time, when request was emitted from input process", py::dynamic_attr())
        .def(py::init(), py::return_value_policy::reference)
        .def_readwrite("id", &Request::id)
        .def_readwrite("rtype", &Request::rtype)
        .def_readwrite("status", &Request::status)
        .def_readwrite("status_change_at", &Request::status_change_at)
        .def_readwrite("wait_time", &Request::wait_time)
        .def_readwrite("emitted_at", &Request::emitted_at)
        .def("__repr__",
             [](const Request r)
             {
                 return string_sprintf("Request{ id: %d, rtype : %d, status : %d, emitted_at : %g, wait_time : %g, status_change_at : %g }", r.id, r.rtype, r.status, r.emitted_at, r.wait_time, r.status_change_at);
             });
    m.attr("TYPE_INPUT") = py::int_(typeInput);
    m.attr("TYPE_CALLED") = py::int_(typeCalled);
    m.attr("TYPE_STATE") = py::int_(typeState);

    m.attr("STATUS_TRAVEL") = py::int_(statusTravel);
    m.attr("STATUS_SERVING") = py::int_(statusServing);
    m.attr("STATUS_SERVED") = py::int_(statusServed);
    m.attr("STATUS_LEAVE") = py::int_(statusLeave);
    m.attr("STATUS_ARRIVE") = py::int_(statusArrive);

    py::class_<Producer>(m, "Producer", "Base class for every processing unit.")
        .def("produce", &Producer::Produce, py::return_value_policy::reference)
        //.def("produce", [](Producer &r, double time)
        //      { return as_pyarray(r.Produce(time)); })
        .def("input_connect", &Producer::InputAtConnect, "slot_name"_a, "router"_a, py::keep_alive<1, 3>())
        .def("output_connect", &Producer::OutputAtConnect, "slot_name"_a, "router"_a, py::keep_alive<1, 3>())
        .def("inputs", &Producer::Inputs, py::return_value_policy::reference)
        .def("outputs", &Producer::Outputs, py::return_value_policy::reference)
        .def("tag", &Producer::Tag)
        .def_readonly("queue", &Producer::queue, py::return_value_policy::reference)
        /*.def("__repr__",
             [](const Producer &r)
             {
                 return typeid(r).name();
             })*/
        ;

    py::class_<Model>(m, "Model", "Container for managing simulation process.")
        .def(py::init<double>(), "end"_a = 1000, py::return_value_policy::reference)
        .def_readonly("__componenets__", &Model::components, py::return_value_policy::reference)
        .def_readonly("__routers__", &Model::routers, py::return_value_policy::reference)
        .def_readonly("__queue__", &Model::event_queue, py::return_value_policy::reference)
        .def("add_connection", &Model::AddConnection, "from_producer"_a, "from_slot"_a, "to_producer"_a, "to_slot"_a, "Adds request flow between OUT slot of producer A to IN slot of producer B")
        .def("add_hanging_input", &Model::AddHangingInput, "to_producer"_a, "to_slot"_a, "Adds request flow TO a producer. Method presumes requests will be put into queue manually. Return router's label")
        .def("add_hanging_output", &Model::AddHangingOutput, "from_producer"_a, "from_slot"_a, "Adds request flow from a producer. Method presumes outgoing requests will be handled manually. Returns router's label")
        .def("add_hanging_output_noqueue", &Model::AddHangingOutputNoQueue, "from_producer"_a, "from_slot"_a, "Adds request flow from a producer. Requests only will be read by attached readers without storing")
        .def("add_producer", &Model::AddProducer, "producer"_a, "label"_a, py::keep_alive<1, 2>(), "Adds producer to model")
        //  .def("add_connection_reader", &Model::AddConnectionReader, "connection"_a, "label"_a, "reader"_a, py::keep_alive<1, 2>(), "Adds producer to model")
        .def("next_step", &Model::NextStep, py::return_value_policy::reference, "Set model time to moment of next event")
        .def("aggregate", &Model::Aggregate, "events"_a, "Gathers return of method 'produce' to sort events in ascending order")
        .def("is_done", &Model::IsDone, "Checks if time == end")
        .def("set_time", &Model::SetTime, "Set current moment of simulation")
        .def("flush", &Model::Flush, "Clears event queue and all routers queue")
        .def("time", &Model::Time, "Current moment of simulation")
        .def("set_end", &Model::SetEnd, "Set moment when simulation has to stop")
        .def("end", &Model::End, "Moment when simulation has to stop")
        .def("event_queue", &Model::Queue, py::return_value_policy::reference, "List of events, which must occur in the model")
        .def("routers", &Model::Routers, py::return_value_policy::reference, "List of connections added by 'add_connection'")
        .def("components", &Model::Components, py::return_value_policy::reference, "List of producers added by 'add_producer'")
        .def("component_at", &Model::ComponentAt, py::keep_alive<1, 0>(), py::return_value_policy::reference, "Get producer")
        .def("router_at", &Model::RouterAt, py::keep_alive<1, 0>(), py::return_value_policy::reference, "Get router")
        .def("__repr__",
             [](const Model &r)
             {
                 return string_sprintf("Model{ time: %g, end : %g, event_queue_len : %d, num_components : %d, num_connections : %d }", r.time, r.end, r.event_queue.size(), r.components.size(), r.routers.size());
             });

    py::class_<Router>(m, "Router", "Request queue")
        .def(py::init(), py::return_value_policy::reference)
        .def("len", &Router::Len, "Returns number of requests contained")
        .def("push", &Router::Push, "request"_a, "Push request in queue")
        .def("pop", &Router::Pop, "Pop request from queue", py::return_value_policy::reference)
        .def("is_empty", &Router::IsEmpty, "Check if queue is empty")
        .def("flush", &Router::Flush, "Clears router queue")
        .def("add_reader", &Router::AddReader, "reader"_a, "label"_a, py::keep_alive<1, 2>(), "Add request reader")
        .def("readers", &Router::Readers, py::return_value_policy::reference, "dictionary of readers")
        .def("reader_at", &Router::ReaderAt, py::keep_alive<1, 0>(), py::return_value_policy::reference, "get reader")
        .def_readonly("pushed_count", &Router::pushed_count, "number of pushed requests")
        .def_readonly("popped_count", &Router::popped_count, "number of popped requests")
        .def_readwrite("__readers__", &Router::readers, py::return_value_policy::reference)
        .def_readonly("__q__", &Router::q, py::return_value_policy::reference)
        .def("__repr__",
             [](const Router &r)
             {
                 return string_sprintf("Router{ queue_len: %d }", r.q.size());
             });

    py::class_<OutputRouter, Router>(m, "OutputRouter", "Endpoint for incoming requests. Popping requests from OutputRouter return empty Request")
        .def(py::init(), py::return_value_policy::reference);

    py::class_<NoneRouter, Router>(m, "NoneRouter", "Always empty queue")
        .def(py::init(), py::return_value_policy::reference);

    py::class_<RouterReader>(m, "RouterReader", "Reades requests passing through router")
        .def("read", &RouterReader::Read, py::keep_alive<1, 2>(), "request"_a, "Get first pushed request");

    py::class_<IntervalRouterReader, RouterReader>(m, "IntervalRouterReader", "Collects statistic on passing requests")
        .def(py::init<double>(), "interval"_a, py::return_value_policy::reference)
        .def("get_distribution_2d", &IntervalRouterReader::GetDistribution, py::return_value_policy::reference)
        .def("get_distribution_summary", &IntervalRouterReader::GetSummaryDistribution, py::return_value_policy::reference)
        .def("get_distribution_input", &IntervalRouterReader::GetInputDistribution, py::return_value_policy::reference)
        .def("get_distribution_called", &IntervalRouterReader::GetCalledDistribution, py::return_value_policy::reference)
        .def("get_mean_input", &IntervalRouterReader::GetMeanInput)
        .def("get_mean_called", &IntervalRouterReader::GetMeanCalled)
        .def("get_variation_called", &IntervalRouterReader::GetVariationIntervalCalled)
        .def("get_variation_input", &IntervalRouterReader::GetVariationIntervalInput)
        .def("__repr__",
             [](const IntervalRouterReader &r)
             {
                 return "IntervalRouterReader";
             });

    py::class_<AttemptCounter, RouterReader>(m, "AttemptCounter", "Collects attempt and wait time statistic on passing requests")
        .def(py::init(), py::return_value_policy::reference)
        .def_readwrite("attempts", &AttemptCounter::attempts, py::keep_alive<1, 0>(), "dictionary of attempts", py::return_value_policy::reference)
        .def_readwrite("wait_time", &AttemptCounter::wait_time, py::keep_alive<1, 0>(), "dictionary of wait_time", py::return_value_policy::reference)
        .def("__repr__",
             [](const AttemptCounter &r)
             {
                 return "AttemptCounter";
             });

    py::class_<TimeCounter, RouterReader>(m, "TimeCounter", "Collects moments when request is being pushed into router")
        .def(py::init(), py::return_value_policy::reference)
        .def_readwrite("counts", &TimeCounter::counts, py::keep_alive<1, 0>(), "vector of moments", py::return_value_policy::reference)
        .def("__repr__",
             [](const TimeCounter &r)
             {
                 return "TimeCounter";
             });

    py::class_<Slot>(m, "Slot")
        .def(py::init<Router &>(), py::return_value_policy::reference)
        .def(py::init<>(), py::return_value_policy::reference)
        .def("connect", &Slot::Connect, "router"_a, py::keep_alive<1, 2>());

    py::class_<InSlot, Slot>(m, "InSlot")
        .def(py::init<Router &>(), py::return_value_policy::reference)
        .def(py::init<>(), py::return_value_policy::reference)
        .def("pop", &InSlot::Pop, py::return_value_policy::reference)
        .def("len", &InSlot::Len)
        .def("is_empty", &InSlot::IsEmpty)
        .def("__repr__",
             [](const InSlot &r)
             {
                 return "InSlot";
             });

    py::class_<OutSlot, Slot>(m, "OutSlot")
        .def(py::init<Router &>(), py::return_value_policy::reference)
        .def(py::init<>(), py::return_value_policy::reference)
        .def("len", &OutSlot::Len)
        .def("push", &OutSlot::Push, "request"_a, py::keep_alive<1, 2>())
        .def("__repr__",
             [](const OutSlot &r)
             {
                 return "OutSlot";
             });

    m.attr("FLOAT_EQ_THRESHOLD") = py::float_(float64_equality_threshold);

    py::class_<Delay>(m, "Delay")
        .def("get", &Delay::Get);

    py::class_<ExponentialDelay, Delay>(m, "ExponentialDelay")
        .def(py::init<double>(), "intensity"_a, py::return_value_policy::reference)
        .def("__repr__",
             [](const ExponentialDelay &r)
             {
                 return "ExponentialDelay";
             });

    py::class_<UniformDelay, Delay>(m, "UniformDelay")
        .def(py::init<double, double>(), "a"_a, "b"_a, py::return_value_policy::reference)
        .def("__repr__",
             [](const UniformDelay &r)
             {
                 return "UniformDelay";
             });

    py::class_<GammaDelay, Delay>(m, "GammaDelay")
        .def(py::init<double, double>(), "k"_a, "theta"_a, py::return_value_policy::reference)
        .def("__repr__",
             [](const GammaDelay &r)
             {
                 return "GammaDelay";
             });

    py::class_<WeibullDelay, Delay>(m, "WeibullDelay")
        .def(py::init<double, double>(), "a"_a, "b"_a, py::return_value_policy::reference)
        .def("__repr__",
             [](const WeibullDelay &r)
             {
                 return "WeibullDelay";
             });

    py::class_<LognormalDelay, Delay>(m, "LognormalDelay")
        .def(py::init<double, double>(), "m"_a, "s"_a, py::return_value_policy::reference)
        .def("__repr__",
             [](const LognormalDelay &r)
             {
                 return "LognormalDelay";
             });

    py::class_<HyperExponential, Delay>(m, "HyperExponential")
        .def(py::init < std::vector<double>, std::vector<double>, "p"_a, "i"_a, py::return_value_policy::reference)
        .def("__repr__",
             [](const HyperExponential &r)
             {
                 return "HyperExponential";
             });

    m.def("get_exponential_delay", &GetExponentialDelay, "Get Exponential sample");

    py::class_<RQTNode, Producer>(m, "RqtNode")
        .def(py::init<Delay &, Delay &>(), "input_delay"_a, "called_delay"_a, py::keep_alive<1, 2>(), py::keep_alive<1, 3>(), py::return_value_policy::reference)
        .def("__repr__",
             [](const RQTNode &r)
             {
                 return "RQTNode";
             });

    py::class_<SimpleNode, Producer>(m, "SimpleNode")
        .def(py::init<Delay &>(), "input_delay"_a, py::keep_alive<1, 2>(), py::return_value_policy::reference)
        .def("__repr__",
             [](const SimpleNode &r)
             {
                 return "SimpleNode";
             });

    py::class_<DumpNode, Producer>(m, "DumpNode").def(py::init<>(), py::return_value_policy::reference).def("__repr__", [](const DumpNode r)
                                                                                                            { return "DumpNode"; });

    py::class_<RQNode, Producer>(m, "RqNode")
        .def(py::init<Delay &>(), "input_delay"_a, py::keep_alive<1, 2>(), py::return_value_policy::reference)
        .def("__repr__",
             [](const RQNode &r)
             {
                 return "RQNode";
             });

    py::class_<SimpleInput, Producer>(m, "SimpleInput", "Poisson input process")
        .def(py::init<Delay &, int, double>(), "delay"_a, "request_type"_a = typeInput, "init_time"_a = 0, py::keep_alive<1, 2>(), py::return_value_policy::reference)
        .def("__repr__",
             [](const SimpleInput &r)
             {
                 return "SimpleInput";
             });

    py::class_<MMPP, Producer>(m, "MMPPInput", "MMPP input process")
        .def(py::init<std::vector<double>, std::vector<std::vector<double>>, int, double>(), "lambda"_a, "Q"_a, "request_type"_a = typeInput, "init_time"_a = 0, py::return_value_policy::reference)
        .def("__repr__",
             [](const MMPP &r)
             {
                 return "MMPP";
             });

    py::class_<IOrbit, Producer>(m, "IOrbit")
        .def("append", &IOrbit::Append, "time"_a);
    py::class_<Orbit, IOrbit>(m, "Orbit")
        .def(py::init<Delay &>(), "delay"_a, py::keep_alive<1, 2>(), py::return_value_policy::reference)
        .def_readonly("__requests__", &Orbit::requests, py::return_value_policy::reference)
        .def("__repr__",
             [](const Orbit &r)
             {
                 return "Orbit";
             });
}