from rq_analysis import simulation as rq
import time

#Init model
model = rq.Model()
model.set_time(0) 
model.set_end (1000000)
model.add_producer(rq.MMPPInput(
    [0.079,0.539,0.102],
    [[-0.359,0.191,0.168],
    [0.286,-0.41,0.123],
    [0.302,0.075,-0.378]],0,0),"input")
model.add_producer(rq.SimpleInput(rq.ExponentialDelay(0.6),1,0),"call")
model.add_producer(rq.Orbit(rq.ExponentialDelay(0.81)),"orbit")
model.add_producer(rq.RqtNode(rq.ExponentialDelay(1.3),rq.ExponentialDelay(1)),"node")
#model.add_producer(rq.StatCollector(10),"stat")

print("Components:",model.components())

#Init connections
nodein = model.add_connection("input","out_slot","node","in_slot")
model.add_connection("call","out_slot","node","call_slot")
model.add_connection("node","orbit_append_slot","orbit","in_slot")
model.add_connection("orbit","out_slot","node","orbit_slot")
#model.add_connection("node","out_slot","stat","in_slot")
output = model.add_hanging_output_noqueue("node","out_slot")

print("Connections",model.routers())


model.add_connection_reader(output,"stat",rq.IntervalRouterReader(20))
model.add_connection_reader(nodein,"count",rq.AttemptCounter())
print(model.router_at(output).readers())

start = time.time()
end = 0
c = 0
while True:
        c+=1
        t = model.next_step()
        model.aggregate(model.component_at("input").produce(t))
        model.aggregate(model.component_at("orbit").produce(t))
        model.aggregate(model.component_at("call").produce(t))
        model.aggregate(model.component_at("node").produce(t))
        model.aggregate(model.component_at("orbit").append(t))
        if model.is_done():
            end = time.time()
            break
print("Time: ",model.time())
print("Iters: ",c)
print("Elapsed: ",end - start)
print("Distr:",model.router_at(output).reader_at('stat').get_distribution_2d())