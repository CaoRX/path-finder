# path-finder

Find the optimal path between locations, both by time and money cost.

Input format can be found in design.txt: an example of data file can be found in data.txt, which comes from 5 stations on Yamanote line(shinjyuku, shibuya, ueno, shinakawa, ikebukuro).

Compile:
```
g++ -std=c++1z path-finder.cpp -o path-finder.out
```

Run:
```
./path-finder.out [filename]
```

Output like:
```
begin optimization
Time optimization path has been found.
Money optimization path has been found.
Time optimal path: 
---------------------------------------------------
Go from 0 to 1
use method 0: time cost = 4, money cost = 170
on the way: [5, 9], finish at 10
Go from 1 to 3
use method 1: time cost = 14, money cost = 180
on the way: [12, 26], finish at 28
Go from 3 to 2
use method 0: time cost = 23, money cost = 180
on the way: [28, 51], finish at 52
Go from 2 to 0
use method 3: time cost = 2, money cost = 1200
on the way: [52, 54], finish at 54
Go from 0 to 4
use method 0: time cost = 6, money cost = 170
on the way: [54, 60], finish at 65
Go from 4 to 0
use method 0: time cost = 5, money cost = 170
on the way: [65, 70], finish at 70
total time cost = 70
total money cost = 2070
---------------------------------------------------
Money optimal path: 
---------------------------------------------------
Go from 0 to 1
use method 0: time cost = 4, money cost = 170
on the way: [5, 9], finish at 10
Go from 1 to 3
use method 1: time cost = 14, money cost = 180
on the way: [11, 25], finish at 27
Go from 3 to 2
use method 0: time cost = 23, money cost = 180
on the way: [27, 50], finish at 51
Go from 2 to 4
use method 0: time cost = 23, money cost = 180
on the way: [51, 74], finish at 79
Go from 4 to 0
use method 0: time cost = 5, money cost = 170
on the way: [79, 84], finish at 84
total time cost = 84
total money cost = 880
---------------------------------------------------
```
