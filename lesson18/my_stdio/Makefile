libmystdio.a:my_stdio.c my_string.o
	ar -rc $@ $^
%.o:%.c
	gcc -c $<


.PHONY:clean
clean:
	rm -rf *.a *.o stdc*
	@echo "clean .. done"

.PHONY:output
output:
		@mkdir -p stdc/include
		@mkdir -p stdc/lib
		@cp -f *.h stdc/include 
		@cp -f *.a stdc/lib
		@tar -czf stdc.tgz stdc
		@echo "output stdc ... done"


