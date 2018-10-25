

cc =g++ -std=c++11 -pthread -I../include/ 

prom=testAll
src = $(shell find . -name "*.cpp")
obj = $(src:%.cpp=%.o)


srcobj = $(shell find ../src/ -name "*.o"| grep -v "main.o")


$(prom):$(obj)
	$(cc) -o $(prom) *.o $(srcobj)  libgtest.a




thread_safe_queue_test.o: thread_safe_queue_test.cpp  ../src/thread_safe_queue.h
	$(cc) -c $< -o $@


utility_test.o:utility_test.cpp  ../src/utility.h 
	$(cc) -c $< -o $@

down_loder_test.o:down_loder_test.cpp ../src/down_loder.h  
	$(cc) -c $< -o $@

testAll.o: testAll.cpp
	$(cc) -c $< -o $@


clean:
	rm -rf $(obj) $(prom)
