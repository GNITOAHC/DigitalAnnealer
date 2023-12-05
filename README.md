# Annealer

## Prerequisite

First, build the project via `make`. For more info, run `./main --help`.

```shell
$ ./main --help
Usage: ./main [options]
Options:
  --tri <length> <height>          Specify a triangular lattice
  --qubo                           Specify that the graph is a QUBO
  --file <source>                  The source file path of the input
  --default-tri <length> <height>  Use built-in tool to create triangular lattice
  --help                           Display this information
```

## Running the script

First, create a formated input file representing the function to be anneal.

- If there are two polynomials, format with `polynomial1 polynomial2 coefficient`.
- If there is only one polynomial, format with `polynomial coefficient`.
- If it's a constant, just write it on a single line.

e.x. $1 x_1 x_2 + 1 x_1 + 1 x_2 + 1$

```txt
1 2 1
1 1
2 1
1
```

## Samples

1. Run `sh run.sh` will run `./main --tri 12 32 < sample.in`, which input a ising model with length of 12 and height of 32.

   ```shell
   $ cat run.sh
   make clean && make
   ./main --tri 12 32 < sample.in

   $ run.sh
   TRI
   Hamiltonian energy: 10085.41901
   Hamiltonian energy: -8103.183792
   Order parameter length squared: 0.9431966146
   ```

2. `sample2.in` and `sample3.in` is a **QUBO/Ising Model** evaluation transform equivalent. `sample2.in` with `--qubo` will be translated to ising model.

   ```shell
   $ ./main --qubo < sample2.in
   QUBO
   Hamiltonian energy: 4
   Hamiltonian energy: 0

   $ ./main < sample3.in
   Hamiltonian energy: 4
   Hamiltonian energy: 0
   ```

3. `sample4.in` is a 9 \* 9 ising model

   ```shell
   $ ./main --tri 9 1 < sample4.in
   TRI
   Hamiltonian energy: 243
   Hamiltonian energy: -81
   Order parameter length squared: 0.7974394147
   ```
