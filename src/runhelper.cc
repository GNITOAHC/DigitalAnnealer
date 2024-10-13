#include "runhelper.h"
#include "args/Args.h"
#include "graph/tri/tri.h"

#include <cstdarg>
#include <cstring>
#include <memory>

// String format
std::string format (const std::string fmt_str, ...) {
    int final_n,
        n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while (1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n) n += abs(final_n - n + 1);
        else break;
    }
    return std::string(formatted.get());
}

std::string getfilename (const ANNEAL_FUNC& a, const bool& hlayer, const bool config = false) {
    std::string filenames[4] = { "%dL%d_H%d_Ti%f_Tf%f_tau%d.tsv", "%dL%d_H%d_Gi%f_Gf%f_tau%d.tsv",
                                 "%dtri_%d_%d_Ti%f_Tf%f_tau%d.tsv",
                                 "%dtri_%d_%d_Gi%f_Gf%f_tau%d.tsv" };
    switch (a) {
        case ANNEAL_FUNC::SA:
            {
                if (hlayer) return filenames[0];
                return "cfg_" + filenames[0];
            };
        case ANNEAL_FUNC::SQA:
            {
                if (hlayer) return filenames[1];
                return "cfg_" + filenames[1];
            };
        default: return "xx";
    }
}

std::string getTriName (const ANNEAL_FUNC& a) {
    switch (a) {
        case ANNEAL_FUNC::SA: return "tri_%d_%d_Ti%f_Tf%f_tau%d.tsv";
        case ANNEAL_FUNC::SQA: return "tri_%d_%d_Gi%f_Gf%f_tau%d.tsv";
        default: return "xx";
    }
}

void printSA (const Anlr_SA& sa, const Params_SA& p) {
    const int l = sa.getLength(), h = sa.getHeight(), t = p.tau, r = p.rank;
    const double it = p.init_t, ft = p.final_t;
    std::ofstream outfile;
    // Print HLayer
    std::string filename = format(getfilename(ANNEAL_FUNC::SA, true), r, l, h, it, ft, t);
    outfile.open(filename, std::ios::out);
    sa.printHLayer(outfile);
    outfile.close();
    // Print Config
    filename = format(getfilename(ANNEAL_FUNC::SA, false, true), r, l, h, it, ft, t);
    outfile.open(filename, std::ios::out);
    sa.printConfig(outfile);
    outfile.close();
    return;
}

void printTriSA (const Anlr_SA& sa, const Params_SA& p) {
    const int l = sa.getLength(), h = sa.getHeight(), t = p.tau, r = p.rank;
    const double it = p.init_t, ft = p.final_t;
    std::ofstream outfile;

    std::string filename = format(getTriName(ANNEAL_FUNC::SA), r, l, h, it, ft, t);
    outfile.open(filename, std::ios::out);
    tri::printTriConf(sa.getSpins(), sa.getLength(), outfile);
    outfile.close();
}

void printSQA (const Anlr_SQA& sqa, const Params_SQA& p) {
    const int l = sqa.getLength(), h = sqa.getHeight(), t = p.tau, r = p.rank;
    const double ig = p.init_g, fg = p.final_g;
    std::ofstream outfile;
    // Print HLayer
    std::string filename = format(getfilename(ANNEAL_FUNC::SQA, true), r, l, h, ig, fg, t);
    outfile.open(filename, std::ios::out);
    sqa.printHLayer(outfile);
    outfile.close();
    // Print Config
    filename = format(getfilename(ANNEAL_FUNC::SQA, false, true), r, l, h, ig, fg, t);
    outfile.open(filename, std::ios::out);
    sqa.printConfig(outfile);
    outfile.close();
    return;
}

void printTriSQA (const Anlr_SQA& sqa, const Params_SQA& p) {
    const int l = sqa.getLength(), h = sqa.getHeight(), t = p.tau, r = p.rank;
    const double ig = p.init_g, fg = p.final_g;
    std::ofstream outfile;

    std::string filename = format(getTriName(ANNEAL_FUNC::SQA), r, l, h, ig, fg, t);
    outfile.open(filename, std::ios::out);
    tri::printTriConf(sqa.getSpins(), sqa.getLength(), outfile);
    outfile.close();
}
