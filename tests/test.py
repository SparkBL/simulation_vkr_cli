import rq_simulation as rq


#Init model
model = rq.RqModel()

model.add_producer(rq.SimpleInput(rq.ExponentialDelay(1),0,0),"input")
#model.add_producer(rq.SimpleInput(rq.ExponentialDelay(1),0,0),"call")
#model.add_producer(rq.Orbit(rq.ExponentialDelay(0.5)),"orbit")
#model.add_producer(rq.RqtNode(rq.ExponentialDelay(1.2),rq.ExponentialDelay(1.2)),"node")
#model.add_producer(rq.CustomCollector(),"stat")

print(model.components)