build:
	g++ -I. -std=c++11 ID3.cc -o ID3
clean:
	rm ID3
test:
	./ID3 test_set.csv blah OUTPUT_testset.txt
run:
	./ID3 training_set.csv test_set.csv model
