import unittest

import cjson


class TestCJson(unittest.TestCase):
    def test_loads_simple(self):
        json_str = '{"hello": 10, "world": "100500"}'
        result = cjson.loads(json_str)
        expected = {'hello': 10, 'world': '100500'}
        self.assertEqual(result, expected)

    def test_loads_nested(self):
        json_str = '{"outer": {"inner": 42}}'
        result = cjson.loads(json_str)
        expected = {'outer': {'inner': 42}}
        self.assertEqual(result, expected)

    def test_loads_array(self):
        json_str = '{"numbers": [1, 2, 3], "strings": ["a", "b", "c"]}'
        result = cjson.loads(json_str)
        expected = {'numbers': [1, 2, 3], 'strings': ['a', 'b', 'c']}
        self.assertEqual(result, expected)

    def test_loads_booleans(self):
        json_str = '{"true_val": true, "false_val": false}'
        result = cjson.loads(json_str)
        expected = {'true_val': True, 'false_val': False}
        self.assertEqual(result, expected)

    def test_loads_null(self):
        json_str = '{"nothing": null}'
        result = cjson.loads(json_str)
        expected = {'nothing': None}
        self.assertEqual(result, expected)

    def test_loads_empty_object(self):
        json_str = '{}'
        result = cjson.loads(json_str)
        expected = {}
        self.assertEqual(result, expected)

    def test_loads_invalid(self):
        json_str = '{"hello": 10, "world": "100500"'
        with self.assertRaises(RuntimeError):
            cjson.loads(json_str)

    def test_dumps_simple(self):
        json_dict = {'hello': 10}
        result = cjson.dumps(json_dict)
        expected = '{"hello": 10}'
        self.assertEqual(result, expected)

    def test_dumps_nested(self):
        json_dict = {'outer': {'inner': 42}}
        result = cjson.dumps(json_dict)
        expected = '{"outer": {"inner": 42}}'
        self.assertEqual(result, expected)

    def test_dumps_array(self):
        json_dict = {'numbers': [1, 2, 3], 'strings': ['a', 'b', 'c']}
        result = cjson.dumps(json_dict)
        expected = '{"numbers": [1, 2, 3], "strings": ["a", "b", "c"]}'
        self.assertEqual(result, expected)

    def test_dumps_booleans(self):
        json_dict = {'true_val': True, 'false_val': False}
        result = cjson.dumps(json_dict)
        expected = '{"true_val": 1, "false_val": 0}'
        self.assertEqual(result, expected)

    def test_dumps_null(self):
        json_dict = {'nothing': None}
        result = cjson.dumps(json_dict)
        expected = '{"nothing": null}'
        self.assertEqual(result, expected)

    def test_dumps_empty_object(self):
        json_dict = {}
        result = cjson.dumps(json_dict)
        expected = '{}'
        self.assertEqual(result, expected)


if __name__ == '__main__':
    unittest.main()