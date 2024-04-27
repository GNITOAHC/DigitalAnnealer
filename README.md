# Annealer

## Prerequisite

First, build the project via `make`. For more info, run `./main --help`.

```shell
$ ./main_exe --help
Options:
  --qubo                     Specify that the graph is a QUBO
  --file <source>            The source file path of the input
  --h-tri <length>           Use built-in tool to create triangular lattice
  --ini-g <gamma>            Specify an initial gamma value for triangular lattice
  --final-g <gamma>          Specify an final gamma value for triangular lattice
  --ini-t <temp>             Specify an initial temperature value for triangular lattice
  --final-t <temp>           Specify an final temperature value for triangular lattice
  --tau <tau>                Specify a tau for annealer
  --func <func_string>       Specify a function for annealer, either "sa" or "sqa"
  --height <height>          Specify a height for triangular lattice ( When annealing with func sqa ) default 8
  --print-conf               Output the configuration
  --help                     Display this information
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

All the samples are in the direstory `sample`. Please `make` then copy `cp ./main_exe ./sample/main_exe` the executable to the `sample` directory.

> [!NOTE]  
> Run the following inside `./sample` directory.

1. Run `sh run.sh` will run `./main_exe --h-tri 9 --ini-g 0.2 --func sa`, which use default triangular lattice with length 9 & initialized the gamma to 0.2.

   ```shell
   $ ./main_exe --h-tri 9 --ini-g 0.2 --func sa
   Hamiltonian energy: 243
   Simulated annealing
   Hamiltonian energy: -81
   Squared order parameter debug section END:
   layer 0: 0.000001
   ```

2. `sample2.in` and `sample3.in` is a **QUBO/Ising Model** evaluation transform equivalent. `sample2.in` with `--qubo` will be translated to ising model.

   ```shell
   $ ./main_exe --file sample2.in --qubo
   Simulated annealing
   Hamiltonian energy: 0

   $ ./main_exe --file sample3.in
   Simulated annealing
   Hamiltonian energy: 0
   ```

3. To use default triangular model, use `--h-tri` to specify the length and use `--height` to specify the height.

   > Only `--func sqa` i.e. simulated quantum annealing support the height parameter.

   ```shell
   $ ./main_exe --h-tri 9 --ini-g 0.2 --func sqa --height 32
   Hamiltonian energy: 243
   Simulated quantum annealing
   Hamiltonian energy: -2412
   Squared order parameter debug section END:
   layer 0: 0.000000
   layer 1: 0.000001
   ...
   ```
