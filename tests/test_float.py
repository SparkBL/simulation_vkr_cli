import rq_simulation as rq
import time

#Init model
model = rq.RqModel()
model.time = 0 
model.end = 1000000
model.add_producer(rq.MMPPInput(
    rq.FloatVector([0.079,0.539,0.102]),
    rq.FloatMatrix([rq.FloatVector([-0.359,0.191,0.168]),
    rq.FloatVector([0.286,-0.41,0.123]),
    rq.FloatVector([0.302,0.075,-0.378])]),0,0),"input")
model.add_producer(rq.SimpleInput(rq.ExponentialDelay(0.6),1,0),"call")
model.add_producer(rq.Orbit(rq.ExponentialDelay(0.81)),"orbit")
model.add_producer(rq.RqtNode(rq.ExponentialDelay(1.3),rq.ExponentialDelay(1)),"node")
model.add_producer(rq.StatCollector(10),"stat")

print("Components:",model.components)

#Init connections
model.add_connection("input","out_slot","node","in_slot")
model.add_connection("call","out_slot","node","call_slot")
model.add_connection("node","orbit_append_slot","orbit","in_slot")
model.add_connection("orbit","out_slot","node","orbit_slot")
model.add_connection("node","out_slot","stat","in_slot")

print("Connections",model.routers)


start = time.time()
end = 0
c = 0
while True:
        c+=1
        t = model.next_step()
     #   print("Inoking input ",model.components["input"])
        model.aggregate(model.components["input"].produce(t))
        
     #   print("Inoking orbit",model.components["orbit"])
        model.aggregate(model.components["orbit"].produce(t))
        
    #    print("Inoking call",model.components["call"])
        model.aggregate(model.components["call"].produce(t))
        
    #    print("Inoking node",model.components["node"])
        model.aggregate(model.components["node"].produce(t))
    #    print("Inoking orbit append",model.components["orbit"])
        model.aggregate(model.components["orbit"].append(t))

        model.aggregate(model.components["stat"].produce(t))
        if model.is_done():
            end = time.time()
            break
print("Time: ",model.time)
print("Iters: ",c)
print("Elapsed: ",end - start)
print("Distr:",model.components["stat"].get_distribution2d())