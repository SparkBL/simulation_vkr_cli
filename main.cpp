#include "model.hpp"
#include "orbit.hpp"
//#include "stats.hpp"
#include "utils.hpp"
#include "node.hpp"
#include "stream.hpp"
#include "router.hpp"
#include "request.hpp"
#include "python3.8/Python.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/stl_bind.h>
namespace py = pybind11;
PYBIND11_MAKE_OPAQUE(std::vector<double, std::allocator<double>>);

PYBIND11_MODULE(rq, m)
{
    m.doc() = "Python library for retrial queuing system modeling";
    /* py::class_<std::vector<double, std::allocator<double>>>(m, "FloatVector")
         .def(py::init<>())
         .def("clear", &std::vector<double, std::allocator<double>>::clear)
         .def("pop_back", &std::vector<double, std::allocator<double>>::pop_back)
         .def("__len__", [](const std::vector<double, std::allocator<double>> &v)
              { return v.size(); })
         .def(
             "__iter__", [](std::vector<double, std::allocator<double>> &v)
             { return py::make_iterator(v.begin(), v.end()); },
             py::keep_alive<0, 1>());*/
    py::bind_vector<std::vector<double, std::allocator<double>>>(m, "FloatVector");
    py::class_<Request>(m, "Request")
        .def(py::init())
        .def_readwrite("rtype", &Request::rtype)
        .def_readwrite("status", &Request::status)
        .def_readwrite("status_change_at", &Request::status_change_at);
    m.attr("TYPE_INPUT") = py::int_(typeInput);
    m.attr("TYPE_CALLED") = py::int_(typeCalled);
    m.attr("TYPE_STATE") = py::int_(typeState);

    m.attr("STATUS_TRAVEL") = py::int_(statusTravel);
    m.attr("STATUS_SERVING") = py::int_(statusServing);
    m.attr("STATUS_SERVED") = py::int_(statusServed);
    m.attr("STATUS_LEAVE") = py::int_(statusLeave);
    m.attr("STATUS_ARRIVE") = py::int_(statusArrive);

    py::class_<Producer>(m, "Producer")
        .def("produce", &Producer::Produce)
        .def("input_connect", &Producer::InputAtConnect)
        .def("output_connect", &Producer::OutputAtConnect)
        .def("inputs", &Producer::Inputs)
        .def("outputs", &Producer::Outputs)
        .def("tag", &Producer::Tag)
        .def_readwrite("queue", &Producer::queue, py::return_value_policy::automatic_reference);

    py::class_<Model>(m, "RqModel")
        .def(py::init())
        .def("init", &Model::Init)
        .def("add_connection", &Model::AddConnection)
        .def("add_connection", &Model::AddProducer)
        .def("next_step", &Model::NextStep)
        .def("aggregate", &Model::Aggregate)
        .def_readwrite("time", &Model::time)
        .def_readwrite("end", &Model::end)
        .def_readwrite("event_queue", &Model::event_queue);

    py::class_<Router>(m, "Router")
        .def(py::init())
        .def("pop", &Router::Pop)
        .def("len", &Router::Len)
        .def("push", &Router::Push)
        .def("is_empty", &Router::IsEmpty)
        .def_readwrite("__q__", &Router::q_);

    py::class_<Slot>(m, "Slot")
        .def(py::init<Router *>())
        .def("connect", &Slot::Connect);

    py::class_<InSlot, Slot>(m, "InSlot")
        .def(py::init<Router *>())
        .def("pop", &InSlot::Pop)
        .def("len", &InSlot::Len)
        .def("is_empty", &InSlot::IsEmpty);

    py::class_<OutSlot, Slot>(m, "OutSlot")
        .def(py::init<Router *>())
        .def("len", &OutSlot::Len)
        .def("push", &OutSlot::Push);

    m.attr("FLOAT_EQ_THRESHOLD") = py::float_(float64_equality_threshold);

    py::class_<Delay>(m, "Delay")
        .def("get", &Delay::Get);

    py::class_<ExponentialDelay, Delay>(m, "ExponentialDelay")
        .def(py::init<double>());

    py::class_<UniformDelay, Delay>(m, "UniformDelay")
        .def(py::init<double, double>());

    py::class_<GammaDelay, Delay>(m, "GammaDelay")
        .def(py::init<double, double>());

    m.def("get_exponential_delay", &GetExponentialDelay, "Get Exponential sample");

    py::class_<RQTNode, Producer>(m, "RqtNode")
        .def(py::init<Delay *, Delay *>());

    py::class_<SimpleInput, Producer>(m, "SimpleInput")
        .def(py::init<Delay *, int, double>());

    py::class_<IOrbit, Producer>(m, "IOrbit")
        .def("append", &IOrbit::Append);
    py::class_<Orbit, IOrbit>(m, "Orbit")
        .def(py::init<Delay *>());
}
