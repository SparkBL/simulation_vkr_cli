import rq_analysis.simulation as rq

m = rq.Router()

print(m.len())
print(m.push(rq.Request()))
print(m.pop())


rs = []
for i in range(0,100):
    r = rq.Router()
    r.add_reader(rq.TimeCounter(),"count")
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    r.push(rq.Request())
    rs += [r]


for r in rs:
    while not r.is_empty():
        r.pop()
    print(r.reader_at('count').counts)
    print(r.reader_at('count').counts)