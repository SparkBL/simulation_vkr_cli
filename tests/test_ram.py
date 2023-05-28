from q_analysis import simulation as rq
from q_analysis import utils as uu
import time
import os, psutil; 
print("On start", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2)
#Init model
#model = uu.PyModel()
model = rq.Model()
model.set_time(0) 
model.set_end (2000000)
#model.add_producer(rq.MMPPInput(
#    [1,0.539,1.2],
#    [[-0.359,0.191,0.168],
#    [0.286,-0.41,0.123],
#    [0.302,0.075,-0.378]],0,0),"input")
model.add_producer(rq.SimpleInput(rq.ExponentialDelay(1.1),1,0),"input")
#model.add_producer(rq.SimpleInput(rq.ExponentialDelay(0.6),1,0),"call")
#model.add_producer(rq.Orbit(rq.ExponentialDelay(0.81)),"orbit")
#model.add_producer(rq.RqtNode(rq.ExponentialDelay(1.3),rq.ExponentialDelay(1)),"node")
model.add_producer(rq.SimpleNode(rq.ExponentialDelay(1.3)),"node")
#model.add_producer(rq.DumpNode(),"dump")
#model.add_producer(rq.StatCollector(10),"stat")

#Init connections
nodein = model.add_connection("input","out_slot","node","in_slot")
#callsl = model.add_connection("call","out_slot","node","call_slot")
#orbslot = model.add_connection("orbit","out_slot","node","orbit_slot")
output = model.add_hanging_output_noqueue("node","out_slot")
#orbappslot = model.add_connection("node","orbit_append_slot","orbit","in_slot")


r = []
print("After init", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2)
last = psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2
#model.router_at(output).add_reader(rq.TimeCounter(),"count")
print("After init", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2)
start = time.time()
end = 0
c = 0
c_last = 0
while True:
        c+=1
        t = model.next_step()
        model.aggregate(model.component_at("input").produce(t))
        #if last != psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2:
           # print(
           #     c - c_last, 
           #     last - psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2,
           #     len(model.__queue__),
           #     len(model.component_at('input').queue),
           #     len(model.component_at('node').queue),
            #    model.router_at(nodein).len(),
           #     model.router_at(output).len()#,
              #  len(model.router_at(output).reader_at('count').counts)
           #   )
           # last = psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2
           # c_last = c
        model.aggregate(model.component_at("node").produce(t))
        if model.is_done():
            end = time.time()
            break
print("Time: ",model.time)
print("Iters: ",c)
print("Elapsed: ",end - start)
ll = model.router_at(output)

#print("Distr:",len(ll.reader_at('count').counts))
print(nodein, model.router_at(nodein).len())
#print(callsl,  model.router_at(callsl).len())
#print(orbslot,  model.router_at(orbslot).len())
print(output,  model.router_at(output).len())
#print(orbappslot,  model.router_at(orbappslot).len())
print("q",  len(model.__queue__))
print("After end", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2)
#model.flush()
print("After flush", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2)
del model
print("After del model", psutil.Process(os.getpid()).memory_info().rss / 1024 ** 2)