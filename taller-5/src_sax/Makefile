NVCC = nvcc

all: saxpy

%.o : %.cu
	$(NVCC) -c $< -o $@

matmul : matmul.o
	$(NVCC) $^ -o $@

clean:
	rm -rf *.o *.a matmul
