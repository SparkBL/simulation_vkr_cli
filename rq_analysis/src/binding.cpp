#include "delay.hpp"
#include "model.hpp"
#include "producer.hpp"
#include "node.hpp"
#include "orbit.hpp"
#include "request.hpp"
#include "router.hpp"
#include "stats.hpp"
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
struct TestS
{
    std::unordered_map<std::string, Producer *> comps = {};
};

PYBIND11_MODULE(simulation, m)
{
    m.attr("__name__") = "rq_analysis.simulation";
    m.doc() = "Python library for retrial queuing system modeling";
    // m.attr("__name__") = "rq_simulation.simulation";
    //  py::bind_map<std::unordered_map<std::string, Producer *>>(m, "Components");
    //   py::bind_map<std::unordered_map<std::string, Router *>>(m, "Connections");
    //   py::bind_vector<std::vector<double, std::allocator<double>>>(m, "FloatVector", py::buffer_protocol());
    //  py::bind_vector<std::vector<std::vector<double>>>(m, "FloatMatrix");
    py::class_<Request>(m, "Request", "Basic unit of simulation.\nrtype - request type\nstatus_change_at\nstatus - current status of request\nmoment of model time, when request has to change its status\nemitted_at - moment of time, when request was emitted from input process", py::dynamic_attr())
        .def(py::init())
        .def_readwrite("rtype", &Request::rtype)
        .def_readwrite("status", &Request::status)
        .def_readwrite("status_change_at", &Request::status_change_at)
        .def_readwrite("emitted_at", &Request::emitted_at)
        .def_readwrite("emitted_at", &Request::wait_time)
        .def_readwrite("attempts", &Request::attempts)
        .def("__repr__",
             [](const Request r)
             {
                 return string_sprintf("Request{rtype = %d, status = %d, emitted_at = %g, wait_time = %g, status_change_at = %g, attempts = %g}", r.rtype, r.status, r.emitted_at, r.wait_time, r.status_change_at, r.attempts);
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
        .def("produce", &Producer::Produce, py::return_value_policy::reference_internal)
        //.def("produce", [](Producer &r, double time)
        //      { return as_pyarray(r.Produce(time)); })
        .def("input_connect", &Producer::InputAtConnect, "slot_name"_a, "router"_a)
        .def("output_connect", &Producer::OutputAtConnect, "slot_name"_a, "router"_a)
        .def("inputs", &Producer::Inputs)
        .def("outputs", &Producer::Outputs)
        .def("tag", &Producer::Tag)
        .def_readwrite("queue", &Producer::queue, py::return_value_policy::reference_internal);

    py::class_<Model>(m, "RqModel", "Container for managing simulation process.")
        .def(py::init<double>(), "end"_a = 1000)
        .def("add_connection", &Model::AddConnection, "from_producer"_a, "from_slot"_a, "to_producer"_a, "to_slot"_a, "Adds request flow between OUT slot of producer A to IN slot of producer B")
        .def("add_producer", &Model::AddProducer, "producer"_a, "label"_a, py::keep_alive<1, 2>(), "Adds producer to model")
        .def("next_step", &Model::NextStep, "Set model time to moment of next event")
        .def("aggregate", &Model::Aggregate, "events"_a, "Gathers return of method 'produce' to sort events in ascending order")
        .def("is_done", &Model::IsDone, "Checks if time == end")
        .def_readwrite("time", &Model::time, "Current moment of simulation")
        .def_readwrite("end", &Model::end, "Moment when simulation has to stop")
        .def_readwrite("event_queue", &Model::event_queue, py::return_value_policy::reference_internal, "List of events, which must occur in the model")
        .def_readwrite("routers", &Model::routers, py::return_value_policy::reference_internal, "List of connections added by 'add_connection'")
        .def_readwrite("components", &Model::components, py::return_value_policy::reference_internal, "List of producers added by 'add_producer'");

    py::class_<Router>(m, "Router", "Makes requests flow through itself")
        .def(py::init())
        .def("pop", &Router::Pop, "Get first pushed request")
        .def("len", &Router::Len, "Returns number of requests contained")
        .def("push", &Router::Push, "request"_a, "Push request in queue")
        .def("is_empty", &Router::IsEmpty, "Check if queue is empty")
        .def("add_reader", &Router::AddReader, "reader"_a, "Add request reader")
        .def_readwrite("pushed_count", &Router::pushed_count)
        .def_readwrite("popped_count", &Router::popped_count)
        .def_readwrite("__q__", &Router::q);

    py::class_<RouterReader>(m, "RouterReader", "Reades requests passing through router")
        .def("read", &RouterReader::Read, "requrest"_a, "Get first pushed request");

    py::class_<IntervalRouterReader, RouterReader>(m, "IntervalRouterReader")
        .def(py::init<double>(), "interval"_a);

    py::class_<Slot>(m, "Slot")
        .def(py::init<Router *>(), "router"_a)
        .def("connect", &Slot::Connect, "router"_a);

    py::class_<InSlot, Slot>(m, "InSlot")
        .def(py::init<Router *>(), "router"_a)
        .def("pop", &InSlot::Pop)
        .def("len", &InSlot::Len)
        .def("is_empty", &InSlot::IsEmpty);

    py::class_<OutSlot, Slot>(m, "OutSlot")
        .def(py::init<Router *>(), "router"_a)
        .def("len", &OutSlot::Len)
        .def("push", &OutSlot::Push, "request"_a);

    m.attr("FLOAT_EQ_THRESHOLD") = py::float_(float64_equality_threshold);

    py::class_<Delay>(m, "Delay")
        .def("get", &Delay::Get);

    py::class_<ExponentialDelay, Delay>(m, "ExponentialDelay")
        .def(py::init<double>(), "intensity"_a);

    py::class_<UniformDelay, Delay>(m, "UniformDelay")
        .def(py::init<double, double>(), "a"_a, "b"_a);

    py::class_<GammaDelay, Delay>(m, "GammaDelay")
        .def(py::init<double, double>(), "k"_a, "theta"_a);

    m.def("get_exponential_delay", &GetExponentialDelay, "Get Exponential sample");

    py::class_<RQTNode, Producer>(m, "RqtNode")
        .def(py::init<Delay *, Delay *>(), "input_delay"_a, "called_delay"_a, py::keep_alive<1, 2>(), py::keep_alive<1, 3>());

    py::class_<SimpleNode, Producer>(m, "SimpleNode")
        .def(py::init<Delay *>(), "input_delay"_a, py::keep_alive<1, 2>());

    py::class_<RQNode, Producer>(m, "RqNode")
        .def(py::init<Delay *>(), "input_delay"_a, py::keep_alive<1, 2>());

    py::class_<SimpleInput, Producer>(m, "SimpleInput")
        .def(py::init<Delay *, int, double>(), "delay"_a, "request_type"_a = typeInput, "init_time"_a = 0, py::keep_alive<1, 2>());

    py::class_<MMPP, Producer>(m, "MMPPInput")
        .def(py::init<std::vector<double>, std::vector<std::vector<double>>, int, double>(), "lambda"_a, "Q"_a, "request_type"_a = typeInput, "init_time"_a = 0);

    py::class_<IOrbit, Producer>(m, "IOrbit")
        .def("append", &IOrbit::Append, "time"_a);
    py::class_<Orbit, IOrbit>(m, "Orbit")
        .def(py::init<Delay *>(), "delay"_a, py::keep_alive<1, 2>());

    py::class_<StatCollector, Producer>(m, "StatCollector")
        .def(py::init<double>(), "interval"_a)
        .def("get_distribution2d", &StatCollector::GetDistribution)
        .def("get_distribution_summary", &StatCollector::GetSummaryDistribution)
        .def("get_distribution_input", &StatCollector::GetInputDistribution)
        .def("get_distribution_called", &StatCollector::GetCalledDistribution)
        .def("get_mean_input", &StatCollector::GetMeanInput)
        .def("get_mean_called", &StatCollector::GetMeanCalled)
        .def("get_variation_called", &StatCollector::GetVariationIntervalCalled)
        .def("get_variation_input", &StatCollector::GetVariationIntervalInput);

    py::class_<CustomCollector, Producer>(m, "CustomCollector")
        .def(py::init())
        .def("get_requests", &CustomCollector::GetRequests);
}
