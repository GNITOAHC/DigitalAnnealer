#include "./algo/sa/sa.h"
#include "./algo/sqa/sqa.h"

std::string format(const std::string fmt_str, ...);

/*
 * --print-conf
 *  Print to L<len>_H<height>_Ti<init-t>_Tf<final-t>_tau<tau>.tsv <- Simulated Annealing
 *  Print to L<len>_H<height>_Gi<init-g>_Gf<final-g>_tau<tau>.tsv <- Simulated Quantum Annealing
 *  Print to tri_<len>_<height>_Ti<init-t>_Tf<final-t>_tau<tau>.tsv <- Triangular lattice for Simulated Annealing
 *  Print to tri_<len>_<height>_Gi<init-g>_Gf<final-g>_tau<tau>.tsv <- Triangular lattice for Simulated Quantum Annealing
 */

void printSA(const Anlr_SA&, const Params_SA&);
void printTriSA(const Anlr_SA&, const Params_SA&);

void printSQA(const Anlr_SQA&, const Params_SQA&);
void printTriSQA(const Anlr_SQA&, const Params_SQA&);
