.PHONY: test

build:
	pio run -e ota

buildloop:
	-$(MAKE) build
	fswatch include/* lib/* src/* | xargs $(MAKE) build

test:
	pio test -e test

testloop:
	-$(MAKE) test
	fswatch include/* lib/* src/* |xargs $(MAKE) test

upload:
	pio run -e ota --target upload && sleep 15 && curl -s http://172.16.1.132:9090