JUNIT=./junit-4.12.jar:./hamcrest-core-1.3.jar
CP=.:../f21

lecture21:
	cd ../f21; javac -d . List.java; cd -

%.class: 
	@javac -cp $(JUNIT):$(CP) $*.java

test: clean TDD.class
	@java -cp $(JUNIT):$(CP) org.junit.runner.JUnitCore TDD

clean:
	@rm -f *.class
