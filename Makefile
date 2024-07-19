PYTHON_INCLUDE = /usr/include/python3.10/
PYTHON_LIB = /usr/lib/python3.10/
PYTHON_VERSION = 3.10

SRC_DIR = src
TESTS_DIR = tests
MODULE = cjson
TARGET = $(SRC_DIR)/$(MODULE).so

CFLAGS = -Wall -O3 -shared -fPIC -I$(PYTHON_INCLUDE)
LDFLAGS = -L$(PYTHON_LIB) -lpython$(PYTHON_VERSION)

all: $(TARGET)

$(TARGET): $(SRC_DIR)/cjson.c $(SRC_DIR)/cjson.h
	gcc $(CFLAGS) -o $(TESTS_DIR)/cjson.so $(SRC_DIR)/cjson.c $(LDFLAGS)

test: $(TARGET)
	PYTHONPATH=$(TESTS_DIR) python3 -m unittest discover -s $(TESTS_DIR)