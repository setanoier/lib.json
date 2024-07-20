# JSON Parsing and Serialization Library

The experimental library for parsing and serializing JSON files written in C using CAPI. Below are the results of the comparison with the `json` and `ujson` libraries:

```Shell
==== default json.dumps: 0.002531949 s
==== pip ujson.dumps: 0.001220959 s
==== module cjson.dumps: 0.575434831 s
-----------------------------------------
==== default json.loads: 0.00016752 s
==== pip ujson.loads: 0.000108336 s
==== module cjson.loads: 0.004871932 s
```

## Getting started

To start using the library on your machine, you should install Python and then all the necessary dependencies using `requirements.txt`:

```Shell
sudo apt-get update
sudo apt-get install -y python3.10-dev
pip install --upgrade pip
pip install -r requirements.txt
```

## References

https://docs.python.org/3/c-api/index.html
https://www.youtube.com/watch?v=nHEF1epuuco&ab_channel=TobiasBergkvist