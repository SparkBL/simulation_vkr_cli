from rq_analysis import simulation as rq
import time

print("### Request test ###")
r = rq.Request()
rr = [r]
print(r)
r.status_change_at = 0.2
print(r,rr)
