

main: src/main.c
	@echo ""
	@echo "********* Compiling Reduse.c application *********"
	gcc -o dist/main ./src/main.c
	@echo "**************************************************"

clean:
	@echo
	@echo "********** Removing all generated files **********"
	rm dist/main.exe --force
	@echo "**************************************************"