import numpy as np
import random

f = open('../input.csv', 'w')
for i in np.arange(50):
    val = random.random()
    f.writelines(str(val)+'\n')