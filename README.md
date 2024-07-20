# JSON Parsing and Serialization Library

The experimental library for parsing and serializing JSON files written in C using CAPI. Below are the results of the comparison with the `json` and `ujson` libraries:

```Shell
==== default json.dumps: 0.982744965 s
==== pip ujson.dumps: 0.655773092 s
==== module cjson.dumps: 0.893803159 s
-----------------------------------------
==== default json.loads: 0.000258516 s
==== pip ujson.loads: 0.00019135 s
==== module cjson.loads: 0.001362562 s
```
