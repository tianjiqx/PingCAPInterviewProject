

cc =g++ -std=c++11 -pthread
prom = downloader
#deps = common.h task.h province_server_node.h http_client thread_safe_queue.h  down_loader.h utility.h
#obj = main.o utility.o down_loder.o common.o

deps = $(shell find ./ -name "*.h")
src = $(shell find . -name "*.cpp")
obj = $(src:%.cpp=%.o)

$(prom):$(obj)
	$(cc) -o $(prom) $(obj)


common.o: common.cpp common.h
	$(cc) -c $< -o $@
down_loder.o: down_loder.cpp down_loder.h task.h http_client.h common.h \
 province_server_node.h thread_safe_queue.h
	$(cc) -c $< -o $@
main.o: main.cpp utility.h province_server_node.h common.h task.h \
 http_client.h thread_safe_queue.h down_loder.h
	$(cc) -c $< -o $@
utility.o: utility.cpp utility.h province_server_node.h common.h task.h \
 http_client.h thread_safe_queue.h
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(prom)



