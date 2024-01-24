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
  --gamma <gamma>                  Specify a gamma value for triangular lattice
  --temperature-tau <tau>          Specify a temperature tau
  --gamma-tau <tau>                Specify a gamma tau
  --final-gamma <gamma>            Specify a final gamma value
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

All the samples are in the direstory `sample`. Please `make` then copy `cp ./main ./sample/main` the executable to the `sample` directory.

> [!NOTE]  
> Run the following inside `./sample` directory.

1. Run `sh run.sh` will run `./main --tri 12 32 --file sample.in`, which input a ising model with length of 12 and height of 32.

   ```shell
   $ cat run.sh
   ./main --tri 12 32 --file ./sample.in

   $ run.sh
   TRI
   Hamiltonian energy: 10085.41901
   Hamiltonian energy: -8268.693888
   Order parameter length squared: 0.9077329282
   ```

2. `sample2.in` and `sample3.in` is a **QUBO/Ising Model** evaluation transform equivalent. `sample2.in` with `--qubo` will be translated to ising model.

   ```shell
   $ ./main --qubo --file sample2.in
   QUBO
   Hamiltonian energy: 4
   Hamiltonian energy: 0

   $ ./main --file sample3.in
   Hamiltonian energy: 4
   Hamiltonian energy: 0
   ```

3. `sample4.in` is a 9 \* 9 ising model

   ```shell
   $ ./main --tri 9 1 --file sample4.in
   TRI
   Hamiltonian energy: 243
   Hamiltonian energy: -81
   Order parameter length squared: 0.7974394147
   ```

4. To use default triangular model, use `--default-tri` to specifc the length and height.

   ```shell
   $ ./main --default-tri 9 32 --gamma 0.2
   Hamiltonian energy: 9878.952046
   Hamiltonian energy: -4633.291415
   Order parameter length squared: 0.8585962506
   ```
